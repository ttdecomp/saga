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

typedef struct nuvpregion_s {
    f32 source_x;
    f32 source_y;
    f32 source_width;
    f32 source_height;
    f32 dest_x;
    f32 dest_y;
    f32 dest_width;
    f32 dest_height;
    f32 width_scale;
    f32 height_scale;
    f32 x_offset;
    f32 y_offset;
    f32 projection_x_scale;
    f32 projection_y_scale;
    f32 projection_x_offset;
    f32 projection_y_offset;
} NUVPREGION;

#ifdef __cplusplus

void NuPs2GetViewport(NUVIEWPORT *vp);
void NuVpSetScalingMtx(void);

extern NUVPREGION g_NuVpRegion;
extern i32 PS2_VREZ_H;
extern i32 PS2_VREZ_W;

extern "C" {
#endif

    void NuVpRestore(void);
    NUVIEWPORT *NuVpGetCurrentViewport(void);
    void NuVpGetPosition2(f32 *x, f32 *y);
    void NuVpGetSize2(f32 *width, f32 *height);
    void NuVpGetScalingMtx(NUMTX *dest);
    void NuVpUpdate(void);
    void NuVpInit(void);
    void NuVpResetRegions(void);
    void NuVpSetRegions(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x, f32 dest_y,
                        f32 dest_right, f32 dest_bottom);

#ifdef __cplusplus
}
#endif
