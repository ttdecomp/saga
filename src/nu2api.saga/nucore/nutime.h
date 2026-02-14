#pragma once

#include "nu2api.saga/nucore/common.h"

typedef struct nutime_s {
    u32 low;
    i32 high;
} NUTIME;

#ifdef __cplusplus
void NuTimeGetTicksPS(u32 *low, u32 *high);
u64 NuGetCurrentTimeMilisecondsPS(void);

extern "C" {
#endif
    void NuTimeInitPS(void);

    void NuTimeGet(NUTIME *t);

    f32 NuTimeSeconds(NUTIME *t);

    void NuTimeSub(NUTIME *t, NUTIME *a, NUTIME *b);
#ifdef __cplusplus
}
#endif
