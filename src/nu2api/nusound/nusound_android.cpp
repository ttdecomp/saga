// NuSoundAndroid — decompiled from libTTapp.so
// (nu2api.2013/nusound/android/nusound_android.cpp).
//
// InitAudioDevice / ShutdownAudioDevice are the hardware boundary. On the
// Android target they stay as recorded (the device itself is not part of the
// decompilation scope); the HOST build (HOST_BUILD) runs the same call flow
// against the fake OpenSL object model in host-utils/nusound/opensl_host.cpp,
// which writes the queued PCM to the real host audio device through SDL3.

#include "nu2api/nusound/nusound_android.hpp"

#include "decomp.h"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nuthread.h"

#ifdef HOST_BUILD
#include "../../host-utils/nusound/opensl_host.hpp"
#endif

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

#ifdef HOST_BUILD

namespace {

    // OpenSL interface id tokens — must match the anonymous-namespace
    // constants in nusound_voice_android.cpp.
    const void *SL_IID_ENGINE = (const void *)0x00010004;
    const void *SL_IID_ENGINECAPABILITIES = (const void *)0x00010005;
    const void *SL_IID_VOLUME = (const void *)0x00010003;
    const void *SL_IID_ENVIRONMENTALREVERB = (const void *)0x00010006;

    // ObjectItf / SLEngineItf slot typedefs for the InitAudioDevice flow.
    typedef u32 (*ObjectRealizeFn)(void *, u32);
    typedef u32 (*ObjectGetInterfaceFn)(void *, const void *, void **);
    typedef u32 (*ObjectDestroyFn)(void *);
    typedef u32 (*EngineCreateOutputMixFn)(void *, void **, u32, const void **, const u32 *);
    typedef u32 (*QuerySupportedProfilesFn)(void *, u16 *);

#define SL_SLOT(itf, fn_type, byte_offset) (*(fn_type *)((char *)(*(void **)(itf)) + (byte_offset)))

} // namespace

// libTTapp.so 0x32b1d0: create the engine object, realize it, query the
// engine capabilities, fetch the engine interface, create + realize the
// output mix, record the closest supported config, then start the clock
// thread. The fake OpenSL objects (opensl_host.cpp) stand in for the device.
bool NuSoundAndroid::InitAudioDevice() {
    u32 engine_options[2] = {1, 1}; // SL_ENGINEOPTION_THREADSAFE = SL_BOOLEAN_TRUE
    u32 error = hostsl::HostCreateEngine(&this->engine_object, 1, engine_options, 0, NULL, NULL);
    if (ReportErrorCode(error, "HostCreateEngine") != 0) {
        return false;
    }

    error = SL_SLOT(this->engine_object, ObjectRealizeFn, 0)(this->engine_object, 0);
    if (ReportErrorCode(error, "Realize the engine object") != 0) {
        return false;
    }

    // Engine capabilities: the fake advertises no optional profiles, so the
    // original's gated output queries are skipped (bare-device path).
    void *capabilities = NULL;
    error = SL_SLOT(this->engine_object, ObjectGetInterfaceFn, 0xc)(this->engine_object, SL_IID_ENGINECAPABILITIES,
                                                                    &capabilities);
    if (ReportErrorCode(error, "Get the engine capabilities interface") == 0) {
        u16 profiles = 0;
        error = SL_SLOT(capabilities, QuerySupportedProfilesFn, 0)(capabilities, &profiles);
        if (ReportErrorCode(error, "QuerySupportedProfiles") == 0 && (profiles & 4) != 0) {
            LOG_WARN("NuSound host: optional output queries not implemented (profiles=%u)", profiles);
        }
    }

    error = SL_SLOT(this->engine_object, ObjectGetInterfaceFn, 0xc)(this->engine_object, SL_IID_ENGINE,
                                                                    &this->audio_engine);
    if (ReportErrorCode(error, "Get the engine interface") != 0) {
        return false;
    }

    error = SL_SLOT(this->audio_engine, EngineCreateOutputMixFn, 0x1c)(this->audio_engine, &this->output_mix, 0, NULL,
                                                                       NULL);
    if (ReportErrorCode(error, "Create the output mix object") != 0) {
        return false;
    }

    error = SL_SLOT(this->output_mix, ObjectRealizeFn, 0)(this->output_mix, 0);
    if (ReportErrorCode(error, "Realize the output mix object") != 0) {
        return false;
    }

    // The original also fetches two output-mix interfaces (volume /
    // environmental reverb) and tolerates both failing; nothing in the
    // transcribed scope reads them, and the fake mix offers none.
    void *mix_volume = NULL;
    void *mix_reverb = NULL;
    SL_SLOT(this->output_mix, ObjectGetInterfaceFn, 0xc)(this->output_mix, SL_IID_VOLUME, &mix_volume);
    SL_SLOT(this->output_mix, ObjectGetInterfaceFn, 0xc)(this->output_mix, SL_IID_ENVIRONMENTALREVERB, &mix_reverb);

    NuSoundSystem::sOutputConfig = this->GetClosestSupportedConfig(2);
    NuSoundSystem::sNumAvailableOutputDevices = 1;

    if (++NuSoundAndroid::m_workerThreadCount == 1) {
        NuCore::m_threadManager->CreateThread(AndroidNuSoundClockThread, this, 2, "AndroidNuSoundClock", 0,
                                              NUTHREADCAFECORE_UNKNOWN_1, NUTHREADXBOX360CORE_UNKNOWN_1);
    }

    return true;
}

void NuSoundAndroid::ShutdownAudioDevice() {
    // libTTapp.so 0x32aec0 drops the worker count and releases the device
    // objects (destroy output mix, then the engine object).
    if (NuSoundAndroid::m_workerThreadCount > 0) {
        NuSoundAndroid::m_workerThreadCount--;
    }
    if (this->output_mix != NULL) {
        SL_SLOT(this->output_mix, ObjectDestroyFn, 0x18)(this->output_mix);
        this->output_mix = NULL;
    }
    if (this->engine_object != NULL) {
        SL_SLOT(this->engine_object, ObjectDestroyFn, 0x18)(this->engine_object);
        this->engine_object = NULL;
    }
}

#else

bool NuSoundAndroid::InitAudioDevice() {
    LOG_WARN("NuSoundAndroid::InitAudioDevice is not implemented");
    return true;
}

void NuSoundAndroid::ShutdownAudioDevice() {
}

#endif

void NuSoundAndroid::UpdateAudioDevice() {
    // The original only polls the application state here.
    NuApplicationState *state = NuCore::GetApplicationState();
    state->GetStatus();
}
