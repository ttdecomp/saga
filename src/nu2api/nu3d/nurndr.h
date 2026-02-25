#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

#define NURNDR_STREAM_MAX_BUFFERS 2

typedef struct rndrstream_s {
} RNDRSTREAM;

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

#ifdef __cplusplus

void NuRndrStreamInit(i32 stream_buffer_size, VARIPTR *buffer);

extern "C" {
#endif
    extern i32 nurndr_pixel_width;
    extern i32 nurndr_pixel_height;

    void NuRndrInitEx(i32 stream_buffer_size, VARIPTR *buffer);

    i32 NuRndrSetViewMtx(NUMTX *vpcs_mtx, NUMTX *viewport_vpc_mtx, NUMTX *scissor_vpc_mtx);

    void FaceYDirStream(i32 y_angle);
#ifdef __cplusplus
}
#endif
