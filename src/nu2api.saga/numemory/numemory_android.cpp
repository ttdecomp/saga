#include <stdio.h>
#include <stdlib.h>

#include "nu2api.saga/numemory/numemory_android.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/numemory/NuMemoryManager.h"

NuMemoryPS::Mem1EventHandler::Mem1EventHandler() {
    this->page_count = 0;
}

bool NuMemoryPS::Mem1EventHandler::AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) {
    void *ptr;

    size = MAX(size, 0x400000);

    ptr = malloc(size);
    if (ptr != NULL) {
        manager->AddPage(ptr, size, false);
        this->page_count++;

        return 1;
    }

    printf("malloc/VirtualAlloc failed!");

    return false;
}

bool NuMemoryPS::Mem1EventHandler::ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) {
    free(ptr);
    this->page_count--;

    return true;
}

NuMemoryPS::Mem2EventHandler::Mem2EventHandler() {
}

bool NuMemoryPS::Mem2EventHandler::AllocatePage(NuMemoryManager *manager, unsigned int size, unsigned int _unknown) {
    return false;
}

bool NuMemoryPS::Mem2EventHandler::ReleasePage(NuMemoryManager *manager, void *ptr, unsigned int _unknown) {
    return false;
}
