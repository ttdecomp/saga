#pragma once

#include "nu2api/nucore/common.h"

#include <pthread.h>

class NuSoundMemoryBuffer {
    void *address;
    u8 size_l;
    u8 size_m;
    u8 size_h;
    u8 flags;
    u32 field4_0x8;
    NuSoundMemoryBuffer *next;

  private:
    static pthread_mutex_t s_cs;

    void BeginCriticalSection();
    void EndCriticalSection();

  public:
    NuSoundMemoryBuffer();
    ~NuSoundMemoryBuffer();

    void SetNext(NuSoundMemoryBuffer *next);
    void SetSize(u32 size);
    void SetAddress(void *address);
    void SetAlloced(bool);
    void SetPrev(NuSoundMemoryBuffer *);
    void *Lock(const char *name);
    void Unlock();

    void GetAddress();
    void GetLockReason();
    void GetNext();
    void GetPrev();
    void GetSize();
    bool IsAlloced();
    bool IsLocked();
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
    NuSoundMemoryManager();
    ~NuSoundMemoryManager();

    u32 Init(const char *name, void *memory, u32 size, u32 align, u32 param_5);

    static NuSoundMemoryBuffer *PopFreeBuffer();

    void EnableDefragOnAlloc(bool value);

    NuSoundMemoryBuffer *Alloc(u32 size);

    void AllocAddress(unsigned int);
    void CheckAndMergeFreeBufferNext(NuSoundMemoryBuffer *);
    void CheckAndMergeFreeBufferPrev(NuSoundMemoryBuffer *);
    void CheckList();
    void CountAdjacentFreeBuffers(NuSoundMemoryBuffer *);
    void Defragment(unsigned int);
    void EnableDebug(bool);
    void EnableDefragOnFree(bool);
    void Free(NuSoundMemoryBuffer *);
    void FreeAddress(void *);
    void GetFree();
    void GetSize();
    void GetUsed();
    void MergeFreeBuffer(NuSoundMemoryBuffer *);
    void MoveLargestTrailingBufferIntoBuffer(NuSoundMemoryBuffer *, NuSoundMemoryBuffer **, NuSoundMemoryBuffer **);
    void OutputList();
    void OutputMap();
    void PushFreeBuffer(NuSoundMemoryBuffer *);
    void Release();
    void RenderMap(float, float, float);
    void SplitFreeBuffer(NuSoundMemoryBuffer *, unsigned int, NuSoundMemoryBuffer **);
    void SwapOrMergeAdjacentBuffers(NuSoundMemoryBuffer *);
    void SwapSimilarBuffers(NuSoundMemoryBuffer *, NuSoundMemoryBuffer *);
};
