#include <pthread.h>
#include <sched.h>
#include <string.h>

#include "nu2api.saga/nuthread/nuthread.h"

#include "nu2api.saga/nucore/nucore.h"
#include "nu2api.saga/nucore/nustring.h"

NuThreadBase *g_bgProcThread = NULL;

static pthread_mutex_t NuThread_CriticalSections[16];

int32_t NuThreadCriticalSectionBegin(int32_t index) {
    return pthread_mutex_lock(&NuThread_CriticalSections[index]);
}

int32_t NuThreadCriticalSectionEnd(int32_t index) {
    return pthread_mutex_unlock(&NuThread_CriticalSections[index]);
}

static uint8_t NuThread_CriticalSectionsUsed[16] = {0};

int32_t NuThreadCreateCriticalSection(void) {
    int32_t index = -1;

    for (int32_t i = 0; i < 16; i++) {
        if (NuThread_CriticalSectionsUsed[i] == '\0') {
            index = i;

            break;
        }
    }

    if (index == -1) {
        return -1;
    }

    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, 1);
    pthread_mutex_init(&NuThread_CriticalSections[index], &attr);
    pthread_mutexattr_destroy(&attr);

    NuThread_CriticalSectionsUsed[index] = 1;

    return index;
}

int bgProcIsBgThread(void) {
    return NuCore::m_threadManager->GetCurrentThread() == g_bgProcThread;
}

int32_t NuThreadManager::AllocTLS() {
    for (int32_t i = 0; i < 32; i++) {
        if ((this->bitflags >> (i & 0x1f) & 1) == 0) {
            this->bitflags |= 1 << ((uint8_t)i & 0x1f);
            return i;
        }
    }

    return -1;
}

NuMemoryManager *NuThreadBase::GetLocalStorage(uint32_t index) const {
    return this->memoryManagers[index];
}

static __thread NuThreadBase *g_currentThread = NULL;

NuThreadBase *NuThreadGetCurrentThread() {
    return g_currentThread;
}

NuThreadBase *NuThreadManager::GetCurrentThread() {
    return NuThreadGetCurrentThread();
}

NuThreadBase::NuThreadBase(const NuThreadCreateParameters &params) {
    this->threadFn = params.threadFn;
    this->fnArg = params.fnArg;

    this->name[0] = '\0';

    memset(this->memoryManagers, 0, sizeof(this->memoryManagers));
}

void NuThreadBase::SetDebugName(const char *name) {
    NuStrNCpy(this->name, name, 0x20);
}

void (*NuThreadBase::GetThreadFn() const)(void *) {
    return this->threadFn;
}

void *NuThreadBase::GetParam() const {
    return this->fnArg;
}

NuThreadManager::NuThreadManager() {
    this->bitflags = 0;
}

NuThread *NuThreadManager::CreateThread(void (*threadFn)(void *), void *fnArg, int priority, const char *name,
                                        int stackSize, NUTHREADCAFECORE nuthreadCafeCore,
                                        NUTHREADXBOX360CORE nuthreadXbox360Core) {
    static int ThreadPriorityMap[] = {0, 1, 2, 3, 4};

    NuThreadCreateParameters params;
    NuThread *thread;

    params = {.threadFn = threadFn,
              .fnArg = fnArg,
              .priority = ThreadPriorityMap[priority + 2],
              .name = name,
              .stackSize = stackSize == 0 ? 0x8000 : stackSize,
              .isSuspended = false,
              .nuthreadCafeCore = nuthreadCafeCore,
              .nuthreadXbox360Core = nuthreadXbox360Core,
              .useCurrent = false};

    return new NuThread(params);
}

static __thread pthread_t g_currentPthread;

NuThread::NuThread(const NuThreadCreateParameters &params) : NuThreadBase(params) {
    sched_param scheduling;
    pthread_attr_t attrs;

    if (params.useCurrent) {
        g_currentPthread = pthread_self();

        return;
    }

    switch (params.priority) {
        case 0:
            scheduling.sched_priority = -2;
            break;
        case 1:
            scheduling.sched_priority = -1;
            break;
        case 2:
            scheduling.sched_priority = 0;
            break;
        case 3:
            scheduling.sched_priority = 1;
            break;
        case 4:
            scheduling.sched_priority = 2;
            break;
    }

    this->isSuspended = params.isSuspended;

    pthread_attr_init(&attrs);
    pthread_attr_setschedparam(&attrs, &scheduling);

    pthread_create(&g_currentPthread, &attrs, &ThreadMain, this);

    pthread_attr_destroy(&attrs);

    SetDebugName(params.name);
}

NuThread *NuThreadInitPS() {
    pthread_t current;
    int policy;
    sched_param scheduling;
    NuThreadCreateParameters createParams;
    NuThread *thread;

    current = pthread_self();

    policy = 0;
    pthread_getschedparam(current, &policy, &scheduling);

    createParams = {
        .threadFn = NULL,
        .fnArg = NULL,
        .priority = 1,
        .name = "",
        .stackSize = 0,
        .isSuspended = false,
        .nuthreadCafeCore = NUTHREADCAFECORE_UNKNOWN_1,
        .nuthreadXbox360Core = NUTHREADXBOX360CORE_UNKNOWN_1,
        .useCurrent = true,
    };

    thread = new NuThread(createParams);
    thread->SetDebugName("Main");

    g_currentThread = thread;

    return thread;
}

void *NuThread::ThreadMain(void *thread) {
    NuThread *self = ((NuThread *)thread);

    LOG_INFO("name=%s", self->name);

    g_currentThread = self;

    while (self->startSignal != 0) {
        NuThreadSleep(1);
    }

    void (*threadFn)(void *) = self->GetThreadFn();
    void *param = self->GetParam();

    (*threadFn)(param);

    self->~NuThread();
    delete self;

    return NULL;
}

void NuThreadSleep(int32_t seconds) {
    timespec time;

    if (seconds < 1000) {
        time.tv_sec = 0;
    } else {
        time.tv_sec = seconds / 1000;
        seconds %= 1000;
    }

    time.tv_nsec = seconds * 1000000;

    nanosleep(&time, NULL);
}
