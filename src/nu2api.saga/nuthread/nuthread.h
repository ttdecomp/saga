#pragma once

#include <stdint.h>

#include "nu2api.saga/numemory/NuMemoryManager.h"

typedef enum NUTHREADCAFECORE {
    NUTHREADCAFECORE_UNKNOWN_1 = 1,
    NUTHREADCAFECORE_UNKNOWN_2 = 2,
} NUTHREADCAFECORE;

typedef enum NUTHREADXBOX360CORE {
    NUTHREADXBOX360CORE_UNKNOWN_1 = 1,
    NUTHREADXBOX360CORE_UNKNOWN_2 = 2,
} NUTHREADXBOX360CORE;

#ifdef __cplusplus
extern "C" {
#endif
    int32_t NuThreadCreateCriticalSection();
    void NuThreadDestroyCriticalSection(int index);

    int32_t NuThreadCriticalSectionBegin(int32_t index);
    int32_t NuThreadCriticalSectionEnd(int32_t index);
#ifdef __cplusplus
}

struct NuThreadCreateParameters {
    void (*thread_fn)(void *);
    void *fn_arg;
    int32_t priority;
    const char *name;
    int32_t stack_size;
    bool is_suspended;
    NUTHREADCAFECORE cafe_core;
    NUTHREADXBOX360CORE xbox360_core;
    bool use_current;
};

class NuThreadBase {
  protected:
    void (*thread_fn)(void *);
    void *fn_arg;
    char name[32];
    NuMemoryManager *memory_managers[32];

  public:
    NuThreadBase(const NuThreadCreateParameters &params);

    NuMemoryManager *GetLocalStorage(uint32_t index) const;

    void SetDebugName(const char *name);

    void (*GetThreadFn() const)(void *);
    void *GetParam() const;

  protected:
    ~NuThreadBase();
};

extern NuThreadBase *g_bgProcThread;

int bgProcIsBgThread(void);

class NuThread : public NuThreadBase {
  private:
    bool is_suspended;

    static void *ThreadMain(void *self);

  public:
    NuThread(const NuThreadCreateParameters &params);

  protected:
    ~NuThread();
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

    NuThread *CreateThread(void (*thread_fn)(void *), void *fn_arg, int priority, const char *name, int stack_size,
                           NUTHREADCAFECORE cafe_core, NUTHREADXBOX360CORE xbox360_core);
};

NuThreadBase *NuThreadGetCurrentThread();
void NuThreadSleep(int32_t seconds);
NuThread *NuThreadInitPS();

#endif
