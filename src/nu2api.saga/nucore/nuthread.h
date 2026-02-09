#pragma once

#include "nu2api.saga/nucore/NuMemoryManager.h"
#include "nu2api.saga/nucore/common.h"

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
    extern u32 nu_current_thread_id;

    i32 NuThreadCreateCriticalSection();
    void NuThreadDestroyCriticalSection(int index);

    i32 NuThreadCriticalSectionBegin(i32 index);
    i32 NuThreadCriticalSectionEnd(i32 index);
#ifdef __cplusplus
}

struct NuThreadCreateParameters {
    void (*thread_fn)(void *);
    void *fn_arg;
    i32 priority;
    const char *name;
    i32 stack_size;
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

    NuMemoryManager *GetLocalStorage(u32 index) const;

    void SetDebugName(const char *name);

    void (*GetThreadFn() const)(void *);
    void *GetParam() const;

  protected:
    ~NuThreadBase();
};

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
    u32 bitflags;

  public:
    i32 AllocTLS();
    NuThreadBase *GetCurrentThread();

    NuThread *CreateThread(void (*thread_fn)(void *), void *fn_arg, int priority, const char *name, int stack_size,
                           NUTHREADCAFECORE cafe_core, NUTHREADXBOX360CORE xbox360_core);
};

NuThreadBase *NuThreadGetCurrentThread();
void NuThreadSleep(i32 seconds);
NuThread *NuThreadInitPS();

#endif
