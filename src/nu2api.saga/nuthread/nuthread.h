#pragma once

#include <stdint.h>

#include "nu2api.saga/numemory/numemory.h"

#include "decomp.h"

C_API_START

int32_t NuThreadCriticalSectionBegin(int32_t index);

int32_t NuThreadCriticalSectionEnd(int32_t index);

int32_t NuThreadCreateCriticalSection();

C_API_END

#ifdef __cplusplus

class NuThreadBase {
  private:
    NuMemoryManager *memoryManagers[32];

  public:
    NuMemoryManager *GetLocalStorage(int32_t index);
};

class NuThreadManager {
  private:
    uint32_t bitflags = 0;

  public:
    int32_t AllocTLS();
    static NuThreadBase *GetCurrentThread();
};

#endif