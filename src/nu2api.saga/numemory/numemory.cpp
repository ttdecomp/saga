#include "nu2api.saga/numemory/numemory.h"

#include "nu2api.saga/nucore/nucore.h"

#include <iostream>
#include <pthread.h>

static NuMemory *g_memory = NULL;

static const char *g_categoryNames[51] = {
    "NONE",          "INITIAL",      "HARDWAREINIT",  "NUFILE",      "NUFONT",         "NUSCENE",       "NURENDER",
    "NUSOUND",       "NUDEBUG",      "CHARS",         "CHARANIMS",   "SPLITSCREEN",    "DYNOTERRAIN",   "DYNODATA",
    "DYNOSYSTEM",    "AI",           "PARTICLE",      "VFX",         "TECH",           "SCRIPT",        "RESOURCE",
    "GAMEFRAMEWORK", "LEVEL",        "CUTSCENE",      "KRAWLIE",     "GAME",           "EDITOR",        "ANIMATION",
    "MANAGERS",      "ENGINE",       "G_LEGOSETS",    "L_LEGOSETS",  "TECH_GIN",       "TECH_GIZMO",    "TECH_GADGET",
    "TECH_FLOW",     "TECH_ANIM",    "TECH_LIGHTING", "TECH_AI",     "TECH_AI_EDMESH", "TECH_AI_GRAPH", "TECH_CAMERA",
    "TECH_AUDIO",    "TECH_PICKUPS", "TECH_KRAWLIE",  "TECH_STREAM", "TECH_GRASS",     "TECH_MAINLED",  "TECH_FIXUP",
    "RUNTIME",       "TTSHARED",
};

NuMemory::NuMemory(void *buffer) {
}

int32_t NuMemoryManager::CalculateBlockSize(uint32_t size) {
    uint uVar1;
    int blockSize;
    uint uVar2;

    uVar1 = size + 3 & 0xfffffffc;
    uVar2 = 8;
    if (7 < uVar1) {
        uVar2 = uVar1;
    }
    blockSize = uVar2 + m_headerSize + 4;
    if (29 < this->field12_0xc) {
        blockSize = uVar2 + m_headerSize + 8;
    }
    return blockSize;
}

void *NuMemoryManager::_BlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, char *name, uint16_t param_5) {
    void *ptr = this->_TryBlockAlloc(size, param_2, param_3, name, param_5);

    if (ptr == NULL) {
        this->____WE_HAVE_RUN_OUT_OF_MEMORY_____(size, name);
    }

    return ptr;
}

void *NuMemoryManager::_TryBlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, char *name, uint16_t param_5) {
    LOG_INFO("size=%x, param_2=%u, param_3=%u, name='%s', param_5=%u", size, param_2, param_3, name, param_5);

    // UNIMPLEMENTED();
    return malloc(size);
}

void NuMemoryManager::____WE_HAVE_RUN_OUT_OF_MEMORY_____(uint32_t size, char *name) {
    UNIMPLEMENTED();
}

void NuMemory::InitalizeThreadLocalStorage() {
    if ((this->tlsIndex == -1) && NuCore::m_threadManager != NULL) {
        this->tlsIndex = NuCore::m_threadManager->AllocTLS();
    }
}

NuMemoryManager *NuMemory::GetThreadMem() {
    this->InitalizeThreadLocalStorage();

    if (this->tlsIndex != -1) {
        NuThreadBase *thread = NuThreadManager::GetCurrentThread();
        if (thread == NULL) {
            NuMemoryManager *manager = thread->GetLocalStorage(this->tlsIndex);
            if (manager != NULL) {
                return manager;
            }
        }
    }

    return this->memoryManager;
}

static NuMemory g_memoryBuffer = {0};

NuMemory *NuMemoryGet() {
    static char nuMemoryHugeArray[65000];

    if (g_memory != NULL) {
        return g_memory;
    }

    new (&g_memoryBuffer) NuMemory{(void *)nuMemoryHugeArray};

    g_memory = &g_memoryBuffer;

    return &g_memoryBuffer;
}
