#pragma once

#include "nu2api/nusound/nusound_system.hpp"

struct NuSoundAndroid : public NuSoundSystem {

    bool InitAudioDevice() override {
        LOG_WARN("NuSoundAndroid::InitAudioDevice is not implemented");
        return true;
    }

    void AndroidNuSoundClockThread(void *);
    void CreateEffect(NuSoundEffect::EffectType);
    void CreateVoice(NuSoundSource *, bool);
    void IsValidBitRate(unsigned int);
    void IsValidSampleRate(unsigned int);
    void ReportErrorCode(unsigned int, char const *);
    void ShutdownAudioDevice();
    void UpdateAudioDevice();
};

extern NuSoundAndroid NuSound;
