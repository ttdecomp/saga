#include "nu2api.saga/nucore/nutime.h"

#include <sys/time.h>
#include <time.h>

void NuTimeGetTicksPS(u32 *low, u32 *high) {
    struct timespec ts;
    u64 ticks;

    clock_gettime(CLOCK_REALTIME, &ts);

    ticks = ts.tv_nsec / 1000 + (u64)ts.tv_sec * 1000000;

    *high = ticks >> 32;
    *low = ticks;
}

u64 NuGetCurrentTimeMilisecondsPS(void) {
    struct timespec ts;

    clock_gettime(0, &ts);

    return (u64)ts.tv_nsec + (u64)ts.tv_sec * 1000;
}

static u64 g_startTime;

void NuTimeInitPS(void) {
    g_startTime = NuGetCurrentTimeMilisecondsPS();
}

void NuTimeGet(NUTIME *t) {
    NuTimeGetTicksPS(&t->low, (u32 *)&t->high);
}

f32 NuTimeSeconds(NUTIME *t) {
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
