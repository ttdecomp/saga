#include "nu2api/nusound/nusound_buffer.hpp"

#include "nu2api/nucore/nuthread.h"

static NuCriticalSection sCriticalSection{""};

NuSoundBuffer::NuSoundBuffer() {
    this->context.read_size = 0;
    this->context.size2 = 0;
    this->context.flags &= 0xfc;
    this->context.field5_0x20 = 0;
    this->context.size3 = 0;

    this->address = NULL;
    this->lock_count = 0;
    this->size = 0;
    this->memory_buffer = NULL;
    this->allocated = 0;
    this->memory_discipline = NuSoundSystem::MemoryDiscipline::SAMPLE;
}

NuSoundBuffer::NuSoundBuffer(char *address, u64 size) {
    this->context.flags &= 0xfc;
    this->context.read_size = 0;
    this->context.size2 = 0;
    this->context.field5_0x20 = 0;
    this->context.size3 = 0;
    this->allocated = 1;
    this->memory_buffer = (NuSoundMemoryBuffer *)0x0;
    this->lock_count = 0;

    Provide(address, size);
}

void NuSoundBuffer::Free() {
    if (this->memory_buffer != NULL) {
        NuSoundSystem::FreeMemory(this->memory_discipline, (usize)this->memory_buffer, this->size);
    }

    this->context.flags &= 0xfc;

    this->address = NULL;
    this->size = 0;
    this->memory_buffer = NULL;

    this->context.field5_0x20 = 0;
    this->context.read_size = 0;
    this->context.size2 = 0;
    this->context.size3 = 0;
}

bool NuSoundBuffer::IsAllocated() const {
    if (this->allocated == 0) {
        return this->memory_buffer != NULL;
    } else {
        return true;
    }
}

void NuSoundBuffer::SetCurrentContext(Context &context) {
    this->context = context;
}

void *NuSoundBuffer::GetAddress() const {
    return this->address;
}

u64 NuSoundBuffer::GetBufferSize() const {
    return this->size;
}

bool NuSoundBuffer::IsLocked() const {
    return this->lock_count > 0;
}

i32 NuSoundBuffer::Allocate(u64 size, NuSoundSystem::MemoryDiscipline disc) {
    if (!IsAllocated() || this->size < size) {
        if (IsAllocated()) {
            Free();
        }

        this->memory_buffer = (NuSoundMemoryBuffer *)NuSoundSystem::_AllocMemory(
            disc, size, 4, "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_buffer.cpp :53");

        if (this->memory_buffer == NULL) {
            if (NuSoundSystem::GetFreeMemory(disc) < size) {
                return -2;
            } else {
                return -1;
            }
        } else {
            this->address = this->memory_buffer;
            this->size = size;
            this->memory_discipline = disc;
            return 1;
        }
    } else {
        return 1;
    }
}

i32 NuSoundBuffer::Provide(char *address, u64 size) {
    this->address = address;
    this->allocated = 1;
    this->size = size;

    return 1;
}

void NuSoundBuffer::Lock() {
    sCriticalSection.Lock();

    if (this->lock_count && this->allocated == 0) {
        this->address = this->memory_buffer->Lock("NuSoundBuffer::Lock()");
    }

    this->lock_count++;

    sCriticalSection.Unlock();
}

void NuSoundBuffer::Unlock() {
    sCriticalSection.Lock();

    this->lock_count--;

    if (this->lock_count == 0 && this->allocated == 0) {
        this->memory_buffer->Unlock();
        this->address = NULL;
    }

    sCriticalSection.Unlock();
}
