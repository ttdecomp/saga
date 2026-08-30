#include "nu2api/nusound/nusound_memorymanager.hpp"

#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound_system.hpp"

#include <cstring>
#include <new>

pthread_mutex_t NuSoundMemoryBuffer::s_cs = PTHREAD_MUTEX_INITIALIZER;

void NuSoundMemoryBuffer::BeginCriticalSection() {
    pthread_mutex_lock(&s_cs);
}

void NuSoundMemoryBuffer::EndCriticalSection() {
    pthread_mutex_unlock(&s_cs);
}

void NuSoundMemoryBuffer::SetNext(NuSoundMemoryBuffer *next) {
    this->next = next;
}

void NuSoundMemoryBuffer::SetPrev(NuSoundMemoryBuffer *prev) {
    this->prev = prev;
}

void NuSoundMemoryBuffer::SetSize(u32 size) {
    this->size_l = size;
    this->size_m = (size >> 8);
    this->size_h = (size >> 16);

    this->flags = this->flags & 0xc0 | (u8)(size >> 24) & 63;
}

void NuSoundMemoryBuffer::SetAddress(void *address) {
    this->address = address;
}

// libTTapp.so 0x3211d0: alloced flag = flags bit 6.
void NuSoundMemoryBuffer::SetAlloced(bool alloced) {
    this->flags = (this->flags & ~0x40) | (((u8)alloced & 1) << 6);
}

void *NuSoundMemoryBuffer::Lock(const char *name) {
    BeginCriticalSection();

    u8 c = this->flags;
    while (c < 0) {
        EndCriticalSection();
        NuThreadSleep(0);
        BeginCriticalSection();
        c = this->flags;
    }
    this->flags = c | 0x80;

    EndCriticalSection();

    return this->address;
}

void NuSoundMemoryBuffer::Unlock() {
    BeginCriticalSection();
    this->flags = this->flags & 0x7f;
    EndCriticalSection();
}

// libTTapp.so 0x3211c0
void *NuSoundMemoryBuffer::GetAddress() {
    return this->address;
}

// libTTapp.so 0x321230
NuSoundMemoryBuffer *NuSoundMemoryBuffer::GetNext() {
    return this->next;
}

// libTTapp.so 0x321210
NuSoundMemoryBuffer *NuSoundMemoryBuffer::GetPrev() {
    return this->prev;
}

// libTTapp.so 0x321180
u32 NuSoundMemoryBuffer::GetSize() {
    return (u32)this->size_l | (u32)this->size_m << 8 | (u32)this->size_h << 16 | (u32)(this->flags & 63) << 24;
}

// libTTapp.so 0x3211f0: alloced flag = flags bit 6.
bool NuSoundMemoryBuffer::IsAlloced() {
    return (this->flags >> 6) & 1;
}

// libTTapp.so 0x3211e0: locked flag = flags bit 7.
bool NuSoundMemoryBuffer::IsLocked() {
    return (this->flags >> 7) & 1;
}

NuSoundMemoryBuffer::NuSoundMemoryBuffer() {
    this->address = NULL;
    this->size_l = 0;
    this->size_m = 0;
    this->size_h = 0;
    this->flags = 0;
    this->prev = NULL;
    this->next = NULL;
}

NuSoundMemoryBuffer::~NuSoundMemoryBuffer() {
}

// libTTapp.so 0x321510: the buffer headers come out of the SCRATCH heap.
NuSoundMemoryBuffer *NuSoundMemoryManager::PopFreeBuffer() {
    NuSoundMemoryBuffer *buf = (NuSoundMemoryBuffer *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, 0x10, 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_memorymanager.cpp:339");

    new (buf) NuSoundMemoryBuffer{};

    buf->SetNext(NULL);

    return buf;
}

// libTTapp.so 0x321650: hand the header back to the SCRATCH heap.
void NuSoundMemoryManager::PushFreeBuffer(NuSoundMemoryBuffer *buffer) {
    NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, (usize)buffer, 0);
}

// libTTapp.so 0x321580
u32 NuSoundMemoryManager::Init(const char *name, void *memory, u32 size, u32 align, u32 param_5) {
    if ((usize)memory % align == 0 && param_5 % align == 0) {
        this->align = align;
        this->memory = memory;
        this->size = size;
        this->field3_0xc = param_5;

        size_t len = strlen(name);
        u16 len_1 = len + 1;

        this->name_length = len_1;
        this->name_length2 = len_1;
        this->name = name;
        this->memory2 = memory;
        this->size2 = size;
        this->free_bytes = size;

        NuSoundMemoryBuffer *buffer = PopFreeBuffer();

        this->free_list_head = buffer;

        buffer->SetSize(this->free_bytes);
        this->free_list_head->SetAddress(this->memory2);
    }

    return 1;
}

void NuSoundMemoryManager::EnableDefragOnAlloc(bool value) {
    this->flags = this->flags & 0xfd | value << 1;
}

NuSoundMemoryManager::NuSoundMemoryManager() {
    this->memory = NULL;
    this->size = 0;
    this->align = 0;
    this->field3_0xc = 0;
    this->name_length = 0;
    this->name_length2 = 0;
    this->name = NULL;
    this->free_list_head = NULL;
    this->memory2 = NULL;
    this->size2 = 0;
    this->free_bytes = 0;

    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&this->mutex, &attributes);
    pthread_mutexattr_destroy(&attributes);

    this->flags &= 0xf8;
    this->free_count = 0;
}

NuSoundMemoryManager::~NuSoundMemoryManager() {
}

// libTTapp.so 0x3221e0: relocate trailing blocks to satisfy an alloc. Not
// transcribed yet; returns no block so Alloc fails cleanly.
NuSoundMemoryBuffer *NuSoundMemoryManager::Defragment(u32 size) {
    (void)size;
    return NULL;
}

// libTTapp.so 0x3223c0
u32 NuSoundMemoryManager::GetFree() {
    return this->free_bytes;
}

// libTTapp.so 0x3223d0: best-fit allocation from the free list, splitting the
// chosen free block (or using it whole when it fits exactly). When no block
// fits and defrag-on-alloc is enabled, Defragment() runs first.
NuSoundMemoryBuffer *NuSoundMemoryManager::Alloc(u32 size) {
    u32 alloc_size = size;
    if (alloc_size % this->field3_0xc != 0) {
        alloc_size = alloc_size + this->field3_0xc - alloc_size % this->field3_0xc;
    }

    pthread_mutex_lock(&this->mutex);

    NuSoundMemoryBuffer *best = NULL;
    if (alloc_size <= this->GetFree()) {
        for (NuSoundMemoryBuffer *buf = this->free_list_head; buf != NULL;) {
            if (!buf->IsAlloced()) {
                u32 buf_size = buf->GetSize();
                if (buf_size >= alloc_size) {
                    if (best == NULL || best->GetSize() > buf_size) {
                        if (buf_size == alloc_size) {
                            best = buf;
                            break;
                        }
                        best = buf;
                    }
                }
            }
            buf = buf->GetNext();
        }

        if (best == NULL && (this->flags & 2) != 0) {
            best = this->Defragment(alloc_size);
        }

        if (best != NULL) {
            if (best->GetSize() != alloc_size) {
                best = this->SplitFreeBuffer(best, alloc_size, NULL);
            }
            best->SetAlloced(true);
            this->free_bytes = this->free_bytes - alloc_size;
        }
    }

    pthread_mutex_unlock(&this->mutex);
    return best;
}

// libTTapp.so 0x321690: carve `size` bytes off the front of `buffer`. When the
// block is no larger than the request it is returned unchanged; otherwise a
// fresh header is popped for the remainder behind the carved front.
NuSoundMemoryBuffer *NuSoundMemoryManager::SplitFreeBuffer(NuSoundMemoryBuffer *buffer, u32 size,
                                                           NuSoundMemoryBuffer **remainder) {
    u32 total = buffer->GetSize();
    if (total > size) {
        NuSoundMemoryBuffer *tail = PopFreeBuffer();

        tail->SetSize(total - size);
        tail->SetAddress((char *)buffer->GetAddress() + size);

        buffer->SetSize(size);

        NuSoundMemoryBuffer *next = buffer->GetNext();
        buffer->SetNext(tail);
        tail->SetPrev(buffer);
        tail->SetNext(next);
        if (next != NULL) {
            next->SetPrev(tail);
        }

        if (remainder != NULL) {
            *remainder = tail;
        }
    } else if (remainder != NULL) {
        *remainder = NULL;
    }

    return buffer;
}

// libTTapp.so 0x321960: mark the block free, return its bytes to the free
// count and coalesce it with its free neighbours.
void NuSoundMemoryManager::Free(NuSoundMemoryBuffer *buffer) {
    pthread_mutex_lock(&this->mutex);

    buffer->SetAlloced(false);
    this->free_bytes = this->free_bytes + buffer->GetSize();
    this->MergeFreeBuffer(buffer);
    this->free_count = this->free_count + 1;

    pthread_mutex_unlock(&this->mutex);
}

// libTTapp.so 0x321910
NuSoundMemoryBuffer *NuSoundMemoryManager::MergeFreeBuffer(NuSoundMemoryBuffer *buffer) {
    buffer = this->CheckAndMergeFreeBufferPrev(buffer);
    return this->CheckAndMergeFreeBufferNext(buffer);
}

// libTTapp.so 0x321790: when the previous block is free too, grow it over this
// block, relink the list and push this block's header back to the heap.
NuSoundMemoryBuffer *NuSoundMemoryManager::CheckAndMergeFreeBufferPrev(NuSoundMemoryBuffer *buffer) {
    NuSoundMemoryBuffer *prev = buffer->GetPrev();
    if (prev == NULL || prev->IsAlloced()) {
        return buffer;
    }

    prev->SetSize(prev->GetSize() + buffer->GetSize());

    NuSoundMemoryBuffer *next = buffer->GetNext();
    prev->SetNext(next);
    if (next != NULL) {
        next->SetPrev(prev);
    }

    buffer->SetAddress(NULL);
    this->PushFreeBuffer(buffer);

    return prev;
}

// libTTapp.so 0x321850: when the next block is free too, grow this block over
// it, relink the list and push the next block's header back to the heap.
NuSoundMemoryBuffer *NuSoundMemoryManager::CheckAndMergeFreeBufferNext(NuSoundMemoryBuffer *buffer) {
    NuSoundMemoryBuffer *next = buffer->GetNext();
    if (next == NULL || next->IsAlloced()) {
        return buffer;
    }

    buffer->SetSize(buffer->GetSize() + next->GetSize());

    NuSoundMemoryBuffer *next2 = next->GetNext();
    buffer->SetNext(next2);
    if (next2 != NULL) {
        next2->SetPrev(buffer);
    }

    next->SetAddress(NULL);
    this->PushFreeBuffer(next);

    return buffer;
}

// libTTapp.so 0x321360: the lock-reason getter; the device build compiles it
// to a constant NULL return.
const char *NuSoundMemoryBuffer::GetLockReason() {
    return NULL;
}

// libTTapp.so 0x3223b0: the pool's total byte size.
u32 NuSoundMemoryManager::GetSize() {
    return this->size2;
}

// libTTapp.so 0x322590: the pool's in-use byte count.
u32 NuSoundMemoryManager::GetUsed() {
    return this->size2 - this->free_bytes;
}

// libTTapp.so 0x321430: hand every free-list header back to SCRATCH, drop the
// pool's bookkeeping and report success.
bool NuSoundMemoryManager::Release() {
    NuSoundMemoryBuffer *buffer = this->free_list_head;

    this->memory = NULL;
    this->size = 0;
    this->align = 0;
    this->field3_0xc = 0;

    while (buffer != NULL) {
        this->free_list_head = buffer->GetNext();
        NuSoundSystem::FreeMemory(NuSoundSystem::MemoryDiscipline::SCRATCH, (usize)buffer, 0);
        buffer = this->free_list_head;
    }

    this->memory2 = NULL;
    this->size2 = 0;
    this->free_bytes = 0;
    return true;
}

// libTTapp.so 0x322550 (AllocAddress). Debug-only; not transcribed yet. It
// takes a raw pool offset, so returning nothing keeps every caller's flow
// intact without inventing behavior.
void NuSoundMemoryManager::AllocAddress(u32 address) {
    (void)address;
}

// libTTapp.so 0x3225a0 (CheckList). Debug-only list validator; not
// transcribed yet.
void NuSoundMemoryManager::CheckList() {
}

// libTTapp.so 0x321f40 (CountAdjacentFreeBuffers). Debug-only; not
// transcribed yet.
void NuSoundMemoryManager::CountAdjacentFreeBuffers(NuSoundMemoryBuffer *buffer) {
    (void)buffer;
}

// libTTapp.so 0x322750 (EnableDebug). Debug-only flag setter; not transcribed
// yet.
void NuSoundMemoryManager::EnableDebug(bool enable) {
    (void)enable;
}

// libTTapp.so 0x322790 (EnableDefragOnFree). Debug-only flag setter; not
// transcribed yet.
void NuSoundMemoryManager::EnableDefragOnFree(bool enable) {
    (void)enable;
}

// libTTapp.so 0x3219e0 (FreeAddress). Debug-only; not transcribed yet.
void NuSoundMemoryManager::FreeAddress(void *address) {
    (void)address;
}

// libTTapp.so 0x321fe0 (MoveLargestTrailingBufferIntoBuffer). Defrag
// helper; not transcribed yet.
void NuSoundMemoryManager::MoveLargestTrailingBufferIntoBuffer(NuSoundMemoryBuffer *buffer, NuSoundMemoryBuffer **out_a,
                                                               NuSoundMemoryBuffer **out_b) {
    (void)buffer;
    (void)out_a;
    (void)out_b;
}

// libTTapp.so 0x322640 (OutputList). Debug dump over the free list; not
// transcribed yet.
void NuSoundMemoryManager::OutputList() {
}

// libTTapp.so 0x322680 (OutputMap). Debug dump of the block map; not
// transcribed yet.
void NuSoundMemoryManager::OutputMap() {
}

// libTTapp.so 0x3227b0 (RenderMap). On-screen debug map; not transcribed yet.
void NuSoundMemoryManager::RenderMap(f32 x, f32 y, f32 scale) {
    (void)x;
    (void)y;
    (void)scale;
}

// libTTapp.so 0x321d10 (SwapOrMergeAdjacentBuffers). Defrag helper; not
// transcribed yet.
void NuSoundMemoryManager::SwapOrMergeAdjacentBuffers(NuSoundMemoryBuffer *buffer) {
    (void)buffer;
}

// libTTapp.so 0x321a60 (SwapSimilarBuffers). Defrag helper; not transcribed
// yet.
void NuSoundMemoryManager::SwapSimilarBuffers(NuSoundMemoryBuffer *a, NuSoundMemoryBuffer *b) {
    (void)a;
    (void)b;
}
