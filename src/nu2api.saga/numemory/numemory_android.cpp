#include <stdio.h>
#include <stdlib.h>

#include "nu2api.saga/numemory/numemory_android.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/numemory/NuMemoryManager.h"

NuMemoryPS::Mem1EventHandler::Mem1EventHandler() {
    this->pageCount = 0;
}

int NuMemoryPS::Mem1EventHandler::AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) {
    unsigned int final_size;
    void *ptr;

    final_size = MAX(size, 0x400000);

    ptr = malloc(final_size);
    if (ptr != NULL) {
        manager->AddPage(ptr, final_size, false);
        this->pageCount++;

        return 1;
    }

    printf("malloc/VirtualAlloc failed!");

    return 0;
}

int NuMemoryPS::Mem1EventHandler::ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) {
    free(ptr);
    this->pageCount--;

    return 1;
}

NuMemoryPS::Mem2EventHandler::Mem2EventHandler() {
}

int NuMemoryPS::Mem2EventHandler::AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) {
    return 0;
}

int NuMemoryPS::Mem2EventHandler::ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) {
    return 0;
}
