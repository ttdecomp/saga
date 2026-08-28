#include "legoapi/legoapi_types.h"

#include <string.h>

// MemoryManager::AllocPool @0x425740.
//
// The manager block (theMemoryManager, 0x248 bytes) is laid out by the
// LoadPermData inline bump-allocator init:
//   0x00 linear-pool cursor cell   0x04 linear-pool end cell
//   0x08 -> cursor cell            0x0c -> end cell
//   0x10 high-water cursor         0x14 bytes handed out
//   0x18 bytes remaining           0x1c.. per-size-class free lists
// Pools are 32-byte classes; a freed block is pushed on its class list and
// recycled by the pop path below (memset of the requested size only), while a
// fresh carve rounds up to the full 32-byte block and updates the counters.
void *MemoryManager::AllocPool(u32 size, i32 zero) {
    i32 size_class;
    if ((i32)(size - 1) < 0) {
        size_class = (i32)(size + 0x1e) >> 5;
    } else {
        size_class = (i32)(size - 1) >> 5;
    }

    u8 *mm = reinterpret_cast<u8 *>(this);
    void **free_head = reinterpret_cast<void **>(mm + 0xc + (size_class + 4) * 4);
    void *item = *free_head;
    if (item != NULL) {
        *free_head = *reinterpret_cast<void **>(item);
        if (zero != 0) {
            memset(item, 0, size);
        }
        return item;
    }

    u32 block = (u32)(size_class + 1) << 5;
    u32 *cursor = *reinterpret_cast<u32 **>(mm + 0x8);
    u32 *endp = *reinterpret_cast<u32 **>(mm + 0xc);
    if ((u32)(*endp - *cursor) <= block) {
        return NULL;
    }
    u32 aligned = (*cursor + 0xf) & ~0xfu;
    *cursor = aligned;
    item = reinterpret_cast<void *>(aligned);
    *cursor = aligned + block;
    memset(item, 0, block);
    *reinterpret_cast<u32 *>(mm + 0x14) += block;
    *reinterpret_cast<u32 *>(mm + 0x18) -= block;
    *reinterpret_cast<u32 *>(mm + 0x10) = *cursor;
    return item;
}

// MemoryManager::FreePool @0x425810: pushes the block onto its size class's
// free list (the next AllocPool of that class pops it, first word = next).
void MemoryManager::FreePool(void *item, u32 size) {
    i32 size_class;
    if ((i32)(size - 1) < 0) {
        size_class = (i32)(size + 0x1e) >> 5;
    } else {
        size_class = (i32)(size - 1) >> 5;
    }

    u8 *mm = reinterpret_cast<u8 *>(this);
    void **free_head = reinterpret_cast<void **>(mm + 0xc + (size_class + 4) * 4);
    *reinterpret_cast<void **>(item) = *free_head;
    *free_head = item;
}

extern "C" {

    void CrashDataPtr(void) {
    }

    void DebAlloc(void) {
    }

    void DebFree(void) {
    }

    void DebFreeAllChunksInstantly(void) {
    }

    void DebFreeInstantly(void) {
    }

    void DebFreeOrphansInstantly(void) {
    }

    void DebReAlloc(void) {
    }

    void DebReAlloc2(void) {
    }

} // extern "C"
