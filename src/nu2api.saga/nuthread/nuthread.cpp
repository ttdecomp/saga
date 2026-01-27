#include <pthread.h>

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nuthread/nuthread.h"

static pthread_mutex_t NuThread_CriticalSections[16] = {PTHREAD_MUTEX_INITIALIZER};

int32_t NuThreadCriticalSectionBegin(int32_t index) {
    return pthread_mutex_lock(&NuThread_CriticalSections[index]);
}

int32_t NuThreadCriticalSectionEnd(int32_t index) {
    return pthread_mutex_unlock(&NuThread_CriticalSections[index]);
}

static uint8_t NuThread_CriticalSectionsUsed[16] = {0};

int32_t NuThreadCreateCriticalSection() {
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

/// the original implementation uses gcc's emulated TLS
NuThreadBase *NuThreadGetCurrentThread() {
    return NULL;
}

NuThreadBase *NuThreadManager::GetCurrentThread() {
    return NuThreadGetCurrentThread();
}

NuThreadBase::NuThreadBase(const NuThreadCreateParameters &params) {
}

void NuThreadBase::SetDebugName(const char *name) {
    NuStrNCpy(this->name, name, 0x20);
}

void (*NuThreadBase::GetThreadFn() const)(void *) {
    return this->threadFn;
}
void *NuThreadBase::GetParam() const {
    return this->param;
}

NuThread *NuThreadManager::CreateThread(void (*func)(void *), void *arg, int priority, char *name, int param_5,
                                        void *nuthreadCafeCore, void *nuthreadXbox360Core) {
    NuThreadCreateParameters params = {
        func, arg, priority, name, (uint8_t)param_5, nuthreadCafeCore, nuthreadXbox360Core,
    };
    return new NuThread(params);
}

static thread_local pthread_t g_currentPthread;

NuThread::NuThread(const NuThreadCreateParameters &params) : NuThreadBase(params) {
    pthread_t *thread = (pthread_t *)__emutls_get_address(&g_currentPthread);

    if (params.field20_0x20 != 0) {
        // ppVar1 = (pthread_t *)__emutls_get_address(__emutls_v._ZL16g_currentPthread);
        // *ppVar1 = pVar2;
        *thread = pthread_self();
    } else {

        struct sched_param param;
        switch (params.priority) {
            case 0:
                // priority = -2;
                param.sched_priority = -2;
                break;
            case 1:
                param.sched_priority = -1;
                break;
            case 2:
                param.sched_priority = 0;
                break;
            case 3:
                param.sched_priority = 1;
                break;
            case 4:
                param.sched_priority = 2;
                break;
        }

        // this->field_0xa8 = params.field8_0x14;

        pthread_attr_t attrs;
        pthread_attr_init(&attrs);

        pthread_attr_setschedparam(&attrs, &param);

        pthread_create(thread, &attrs, (void *(*)(void *))(&NuThread::ThreadMain), this);
        pthread_attr_destroy(&attrs);

        SetDebugName(params.name);
    }
}

void *NuThread::ThreadMain() {
    LOG_INFO("name=%s", this->name);

    while (this->startSignal != 0) {
        NuThreadSleep(1);
    }

    void (*threadFn)(void *) = GetThreadFn();
    void *param = GetParam();

    (*threadFn)(param);

    delete this;

    return NULL;
}

void NuThreadSleep(int32_t seconds) {
    timespec time;

    if (seconds < 1000) {
        time.tv_sec = 0;
    } else {
        time.tv_sec = seconds / 1000;
        seconds = seconds % 1000;
    }
    time.tv_nsec = seconds * 1000000;

    nanosleep(&time, NULL);
}
