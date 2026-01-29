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
    VARIPTR buf_ptr;

    buf_ptr.void_ptr = *buf;
    this->tlsIndex = -1;

    NuMemoryManager::SetFlags(0);

    buf_ptr.addr = ALIGN(buf_ptr.addr, 0x8);

    this->errorHandler = new (&buf_ptr.void_ptr) MemErrorHandler();
}

unsigned int NuMemoryManager::m_headerSize;

void NuMemoryManager::SetFlags(unsigned int flags) {
    if ((flags & 4) == 0) {
        NuMemoryManager::m_headerSize = 4;
    } else {
        NuMemoryManager::m_headerSize = 12;
    }
}

unsigned int NuMemoryManager::CalculateBlockSize(unsigned int size) {
    uint uVar1;
    int blockSize;
    uint uVar2;

    uVar1 = ALIGN(size, 0x4);
    uVar2 = uVar1 >= 8 ? uVar1 : 8;

    blockSize = uVar2 + NuMemoryManager::m_headerSize + 4;
    if (this->idx > 29) {
        blockSize = uVar2 + NuMemoryManager::m_headerSize + 8;
    }

    return blockSize;
}

void *NuMemoryManager::_BlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, const char *name,
                                   uint16_t param_5) {
    void *ptr = this->_TryBlockAlloc(size, param_2, param_3, name, param_5);

    if (ptr == NULL) {
        this->____WE_HAVE_RUN_OUT_OF_MEMORY_____(size, name);
    }

    return ptr;
}

void *NuMemoryManager::_TryBlockAlloc(uint32_t size, uint32_t param_2, uint32_t param_3, const char *name,
                                      uint16_t param_5) {
    LOG_INFO("size=%x, param_2=%u, param_3=%u, name='%s', param_5=%u", size, param_2, param_3, name, param_5);

    // UNIMPLEMENTED();
    return malloc(size);
}

void NuMemoryManager::____WE_HAVE_RUN_OUT_OF_MEMORY_____(uint32_t size, const char *name) {
    UNIMPLEMENTED();
}

void NuMemoryManager::IErrorHandler::HandleError(NuMemoryManager *manager, ErrorCode code, const char *msg) {
}

int NuMemoryManager::IErrorHandler::OpenDump(NuMemoryManager *manager, const char *filename, unsigned int &id) {
    return 0;
}

void NuMemoryManager::IErrorHandler::CloseDump(NuMemoryManager *manager, unsigned int id) {
}

void NuMemoryManager::IErrorHandler::Dump(NuMemoryManager *manager, unsigned int id, const char *msg) {
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

static char g_memoryBuffer[0x10000];

NuMemory *NuMemoryGet() {
    if (g_memory != NULL) {
        return g_memory;
    }

    new (&g_memoryBuffer) NuMemory{(void **)&g_memoryBuffer + sizeof(NuMemory)};
    g_memory = (NuMemory *)&g_memoryBuffer;
    return (NuMemory *)&g_memoryBuffer;
}
