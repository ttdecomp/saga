#include "nu2api/nucore/NuMemoryManager.h"

#include <stdlib.h>

#include "decomp.h"
#include "nu2api/nucore/common.h"

void *NuMemoryManager::_TryBlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    LOG_WARN("_TryBlockAlloc(size=%u, alignment=%u, flags=%u, name=%s, category=%u) - replacing with malloc", size,
             alignment, flags, name, category);
    return malloc(size);
}

void NuMemoryManager::BlockFree(void *ptr, u32 flags) {
    LOG_WARN("replacing BlockFree with free");
    free(ptr);
}

void *NuMemoryManager::_BlockReAlloc(void *ptr, u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    LOG_WARN("replacing _BlockReAlloc with realloc");
    return realloc(ptr, size);
}
