#include <new>
#include <pthread.h>

#include "nu2api.saga/numemory/numemory.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nucore.h"

static NuMemory *g_memory = NULL;

const char *g_categoryNames[51] = {
    "NONE",          "INITIAL",      "HARDWAREINIT",  "NUFILE",      "NUFONT",         "NUSCENE",       "NURENDER",
    "NUSOUND",       "NUDEBUG",      "CHARS",         "CHARANIMS",   "SPLITSCREEN",    "DYNOTERRAIN",   "DYNODATA",
    "DYNOSYSTEM",    "AI",           "PARTICLE",      "VFX",         "TECH",           "SCRIPT",        "RESOURCE",
    "GAMEFRAMEWORK", "LEVEL",        "CUTSCENE",      "KRAWLIE",     "GAME",           "EDITOR",        "ANIMATION",
    "MANAGERS",      "ENGINE",       "G_LEGOSETS",    "L_LEGOSETS",  "TECH_GIN",       "TECH_GIZMO",    "TECH_GADGET",
    "TECH_FLOW",     "TECH_ANIM",    "TECH_LIGHTING", "TECH_AI",     "TECH_AI_EDMESH", "TECH_AI_GRAPH", "TECH_CAMERA",
    "TECH_AUDIO",    "TECH_PICKUPS", "TECH_KRAWLIE",  "TECH_STREAM", "TECH_GRASS",     "TECH_MAINLED",  "TECH_FIXUP",
    "RUNTIME",       "TTSHARED",
};

NuMemory::NuMemory(void **buf) {
    void *ptr;
    NuMemoryManager *mem1;

    ptr = *buf;
    this->tls_index = -1;

    NuMemoryManager::SetFlags(0);

    ptr = (void *)ALIGN((int)ptr, 0x8);

    this->error_handler = new (ptr) MemErrorHandler();
    ptr = (void *)((int)ptr + sizeof(MemErrorHandler) + 0x100);

    this->mem1_event_handler = new (ptr) NuMemoryPS::Mem1EventHandler();
    ptr = (void *)((int)ptr + sizeof(NuMemoryPS::Mem1EventHandler));

    this->mem1_manager = new (ptr) NuMemoryManager(this->mem1_event_handler, this->error_handler, "MEM1",
                                                   g_categoryNames, sizeof(g_categoryNames) / sizeof(char *));
    ptr = (void *)((int)ptr + sizeof(NuMemoryManager));

    this->mem2_event_handler = new (ptr) NuMemoryPS::Mem2EventHandler();
    ptr = (void *)((int)ptr + sizeof(NuMemoryPS::Mem2EventHandler));

    this->mem2_manager = new (ptr) NuMemoryManager(this->mem2_event_handler, this->error_handler, "MEM2",
                                                   g_categoryNames, sizeof(g_categoryNames) / sizeof(char *));
    ptr = (void *)((int)ptr + sizeof(NuMemoryManager));

    this->fixed_pool_event_handler = new (ptr) FixedPoolEventHandler();
    ptr = (void *)((int)ptr + sizeof(FixedPoolEventHandler));

    this->dynamic_pool_event_handler = new (ptr) DynamicPoolEventHandler();
    ptr = (void *)((int)ptr + sizeof(DynamicPoolEventHandler));

    this->unknown = 0;
}

NuMemoryManager *NuMemory::GetThreadMem() {
    this->InitalizeThreadLocalStorage();

    if (this->tls_index != -1) {
        NuThreadBase *thread = NuCore::m_threadManager->GetCurrentThread();

        if (thread != NULL) {
            NuMemoryManager *manager = thread->GetLocalStorage(this->tls_index);

            if (manager != NULL) {
                return manager;
            }
        }
    }

    return this->mem1_manager;
}

static char g_memoryBuffer[0x10000];

NuMemory *NuMemoryGet() {
    void *mem;
    char *aligned;

    if (g_memory != NULL) {
        return g_memory;
    }

    aligned = (char *)ALIGN((int)g_memoryBuffer, 0x8);

    mem = aligned + sizeof(NuMemory);
    g_memory = new (aligned) NuMemory(&mem);
    return g_memory;
}

void NuMemory::InitalizeThreadLocalStorage() {
    if (this->tls_index == -1 && NuCore::m_threadManager != NULL) {
        this->tls_index = NuCore::m_threadManager->AllocTLS();
    }
}

int NuMemory::FixedPoolEventHandler::AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int _unknown2,
                                                  const char *_unknown3) {
    return 0;
}

int NuMemory::FixedPoolEventHandler::ReleasePage(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);

    return 1;
}

void NuMemory::FixedPoolEventHandler::ForceReleasePage(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);
}

void *NuMemory::FixedPoolEventHandler::AllocateLargeBlock(NuMemoryPool *pool, unsigned int size, unsigned int alignment,
                                                          const char *name) {
    return NuMemoryGet()->GetThreadMem()->_BlockAlloc(size, alignment, 0, name, 0);
}

void NuMemory::FixedPoolEventHandler::FreeLargeBlock(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);
}

int NuMemory::DynamicPoolEventHandler::AllocatePage(NuMemoryPool *pool, unsigned int _unknown, unsigned int alignment,
                                                    const char *name) {
    void *page = NuMemoryGet()->GetThreadMem()->_BlockAlloc(0x4000, alignment, 0, name, 0);
    pool->AddPage(page, 0x4000);

    return 1;
}

int NuMemory::DynamicPoolEventHandler::ReleasePage(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);

    return 1;
}

void NuMemory::DynamicPoolEventHandler::ForceReleasePage(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);
}

void *NuMemory::DynamicPoolEventHandler::AllocateLargeBlock(NuMemoryPool *pool, unsigned int size,
                                                            unsigned int alignment, const char *name) {
    return NuMemoryGet()->GetThreadMem()->_BlockAlloc(size, alignment, 0, name, 0);
}

void NuMemory::DynamicPoolEventHandler::FreeLargeBlock(NuMemoryPool *pool, void *ptr) {
    NuMemoryGet()->GetThreadMem()->BlockFree(ptr, 0);
}
