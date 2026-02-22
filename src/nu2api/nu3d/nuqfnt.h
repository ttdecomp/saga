#pragma once

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/common.h"

typedef enum {
    NUQFNT_CSMODE_UNINITIALISED = 0,
    NUQFNT_CSMODE_PS2 = 1,
    NUQFNT_CSMODE_PIXEL = 2,
    NUQFNT_CSMODE_NORMALISED = 3,
    NUQFNT_CSMODE_ABSOLUTE = 4,
    NUQFNT_CSMODE_CNT = 5,
} NUQFNT_CSMODE;

typedef struct vufnt_s {
    char filler1[0x18];

    f32 baseline;

    char unknown_1c[0x8];

    f32 ic_gap; // 0x24

    char filler2[0x4]; // 0x28-0x2B

    f32 *x_scale;
    f32 *y_scale;

    char filler3[0x8]; // 0x34-0x3B

    VARIPTR *hdr; // 0x3c

    NUMTL *mtl; // 0x40

    u32 *color_abgr; // 0x44

    void *platform_data; // 0x48
} VUFNT;

typedef void NUQFNT;

#ifdef __cplusplus

i32 NuQFntReadPS(VUFNT *font, int tex_id, int flags, int render_plane, VARIPTR *buf, VARIPTR buf_end);

extern "C" {
#endif
    extern NUQFNT_CSMODE NuQFntCSMode;

    extern f32 qfnt_rezscale_w;
    extern f32 qfnt_rezscale_h;

    extern f32 qfnt_offscale_x;
    extern f32 qfnt_offscale_y;

    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;

    void NuQFntInit(VARIPTR *buf, VARIPTR buf_end);

    NUQFNT *NuQFntRead(char *filename, VARIPTR *buf, VARIPTR buf_end);
    NUQFNT *NuQFntReadBuffer(VARIPTR *font, VARIPTR *buf, VARIPTR buf_end);

    NUQFNT_CSMODE NuQFntSetCoordinateSystem(NUQFNT_CSMODE mode);
    void NuQFntSetICGap(NUQFNT *font, float ic_gap);
    void NuQFntSetJustifiedTolerances(f32 squash, f32 stretch);
    void NuQFntSetMtx(NUQFNT *font, NUMTX *mtx);
    void NuQFntSetMtxRS(RNDRSTREAM *stream, NUQFNT *font, NUMTX *mtx);

    f32 NuQFntBaseline(NUQFNT *font);
#ifdef __cplusplus
}
#endif
