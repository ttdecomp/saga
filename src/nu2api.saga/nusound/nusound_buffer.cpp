#include "nu2api.saga/nusound/nusound_buffer.hpp"

bool NuSoundBuffer::IsAllocated() const {
    if (this->allocated == 0) {
        return this->addr != NULL;
    } else {
        return true;
    }
}

void NuSoundBuffer::Free() {
    if (this->addr != NULL) {
        NuSoundSystem::FreeMemory(this->memory_discipline, (usize)this->addr, this->size);
    }

    this->flags = this->flags & 0xfc;
    this->addr = NULL;
    this->address = NULL;
    this->size = 0;

    // this->field6_0x1c = 0;
    // this->field7_0x20 = 0;
    // this->field8_0x24 = 0;
    // this->field9_0x28 = 0;
    // this->field10_0x2c = 0;
    // this->field11_0x30 = 0;
    // this->field20_0x3c = 0;

    return;
}

i32 NuSoundBuffer::Allocate(u64 size, NuSoundSystem::MemoryDiscipline disc) {
    if (!IsAllocated() || this->size < size) {
        if (IsAllocated()) {
            Free();
        }

        this->addr = (void *)NuSoundSystem::_AllocMemory(
            disc, size, 4, "i:/SagaTouch-Android_9176564/nu2api.2013/nusound/nusound_buffer.cpp :53");

        if (this->addr == NULL) {
            u32 free = NuSoundSystem::GetFreeMemory(disc);
            if (free < size) {
                return -2;
            } else {
                return -1;
            }
        } else {
            this->address = this->addr;
            this->size = size;
            this->memory_discipline = disc;
            return 1;
        }
    } else {
        return 1;
    }
}
