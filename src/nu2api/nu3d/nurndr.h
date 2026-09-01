#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

#define NURNDR_STREAM_MAX_BUFFERS 2

typedef struct rndrstream_s RNDRSTREAM;

typedef i32 NUCOLOUR32;

#define RGBA_TO_NUCOLOUR32(r, g, b, a) ((u8)(a) << 0x18) | ((u8)(b) << 0x10) | ((u8)(g) << 0x08) | ((u8)(r) << 0x00);

typedef struct nucolour3_s {
    f32 r;
    f32 g;
    f32 b;
} NUCOLOUR3;

typedef struct nucolour4_s {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} NUCOLOUR4;

typedef struct NURND_SHADOW_s {
    NUVEC position;
    f32 radius;
    u16 opacity;
    u16 x_rotation;
    u16 y_rotation;
    u16 z_rotation;
} NURND_SHADOW_s;

DECOMP_ASSERT(sizeof(NURND_SHADOW_s) == 0x18, "NURND_SHADOW_s ABI");

extern i32 g_backingWidth;
extern i32 g_backingHeight;

#ifdef __cplusplus

void NuRndrStreamInit(i32 stream_buffer_size, VARIPTR *buffer);

extern "C" {
#endif
    extern i32 nurndr_pixel_width;
    extern i32 nurndr_pixel_height;
    extern i32 NuRndrShadowCnt;
    extern NURND_SHADOW_s NuRndrShadPolDat[128];

    void NuRndrInitEx(i32 stream_buffer_size, VARIPTR *buffer);

    i32 NuRndrSetViewMtx(NUMTX *vpcs_mtx, NUMTX *viewport_vpc_mtx, NUMTX *scissor_vpc_mtx);
    void NuRndrStateUpdateCameraState(void);

    i32 NuRndrSetAmbientLightPS(const NUCOLOUR3 *colour);
    i32 NuRndrSetDirectionalLightsPS(const NUVEC *dir0, const NUCOLOUR3 *colour0, const NUVEC *dir1,
                                     const NUCOLOUR3 *colour1, const NUVEC *dir2, const NUCOLOUR3 *colour2);
    i32 NuRndrSetFxMtx(NUMTX *matrix);
    void NuRndrSetSpecularLightPS(const NUVEC *direction, const NUCOLOUR4 *intensity);
    void NuRndrStartReflectionRender(i32 clear_depth);
    void NuRndrEndReflectionRender(void);

    void FaceYDirStream(i32 y_angle);
    void NuRndrAddShadow(NUVEC *position, f32 radius, i32 opacity, i32 x_rotation, i32 y_rotation, i32 z_rotation);
#ifdef __cplusplus
}
#endif
