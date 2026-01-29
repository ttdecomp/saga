#include "decomp.h"

#include "nu2api.saga/numemory/NuMemoryManager.h"

#include "nu2api.saga/nucore/common.h"

unsigned int NuMemoryManager::m_flags;
pthread_mutex_t *NuMemoryManager::m_globalCriticalSection;
pthread_mutex_t NuMemoryManager::m_globalCriticalSectionBuff;
unsigned int NuMemoryManager::m_headerSize;

NuMemoryManager::NuMemoryManager(IEventHandler *eventHandler, IErrorHandler *errorHandler, const char *name,
                                 const char **categoryNames, unsigned int categoryCount) {
}

void NuMemoryManager::SetFlags(unsigned int flags) {
    m_flags = flags;

    if ((flags & 8) != 0) {
        m_flags |= 4;
        m_headerSize = 0x90;
    } else if ((flags & 4) != 0) {
        m_headerSize = 0xc;
    } else {
        m_headerSize = 0x4;
    }
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

void NuMemoryManager::BlockFree(void *ptr, unsigned int flags) {
}

void NuMemoryManager::AddPage(void *ptr, unsigned int size, bool _unknown) {
}

unsigned int NuMemoryManager::CalculateBlockSize(unsigned int size) {
    unsigned int aligned;
    int blockSize;
    unsigned int memSize;

    aligned = ALIGN(size, 0x4);
    memSize = MAX(aligned, 8);

    blockSize = memSize + m_headerSize + 4;
    if (this->idx > 29) {
        blockSize = memSize + m_headerSize + 8;
    }

    return blockSize;
}

unsigned int NuMemoryManager::GetLargeBinIndex(unsigned int size) {
}

unsigned int NuMemoryManager::GetSmallBinIndex(unsigned int size) {
    return size >> 2;
}

void NuMemoryManager::ValidateAllocAlignment(unsigned int alignment) {
}

void NuMemoryManager::ValidateAllocSize(unsigned int size) {
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
