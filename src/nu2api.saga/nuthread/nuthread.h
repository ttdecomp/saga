#pragma once

#include <stdint.h>

#include "nu2api.saga/numemory/NuMemoryManager.h"

#include "decomp.h"

typedef enum NUTHREADCAFECORE {
    NUTHREADCAFECORE_UNKNOWN_1 = 1,
    NUTHREADCAFECORE_UNKNOWN_2 = 2,
} NUTHREADCAFECORE;

typedef enum NUTHREADXBOX360CORE {
    NUTHREADXBOX360CORE_UNKNOWN_1 = 1,
    NUTHREADXBOX360CORE_UNKNOWN_2 = 2,
} NUTHREADXBOX360CORE;

C_API_START

int32_t NuThreadCriticalSectionBegin(int32_t index);

int32_t NuThreadCriticalSectionEnd(int32_t index);

int32_t NuThreadCreateCriticalSection(void);

C_API_END

#ifdef __cplusplus

struct NuThreadCreateParameters {
    void (*threadFn)(void *);
    void *fnArg;
    int32_t priority;
    const char *name;
    int32_t stackSize;
    bool isSuspended;
    NUTHREADCAFECORE nuthreadCafeCore;
    NUTHREADXBOX360CORE nuthreadXbox360Core;
    bool useCurrent;
};

class NuThreadBase {
  protected:
    void (*threadFn)(void *);
    void *fnArg;
    char name[32];
    NuMemoryManager *memoryManagers[32];

  public:
    NuThreadBase(const NuThreadCreateParameters &params);

    NuMemoryManager *GetLocalStorage(uint32_t index) const;

    void SetDebugName(const char *name);

    void (*GetThreadFn() const)(void *);
    void *GetParam() const;
};

extern NuThreadBase *g_bgProcThread;

int bgProcIsBgThread(void);

class NuThread : public NuThreadBase {
  private:
    volatile uint8_t startSignal;
    bool isSuspended;

    static void *ThreadMain(void *self);

  public:
    NuThread(const NuThreadCreateParameters &params);
};

class NuThreadManager {
  public:
    NuThreadManager();

  private:
    NuThread *thread;
    uint32_t bitflags;

  public:
    int32_t AllocTLS();
    NuThreadBase *GetCurrentThread();

    NuThread *CreateThread(void (*threadFn)(void *), void *fnArg, int priority, const char *name, int stackSize,
                           NUTHREADCAFECORE nuthreadCafeCore, NUTHREADXBOX360CORE nuthreadXbox360Core);
};

void NuThreadSleep(int32_t seconds);
NuThread *NuThreadInitPS();

#endif
