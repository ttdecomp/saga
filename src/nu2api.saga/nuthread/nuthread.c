#include "nu2api.saga/nuthread/nuthread.h"

#include <pthread.h>

static pthread_mutex_t NuThread_CriticalSections[1];

int NuThreadCriticalSectionBegin(int index) {
    return pthread_mutex_lock(&NuThread_CriticalSections[index]);
}

int NuThreadCriticalSectionEnd(int index) {
    return pthread_mutex_unlock(&NuThread_CriticalSections[index]);
}
