#pragma once

#include "nu2api/nucore/common.h"

#include <pthread.h>

// libTTapp.so layout: address@0x0, size bytes@0x4..0x6, flags@0x7
// (bit7 = locked, bit6 = alloced), prev@0x8, next@0xc.
class NuSoundMemoryBuffer {
    void *address;
    u8 size_l;
    u8 size_m;
    u8 size_h;
    u8 flags;
    NuSoundMemoryBuffer *prev;
    NuSoundMemoryBuffer *next;

  private:
    static pthread_mutex_t s_cs;

    void BeginCriticalSection();
    void EndCriticalSection();

  public:
    NuSoundMemoryBuffer();
    ~NuSoundMemoryBuffer();

    void SetNext(NuSoundMemoryBuffer *next);
    void SetPrev(NuSoundMemoryBuffer *prev);
    void SetSize(u32 size);
    void SetAddress(void *address);
    void SetAlloced(bool alloced);
    void *Lock(const char *name);
    void Unlock();

    void *GetAddress();
    NuSoundMemoryBuffer *GetNext();
    NuSoundMemoryBuffer *GetPrev();
    u32 GetSize();
    bool IsAlloced();
    bool IsLocked();
    const char *GetLockReason(); // 0x321360: lock-reason getter, always NULL on device
};

class NuSoundMemoryManager {
    void *memory;
    u32 size;
    u32 align;
    u32 field3_0xc;
    u16 name_length;
    u16 name_length2;
    const char *name;
    NuSoundMemoryBuffer *free_list_head;
    void *memory2;
    u32 size2;
    u32 free_bytes;
    pthread_mutex_t mutex;
    u8 flags;
    u32 free_count;

  public:
    NuSoundMemoryManager();
    ~NuSoundMemoryManager();

    u32 Init(const char *name, void *memory, u32 size, u32 align, u32 param_5);

    static NuSoundMemoryBuffer *PopFreeBuffer();
    void PushFreeBuffer(NuSoundMemoryBuffer *buffer);

    void EnableDefragOnAlloc(bool value);

    NuSoundMemoryBuffer *Alloc(u32 size);
    NuSoundMemoryBuffer *Defragment(u32 size);
    u32 GetFree();
    void Free(NuSoundMemoryBuffer *buffer);
    NuSoundMemoryBuffer *MergeFreeBuffer(NuSoundMemoryBuffer *buffer);
    NuSoundMemoryBuffer *SplitFreeBuffer(NuSoundMemoryBuffer *buffer, u32 size, NuSoundMemoryBuffer **remainder);

    bool Release();

    u32 GetSize(); // 0x3223b0: the pool's total byte size (device +0x20)
    u32 GetUsed(); // 0x322590: GetSize() minus the free counter

    // Debug/diagnostic helpers (device addresses in nusound_memorymanager.cpp).
    void AllocAddress(u32 address);
    void CheckList();
    void CountAdjacentFreeBuffers(NuSoundMemoryBuffer *buffer);
    void EnableDebug(bool enable);
    void EnableDefragOnFree(bool enable);
    void FreeAddress(void *address);
    void MoveLargestTrailingBufferIntoBuffer(NuSoundMemoryBuffer *buffer, NuSoundMemoryBuffer **out_a,
                                             NuSoundMemoryBuffer **out_b);
    void OutputList();
    void OutputMap();
    void RenderMap(f32 x, f32 y, f32 scale);
    void SwapOrMergeAdjacentBuffers(NuSoundMemoryBuffer *buffer);
    void SwapSimilarBuffers(NuSoundMemoryBuffer *a, NuSoundMemoryBuffer *b);

  private:
    NuSoundMemoryBuffer *CheckAndMergeFreeBufferNext(NuSoundMemoryBuffer *buffer);
    NuSoundMemoryBuffer *CheckAndMergeFreeBufferPrev(NuSoundMemoryBuffer *buffer);
};
