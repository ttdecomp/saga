#include "nu2api/nusound/nusound3_include.hpp"

#include "decomp.h"

#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nuvector.hpp"
#include "nu2api/numath/numath.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nusound/nusound_android.hpp"
#include "nu2api/nusound/nusound_buffer.hpp"
#include "nu2api/nusound/nusound_loader.hpp"
#include "nu2api/nusound/nusound_loader_ogg.hpp"
#include "nu2api/nusound/nusound_streamer.hpp"
#include "nu2api/nusound/nusound_voice.hpp"
#include "nu2api/nusound/nusound_decoder.hpp"

#include <new>
#include <stdarg.h>

// Stereo stream slot (0x10 bytes in the original):
//   +0x0 stream            the streaming sample playing in this slot
//   +0x4 volume            the PS2 volume (0..16383) as raw bits
//   +0x8 loop              PENDING-START flag: PlayStereoV sets it to 1, and
//                          NuSound3Update clears it once the voice is created
//   +0xc field_0xc         the voice/loader loop flag (PlayStereoV key 0xb)
//   +0xd field_0xd         started flag: set once the voice began playing
struct NuSoundStream {
    NuSoundStreamingSample *stream;
    i32 ps2volume;
    i32 loop;
    u8 field_0xc;
    u8 field_0xd;
};

// The single stereo-stream voice shared by both stream slots.
static NuSoundWeakPtr<NuSoundVoice> g_StereoStreamVoice;

static NuSoundListener g_NuSoundListener;
static NuEListNode<NuSoundListener> g_NuSoundListenerNode;
static NuEList<NuSoundListener> g_NuSoundListenerList;
static NUMTX g_NuSoundHeadMatrix;
// The original focused the listener on the player object; the title screen
// runs before gameplay, where it is NULL and the focus stays disabled.
static void *g_NuSoundFocusPlayer = NULL;
static nuvec_s g_NuSoundFocusPosition;

extern "C" {
    const char *audio_ps2_music_ext = ".vag";
}

static NuVector<nusound_filename_info_s> g_NuSoundSamples;

static NuSoundStream *g_NuSoundStreams[4] = {0};

static NuSoundLoadTrigger g_NuSoundLoadTrigger;

// 0x44-byte voice wrapper the NuSound3 API keeps per playing source. The
// elists link these while they are pending playback / active / pending
// destruction.
struct NuSound3Voice {
    NuSoundWeakPtr<NuSoundVoice> weak_ptr;
    NuSoundSource *source;
    f32 pitch;     // +0x1c
    i32 volume;    // +0x20 raw volume bits
    f32 falloff_a; // +0x24
    f32 falloff_b; // +0x28
    bool has_3d;
    nuvec_s position;
    f32 pan;
    f32 rnd;
    f32 rumble;
    i32 pause_counter; // frames in the update sweep
};

namespace {

    // NuEList append/remove, done here as local helpers so the list header keeps
    // emitting no inline code into every translation unit that pulls it in.
    void StreamListPushBack(NuEList<NuSound3Voice> *list, NuEListNode<NuSound3Voice> *node) {
        node->prev = list->tail;
        node->next = NULL;
        if (list->tail != NULL) {
            list->tail->next = node;
        } else {
            list->head = node;
        }
        list->tail = node;
        list->length++;
    }

    void StreamListRemove(NuEList<NuSound3Voice> *list, NuEListNode<NuSound3Voice> *node) {
        if (node->prev != NULL) {
            node->prev->next = node->next;
        } else {
            list->head = node->next;
        }
        if (node->next != NULL) {
            node->next->prev = node->prev;
        } else {
            list->tail = node->prev;
        }
        list->length--;
    }

} // namespace

static NuEList<NuSound3Voice> g_NuSoundVoicesPendingPlayback{};
static NuEList<NuSound3Voice> g_NuSoundVoicesActive{};
static NuEList<NuSound3Voice> g_NuSoundVoicesPendingDestruction{};

static NuSoundBuffer g_NuSoundStreamBuffers[4];

static NuSoundStreamer *g_NuSoundStreamer = NULL;

static i32 g_NuSoundLoadBits = 0;
static NuThread *g_NuSoundLoadThread = NULL;
static pthread_mutex_t g_NuSoundLoadCriticalSection = PTHREAD_MUTEX_INITIALIZER;

void NuSound3SampleLoadThread(void *arg) {
    do {
    } while (g_NuSoundLoadBits == 0);

    if (g_NuSoundSamples.length != 0) {
    }
}

nusound_filename_info_s *ConfigureMusic(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd) {
    nusound_filename_info_s *finfo;

    music_man.Initialise("audio\\music.cfg", NULL, bufferStart, *bufferEnd);
    music_man.GetSoundFiles(&finfo, NULL);

    audio_ps2_music_ext = ".mib";

    // MusicConfig *musicConfig;
    // musicConfig = (MusicConfig *)((i32)bufferStart->voidptr + 3U & 0xfffffffc);

    // musicConfig->field0_0x0 = 0;
    // ActionPairTab = &musicConfig->actionTab;
    // musicConfig->actionTab = -1;
    // musicConfig->ambientTab = -1;
    // AmbientPairTab = &musicConfig->ambientTab;
    // bufferStart->voidptr = musicConfig + 1;

    return finfo;
}

NuSoundLoader *NuSoundSystem::CreateFileLoader(FileType type) {
    NuSoundLoaderWAV *wav_loader;

    switch (type) {
        case FileType::WAV:
            // libTTapp.so allocates the WAV loader from SCRATCH (0x1c bytes,
            // nusound.cpp:1233) and placement-news it in place.
            wav_loader =
                (NuSoundLoaderWAV *)_AllocMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, 0x1c, 4,
                                                 "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:1233");
            if (wav_loader != NULL) {
                new (wav_loader) NuSoundLoaderWAV();
                return wav_loader;
            }
            break;
        case FileType::OGG:

            NuSoundLoaderOGG *ogg_loader =
                (NuSoundLoaderOGG *)_AllocMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, 0x2e8, 4,
                                                 "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound.cpp:1247");
            if (ogg_loader != NULL) {
                new (ogg_loader) NuSoundLoaderOGG();
                return ogg_loader;
            }

            break;
    }

    return NULL;
}

void NuSound3Init(i32 zero) {
    bool is_crappy = NuIOS_IsLowEndDevice();

    NuCore::Initialize();

    NuSound.Initialise(0x633333 + (is_crappy ? 0 : 0x1ccccd));
    // libTTapp.so 0x3109af: NuSound3Init brings up the decoder singleton
    // thread right after the sound system.
    NuSoundDecoder::Initialise();

    // NuMemoryGet()->GetThreadMem()->_BlockAlloc(0xa48, 4, 1, "", 7);
    NuSoundStreamer *streamer = NU_ALLOC_T(NuSoundStreamer, 1, "", NUMEMORY_CATEGORY_NUSOUND);
    if (streamer != NULL) {
        new (streamer) NuSoundStreamer{};
    }
    g_NuSoundStreamer = streamer;

    // g_NuSoundLoadThread = NuThreadManager::CreateThread(NuCore::m_threadManager, NuSound3SampleLoadThread, (void
    // *)0x0,
    // 0, "NuSoundLoadThread", 0, 1, 1);
    g_NuSoundLoadThread =
        NuCore::m_threadManager->CreateThread(NuSound3SampleLoadThread, NULL, 0, "NuSoundLoadThread", 0,
                                              NUTHREADCAFECORE_UNKNOWN_1, NUTHREADXBOX360CORE_UNKNOWN_1);

    // NuSound3Init registers the single 3D listener with the head matrix of
    // the title screen camera.
    NuSound.AddListener(&g_NuSoundListener);
    g_NuSoundListener.SetHeadMatrix((const VuMtx *)&g_NuSoundHeadMatrix);
    g_NuSoundListener.Enable();

    g_NuSoundStreamBuffers[0].Allocate(NuSoundSystem::GetStreamBufferSize() / 2,
                                       NuSoundSystem::MemoryDiscipline::SAMPLE);
    g_NuSoundStreamBuffers[1].Allocate(NuSoundSystem::GetStreamBufferSize() / 2,
                                       NuSoundSystem::MemoryDiscipline::SAMPLE);
    g_NuSoundStreamBuffers[2].Allocate(NuSoundSystem::GetStreamBufferSize() / 2,
                                       NuSoundSystem::MemoryDiscipline::SAMPLE);
    g_NuSoundStreamBuffers[3].Allocate(NuSoundSystem::GetStreamBufferSize() / 2,
                                       NuSoundSystem::MemoryDiscipline::SAMPLE);
}

i32 NuSound3InitV(VARIPTR *bufferStart, VARIPTR bufferEnd, i32 zero1, i32 zero2) {
    NuSound3Init(0);
    return 1;
}

// PS2 volume (0..16383) to the engine's linear scalar.
f32 PS2VolumeToScalar(i32 volume) {
    return (f32)volume / 16383.0f;
}

// NuSound3PlayStereoV is the original varargs play entry. The music player
// pushes every argument as a raw dword EXCEPT STARTOFFSET, which is a true
// 8-byte double: VOL is float bits in an int (always 0 from NuMusic, which
// drives volume via NuSound3SetStereoStreamVolume instead), PITCH is a raw
// fixed-point dword the original reads and ignores. The sample's two stream
// buffers are carved out of g_NuSoundStreamBuffers[stream_index * 2] and
// [stream_index * 2 + 1].
i32 NuSound3PlayStereoV(NUSOUNDPLAYTOK token, ...) {
    va_list args;
    va_start(args, token);

    i32 stream_index = 0;
    i32 sample_index = -1;
    i32 volume_bits = 0;
    i32 pitch = 0;
    f32 start_offset = 0.0f;
    i32 voice_loop = 0;

    while (token != NUSOUNDPLAYTOK_END) {
        switch (token) {
            case NUSOUNDPLAYTOK_STEREOSTREAM: {
                stream_index = va_arg(args, i32);
                break;
            }
            case NUSOUNDPLAYTOK_SAMPLE: {
                sample_index = va_arg(args, i32);
                break;
            }
            case NUSOUNDPLAYTOK_VOL: {
                volume_bits = va_arg(args, i32);
                break;
            }
            case NUSOUNDPLAYTOK_PITCH: {
                pitch = va_arg(args, i32);
                break;
            }
            case NUSOUNDPLAYTOK_STARTOFFSET: {
                start_offset = (f32)va_arg(args, double);
                break;
            }
            case NUSOUNDPLAYTOK_LOOPTYPE: {
                voice_loop = va_arg(args, i32);
                break;
            }
            default: {
                LOG_WARN("Unknown token %d", token);
                break;
            }
        }

        token = (NUSOUNDPLAYTOK)va_arg(args, u32);
    }

    if (stream_index < 0 || stream_index >= 4) {
        return 0;
    }

    NuSoundStreamingSample *streaming_sample = g_NuSoundSamples.data[sample_index].sample;
    if (streaming_sample->GetThreadQueueCount() < 1) {
        NuSoundStream *stream_ptr = g_NuSoundStreams[stream_index];
        if (stream_ptr != NULL) {
            NuSoundStreamingSample *stream = stream_ptr->stream;
            if (stream != streaming_sample) {
                NuSound3StopStereoStream(stream_index);
                stream = stream_ptr->stream;
            }

            NuSoundSample::LoadState load_state = stream->GetLoadState();
            if ((load_state == NuSoundSample::LoadState::NOT_LOADED) && stream->GetResourceCount() == 0) {
                delete stream_ptr;
                g_NuSoundStreams[stream_index] = NULL;
            } else if (g_NuSoundStreams[stream_index] != NULL) {
                return 0;
            }
        }

        if (streaming_sample != NULL && streaming_sample->GetResourceCount() == 0) {
            NuSoundStream *node = new NuSoundStream();

            node->ps2volume = volume_bits;
            node->loop = 1; // PENDING-START; NuSound3Update creates the voice.
            node->field_0xc = (u8)(voice_loop != 0);
            node->field_0xd = 0;
            g_NuSoundStreams[stream_index] = node;
            node->stream = streaming_sample;

            streaming_sample->sound_buffer1 = &g_NuSoundStreamBuffers[stream_index * 2];
            streaming_sample->sound_buffer2 = &g_NuSoundStreamBuffers[stream_index * 2 + 1];

            streaming_sample->Reference();
            g_NuSoundStreamer->RequestCue(streaming_sample, node->field_0xc != 0, start_offset, false);

            return 1;
        }
    }

    return 0;
}

// NuSound3StopStereoStream tears down the stream node in a stereo-stream slot.
// The slot's node itself is deleted by NuSound3Update once the sample has
// fully unloaded.
void NuSound3StopStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }

    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream == NULL) {
        return;
    }

    if (g_StereoStreamVoice.obj != NULL) {
        if (stream->field_0xd != 0) {
            ((NuSoundVoice *)g_StereoStreamVoice.obj)->Stop(true);
            NuSound.ReleaseVoice((NuSoundVoice *)g_StereoStreamVoice.obj);
            stream->field_0xd = 0;
        }
        g_StereoStreamVoice.Set(NULL);
        g_StereoStreamVoice.bool_value = false;
    }

    NuSoundStreamingSample *sample = stream->stream;
    if (sample != NULL && sample->GetResourceCount() > 0) {
        sample->Release();
        g_NuSoundStreamer->RequestClose(sample);
    }
}

void NuSound3PauseStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream != NULL && stream->field_0xd != 0 && g_StereoStreamVoice.obj != NULL) {
        ((NuSoundVoice *)g_StereoStreamVoice.obj)->Pause();
    }
}

void NuSound3ResumeStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream != NULL && stream->field_0xd != 0 && g_StereoStreamVoice.obj != NULL) {
        ((NuSoundVoice *)g_StereoStreamVoice.obj)->Resume();
    }
}

// NuSound3CreateVoice wraps a one-shot 3D sound source into a NuSound3Voice
// and queues it for voice creation in NuSound3Update. Sources are limited to
// three pending plays each, sixteen in flight overall, and the oldest active
// voice gets stolen first when the budget is exhausted.
void NuSound3CreateVoice(nuvec_s *pos, i32 index, f32 volume, f32 pitch, i32 falloff_a, i32 falloff_b, f32 pan,
                         bool has_3d) {
    if (NuSound.GetNumAvailableOutputDevices() < 1 || g_NuSoundSamples.length <= index) {
        return;
    }

    NuSoundSource *source = g_NuSoundSamples.data[index].sample;
    NuSoundSample *sample = (NuSoundSample *)source;
    if (sample == NULL || sample->GetLoadState() != NuSoundSample::LoadState::TWO || sample->GetResourceCount() < 1) {
        return;
    }

    // Per-source pending limit: three wrappers already queued for this source.
    i32 pending = 0;
    for (NuEListNode<NuSound3Voice> *node = g_NuSoundVoicesPendingPlayback.head; node != NULL; node = node->next) {
        if (node->data->source == source) {
            pending++;
        }
    }
    if (pending >= 3) {
        return;
    }

    // Total in-flight budget (pending + active); steal the oldest otherwise.
    while (g_NuSoundVoicesPendingPlayback.length + g_NuSoundVoicesActive.length >= 16) {
        NuEListNode<NuSound3Voice> *oldest = g_NuSoundVoicesActive.head;
        if (oldest == NULL) {
            return;
        }
        if (oldest->data->weak_ptr.obj != NULL) {
            ((NuSoundVoice *)oldest->data->weak_ptr.obj)->Stop(true);
            NuSound.ReleaseVoice((NuSoundVoice *)oldest->data->weak_ptr.obj);
            oldest->data->weak_ptr.Set(NULL);
            oldest->data->weak_ptr.bool_value = false;
        }
        StreamListRemove(&g_NuSoundVoicesActive, oldest);
        delete oldest->data;
        delete oldest;
    }

    NuSound3Voice *voice = new NuSound3Voice();
    voice->source = source;
    voice->pitch = pitch;
    voice->volume = *(i32 *)&volume;
    voice->falloff_a = (f32)falloff_a;
    voice->falloff_b = (f32)falloff_b;
    voice->has_3d = has_3d;
    voice->position = *pos;
    voice->pan = pan;
    voice->pause_counter = 0;

    NuEListNode<NuSound3Voice> *node = new NuEListNode<NuSound3Voice>();
    node->data = voice;
    StreamListPushBack(&g_NuSoundVoicesPendingPlayback, node);
}

void NuSound3Update(void) {
    if (NuSound.GetNumAvailableOutputDevices() < 1) {
        NuSound.Update(NuTimeGetFrameTime());
        return;
    }

    // The listener focus follows the player (NULL on the title screen).
    if (g_NuSoundFocusPlayer == NULL) {
        g_NuSoundListener.DisableFocusPosition();
    } else {
        g_NuSoundListener.SetFocusPosition((const VuVec *)&g_NuSoundFocusPosition);
        g_NuSoundListener.EnableFocusPosition();
    }

    pthread_mutex_lock(&NuSound.mutex);

    // (a) Voices queued for destruction: release and unlink.
    for (NuEListNode<NuSound3Voice> *node = g_NuSoundVoicesPendingDestruction.head; node != NULL;) {
        NuEListNode<NuSound3Voice> *next = node->next;
        if (node->data->weak_ptr.obj != NULL) {
            NuSound.ReleaseVoice((NuSoundVoice *)node->data->weak_ptr.obj);
            node->data->weak_ptr.Set(NULL);
            node->data->weak_ptr.bool_value = false;
        }
        StreamListRemove(&g_NuSoundVoicesPendingDestruction, node);
        delete node->data;
        delete node;
        node = next;
    }

    // (b) Active sweep: release stopped voices and long-paused ones.
    for (NuEListNode<NuSound3Voice> *node = g_NuSoundVoicesActive.head; node != NULL;) {
        NuEListNode<NuSound3Voice> *next = node->next;
        NuSoundVoice *voice = (NuSoundVoice *)node->data->weak_ptr.obj;

        if (voice != NULL) {
            NuSoundVoice::PlayState state = voice->GetState();
            if (state != NuSoundVoice::PLAYSTATE_STOPPED && node->data->pause_counter < 16) {
                if ((voice->flags & 8) != 0) {
                    node->data->pause_counter++;
                }
                node = next;
                continue;
            }
            voice->Stop(true);
            NuSound.ReleaseVoice(voice);
            node->data->weak_ptr.Set(NULL);
            node->data->weak_ptr.bool_value = false;
        }

        StreamListRemove(&g_NuSoundVoicesActive, node);
        delete node->data;
        delete node;
        node = next;
    }

    // (c) Pending playback: create voices while the budget allows.
    while (NuSound.voice_count < 30) {
        NuEListNode<NuSound3Voice> *node = g_NuSoundVoicesPendingPlayback.head;
        if (node == NULL) {
            break;
        }

        NuSoundVoice *voice = NuSound.CreateVoice(node->data->source, false);
        if (voice == NULL) {
            break;
        }

        node->data->weak_ptr.Set(voice);
        node->data->weak_ptr.bool_value = true;
        voice->SetAutoDelete(true);
        voice->SetVolume(PS2VolumeToScalar(node->data->volume));
        voice->SetPitch(node->data->pitch);
        if (node->data->has_3d) {
            voice->SetFalloff(node->data->falloff_a, node->data->falloff_b, NuSoundSystem::FalloffType::LINEAR);
            voice->SetPosition((VuVec *)&node->data->position);
            voice->SetSurroundMode(NuSoundSystem::SurroundMode::ZERO);
            voice->SetListeners(&g_NuSoundListenerList);
        }
        voice->Play();

        StreamListRemove(&g_NuSoundVoicesPendingPlayback, node);
        StreamListPushBack(&g_NuSoundVoicesActive, node);
    }

    pthread_mutex_unlock(&NuSound.mutex);

    // (d) The two stereo-stream slots: create the pending voice, keep its
    // volume in sync, and tear the stream down once the voice stopped.
    for (i32 i = 0; i < 2; i++) {
        NuSoundStream *stream = g_NuSoundStreams[i];
        if (stream == NULL) {
            continue;
        }

        if (stream->loop != 0) {
            if (g_StereoStreamVoice.obj == NULL) {
                NuSoundStreamingSample *sample = stream->stream;
                if (sample != NULL && sample->GetLoadState() == NuSoundSample::LoadState::TWO &&
                    sample->GetResourceCount() >= 1 && sample->GetThreadQueueCount() == 0) {
                    NuSoundVoice *voice = NuSound.CreateVoice(sample, stream->field_0xc != 0);
                    if (voice != NULL) {
                        g_StereoStreamVoice.Set(voice);
                        g_StereoStreamVoice.bool_value = true;
                        voice->SetAutoDelete(false);
                        voice->SetVolume(PS2VolumeToScalar(stream->ps2volume));
                        voice->Play();
                        stream->loop = 0;
                        stream->field_0xd = 1;
                    }
                }
            }
        } else if (stream->field_0xd != 0) {
            if (g_StereoStreamVoice.obj != NULL) {
                ((NuSoundVoice *)g_StereoStreamVoice.obj)->SetVolume(PS2VolumeToScalar(stream->ps2volume));
                if (((NuSoundVoice *)g_StereoStreamVoice.obj)->GetState() == NuSoundVoice::PLAYSTATE_STOPPED) {
                    NuSound3StopStereoStream(i);
                }
            } else {
                // The voice was released elsewhere; retire the stream.
                NuSound3StopStereoStream(i);
            }
        }

        // Slot cleanup: fully unloaded streams free their node.
        if (stream->stream == NULL) {
            delete stream;
            g_NuSoundStreams[i] = NULL;
        } else if (stream->stream->GetLoadState() == NuSoundSample::LoadState::NOT_LOADED &&
                   stream->stream->GetResourceCount() == 0 && stream->stream->GetThreadQueueCount() == 0) {
            delete stream;
            g_NuSoundStreams[i] = NULL;
        }
    }

    // (e) Sample-load kick: poke the load trigger when the bit pattern the
    // streaming threads wait on changes.
    static i32 last_load_bits = 0;
    if (g_NuSoundLoadBits != last_load_bits) {
        last_load_bits = g_NuSoundLoadBits;
        pthread_cond_signal(&g_NuSoundLoadTrigger.cond);
    }

    NuSound.Update(NuTimeGetFrameTime());
}

// NuSound3GetStereoStreamStatus reports how many stereo streams are running.
i32 NuSound3GetStereoStreamStatus() {
    i32 status = 0;

    for (i32 i = 0; i < 4; i++) {
        if (g_NuSoundStreams[i] != NULL) {
            status++;
        }
    }

    return status;
}

// Stream key status for the NuMusic voice state machine: 0 = slot empty,
// 1 = stream loaded / playing, 2 = stream finished (the voice stopped or is
// already gone).
i32 NuSound3StreamKeyStatus(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return 0;
    }
    if (g_NuSoundStreams[stream_index] == NULL) {
        return 0;
    }
    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream->field_0xd != 0 &&
        (g_StereoStreamVoice.obj == NULL ||
         ((NuSoundVoice *)g_StereoStreamVoice.obj)->GetState() == NuSoundVoice::PLAYSTATE_STOPPED)) {
        return 2;
    }
    return 1;
}

// Volume arrives as the PS2-style 0..16383 fixed point computed by
// NuMusic::Process. The streamer applies it when mixing.
void NuSound3SetStereoStreamVolume(i32 stream_index, i32 volume) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream != NULL) {
        stream->ps2volume = volume;
        if (stream->field_0xd != 0 && g_StereoStreamVoice.obj != NULL) {
            ((NuSoundVoice *)g_StereoStreamVoice.obj)->SetVolume(PS2VolumeToScalar(stream->ps2volume));
        }
    }
}

// Decibel attenuation from music.cfg (DUCK/ATTENUATION/GLOBALATTENUATION with
// negative values): -100 dB and below is silence, 0 dB and above is unity.
f32 NuSound3dBToAmplitude(f32 db) {
    if (db <= -100.0f) {
        return 0.0f;
    }
    if (db >= 0.0f) {
        return 1.0f;
    }
    return NuExp10(db / 20.0f);
}

void NuSound3SetSampleTable(nusound_filename_info_s *info, variptr_u *buffer_start, variptr_u buffer_end) {
    if (info == NULL) {
        return;
    }

    for (; info->index != -1; info++) {
        // TODO: dont cast classes
        if (info->index < 0x1000) {
            info->sample = (NuSoundStreamingSample *)NuSound.AddSample(info->filename, NuSoundSystem::FileType::OGG,
                                                                       NuSoundSource::FeedType::STREAMING);
        } else {
            info->sample = (NuSoundStreamingSample *)NuSound.AddSample(info->filename, NuSoundSystem::FileType::WAV,
                                                                       NuSoundSource::FeedType::ZERO);
        }

        g_NuSoundSamples.PushBack(*info);
    }
}
