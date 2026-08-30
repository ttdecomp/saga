#include "nu2api/nucore/NuMemoryManager.h"

#include <stdlib.h>

#include <pthread.h>

#include "decomp.h"
#include "nu2api/nucore/common.h"

namespace {
    pthread_mutex_t host_alloc_sizes_lock = PTHREAD_MUTEX_INITIALIZER;
    struct HostAllocSize {
        void *ptr;
        u32 size;
        HostAllocSize *next;
    };
    HostAllocSize *host_alloc_sizes;

    void host_record_alloc(void *ptr, u32 size) {
        HostAllocSize *record = (HostAllocSize *)malloc(sizeof(HostAllocSize));
        if (record == NULL) {
            return;
        }
        record->ptr = ptr;
        record->size = size;
        pthread_mutex_lock(&host_alloc_sizes_lock);
        record->next = host_alloc_sizes;
        host_alloc_sizes = record;
        pthread_mutex_unlock(&host_alloc_sizes_lock);
    }

    u32 host_forget_alloc(void *ptr) {
        pthread_mutex_lock(&host_alloc_sizes_lock);
        HostAllocSize **link = &host_alloc_sizes;
        while (*link != NULL && (*link)->ptr != ptr) {
            link = &(*link)->next;
        }
        HostAllocSize *record = *link;
        u32 size = 0;
        if (record != NULL) {
            *link = record->next;
            size = record->size;
        }
        pthread_mutex_unlock(&host_alloc_sizes_lock);
        free(record);
        return size;
    }
} // namespace

void *NuMemoryManager::_TryBlockAlloc(u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    LOG_DEBUG("_TryBlockAlloc(size=%u, alignment=%u, flags=%u, name=%s, category=%u) - replacing with malloc", size,
              alignment, flags, name, category);
    void *ptr = malloc(size);
    if (ptr != NULL) {
        host_record_alloc(ptr, size);
    }
    return ptr;
}

void NuMemoryManager::BlockFree(void *ptr, u32 flags) {
    LOG_DEBUG("replacing BlockFree with free");
    host_forget_alloc(ptr);
    free(ptr);
}

u32 NuMemoryManager::GetBlockSize(void *ptr) {
    pthread_mutex_lock(&host_alloc_sizes_lock);
    HostAllocSize *record = host_alloc_sizes;
    while (record != NULL && record->ptr != ptr) {
        record = record->next;
    }
    u32 size = record != NULL ? record->size : 0;
    pthread_mutex_unlock(&host_alloc_sizes_lock);
    return size;
}

void *NuMemoryManager::_BlockReAlloc(void *ptr, u32 size, u32 alignment, u32 flags, const char *name, u16 category) {
    LOG_DEBUG("replacing _BlockReAlloc with realloc");
    void *result = realloc(ptr, size);
    if (result != NULL) {
        host_forget_alloc(ptr);
        host_record_alloc(result, size);
    }
    return result;
}
