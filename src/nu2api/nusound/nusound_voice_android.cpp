// NuVoiceAndroid — decompiled from libTTapp.so
// (nu2api.2013/nusound/android/nusound_android.cpp).
//
// Platform voice on top of NuSoundVoice: one OpenSL ES AudioPlayer per voice
// with an ANDROIDSIMPLEBUFFERQUEUE (the device write is the Enqueue call in
// SubmitBuffer) and a VOLUME interface for gain / stereo position. The
// original drove OpenSL through its interface vtables; the exact slots
// libTTapp.so used are reproduced below so the transcription stays faithful.

#include "nu2api_nusound_types.h"

#include "decomp.h"

#include <new>

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nusound/nusound_android.hpp"
#include "nu2api/nusound/nusound_streamer.hpp"

#include <math.h>

namespace {

    // The OpenSL interface ids are opaque tokens here; they are only handed to
    // GetInterface / CreateAudioPlayer, never dereferenced. (Kept internal so the
    // binary surface stays identical to the original's.)
    const void *SL_IID_PLAY = (const void *)0x00010001;
    const void *SL_IID_ANDROIDSIMPLEBUFFERQUEUE = (const void *)0x00010002;
    const void *SL_IID_VOLUME = (const void *)0x00010003;
    const void *SL_IID_ENGINE = (const void *)0x00010004;
    const void *SL_IID_ENGINECAPABILITIES = (const void *)0x00010005;
    const void *SL_IID_ENVIRONMENTALREVERB = (const void *)0x00010006;

    // OpenSL interface vtable slots as used by libTTapp.so.
    typedef u32 (*ObjectRealizeFn)(void *, u32);
    typedef u32 (*ObjectResumeFn)(void *);
    typedef u32 (*ObjectGetStateFn)(void *, u32 *);
    typedef u32 (*ObjectGetInterfaceFn)(void *, const void *, void **);
    typedef u32 (*ObjectDestroyFn)(void *);
    typedef u32 (*EngineCreateAudioPlayerFn)(void *, void **, void *, void *, u32, const void **, const u32 *);
    typedef u32 (*PlaySetPlayStateFn)(void *, u32);
    typedef u32 (*PlayGetPlayStateFn)(void *, u32 *);
    typedef u32 (*PlayGetPositionFn)(void *, u32 *);
    typedef u32 (*PlayRegisterCallbackFn)(void *, void (*)(const SLPlayItf_ *const *, void *, u32), void *);
    typedef u32 (*PlaySetCallbackEventsMaskFn)(void *, u32);
    typedef u32 (*QueueEnqueueFn)(void *, void *, u32);
    typedef u32 (*QueueClearFn)(void *);
    typedef u32 (*QueueGetStateFn)(void *, u32 *);
    typedef u32 (*VolumeSetVolumeLevelFn)(void *, i32);
    typedef u32 (*VolumeEnableStereoPositionFn)(void *, u32);
    typedef u32 (*VolumeSetStereoPositionFn)(void *, i32);

#define SL_SLOT(itf, fn_type, byte_offset) (*(fn_type *)((char *)(*(void **)(itf)) + (byte_offset)))

} // namespace

// ---------------------------------------------------------------------------
// construction / destruction
// ---------------------------------------------------------------------------

NuVoiceAndroid::NuVoiceAndroid(NuSoundSource *sound_source, bool loop) : NuSoundVoice(sound_source, loop) {
    this->player_object = NULL;
    this->play_interface = NULL;
    this->queue_interface = NULL;
    this->field4_0x158 = NULL;
    this->volume_interface = NULL;

    pthread_mutex_init(&this->mutex, NULL);

    this->field7_0x164 = 0;
    this->field8_0x168 = 0;
    this->field9_0x16c = 0;
    this->field10_0x170 = 0;
    this->field11_0x174 = 0;
    this->field12_0x178 = 0;

    this->last_volume_level = -0x8000; // muted until the first mix arrives
    this->hardware_flags = 0;

    // libTTapp.so ctor tail (0x32c1ae): the platform voice builds its OpenSL
    // player immediately, before any Play.
    this->CreateHardwareVoice();
}

NuVoiceAndroid::~NuVoiceAndroid() {
    this->DestroyHardwareVoice();
    pthread_mutex_destroy(&this->mutex);
}

// ---------------------------------------------------------------------------
// the device write
// ---------------------------------------------------------------------------

void NuVoiceAndroid::SubmitBuffer(NuSoundBuffer *buffer) {
    if (buffer == NULL || this->queue_interface == NULL || *(void **)this->queue_interface == NULL) {
        return;
    }

    u32 max_buffer_size = this->sound_source->GetMaxBufferSize();
    NuSoundBuffer::Context &context = buffer->GetCurrentContext();
    // libTTapp.so 0x32bbbe: the enqueue length is the buffer context's
    // read_size (the decoded/loaded byte count).
    u32 size = (u32)context.read_size;

    // The original formatted a debug line (source name + block count) that was
    // never printed; it has no observable effect and is omitted here.

    pthread_mutex_lock(&this->mutex);

    if (size != 0) {
        u32 error =
            SL_SLOT(this->queue_interface, QueueEnqueueFn, 0)(this->queue_interface, buffer->GetAddress(), size);
        NuSoundAndroid::ReportErrorCode(error, "Enqueue buffer");
        this->queued_buffers++;
    }

    if (size < max_buffer_size) {
        this->flags2 |= 2; // 0x32bd0e
        if (size == 0) {
            this->hardware_flags |= 2; // 0x32bd16
        }
    }

    pthread_mutex_unlock(&this->mutex);
}

// ---------------------------------------------------------------------------
// device lifecycle
// ---------------------------------------------------------------------------

void NuVoiceAndroid::CreateHardwareVoice() {
    NuSoundSource *source = this->sound_source;
    if (source == NULL) {
        return;
    }
    NuSoundStreamDesc *desc = source->GetStreamDesc();
    if (desc == NULL) {
        return;
    }

    u32 channels = (u32)desc->GetNumChannels();
    if (channels != 1 && channels != 2) {
        return;
    }
    // OpenSL speaker mask: front-centre for mono, front-left|front-right for
    // stereo.
    u32 speaker_mask = (channels == 1) ? 4 : 3;

    u32 rate_millis = (u32)desc->GetSampleRate() * 1000;
    if (NuSoundAndroid::IsValidSampleRate(rate_millis) == false) {
        return;
    }

    u32 bits = (u32)desc->GetBitsPerChannel();
    if (NuSoundAndroid::IsValidBitRate(bits) == false) {
        return;
    }

    // SLDataLocator_AndroidSimpleBufferQueue { locator type, numBuffers = 2 }.
    u32 locator[2] = {0x800007bd, 2};
    // SLDataFormat_PCM { format type, channels, rate (milliHz), bits,
    // container bits, channel mask, little endian }.
    u32 pcm_format[7] = {2, channels, rate_millis, bits, bits, speaker_mask, 2};
    void *audio_src[2] = {locator, pcm_format};

    // Output mix sink.
    NuSoundSystem *system = NuSoundSystem::GetInstance();
    void *mix_locator[2] = {(void *)4, system->output_mix};
    void *audio_sink[2] = {mix_locator, NULL};

    const void *iids[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME};
    const u32 required[2] = {1, 1};

    void *engine_itf = system->audio_engine;
    u32 error = SL_SLOT(engine_itf, EngineCreateAudioPlayerFn, 8)(engine_itf, &this->player_object, audio_src,
                                                                  audio_sink, 2, iids, required);
    if (NuSoundAndroid::ReportErrorCode(error, "Create audio player") != 0) {
        return;
    }

    if (this->RealiseObject() == false) {
        return;
    }

    this->GetInterfaces();
}

bool NuVoiceAndroid::RealiseObject() {
    if (this->player_object == NULL || *(void **)this->player_object == NULL) {
        return false;
    }
    u32 error = SL_SLOT(this->player_object, ObjectRealizeFn, 0)(this->player_object, 0);
    return NuSoundAndroid::ReportErrorCode(error, "Realize player object") == 0;
}

bool NuVoiceAndroid::GetInterfaces() {
    u32 error = SL_SLOT(this->player_object, ObjectGetInterfaceFn, 0xc)(this->player_object, SL_IID_PLAY,
                                                                        &this->play_interface);
    if (NuSoundAndroid::ReportErrorCode(error, "Get the play interface") != 0) {
        return false;
    }

    error = SL_SLOT(this->player_object, ObjectGetInterfaceFn,
                    0xc)(this->player_object, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &this->queue_interface);
    if (NuSoundAndroid::ReportErrorCode(error, "Get the buffer queue interface") != 0) {
        return false;
    }

    error = SL_SLOT(this->player_object, ObjectGetInterfaceFn, 0xc)(this->player_object, SL_IID_VOLUME,
                                                                    &this->volume_interface);
    if (NuSoundAndroid::ReportErrorCode(error, "Get the volume interface") != 0) {
        return false;
    }

    error = SL_SLOT(this->play_interface, PlayRegisterCallbackFn, 0x10)(this->play_interface,
                                                                        NuVoiceAndroid::PlayerCallback, this);
    if (NuSoundAndroid::ReportErrorCode(error, "register callback on the play interface") != 0) {
        return false;
    }

    error = SL_SLOT(this->play_interface, PlaySetCallbackEventsMaskFn, 0x14)(this->play_interface, 0x1f);
    return NuSoundAndroid::ReportErrorCode(error, "set callback events mask on the play interface") == 0;
}

void NuVoiceAndroid::StartHardwareVoice() {
    if (this->play_interface == NULL || *(void **)this->play_interface == NULL) {
        return;
    }

    // libTTapp.so 0x32b6d4: starting a hardware voice resets all of the
    // platform play-position tracking before arming the deferred start.
    this->last_volume_level = -1;
    this->field7_0x164 = 0;
    this->field8_0x168 = 0;
    this->field9_0x16c = 0;
    this->field10_0x170 = 0;
    this->field11_0x174 = 0;
    this->field12_0x178 = 0;
    this->hardware_flags |= 1;
}

void NuVoiceAndroid::StopHardwareVoice() {
    if (this->play_interface == NULL || *(void **)this->play_interface == NULL) {
        return;
    }

    u32 error = SL_SLOT(this->play_interface, PlaySetPlayStateFn, 4)(this->play_interface, 1);
    NuSoundAndroid::ReportErrorCode(error, "Set the player's state to stopped");

    error = SL_SLOT(this->queue_interface, QueueClearFn, 4)(this->queue_interface);
    NuSoundAndroid::ReportErrorCode(error, "Cleared the buffer queue");

    this->flags2 &= 0xfd;
}

void NuVoiceAndroid::PauseHardwareVoice() {
    if (this->play_interface == NULL || *(void **)this->play_interface == NULL) {
        return;
    }

    u32 error = SL_SLOT(this->play_interface, PlaySetPlayStateFn, 4)(this->play_interface, 2);
    NuSoundAndroid::ReportErrorCode(error, "Set the player's state to paused");
}

void NuVoiceAndroid::ResumeHardwareVoice() {
    if (this->play_interface == NULL || *(void **)this->play_interface == NULL) {
        return;
    }

    u32 error = SL_SLOT(this->play_interface, PlaySetPlayStateFn, 4)(this->play_interface, 3);
    NuSoundAndroid::ReportErrorCode(error, "Set the player's state to playing (resume)");
}

void NuVoiceAndroid::DestroyHardwareVoice() {
    if (this->player_object == NULL) {
        return;
    }

    SL_SLOT(this->player_object, ObjectDestroyFn, 0x18)(this->player_object);

    this->player_object = NULL;
    this->play_interface = NULL;
    this->queue_interface = NULL;
    this->field4_0x158 = NULL;
    this->volume_interface = NULL;
}

// ---------------------------------------------------------------------------
// per-frame device state
// ---------------------------------------------------------------------------

bool NuVoiceAndroid::UpdateState() {
    // libTTapp.so 0x32c1ea: the poll reads the PLAYER OBJECT's state (object
    // vtable slot 0x8, SLObjectItf::GetState) — voice+0x14c is the object,
    // the interfaces hang off it (see GetInterfaces).
    if (this->player_object == NULL || *(void **)this->player_object == NULL) {
        return false;
    }

    u32 state = 2; // SL_OBJECT_STATE_SUSPENDED default
    u32 error = SL_SLOT(this->player_object, ObjectGetStateFn, 8)(this->player_object, &state);
    if (NuSoundAndroid::ReportErrorCode(error, "Get the object state") != 0) {
        return false;
    }

    if (state != 1) {
        if (state != 3) {
            return true;
        }
        // 0x32c260: object vtable slot 0x4 (SLObjectItf::Resume).
        error = SL_SLOT(this->player_object, ObjectResumeFn, 4)(this->player_object);
        return NuSoundAndroid::ReportErrorCode(error, "resume the player object") == 0;
    }

    // Realized: a looping voice re-realizes and restarts per the voice state;
    // a non-looping voice is finished.
    if ((this->flags2 & 8) != 0) {
        if (this->RealiseObject() == false) {
            return false;
        }
        // This apparently inverted test is what the original executes
        // (libTTapp.so 0x32c2a4: test al; jne 0x32c250). GetInterfaces returns
        // true on success, so the priming Update performed by Play returns
        // false here. Since the logical voice is still STOPPED, the caller's
        // Stop(true) is a no-op and the initial queued buffers remain intact.
        if (this->GetInterfaces()) {
            return false;
        }

        NuSoundVoice::PlayState voice_state = this->GetState();
        if (voice_state == PLAYSTATE_STOPPED) {
            this->StopHardwareVoice();
            return true;
        }
        if (voice_state == PLAYSTATE_PAUSED) {
            this->StartHardwareVoice();
            this->PauseHardwareVoice();
            return true;
        }
        this->StartHardwareVoice();
        return true;
    }

    return false;
}

void NuVoiceAndroid::UpdateQueue() {
    if (this->queue_interface == NULL || *(void **)this->queue_interface == NULL) {
        return;
    }

    u32 count = 0;
    u32 error = SL_SLOT(this->queue_interface, QueueGetStateFn, 8)(this->queue_interface, &count);
    if (NuSoundAndroid::ReportErrorCode(error, "Get queue state") != 0) {
        return;
    }

    if (this->sound_source->feed_type == NuSoundSource::FeedType::STREAMING && (this->flags2 & 2) == 0) {
        // Starvation watchdog: remember whether the queue ever ran ahead, and
        // request a refill as soon as it runs low.
        // libTTapp.so 0x32c37e..0x32c3ad reads and writes voice+0x17e,
        // NuVoiceAndroid::hardware_flags. Using NuSoundVoice::flags (+0x31)
        // left bit 4 invisible to UpdateHardwareVoice and delayed every refill
        // until HEADATEND.
        if ((this->hardware_flags & 8) == 0) {
            if (count > 1) {
                this->hardware_flags |= 8;
            }
        } else if (count < 2) {
            this->hardware_flags &= 0xf7;
            this->hardware_flags |= 4;
        }
    }
}

void NuVoiceAndroid::UpdateHardwareVoice(f32 frametime) {
    (void)frametime;

    if (this->UpdateState() == false) {
        this->Stop(true);
        return;
    }

    this->UpdateQueue();

    u8 flags = this->hardware_flags;
    if ((flags & 1) == 0) {
        if ((flags & 2) != 0) {
            this->Stop(true);

            u32 state = 3;
            u32 error = SL_SLOT(this->play_interface, PlayGetPlayStateFn, 8)(this->play_interface, &state);
            NuSoundAndroid::ReportErrorCode(error, "Get the player state");
            if (state == 1) {
                this->hardware_flags &= 0xfd;
            }
        }
    } else {
        u32 error = SL_SLOT(this->play_interface, PlaySetPlayStateFn, 4)(this->play_interface, 3);
        u32 reported = NuSoundAndroid::ReportErrorCode(error, "Set the player's state to playing");
        if (reported == 0) {
            this->hardware_flags &= 0xfe;
        }
    }

    if ((this->hardware_flags & 4) != 0) {
        // The queue drained: ask the source for the next buffer (streaming
        // fills are asynchronous through the streamer, the voice is the
        // callback).
        NuSoundWeakPtr<NuSoundBufferCallback> callback;
        callback.Set(this);
        this->sound_source->RequestBuffer((this->flags2 >> 3) & 1, callback);
        this->hardware_flags &= 0xfb;
    }

    this->UpdateSamplePlaybackCount();
}

void NuVoiceAndroid::ApplyHardwareVoiceMix() {
    if (this->volume_interface == NULL || *(void **)this->volume_interface == NULL) {
        return;
    }

    f32 gain = this->field67_0xa8;
    i16 level = -0x8000;
    if (gain >= 0.01f) {
        level = (i16)(i32)(log10((f64)gain) * 2000.0);
    }

    if (level != this->last_volume_level) {
        u32 error = SL_SLOT(this->volume_interface, VolumeSetVolumeLevelFn, 0xc)(this->volume_interface, level);
        NuSoundAndroid::ReportErrorCode(error, "Volume SetVolumeLevel");
        this->last_volume_level = level;
    }

    NuSoundStreamDesc *desc = this->sound_source->GetStreamDesc();
    u32 channels = (u32)desc->GetNumChannels();
    if (channels == 1) {
        // Mono sources are panned through the stereo position interface from
        // the eight positional gains.
        f32 stereo_gains[64] = {0};
        NuSoundMixer mixer((NuSoundSystem::ChannelConfig){1}, (NuSoundSystem::ChannelConfig){2},
                           (NuSoundMixer::OutputLayout){1}, *(NuSoundSystem::DownmixType *)this->field16_0x3c,
                           this->field15_0x38);
        mixer.Mix(this->mix_gains, stereo_gains);

        u32 error = SL_SLOT(this->volume_interface, VolumeEnableStereoPositionFn, 0x14)(this->volume_interface, 1);
        NuSoundAndroid::ReportErrorCode(error, "Volume EnableStereoPosition(true)");

        error = SL_SLOT(this->volume_interface, VolumeSetStereoPositionFn, 0x1c)(this->volume_interface,
                                                                                 (i32)(stereo_gains[0] * 1000.0f));
        NuSoundAndroid::ReportErrorCode(error, "Volume SetStereoPosition");
    } else {
        u32 error = SL_SLOT(this->volume_interface, VolumeEnableStereoPositionFn, 0x14)(this->volume_interface, 0);
        NuSoundAndroid::ReportErrorCode(error, "Volume EnableStereoPosition(false)");
    }
}

// ---------------------------------------------------------------------------
// async events
// ---------------------------------------------------------------------------

void NuVoiceAndroid::OnPlayerEvent(u32 event) {
    if ((event & 1) == 0) {
        return;
    }

    pthread_mutex_lock(&this->mutex);

    bool finished;
    if (this->sound_source->feed_type == NuSoundSource::FeedType::STREAMING) {
        // Streaming: a looping stream always refills; a non-looping one stops
        // refilling once its last buffer has been queued.
        finished = (this->flags2 & 8) == 0 && (this->flags2 & 2) != 0;
    } else {
        finished = (this->flags2 & 8) == 0;
    }

    if (finished) {
        this->hardware_flags |= 2; // 0x32c8c3
    } else {
        this->hardware_flags |= 4; // 0x32c900
    }

    pthread_mutex_unlock(&this->mutex);
}

void NuVoiceAndroid::PlayerCallback(const SLPlayItf_ *const *player, void *context, u32 event) {
    (void)player;
    if (context != NULL) {
        ((NuVoiceAndroid *)context)->OnPlayerEvent(event);
    }
}

// ---------------------------------------------------------------------------
// playback position
// ---------------------------------------------------------------------------

void NuVoiceAndroid::UpdateSamplePlaybackCount() {
    if (this->play_interface == NULL || *(void **)this->play_interface == NULL) {
        return;
    }

    u32 millisec = 0;
    u32 error = SL_SLOT(this->play_interface, PlayGetPositionFn, 0xc)(this->play_interface, &millisec);
    if (NuSoundAndroid::ReportErrorCode(error, "Player GetPosition") != 0) {
        return;
    }

    NuSoundSource *source = this->sound_source;
    NuSoundStreamDesc *desc = source->GetStreamDesc();
    u32 rate = (u32)desc->GetSampleRate();
    u32 position = (rate / 1000) * millisec;

    if (source->feed_type != NuSoundSource::FeedType::STREAMING) {
        this->field11_0x174 = (i32)position;
        this->field12_0x178 = 0;
        return;
    }

    // Streaming sources report a block-wrapped position: the device position
    // counts inside the two stream buffers, the wrap counters turn it into an
    // absolute sample count.
    u32 max_buffer_size = source->GetMaxBufferSize();
    u32 block_size = (u32)desc->GetBlockSize();
    u64 block_samples = (u64)max_buffer_size / (u64)block_size;

    u64 prev = ((u64)(u32)this->field8_0x168 << 32) | (u32)this->field7_0x164;
    if (position < (u32)(prev >> 32) || (position == (u32)(prev >> 32) && position < (u32)prev)) {
        u32 wrap_lo = this->field9_0x16c + 1;
        this->field10_0x170 += (u32)(this->field9_0x16c + 1 < wrap_lo ? 0 : 1);
        this->field9_0x16c = wrap_lo;
    }
    this->field7_0x164 = (i32)position;
    this->field8_0x168 = (i32)((u64)position >> 32);

    u64 wrap = ((u64)(u32)this->field10_0x170 << 32) | (u32)this->field9_0x16c;
    u64 estimate = (u64)(position % (u32)block_samples) + block_samples * wrap;

    u64 samples;
    if ((estimate >> 32) != 0 || position < (u32)estimate) {
        samples = estimate;
    } else {
        samples = (u64)position;
    }

    this->field11_0x174 = (i32)samples;
    this->field12_0x178 = (i32)(samples >> 32);
}

u64 NuVoiceAndroid::GetPlaybackPositionSamples() {
    return ((u64)(u32)this->field12_0x178 << 32) | (u32)this->field11_0x174;
}

// ---------------------------------------------------------------------------
// voice factory
// ---------------------------------------------------------------------------

NuSoundVoice *NuSoundVoiceFactoryAndroid_PCM::CreateVoice(NuSoundSource *source, bool loop) {
    NuVoiceAndroid *voice = (NuVoiceAndroid *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, 0x180, 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/android/nusound_android.cpp:292");
    if (voice != NULL) {
        new (voice) NuVoiceAndroid(source, loop);
    }
    return voice;
}

NuSoundVoiceFactoryList::NuSoundVoiceFactoryList() {
    for (u32 i = 0; i < 16; i++) {
        this->factories[i] = NULL;
    }
}

void NuSoundVoiceFactoryList::RegisterFactory(NuSoundVoiceFactory *factory, NuSoundStreamDesc::DataFormat format) {
    this->factories[(u32)format] = factory;
}

NuSoundVoiceFactory *NuSoundVoiceFactoryList::GetFactory(NuSoundStreamDesc::DataFormat format) {
    return this->factories[(u32)format];
}
