#include "nu2api/nusound/nusound3_include.hpp"

#include "decomp.h"

#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nuvector.hpp"
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

struct NuSoundStream {
    NuSoundStreamingSample *stream;
    bool paused;
    i32 ps2volume;
    bool loop;
    u8 status;
};

extern "C" {
    const char *audio_ps2_music_ext = ".vag";
}

static NuVector<nusound_filename_info_s> g_NuSoundSamples;

static NuSoundStream *g_NuSoundStreams[4] = {0};

static NuSoundLoadTrigger g_NuSoundLoadTrigger;

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
    switch (type) {
        case FileType::WAV:
            UNIMPLEMENTED("WAV loader");
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
    // NuSoundDecodeThread::Initialise();

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

    // NuSoundSystem::AddListener(&NuSound.parent, &g_NuSoundListener);
    // NuSoundListener::SetHeadMatrix(&g_NuSoundListener, &global_camera.mtx);
    // NuSoundListener::Enable(&g_NuSoundListener);

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

// NuSound3PlayStereoV is the original varargs play entry. The music player
// pushes every argument as a raw dword EXCEPT STARTOFFSET, which is a true
// 8-byte double: VOL is float bits in an int (always 0 from NuMusic, which
// drives volume via NuSound3SetStereoStreamVolume instead), PITCH is a raw
// fixed-point dword the original reads and ignores.
i32 NuSound3PlayStereoV(NUSOUNDPLAYTOK token, ...) {
    va_list args;
    va_start(args, token);

    i32 stream_index = 0;
    i32 sample_index = -1;
    f32 volume = 0.0f;
    i32 pitch = 0;
    f32 start_offset = 0.0f;
    i32 loop_type = 0;

    while (token != NUSOUNDPLAYTOK_END) {
        switch (token) {
            case NUSOUNDPLAYTOK_STEREOSTREAM: {
                stream_index = va_arg(args, i32);
                LOG_DEBUG("stream_index=%d", stream_index);
                break;
            }
            case NUSOUNDPLAYTOK_SAMPLE: {
                sample_index = va_arg(args, i32);
                LOG_DEBUG("sample_index=%d", sample_index);
                break;
            }
            case NUSOUNDPLAYTOK_VOL: {
                u32 bits = (u32)va_arg(args, i32);
                volume = *(f32 *)&bits;
                LOG_DEBUG("volume=%f", volume);
                break;
            }
            case NUSOUNDPLAYTOK_PITCH: {
                pitch = va_arg(args, i32);
                LOG_DEBUG("pitch=%d", pitch);
                break;
            }
            case NUSOUNDPLAYTOK_STARTOFFSET: {
                start_offset = (f32)va_arg(args, double);
                LOG_DEBUG("start_offset=%f", start_offset);
                break;
            }
            case NUSOUNDPLAYTOK_LOOPTYPE: {
                loop_type = va_arg(args, i32);
                LOG_DEBUG("loop_type=%d", loop_type);
                break;
            }
            default: {
                LOG_WARN("Unknown token %d", token);
                break;
            }
        }

        token = (NUSOUNDPLAYTOK)va_arg(args, u32);
    }

    LOG_DEBUG("stream_index=%d, sample_index=%d, volume=%f, pitch=%d, start_offset=%f, loop_type=%d", stream_index,
              sample_index, volume, pitch, start_offset, loop_type);

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
            if ((load_state == NuSoundSample::LoadState::NOT_LOADED) && stream_ptr->stream->GetResourceCount() == 0) {
                delete stream_ptr;
                g_NuSoundStreams[stream_index] = NULL;
            } else if (g_NuSoundStreams[stream_index] != NULL) {
                return 0;
            }
        }

        if (streaming_sample != NULL && streaming_sample->GetResourceCount() == 0) {
            NuSoundStream *node = new NuSoundStream();

            node->status = 0;
            g_NuSoundStreams[stream_index] = node;
            node->paused = false;
            node->stream = streaming_sample;
            node->ps2volume = (i32)volume;
            node->loop = loop_type != 0;

            streaming_sample->buffer1 = &g_NuSoundStreamBuffers[stream_index * 2];
            streaming_sample->buffer2 = &g_NuSoundStreamBuffers[stream_index * 2 + 1];

            streaming_sample->Reference();
            g_NuSoundStreamer->RequestCue(streaming_sample, loop_type != 0, start_offset, false);

            return 1;
        }
    }

    return 0;
}

// NuSound3StopStereoStream tears down the stream node in a stereo-stream slot:
// the original closes the sample with the streamer, releases the reference
// taken by PlayStereoV and frees the node.
void NuSound3StopStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }

    NuSoundStream *stream = g_NuSoundStreams[stream_index];
    if (stream == NULL) {
        return;
    }

    NuSoundStreamingSample *sample = stream->stream;
    if (sample != NULL) {
        g_NuSoundStreamer->RequestClose(sample);
        sample->Release();
    }

    delete stream;
    g_NuSoundStreams[stream_index] = NULL;
}

void NuSound3PauseStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    if (g_NuSoundStreams[stream_index] != NULL) {
        g_NuSoundStreams[stream_index]->paused = true;
    }
}

void NuSound3ResumeStereoStream(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    if (g_NuSoundStreams[stream_index] != NULL) {
        g_NuSoundStreams[stream_index]->paused = false;
    }
}

// Stream key status for the NuMusic voice state machine: 0 = nothing queued in
// the slot, 1 = stream loaded/playing, 2 = stream finished (reported once the
// streamer grows finish plumbing; a live slot currently reports loaded).
i32 NuSound3StreamKeyStatus(i32 stream_index) {
    if (stream_index < 0 || stream_index >= 4) {
        return 0;
    }
    if (g_NuSoundStreams[stream_index] == NULL) {
        return 0;
    }
    return 1;
}

// Volume arrives as the PS2-style 0..16383 fixed point computed by
// NuMusic::Process. The streamer applies it when mixing.
void NuSound3SetStereoStreamVolume(i32 stream_index, i32 volume) {
    if (stream_index < 0 || stream_index >= 4) {
        return;
    }
    if (g_NuSoundStreams[stream_index] != NULL) {
        g_NuSoundStreams[stream_index]->ps2volume = volume;
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
