#pragma once

class NuSoundEffect;

class NuSoundBus {
    NuSoundBus *parent_bus;
    char name[128];

  public:
    NuSoundBus(const char *name, bool is_master);
    NuSoundBus(const char *name, NuSoundBus *parent);
    ~NuSoundBus();

    const char *GetName();

    void AddEffect(NuSoundEffect *);
    void RemoveEffect(NuSoundEffect *);
    void ApplyFinalMix(float *);
    void GetOutputMix(float *);
    void SetOutputMix(float);
    void SetOutputMix(float *);
    void SetOutputBus(NuSoundBus *);
};
