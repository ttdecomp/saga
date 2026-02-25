#include "nu2api/nu3d/nuqfnt.h"

#include <string.h>

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/numtx.h"

typedef struct vufnt_android_s {
    u32 color_abgr;

    // Fields uncertain.
    u8 unknown_04[0xc];

    NUMTX mtx;

    f32 x_scale;
    f32 y_scale;
} VUFNT_ANDROID;

i32 NuQFntReadPS(VUFNT *font, int tex_id, int flags, int render_plane, VARIPTR *buf, VARIPTR buf_end) {
    NUMTL *mtl;
    NUSHADERMTLDESC shader_desc;
    VUFNT_ANDROID *platform_font;

    mtl = NuMtlCreate(1);
    font->mtl = mtl;

    mtl->diffuse_color.r = 1.0f;
    mtl->diffuse_color.g = 1.0f;
    mtl->diffuse_color.b = 1.0f;

    mtl->opacity = 1.0f;

    mtl->attribs.alpha_mode = 1;
    mtl->attribs.unknown_2_4 = 1;
    mtl->tex_id = tex_id;
    mtl->attribs.unknown_1_1_2 = 1;
    mtl->attribs.unknown_1_4_8 = 1;
    mtl->attribs.cull_mode = 2;
    mtl->attribs.z_mode = 3;

    memset(&shader_desc, 0, sizeof(NUSHADERMTLDESC));

    shader_desc.tex_id = tex_id;
    shader_desc.byte4 = 0x10;
    shader_desc.flags = 0x1000;
    shader_desc.mask = 0xffffffff;

    shader_desc.vtx_desc.unknown_0 = 1;
    shader_desc.vtx_desc.unknown_1 = 9;
    shader_desc.vtx_desc.unknown_2 = 4;

    shader_desc.value = 1.0f;

    NuMtlSetShaderDescPS(mtl, &shader_desc);
    NuMtlUpdate(font->mtl);

    font->platform_data = (void *)ALIGN(buf->addr, 0x10);
    buf->addr = ALIGN(buf->addr, 0x10) + sizeof(VUFNT_ANDROID);

    platform_font = (VUFNT_ANDROID *)font->platform_data;
    font->x_scale = &platform_font->x_scale;
    font->y_scale = &platform_font->y_scale;
    font->color_abgr = &platform_font->color_abgr;

    font->hdr = NULL;

    return 0;
}

void NuQFntSetMtxRS(RNDRSTREAM *stream, NUQFNT *font, NUMTX *mtx) {
    VUFNT *vufnt;
    VUFNT_ANDROID *platform_font;

    vufnt = (VUFNT *)font;
    platform_font = (VUFNT_ANDROID *)vufnt->platform_data;

    platform_font->mtx = *mtx;
}
