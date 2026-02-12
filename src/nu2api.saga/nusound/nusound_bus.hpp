#pragma once

class NuSoundBus {
    NuSoundBus *parent_bus;
    char name[128];

  public:
    NuSoundBus(const char *name, bool is_master);

    const char *GetName();
};
