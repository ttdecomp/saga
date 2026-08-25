#include "nu2api/nu3d/numtl.h"

#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nufile/nufile.h"

static i32 max_materials;
static NUMTL *material_list;
i32 numtl_renderplane;

NUMTL *numtl_defaultmtl2d;
NUMTL *numtl_defaultmtl3d;

void NuMtlInitEx(VARIPTR *buf, i32 mtl_count) {
    max_materials = mtl_count;
    material_list = (NUMTL *)ALIGN(buf->addr, 0x10);
    buf->addr = (usize)material_list + mtl_count * sizeof(NUMTL);
}

void DefaultMtl(NUMTL *mtl) {
    // The attribs bytes (0x40-0x44 of NUMTL; attribs lives at 0x40 as a flat
    // 8-byte bitfield word) are set with direct byte/word ops in the original,
    // and diffuse r/g/b (0x54/0x58/0x5c) + opacity (0x70) default to 1.0f.
    f32 one = 1.0f;
    u8 *b = (u8 *)&mtl->attribs;

    u8 v41 = b[1];
    v41 &= 0x0f;
    b[4] |= 0x10;
    u8 v40 = b[0];
    v40 &= 0xc0;
    v41 |= 0x20;
    v40 |= 0x10;
    b[1] = v41;
    u8 v42 = b[2];
    v42 &= 0x88;
    b[0] = v40;
    v42 |= 0x16;
    b[2] = v42;

    mtl->diffuse_color.r = one;
    mtl->diffuse_color.g = one;
    mtl->diffuse_color.b = one;
    mtl->opacity = one;

    u16 w42 = *(u16 *)&b[2];
    w42 &= 0x807f;
    w42 |= 0x3f80;
    *(u16 *)&b[2] = w42;
}

void NuShaderMtlDescInit(NUSHADERMTLDESC *desc) {
    i32 i;

    if (desc == NULL) {
        return;
    }

    memset(desc, 0, sizeof(NUSHADERMTLDESC));

    desc->unknown_24 = 1.0f;
    desc->byte4 |= 0x10;

    desc->flags = 0x1000;

    desc->diffuse_color[0] = RGBA_TO_NUCOLOUR32(0xff, 0x80, 0x80, 0xff);

    for (i = 0; i < 4; i++) {
        desc->tex_anim_data[i] = -1;
    }

    desc->vtx_desc.has_position = 1;
    desc->vtx_desc.has_diffuse = 1;
}

NUMTL *NuMtlCreate(i32 count) {
    i32 i;
    i32 j;
    NUMTL *mtl;
    NUMTL *next;

    next = NULL;

    for (i = 0; i < count; i++) {
        mtl = NULL;

        for (j = 0; j < max_materials; j++) {
            if (!material_list[j].is_used && material_list[j].display_list == NULL) {
                mtl = &material_list[j];
                break;
            }
        }

        memset(mtl, 0, sizeof(NUMTL));

        DefaultMtl(mtl);

        mtl->is_used = true;
        mtl->unknown_0_4 = true;
        mtl->renderplane = numtl_renderplane;
        mtl->attribs.unknown_6_128 = true;

        mtl->next = next;
        next = mtl;
    }

    NuMtlCreatePS(mtl, 0);

    return mtl;
}

void NuMtlUpdate(NUMTL *mtl) {
    NuMtlUpdatePS(mtl);
    mtl->version++;
}

void NuMtlAddEx(numtl_s *, i32) {
}

void NuMtlInsert(numtl_s *, i32) {
}

void NuMtlUpdatePS(numtl_s *) {
}

void NuMtlSetUVOffsetPS(numtl_s *, u32, float, float) {
}

void NuMtlDisableCulling() {
}
