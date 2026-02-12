#pragma once

#include "nu2api.saga/nucore/common.h"

class NuSoundMemoryBuffer {
    void *address;
    u8 size_l;
    u8 size_m;
    u8 size_h;
    u8 flags;
    u32 field4_0x8;
    NuSoundMemoryBuffer *next;

  public:
    void SetNext(NuSoundMemoryBuffer *next);
    void SetSize(u32 size);
    void SetAddress(void *address);
};

class NuSoundMemoryManager {
    void *memory;
    u32 size;
    u32 align;
    u32 field3_0xc;
    u16 name_length;
    u16 name_length2;
    const char *name;
    NuSoundMemoryBuffer *sound_memory_buffer;
    void *memory2;
    u32 size2;
    u32 size3;
    u8 flags;

  public:
    u32 Init(const char *name, void *memory, u32 size, u32 align, u32 param_5);

    static NuSoundMemoryBuffer *PopFreeBuffer();

    void EnableDefragOnAlloc(bool value);

    NuSoundMemoryBuffer *Alloc(u32 size);
};
