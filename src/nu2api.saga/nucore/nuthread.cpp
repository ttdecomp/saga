#include "nu2api.saga/nucore/nuthread.h"

#include <pthread.h>
#include <sched.h>
#include <string.h>

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nustring.h"

u32 nu_current_thread_id;

static pthread_mutex_t NuThread_CriticalSections[16];
static char NuThread_CriticalSectionsUsed[16];

i32 NuThreadCriticalSectionBegin(i32 index) {
    return pthread_mutex_lock(&NuThread_CriticalSections[index]);
}

i32 NuThreadCriticalSectionEnd(i32 index) {
    return pthread_mutex_unlock(&NuThread_CriticalSections[index]);
}

i32 NuThreadCreateCriticalSection(void) {
    i32 index = -1;

    for (i32 i = 0; i < 16; i++) {
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

    NuThread_CriticalSectionsUsed[index] = true;

    return index;
}

void NuThreadDestroyCriticalSection(int index) {
    pthread_mutex_destroy(&NuThread_CriticalSections[index]);

    NuThread_CriticalSectionsUsed[index] = false;
}

NuMemoryManager *NuThreadBase::GetLocalStorage(u32 index) const {
    return this->memory_managers[index];
}

NuThreadBase::NuThreadBase(const NuThreadCreateParameters &params) {
    this->thread_fn = params.thread_fn;
    this->fn_arg = params.fn_arg;

    this->name[0] = '\0';

    memset(this->memory_managers, 0, sizeof(this->memory_managers));
}

NuThreadBase::~NuThreadBase() {
}

void NuThreadBase::SetDebugName(const char *name) {
    NuStrNCpy(this->name, name, 0x20);
}

void (*NuThreadBase::GetThreadFn() const)(void *) {
    return this->thread_fn;
}

void *NuThreadBase::GetParam() const {
    return this->fn_arg;
}
