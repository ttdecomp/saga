#pragma once

#include "nu2api/nusound/nusound_system.hpp"

// The Android NuSoundSystem: OpenSL ES device plumbing. InitAudioDevice /
// ShutdownAudioDevice / UpdateAudioDevice are the hardware boundary — on the
// Android target they stay as recorded; the HOST build replaces Init/Shutdown
// with the same call flow over the fake OpenSL object model in
// host-tests/nusound/opensl_host.cpp, which writes the mixed PCM to the real
// host device through SDL3. Every other function here is decompiled
// faithfully.
struct NuSoundAndroid : public NuSoundSystem {

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
