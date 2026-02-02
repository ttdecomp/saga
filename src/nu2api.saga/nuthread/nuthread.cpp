#include <pthread.h>
#include <sched.h>

#include "nu2api.saga/nuthread/nuthread.h"

static pthread_mutex_t NuThread_CriticalSections[16];
static char NuThread_CriticalSectionsUsed[16];

int32_t NuThreadCriticalSectionBegin(int32_t index) {
    return pthread_mutex_lock(&NuThread_CriticalSections[index]);
}

int32_t NuThreadCriticalSectionEnd(int32_t index) {
    return pthread_mutex_unlock(&NuThread_CriticalSections[index]);
}

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

    NuThread_CriticalSectionsUsed[index] = true;

    return index;
}

void NuThreadDestroyCriticalSection(int index) {
    pthread_mutex_destroy(&NuThread_CriticalSections[index]);

    NuThread_CriticalSectionsUsed[index] = false;
}
