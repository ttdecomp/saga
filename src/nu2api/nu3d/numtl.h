#pragma once

#include "nu2api/nu3d/nurndr.h"

typedef struct numtlattrib_s {
    u32 alpha_mode : 4;
    u32 filter_mode : 2;

    u32 unknown_0_64_128 : 2;

    u32 unknown_1_1_2 : 2;
    u32 unknown_1_4_8 : 2;

    u32 cull_mode : 2;
    u32 z_mode : 2;

    u32 unknown_2_1_2 : 2;
    u32 unknown_2_4 : 1;
    u32 unknown_2_8 : 1;
    u32 alpha_test : 3;
    u32 alpha_ref : 8;

    u32 alpha_fail : 2;

    u32 unknown_4 : 30;
} NUMTLATTRIB;

typedef struct nuvertexdescriptor_s {
    union {
        struct {
            u32 unknown_0 : 8;
            u32 unknown_1 : 8;
            u32 unknown_2 : 8;
            u32 unknown_3_1_2_3 : 3;
            u32 has_half_uvs : 1;
        };

        u32 flags;
    };
} NUVERTEXDESCRIPTOR;

typedef struct nushadermtldesc_s {
    u32 flags;           // 0x0
    i32 tex_id;          // 0x4
    char filler1[0xC];   // 0x8-0x13
    u32 mask;            // 0x14
    char filler2[0xC];   // 0x18-0x23
    float value;         // 0x24
    char filler3[0x114]; // 0x28-0x13B

    NUVERTEXDESCRIPTOR vtx_desc;

    char filler4[0x79];  // 0x13F-0x1B8
    u8 byte4;            // 0x1b9
    char filler5[0x4E];  // 0x1BA-0x207
} NUSHADERMTLDESC;

typedef struct numtl_s {
    u8 unknown_00[0x8];

    // Type uncertain.
    i32 unknown_08;

    char filler1[0x34];

    NUMTLATTRIB attribs;

    char filler2[0xc]; // 0x48-0x53

    NUCOLOUR3 diffuse_color;

    char filler3[0x10]; // 0x60-0x6F

    f32 opacity;

    i16 tex_id;
    i16 sort_pri;

    u8 unknown_78[0x3c];

    NUSHADERMTLDESC shader_desc;

    char filler4[0x8];
} NUMTL;

#ifdef __cplusplus

void DefaultMtl(NUMTL *mtl);
void NuMtlCreatePS(NUMTL *mtl, int unk_flag);

extern "C" {
#endif
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;

    void NuMtlInitEx(VARIPTR *buf, i32 max_mtls);

    void NuShaderMtlDescInit(NUSHADERMTLDESC *shader_mtl_desc);
    void NuMtlSetShaderDescPS(NUMTL *mtl, NUSHADERMTLDESC *shader_mtl_desc);
    NUMTL *NuMtlCreate(int count);
    void NuMtlUpdate(NUMTL *mtl);
#ifdef __cplusplus
}
#endif
