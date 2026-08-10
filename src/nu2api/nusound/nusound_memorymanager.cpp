#include "nu2api/nusound/nusound_memorymanager.hpp"

#include "nu2api/nucore/nuthread.h"
#include "nu2api/nusound/nusound_system.hpp"

#include <cstring>
#include <new>

pthread_mutex_t NuSoundMemoryBuffer::s_cs = PTHREAD_MUTEX_INITIALIZER;

NuSoundMemoryManager::NuSoundMemoryManager()
    : memory(NULL), size(0), align(0), field3_0xc(0), name_length(0), name_length2(0), name(NULL),
      sound_memory_buffer(NULL), memory2(NULL), size2(0), size3(0), unknown_0x30(0) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, 1);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    flags &= 0xf8;
}

void NuSoundMemoryBuffer::SetNext(NuSoundMemoryBuffer *next) {
    this->next = next;
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

NuSoundMemoryBuffer *NuSoundMemoryManager::PopFreeBuffer() {
    NuSoundMemoryBuffer *buf = (NuSoundMemoryBuffer *)NuSoundSystem::_AllocMemory(
        NuSoundSystem::MemoryDiscipline::SCRATCH, 0x10, 4,
        "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_memorymanager.cpp:339");

    new (buf) NuSoundMemoryBuffer{};

    buf->SetNext(NULL);

    return buf;
}

u32 NuSoundMemoryManager::Init(const char *name, void *memory, u32 size, u32 align, u32 param_5) {
    u16 len_1;
    size_t len;
    NuSoundMemoryBuffer *buffer;
    u32 uVar1;

    uVar1 = 0;
    if (((usize)memory % align == 0) && (param_5 % align == 0)) {
        this->align = align;
        this->memory = memory;
        this->size = size;
        this->field3_0xc = param_5;
        uVar1 = 1;
        len = strlen(name);
        len_1 = len + 1;

        this->name_length = len_1;
        this->name_length2 = len_1;
        this->name = name;
        this->memory2 = memory;
        this->size2 = size;
        this->size3 = size;

        buffer = PopFreeBuffer();

        this->sound_memory_buffer = buffer;

        buffer->SetSize(this->size3);
        this->sound_memory_buffer->SetAddress(this->memory2);
    }

    return uVar1;
}

void NuSoundMemoryManager::EnableDefragOnAlloc(bool value) {
    this->flags = this->flags & 0xfd | value << 1;
}

NuSoundMemoryBuffer *NuSoundMemoryManager::Alloc(u32 size) {
#ifdef HOST_BUILD
    LOG_WARN("NuSoundMemoryManager::Alloc is not implemented");
    return (NuSoundMemoryBuffer *)malloc(size);
#endif
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
