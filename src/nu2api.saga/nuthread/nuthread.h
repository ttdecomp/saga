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

struct NuThreadCreateParameters {
    void (*func)(void *);
    void *arg;
    int32_t priority;
    char *name;
    uint8_t field20_0x20;
    void *nuthreadCafeCore;
    void *nuthreadXbox360Core;
};

class NuThreadBase {
  protected:
    NuMemoryManager *memoryManagers[32];
    char name[32];
    void (*threadFn)(void *);
    void *param;

  public:
    NuThreadBase(const NuThreadCreateParameters &params);

    NuMemoryManager *GetLocalStorage(uint32_t index) const;

    void SetDebugName(const char *name);

    void (*GetThreadFn() const)(void *);
    void *GetParam() const;
};

class NuThread : NuThreadBase {
  private:
    volatile uint8_t startSignal = 1;

    void *ThreadMain();

  public:
    NuThread(const NuThreadCreateParameters &params);
};

class NuThreadManager {
  private:
    uint32_t bitflags = 0;

  public:
    int32_t AllocTLS();
    static NuThreadBase *GetCurrentThread();

    NuThread *CreateThread(void (*func)(void *), void *param_2, int priority, char *name, int param_5,
                           void *nuthreadCafeCore, void *nuthreadXbox360Core);
};

void NuThreadSleep(int32_t seconds);

#endif
