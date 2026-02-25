#include "nu2api/nu3d/numtl.h"

#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nufile/nufile.h"

static int max_materials;
static NUMTL *material_list;
int numtl_renderplane;

NUMTL *numtl_defaultmtl2d;
NUMTL *numtl_defaultmtl3d;

void NuMtlInitEx(VARIPTR *buf, i32 max_mtls) {
    // iVar2 = AndroidOBBUtils::LookupPackagePath(path, 1);
    char *path = "res/main.1060.com.wb.lego.tcs.obb";

    NUDATHDR *dat = NuDatOpen(path, buf, 0);
    NuDatSet(dat);

    i32 size = NuFileLoadBuffer("stuff\\text\\badwords.txt", buf->void_ptr, 0x100000);
    LOG_DEBUG("Loaded badwords.txt, size=%d", size);

    // replace \n with ,
    for (i32 i = 0; i < size; i++) {
        if (buf->char_ptr[i] == '\r') {
            buf->char_ptr[i] = ',';
        } else if (buf->char_ptr[i] == '\n') {
            buf->char_ptr[i] = ' ';
        }
    }

    LOG_INFO("%*s", size, buf->char_ptr);
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
    UNIMPLEMENTED();
}

void NuMtlUpdate(NUMTL *mtl) {
    UNIMPLEMENTED();
}
