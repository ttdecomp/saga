#include "nu2api/nu3d/numtl.h"

#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nufile/nufile.h"

static int max_materials;
static NUMTL *material_list;
int numtl_renderplane;

NUMTL *numtl_defaultmtl2d;
NUMTL *numtl_defaultmtl3d;

void NuMtlInitEx(VARIPTR *buf, i32 mtl_count) {
    max_materials = mtl_count;
    material_list = (NUMTL *)ALIGN(buf->addr, 0x10);
    buf->addr = (usize)material_list + mtl_count * sizeof(NUMTL);
}

void DefaultMtl(NUMTL *mtl) {
    UNIMPLEMENTED();
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
    UNIMPLEMENTED();
}
