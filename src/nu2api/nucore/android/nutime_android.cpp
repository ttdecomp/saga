#include "nu2api/nucore/nutime.h"

#include <time.h>

#include "nu2api/nucore/common.h"

static u64 g_startTime;

void NuTimeInitPS(void) {
    g_startTime = NuGetCurrentTimeMilisecondsPS();
}

void NuTimeGetTicksPS(u32 *low, u32 *high) {
    struct timespec ts;
    u64 ticks;

    clock_gettime(CLOCK_REALTIME, &ts);

    ticks = ts.tv_nsec / 1000 + (u64)ts.tv_sec * 1000000;

    *high = ticks >> 32;
    *low = ticks;
}





void NuTimeGetTicksPerSecondPS(u32 *low, u32 *high) {
    i32 micros_per_sec;
    i32 remainder;

    micros_per_sec = 1000000;
    remainder = 0;

    *high = remainder;
    *low = micros_per_sec;
}

u64 NuGetCurrentTimeMilisecondsPS(void) {
    struct timespec ts;

    clock_gettime(0, &ts);

    return (u64)ts.tv_nsec + (u64)ts.tv_sec * 1000;
}
