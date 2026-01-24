#include <sys/time.h>
#include <time.h>

#include "nu2api.saga/nucore/nutime.h"

void NuTimeGetTicksPS(unsigned int *low, unsigned int *high) {
    struct timespec ts;
    unsigned long long ticks;

    clock_gettime(CLOCK_REALTIME, &ts);

    ticks = ts.tv_nsec / 1000 + (unsigned long long)ts.tv_sec * 1000000;

    *high = ticks >> 32;
    *low = ticks;
}

unsigned long long NuGetCurrentTimeMilisecondsPS(void) {
    struct timespec ts;

    clock_gettime(0, &ts);

    return (unsigned long long)ts.tv_nsec + (unsigned long long)ts.tv_sec * 1000;
}

static unsigned long long g_startTime;

void NuTimeInitPS(void) {
    g_startTime = NuGetCurrentTimeMilisecondsPS();
}

void NuTimeGet(NUTIME *t) {
    NuTimeGetTicksPS(&t->low, (unsigned int *)&t->high);
}

void NuTimeSub(NUTIME *t, NUTIME *a, NUTIME *b) {
    if (a->low >= b->low) {
        t->low = a->low - b->low;
        t->high = a->high - b->high;
    } else {
        t->low = a->low - b->low;
        t->high = a->high - 1 - b->high;
    }
}
