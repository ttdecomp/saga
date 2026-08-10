#pragma once

#include "nu2api/nusound/nusound_system.hpp"

struct NuSoundAndroid : public NuSoundSystem {
    NuSoundEffect *CreateEffect(NuSoundEffect::EffectType type) override { return NULL; }

    bool InitAudioDevice() override {
        LOG_WARN("NuSoundAndroid::InitAudioDevice is not implemented");
        return true;
    }

    void ShutdownAudioDevice() override {}
    void UpdateAudioDevice() override {}
    NuSoundVoice *CreateVoice(NuSoundSource *source, bool flag) override { return NULL; }
};

extern NuSoundAndroid NuSound;
