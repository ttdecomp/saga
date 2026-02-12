#pragma once

#include "nu2api.saga/nusound/nusound_system.hpp"

struct NuSoundAndroid : public NuSoundSystem {

    bool InitAudioDevice() override {
        LOG_WARN("NuSoundAndroid::InitAudioDevice is not implemented");
        return true;
    }
};

extern NuSoundAndroid NuSound;
