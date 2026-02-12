#pragma once

#include "nu2api.saga/nucore/common.h"

#include "nu2api.saga/nusound/nusound_system.hpp"

class NuSoundBuffer {
    u64 size;
    i32 allocated;
    void *addr;
    void *address;
    u32 flags;
    NuSoundSystem::MemoryDiscipline memory_discipline;

    bool IsAllocated() const;
    void Free();

  public:
    i32 Allocate(u64 size, NuSoundSystem::MemoryDiscipline disc);
};
