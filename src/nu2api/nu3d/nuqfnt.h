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

typedef struct vufnt_android_s {
    u32 colour;
    f32 x;
    f32 y;
    f32 z;
    NUMTX mtx;
    f32 x_scale;
    f32 y_scale;
} VUFNT_ANDROID;

typedef struct vucharidx_s {
    u16 unicode;
    u16 index;
} VUCHARIDX;

typedef struct vufntchar_s {
    f32 x;
    f32 y;
    f32 width;
} VUFNTCHAR;

typedef struct vufnt_s {
    u8 filler0[6];
    u16 flags;
    i32 size;
    i32 glyph_count;
    i32 unicode_count;
    f32 height;

    f32 baseline;

    f32 space_width;

    i32 mode;

    f32 ic_gap; // 0x24

    char filler2[0x4]; // 0x28-0x2B

    f32 *x_scale;
    f32 *y_scale;

    VUFNTCHAR *glyphs; // 0x34

    VUCHARIDX *unicode_map; // 0x38

    VARIPTR *hdr; // 0x3c

    NUMTL *mtl; // 0x40

    u32 *color_abgr; // 0x44

    VUFNT_ANDROID *platform_data; // 0x48
} VUFNT;

typedef void NUQFNT;

#ifdef __cplusplus

i32 NuQFntReadPS(VUFNT *font, i32 tex_id, i32 flags, i32 render_plane, VARIPTR *buf, VARIPTR buf_end);

extern "C" {
#endif
    extern NUQFNT_CSMODE NuQFntCSMode;

    extern f32 qfnt_rezscale_w;
    extern f32 qfnt_rezscale_h;

    extern f32 qfnt_offscale_x;
    extern f32 qfnt_offscale_y;

    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;
    extern f32 nuqfnt_space_width;
    extern u32 NuQFntMode;

    void NuQFntInit(VARIPTR *buf, VARIPTR buf_end);

    NUQFNT *NuQFntRead(char *filename, VARIPTR *buf, VARIPTR buf_end);
    NUQFNT *NuQFntReadBuffer(VARIPTR *font, VARIPTR *buf, VARIPTR buf_end);
    VUFNT *NuQFntDuplicate(VUFNT *font, i32 flags, i32 render_plane, VARIPTR *buf, VARIPTR *buf_end);

    NUQFNT_CSMODE NuQFntSetCoordinateSystem(NUQFNT_CSMODE mode);
    void NuQFntSetICGap(NUQFNT *font, float ic_gap);
    void NuQFntSetJustifiedTolerances(f32 squash, f32 stretch);
    void NuQFntSetMtx(NUQFNT *font, NUMTX *mtx);
    void NuQFntSetMtxRS(RNDRSTREAM *stream, NUQFNT *font, NUMTX *mtx);
    void NuQFntSetRS(RNDRSTREAM *stream, NUQFNT *font);
    void NuQFntSetColourRS(RNDRSTREAM *stream, NUQFNT *font, u32 colour);
    void NuQFntSetScaleRS(RNDRSTREAM *stream, NUQFNT *font, f32 x_scale, f32 y_scale);
    void NuQFntMoveRS(RNDRSTREAM *stream, NUQFNT *font, f32 x, f32 y, f32 z);
    f32 NuQFntHeight(NUQFNT *font);
    f32 NuQFntPrintLenW(NUQFNT *font, u16 *text);
    f32 NuQFntPrintLenU(NUQFNT *font, char *text);
    void NuQFntPrintCharW(NUQFNT *font, u16 *text, u32 flags);
    void NuQFntPrintRSW(RNDRSTREAM *stream, NUQFNT *font, u16 *text, u32 flags);
    void NuQFntUTF8toQCode(NUQFNT *font, char *text, u16 *encoded);

    f32 NuQFntBaseline(NUQFNT *font);
    void NuQFntSet(NUQFNT *font);
    void NuQFntSetColour(NUQFNT *font, u32 colour);
    void NuQFntSetScale(NUQFNT *font, f32 x_scale, f32 y_scale);
    void NuQFntMove(NUQFNT *font, f32 x, f32 y, f32 z);
    void NuQFntPrintW(NUQFNT *font, u16 *text);
    void NuQFntPrintU(NUQFNT *font, char *text);
    void NuQFntPushPrintMode(u32 mode);
    void NuQFntPopPrintMode(void);
    void NuQFntSetSpaceWidth(NUQFNT *font, f32 width);
    u16 NuQFntEncodeUnicodeChar(NUQFNT *font, u16 character);
    NUQFNT *NuQFntLoadPtr(char *path, char *name, i32 flags, i32 render_plane, VARIPTR *buf, VARIPTR *buf_end);
    f32 NuQFntPrintJustifiedW(NUQFNT *font, u16 *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 width, f32 line_spacing,
                              u32 colour, NUMTX *mtx);
#ifdef __cplusplus
}
#endif
