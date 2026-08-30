#pragma once

#include "nu2api/nusound/nusound_system.hpp"

// The Android NuSoundSystem: shared OpenSL ES device plumbing. The host exports
// the same imported C ABI from its SDL-backed platform adapter.
struct NuSoundAndroid : public NuSoundSystem {

    void *mix_volume;
    void *mix_reverb;
    u32 reverb_properties[7];

    NuSoundAndroid();
    bool InitAudioDevice() override;

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
