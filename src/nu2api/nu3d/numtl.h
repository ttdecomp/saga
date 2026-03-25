#pragma once

#include "nu2api/nu3d/nudlist.h"
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

    u32 uv_mode : 1;

    u32 unknown_4_8 : 1;
    u32 unknown_4_16 : 1;
    u32 unknown_4_32 : 1;
    u32 unknown_4_64 : 1;
    u32 unknown_4_128 : 1;

    u32 unknown_5_1 : 1;
    u32 unknown_5_2 : 1;
    u32 unknown_5_4 : 1;
    u32 unknown_5_8_16 : 2;
    u32 unknown_5_32 : 1;

    u32 layers : 8;

    u32 unknown_6_64 : 1;
    u32 unknown_6_128 : 1;

    u32 padding : 8;
} NUMTLATTRIB;

typedef struct nuvertexdescriptor_s {
    union {
        struct {
            u32 has_position : 1;

            u32 unknown_0_2 : 1;

            u32 has_normal : 1;
            u32 has_packed_normal : 1;
            u32 has_tangent : 1;
            u32 has_packed_tangent : 1;
            u32 has_binormal : 1;
            u32 has_packed_binormal : 1;

            u32 has_diffuse : 1;

            u32 unknown_1_2_4 : 2;
            u32 tex_coord_mode : 3;
            u32 unknown_1_64 : 1;
            u32 unknown_1_128 : 1;

            u32 unknown_2_1 : 1;
            u32 unknown_2_2 : 1;

            u32 has_no_transform : 1;

            u32 unknown_2_8 : 1;
            u32 unknown_2_16 : 1;
            u32 unknown_2_32 : 1;
            u32 unknown_2_64 : 1;
            u32 unknown_2_128 : 1;

            u32 unknown_3_1_2_3 : 3;
            u32 has_half_uvs : 1;
        };

        u32 flags;
    };
} NUVERTEXDESCRIPTOR;

typedef struct nushadermtldesc_s {
    u32 flags;

    i32 diffuse_map_tex_id[4];
    NUCOLOUR32 diffuse_color[4];

    f32 unknown_24;
    u8 unknown_28[0x80];

    u8 unknown_a8;

    u8 unknown_a9[0x63];

    i32 tex_anim_data[4];
    f32 tex_anim_offsets[4][2];

    NUVERTEXDESCRIPTOR vtx_desc;

    char filler4[0x79]; // 0x13F-0x1B8
    u8 byte4;           // 0x1b9
    char filler5[0x4E]; // 0x1BA-0x207
} NUSHADERMTLDESC;

typedef struct numtl_s {
    i16 is_used : 1;
    i16 unknown_0_2 : 1;
    i16 unknown_0_4 : 1;
    i16 unknown_0_8 : 1;

    i16 renderplane : 8;

    struct numtl_s *unknown_04;

    // Type uncertain.
    i32 unknown_08;

    char filler1[0x24];

    struct numtl_s *next;

    u8 unknown_34[0x8];

    NUDISPLAYLIST *display_list;

    NUMTLATTRIB attribs;

    char filler2[0xc]; // 0x48-0x53

    NUCOLOUR3 diffuse_color;

    char filler3[0x10]; // 0x60-0x6F

    f32 opacity;

    i16 tex_id;
    i16 sort_pri;

    // Types uncertain.
    i32 unknown_78;
    i32 unknown_7c;

    f32 delta_u;
    f32 delta_v;

    u8 unknown_88[0x2c];

    NUSHADERMTLDESC shader_desc;

    char filler4[0x8];
} NUMTL;

#ifdef __cplusplus

void DefaultMtl(NUMTL *mtl);
void NuMtlCreatePS(NUMTL *mtl, int is_3d);

extern "C" {
#endif
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;
    void NuMtlAnimate(float speed);
    void NuMtlInitEx(VARIPTR *buf, i32 mtl_count);

    void NuShaderMtlDescInit(NUSHADERMTLDESC *desc);
    void NuMtlSetShaderDescPS(NUMTL *mtl, NUSHADERMTLDESC *desc);
    NUMTL *NuMtlCreate(int count);
    void NuMtlUpdate(NUMTL *mtl);
#ifdef __cplusplus
}

void NuMtlInitExPS(VARIPTR *buf);

#endif
