#include "nu2api.saga/nusound/nusound3_include.hpp"

#include "decomp.h"

#include "nu2api.saga/nuandroid/nuios.h"
#include "nu2api.saga/nucore/nucore.hpp"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nucore/nuthread.h"
#include "nu2api.saga/nucore/nuvector.hpp"
#include "nu2api.saga/numusic/numusic.h"
#include "nu2api.saga/nusound/nusound.h"
#include "nu2api.saga/nusound/nusound_android.hpp"
#include "nu2api.saga/nusound/nusound_buffer.hpp"
#include "nu2api.saga/nusound/nusound_loader.hpp"
#include "nu2api.saga/nusound/nusound_sample.hpp"
#include "nu2api.saga/nusound/nusound_streamer.hpp"
#include "nu2api.saga/nusound/nusound_voice.hpp"

#include <new>
#include <stdarg.h>

struct NuSoundWeakPtrListNode {
    NuSoundStreamingSample *stream;
    bool paused;
    int ps2volume;
    bool loop;
    u8 status;
};

extern "C" {
    const char *audio_ps2_music_ext = ".vag";
}

static NuVector<nusound_filename_info_s> g_NuSoundSamples;

static NuSoundWeakPtrListNode *g_NuSoundStreams[4] = {0};

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
    } while (g_NuSoundLoadBits == NULL);

    if (g_NuSoundSamples.length != 0) {
    }
}

nusound_filename_info_s *ConfigureMusic(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd) {
    nusound_filename_info_s *finfo;

    music_man.Initialise("audio\\music.cfg", NULL, bufferStart, *bufferEnd);
    music_man.GetSoundFiles(&finfo, NULL);

    audio_ps2_music_ext = ".mib";

    // MusicConfig *musicConfig;
    // musicConfig = (MusicConfig *)((int)bufferStart->voidptr + 3U & 0xfffffffc);

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
        case FILE_TYPE_WAV:
            UNIMPLEMENTED("WAV loader");
        case FILE_TYPE_OGG:
            UNIMPLEMENTED("OGG loader");
        default:
            return NULL;
    }
}

void NuSound3Init(i32 zero) {
    bool is_crappy = NuIOS_IsLowEndDevice();

    NuCore::Initialize();

    NuSound.Initialise(0x633333 + (is_crappy ? 0 : 0x1ccccd));
    // NuSoundDecoder::Initialise();

    // NuMemoryGet()->GetThreadMem()->_BlockAlloc(0xa48, 4, 1, "", 7);
    NuSoundStreamer *streamer = (NuSoundStreamer *)NU_ALLOC(0xa48, 4, 1, "", NUMEMORY_CATEGORY_NUSOUND);
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

void NuSound3StopStereoStream(i32 streamIndex) {
    UNIMPLEMENTED("NuSound3StopStereoStream");
}

i32 NuSound3PlayStereoV(NUSOUNDPLAYTOK token, ...) {
    va_list args;
    va_start(args, token);

    i32 stream_index;
    i32 sample_index;
    f32 volume;
    f32 pitch;
    f32 start_offset;
    i32 loop_type;

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
                volume = (f32)va_arg(args, double);
                LOG_DEBUG("volume=%f", volume);
                break;
            }
            case NUSOUNDPLAYTOK_PITCH: {
                pitch = (f32)va_arg(args, double);
                LOG_DEBUG("pitch=%f", pitch);
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

    LOG_DEBUG("stream_index=%d, sample_index=%d, volume=%f, pitch=%f, start_offset=%f, loop_type=%d", stream_index,
              sample_index, volume, pitch, start_offset, loop_type);

    NuSoundStreamingSample *streaming_sample = g_NuSoundSamples.data[sample_index].streaming_sample;
    if (streaming_sample->GetThreadQueueCount() < 1) {
        NuSoundWeakPtrListNode *stream_ptr = g_NuSoundStreams[stream_index];
        if (stream_ptr != NULL) {
            NuSoundStreamingSample *stream = stream_ptr->stream;
            if (stream != streaming_sample) {
                NuSound3StopStereoStream(stream_index);
                stream = stream_ptr->stream;
            }
            LoadState load_state = stream->GetLoadState();
            if ((load_state == 0) && stream_ptr->stream->GetResourceCount() == 0) {
                delete stream_ptr;
                g_NuSoundStreams[stream_index] = NULL;
            } else if (g_NuSoundStreams[stream_index] != (void *)0x0) {
                return 0;
            }
        }
        if (streaming_sample != NULL && streaming_sample->NuSoundSample::GetResourceCount() == 0) {
            NuSoundWeakPtrListNode *node = new NuSoundWeakPtrListNode();

            node->status = 0;
            g_NuSoundStreams[stream_index] = node;
            node->paused = false;
            node->stream = streaming_sample;
            node->ps2volume = (int)volume;
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
