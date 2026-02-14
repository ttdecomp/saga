#include "nu2api.saga/nu3d/numtl.h"

#include "decomp.h"

#include "nu2api.saga/nufile/nufile.h"

static int max_materials;
static NUMTL *material_list;
int numtl_renderplane;

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

void NuMtlCreatePS(NUMTL *mtl, int unk_flag) {
    UNIMPLEMENTED();
}

void NuShaderMtlDescInit(NUSHADERMTLDESC *shader_mtl_desc) {
    UNIMPLEMENTED();
}

NUMTL *NuMtlCreate(int count) {
    UNIMPLEMENTED();
}

void NuMtlUpdate(NUMTL *mtl) {
    UNIMPLEMENTED();
}

void NuMtlSetShaderDescPS(NUMTL *mtl, NUSHADERMTLDESC *shader_mtl_desc) {
    UNIMPLEMENTED();
}
