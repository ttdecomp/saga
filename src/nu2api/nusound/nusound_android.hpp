#pragma once

#include "nu2api/nusound/nusound_system.hpp"

// The Android NuSoundSystem: OpenSL ES device plumbing. InitAudioDevice /
// ShutdownAudioDevice / UpdateAudioDevice are the device boundary — they stay
// stubbed until the host device override lands; every other function here is
// decompiled faithfully.
struct NuSoundAndroid : public NuSoundSystem {

    bool InitAudioDevice() override {
        LOG_WARN("NuSoundAndroid::InitAudioDevice is not implemented");
        return true;
    }

    static i32 m_workerThreadCount;

    static void AndroidNuSoundClockThread(void *);
    void CreateEffect(NuSoundEffect::EffectType);
    NuSoundVoice *CreateVoice(NuSoundSource *, bool) override;
    static bool IsValidBitRate(u32 bits);
    static bool IsValidSampleRate(u32 rate_millis);
    static u32 ReportErrorCode(u32 error, const char *message);
    void ShutdownAudioDevice();
    void UpdateAudioDevice() override;
};

extern NuSoundAndroid NuSound;
