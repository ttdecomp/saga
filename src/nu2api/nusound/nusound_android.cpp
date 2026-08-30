// NuSoundAndroid — decompiled from libTTapp.so
// (nu2api.2013/nusound/android/nusound_android.cpp).
//
// InitAudioDevice / ShutdownAudioDevice use the OpenSL ES ABI imported by the
// shipping binary.  The host supplies that same ABI from its SDL-backed
// adapter, so this reconstructed call flow is shared unchanged.

#include "nu2api/nusound/nusound_android.hpp"

#include "decomp.h"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/opensles_abi.hpp"

NuSoundAndroid NuSound;
i32 NuSoundAndroid::m_workerThreadCount = 0;

void NuSoundAndroid::AndroidNuSoundClockThread(void *) {
    // 5 ms tick driving the audio clock callbacks (the callback list is
    // empty in practice on the title screen).
    NuSoundAndroid *system = &NuSound;
    while (NuSoundAndroid::m_workerThreadCount != 0) {
        system->clock.HandleCallbacks();
        NuThreadSleep(5);
    }
}

// libTTapp.so 0x32b0c0: the android system registers the PCM voice factory
// for DataFormat::ZERO right after the base constructor.
NuSoundAndroid::NuSoundAndroid() : NuSoundSystem() {
    NuSoundVoiceFactoryAndroid_PCM *factory = new NuSoundVoiceFactoryAndroid_PCM();
    this->factory_list.RegisterFactory(factory, NuSoundStreamDesc::DataFormat::ZERO);
}

void NuSoundAndroid::CreateEffect(NuSoundEffect::EffectType) {
}

NuSoundVoice *NuSoundAndroid::CreateVoice(NuSoundSource *source, bool loop) {
    return NuSoundSystem::CreateVoice(source, loop);
}

bool NuSoundAndroid::IsValidBitRate(u32 bits) {
    // OpenSL PCM supports 8 / 16 / 24 bit containers.
    return bits == 8 || bits == 16 || bits == 24;
}

bool NuSoundAndroid::IsValidSampleRate(u32 rate_millis) {
    // OpenSL accepts the standard rates, expressed in milli Hertz.
    switch (rate_millis / 1000) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
            return true;
        default:
            return false;
    }
}

u32 NuSoundAndroid::ReportErrorCode(u32 error, const char *message) {
    if (error != 0) {
        LOG_WARN("NuSound error %u: %s", error, message);
    }
    return error;
}

namespace {
    // ObjectItf / SLEngineItf slot typedefs for the InitAudioDevice flow.
    typedef u32 (*ObjectRealizeFn)(void *, u32);
    typedef u32 (*ObjectGetInterfaceFn)(void *, const void *, void **);
    typedef u32 (*EngineCreateOutputMixFn)(void *, void **, u32, const void **, const u32 *);
    typedef u32 (*QuerySupportedProfilesFn)(void *, u16 *);
    typedef u32 (*QueryAvailableVoicesFn)(void *, u32, i16 *, u32 *, i16 *);
    typedef u32 (*VolumeSetVolumeLevelFn)(void *, i32);
    typedef u32 (*EnvironmentalReverbSetPropertiesFn)(void *, const void *);

#define SL_SLOT(itf, fn_type, byte_offset) (*(fn_type *)((char *)(*(void **)(itf)) + (byte_offset)))

} // namespace

// libTTapp.so 0x32b1d0: create the engine object, realize it, query the
// engine capabilities, fetch the engine interface, create + realize the
// output mix, record the closest supported config, then start the clock
// thread.
bool NuSoundAndroid::InitAudioDevice() {
    u32 engine_options[2] = {1, 1}; // SL_ENGINEOPTION_THREADSAFE = SL_BOOLEAN_TRUE
    u32 error = slCreateEngine(&this->engine_object, 1, engine_options, 0, NULL, NULL);
    if (ReportErrorCode(error, "slCreateEngine") != 0) {
        return false;
    }

    error = SL_SLOT(this->engine_object, ObjectRealizeFn, 0)(this->engine_object, 0);
    if (ReportErrorCode(error, "Realize the engine object") != 0) {
        return false;
    }

    void *capabilities = NULL;
    error = SL_SLOT(this->engine_object, ObjectGetInterfaceFn, 0xc)(this->engine_object, SL_IID_ENGINECAPABILITIES,
                                                                    &capabilities);
    if (ReportErrorCode(error, "Get the engine capabilities interface") == 0) {
        u16 profiles = 0;
        error = SL_SLOT(capabilities, QuerySupportedProfilesFn, 0)(capabilities, &profiles);
        if (ReportErrorCode(error, "QuerySupportedProfiles") == 0 && (profiles & 4) != 0) {
            i16 max_voices = 0;
            u32 absolute_max = 0;
            i16 free_voices = 0;
            error = SL_SLOT(capabilities, QueryAvailableVoicesFn, 4)(capabilities, 1, &max_voices, &absolute_max,
                                                                     &free_voices);
            ReportErrorCode(error, "QueryAvailableVoices (2D audio)");

            max_voices = 0;
            absolute_max = 0;
            free_voices = 0;
            error = SL_SLOT(capabilities, QueryAvailableVoicesFn, 4)(capabilities, 4, &max_voices, &absolute_max,
                                                                     &free_voices);
            ReportErrorCode(error, "QueryAvailableVoices (vibra)");
        }
    }

    error = SL_SLOT(this->engine_object, ObjectGetInterfaceFn, 0xc)(this->engine_object, SL_IID_ENGINE,
                                                                    &this->audio_engine);
    if (ReportErrorCode(error, "Get the engine interface") != 0) {
        return false;
    }

    const void *mix_iids[2] = {SL_IID_VOLUME, SL_IID_ENVIRONMENTALREVERB};
    const u32 mix_required[2] = {0, 0};
    error = SL_SLOT(this->audio_engine, EngineCreateOutputMixFn, 0x1c)(this->audio_engine, &this->output_mix, 2,
                                                                       mix_iids, mix_required);
    if (ReportErrorCode(error, "Create the output mix object") != 0) {
        return false;
    }

    error = SL_SLOT(this->output_mix, ObjectRealizeFn, 0)(this->output_mix, 0);
    if (ReportErrorCode(error, "Realize the output mix object") != 0) {
        return false;
    }

    error = SL_SLOT(this->output_mix, ObjectGetInterfaceFn, 0xc)(this->output_mix, SL_IID_VOLUME, &this->mix_volume);
    if (ReportErrorCode(error, "Get the output mix volume interface") == 0) {
        error = SL_SLOT(this->mix_volume, VolumeSetVolumeLevelFn, 0xc)(this->mix_volume, 0);
        ReportErrorCode(error, "Set the output mix volume");
    }

    error = SL_SLOT(this->output_mix, ObjectGetInterfaceFn, 0xc)(this->output_mix, SL_IID_ENVIRONMENTALREVERB,
                                                                 &this->mix_reverb);
    if (ReportErrorCode(error, "Get the environmental reverb interface") == 0) {
        error = SL_SLOT(this->mix_reverb, EnvironmentalReverbSetPropertiesFn, 0x50)(this->mix_reverb,
                                                                                    this->reverb_properties);
        ReportErrorCode(error, "Set the environmental reverb properties");
    }

    NuSoundSystem::sOutputConfig = this->GetClosestSupportedConfig(2);
    NuSoundSystem::sNumAvailableOutputDevices = 1;

    if (++NuSoundAndroid::m_workerThreadCount == 1) {
        NuCore::m_threadManager->CreateThread(AndroidNuSoundClockThread, this, 2, "AndroidNuSoundClock", 0,
                                              NUTHREADCAFECORE_UNKNOWN_1, NUTHREADXBOX360CORE_UNKNOWN_1);
    }

    return true;
}

void NuSoundAndroid::ShutdownAudioDevice() {
    // libTTapp.so 0x32aec0 only drops the worker count. Object ownership is
    // released by the surrounding NuSound shutdown path.
    if (NuSoundAndroid::m_workerThreadCount > 0) {
        NuSoundAndroid::m_workerThreadCount--;
    }
}

void NuSoundAndroid::UpdateAudioDevice() {
    // The original only polls the application state here.
    NuApplicationState *state = NuCore::GetApplicationState();
    state->GetStatus();
}
