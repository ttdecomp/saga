#include "legoapi/legoapi_types.h"

void MemoryManager::AllocPool(u32, i32) {
}

void MemoryManager::FreePool(void *, u32) {
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
