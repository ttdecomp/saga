#pragma once

struct NuSoundSource;
struct NuSoundBuffer;
struct SLPlayItf_;

struct NuVoiceAndroid {
    void ApplyHardwareVoiceMix();
    void CreateHardwareVoice();
    void DestroyHardwareVoice();
    void GetInterfaces();
    void GetPlaybackPositionSamples();
    NuVoiceAndroid(NuSoundSource *, bool);
    void OnPlayerEvent(u32);
    void PauseHardwareVoice();
    void PlayerCallback(SLPlayItf_ const *const *, void *, u32);
    void RealiseObject();
    void ResumeHardwareVoice();
    void StartHardwareVoice();
    void StopHardwareVoice();
    void SubmitBuffer(NuSoundBuffer *);
    void UpdateHardwareVoice(float);
    void UpdateQueue();
    void UpdateSamplePlaybackCount();
    void UpdateState();
    virtual ~NuVoiceAndroid();
};
