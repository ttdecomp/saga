#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

typedef struct nuviewport_s {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    f32 min_z;
    f32 max_z;
    f32 center_x;
    f32 center_y;
    f32 clip_min_x;
    f32 clip_min_y;
    f32 clip_max_x;
    f32 clip_max_y;
    f32 clip_width;
    f32 clip_height;
    f32 scissor_width;
    f32 scissor_height;
} NUVIEWPORT;

#ifdef __cplusplus

void NuPs2GetViewport(NUVIEWPORT *vp);
void NuVpSetScalingMtx(void);

extern "C" {
#endif

    void NuVpRestore(void);
    void NuVpGetScalingMtx(NUMTX *dest);
    void NuVpUpdate(void);
    void NuVpInit(void);

#ifdef __cplusplus
}
#endif
