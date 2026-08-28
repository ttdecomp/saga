// NuSoundAndroid — decompiled from libTTapp.so
// (nu2api.2013/nusound/android/nusound_android.cpp). The device-touching
// functions (Init/Shutdown/UpdateAudioDevice) are the hardware boundary and
// stay stubbed for now.

#include "nu2api/nusound/nusound_android.hpp"

#include "decomp.h"

#include "nu2api/nucore/nucore.hpp"
#include "nu2api/nucore/nuthread.h"

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

void NuSoundAndroid::ShutdownAudioDevice() {
}

void NuSoundAndroid::UpdateAudioDevice() {
    // The original only polls the application state here.
    NuApplicationState *state = NuCore::GetApplicationState();
    state->GetStatus();
}
