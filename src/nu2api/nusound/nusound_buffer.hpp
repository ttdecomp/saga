#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nusound/nusound_system.hpp"

class NuSoundBuffer {
  public:
    struct Context {
        u64 read_size;
        u64 size2;
        u64 size3;
        i32 field5_0x18;
        i32 flags;
        i32 field5_0x20;

        Context() : read_size(0), size2(0), size3(0), field5_0x18(0), flags(1), field5_0x20(0) {
        }
    };

  private:
    u64 size;
    void *address;
    NuSoundMemoryBuffer *memory_buffer;
    i32 lock_count;
    i32 allocated;
    u32 flags;
    NuSoundSystem::MemoryDiscipline memory_discipline;
    Context context;

  public:
    NuSoundBuffer();
    NuSoundBuffer(char *name, u64 size);

    void Free();

    i32 Provide(char *address, u64 size);

    i32 Allocate(u64 size, NuSoundSystem::MemoryDiscipline disc);

    void Lock();
    void Unlock();

    void SetCurrentContext(Context &context);

    void *GetAddress() const;

    bool IsAllocated() const;

    u64 GetBufferSize() const;

    bool IsLocked() const;
};
