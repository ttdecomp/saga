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

typedef struct nuinternalscene_s {
    i32 field_00;         // 0x00 (Index 0)
    i32 field_04;         // 0x04 (Index 1)
    i32 flags;            // 0x08 (Index 2)
    i32 background_color; // 0x0C (Index 3)
    float field_10;       // 0x10 (Index 4)
    float vp_pos_x;       // 0x14 (Index 5)
    float vp_pos_y;       // 0x18 (Index 6)
    float vp_size_w;      // 0x1C (Index 7)
    float vp_size_h;      // 0x20 (Index 8)

    i32 field_24; // 0x24 (Index 9)
    i32 field_28; // 0x28 (Index 10)

    i32 _pad2C[3]; // 0x2C (Indices 11-13)

    i32 field_38; // 0x38 (Index 14)
    i32 field_3C; // 0x3C (Index 15)
    i32 field_40; // 0x40 (Index 16)

    i32 _pad44; // 0x44 (Index 17)

    i32 field_48; // 0x48 (Index 18)
    i32 field_4C; // 0x4C (Index 19)

    i32 _pad50[2]; // 0x50 (Indices 20-21)

    i32 field_58; // 0x58 (Index 22)

    i32 _pad5C[20]; // 0x5C (Indices 23-42)

    i32 field_AC; // 0xAC (Index 43)

    i32 _padB0[5]; // 0xB0 (Indices 44-48)

    i32 field_C4; // 0xC4 (Index 49)

    i32 _padC8[6]; // 0xC8 (Indices 50-55)

    i32 field_E0; // 0xE0 (Index 56)
    i32 field_E4; // 0xE4 (Index 57)

    i32 _padE8[35]; // 0xE8 (Indices 58-92)

    i32 field_174; // 0x174 (Index 93)
    i32 field_178; // 0x178 (Index 94)

    i32 _pad17C[3]; // 0x17C (Indices 95-97)

    i32 field_188; // 0x188 (Index 98)

    i32 _pad18C[34]; // 0x18C (Indices 99-132)

    i32 field_214; // 0x214 (Index 133)

    i32 _pad218[2]; // 0x218 (Indices 134-135)
} NUINTERNALSCENE;

#ifdef __cplusplus

    void NuRndrStreamInit(i32 stream_buffer_size, VARIPTR *buffer);

extern "C" {
#endif
    extern i32 nurndr_pixel_width;
    extern i32 nurndr_pixel_height;

    extern i32 g_backingWidth;
    extern i32 g_backingHeight;

    void NuRndrInitEx(i32 stream_buffer_size, VARIPTR *buffer);

    i32 NuRndrSetViewMtx(NUMTX *vpcs_mtx, NUMTX *viewport_vpc_mtx, NUMTX *scissor_vpc_mtx);

    void FaceYDirStream(i32 y_angle);
    int NuRndrSwapScreen();
    void NuRndrClear(int a1, int a2, float a3);
    i32 NuHasError(void);
    i32 NuFrameEnd(void);
    int NuRndrBeginScene();
#ifdef __cplusplus
}
#endif
