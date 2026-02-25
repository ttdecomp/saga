#include "decomp.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"

void NuMtlCreatePS(NUMTL *mtl, i32 is_3d) {
    NuShaderMtlDescInit(&mtl->shader_desc);

    if (mtl->tex_id > 0) {
        mtl->shader_desc.diffuse_map_tex_id[0] = mtl->tex_id;
        mtl->shader_desc.unknown_a8 = 1;
        mtl->shader_desc.vtx_desc.unknown_1_8_16_32 = 1;
    } else {
        mtl->shader_desc.diffuse_color[0] = RGBA_TO_NUCOLOUR32(
            mtl->diffuse_color.r * 128.0f, mtl->diffuse_color.g * 128.0f, mtl->diffuse_color.b * 128.0f, 0xff);
    }

    if (!is_3d) {
        mtl->shader_desc.vtx_desc.has_no_transform = 1;
    }
}

void NuMtlSetShaderDescPS(NUMTL *mtl, NUSHADERMTLDESC *desc) {
    UNIMPLEMENTED();
}
