#include <GLES2/gl2.h>

#include <cstring>
#include <vector>

#include "globals.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"

extern i32 pause_rt;
extern NUMTL *pause_rndr_mtl;
extern i32 pause_rndr_on;

void InitStillRender(variptr_u *, variptr_u) {
    static NUNATIVETEX nativePauseTex;

    pause_rt = NuTexGenTexture(&nativePauseTex);
    nativePauseTex.ref_count = 1;
    memset(nativePauseTex.checksum, 0, sizeof(nativePauseTex.checksum));
    nativePauseTex.width = g_backingWidth;
    nativePauseTex.height = g_backingHeight;
    nativePauseTex.image_data = nullptr;
    nativePauseTex.size = 0;

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/legoapi.saga/screen.cpp", 0x561);
    glGenTextures(1, &nativePauseTex.platform.gl_tex);
    glActiveTexture(GL_TEXTURE0);
    g_currentTexUnit = 0;
    glBindTexture(GL_TEXTURE_2D, nativePauseTex.platform.gl_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // The first title transition deliberately uses this target before any
    // backbuffer copy. Android's driver exposes the fresh target as opaque
    // black; Mesa zeroes its alpha as well, which makes the original wipe
    // invisible under alpha blending. Reproduce the Android allocation state
    // in the host backend without changing the decompiled target function.
    std::vector<u32> opaqueBlack(static_cast<usize>(g_backingWidth) * static_cast<usize>(g_backingHeight), 0xff000000u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 opaqueBlack.data());
    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/legoapi.saga/screen.cpp", 0x56d);

    pause_rndr_mtl = NuMtlCreate3D(1);
    pause_rndr_mtl->sort_pri = 0x7fff;
    pause_rndr_mtl->shader_desc.flags = 0x1000;
    pause_rndr_mtl->opacity = 1.0f;
    u8 *attrib = reinterpret_cast<u8 *>(&pause_rndr_mtl->attribs);
    attrib[1] = static_cast<u8>((attrib[1] & 0x30) | 0xc5);
    attrib[2] = static_cast<u8>((attrib[2] & 0xfc) | 6);
    attrib[0] = static_cast<u8>((attrib[0] & 0xc0) | 0x11);
    attrib[1] = 0xe5;
    pause_rndr_mtl->tex_id = static_cast<i16>(pause_rt);
    pause_rndr_mtl->shader_desc.diffuse_color[0] = -1;
    pause_rndr_mtl->shader_desc.unknown_a8 = 1;
    pause_rndr_mtl->shader_desc.vtx_desc.flags |= 0x40800;
    NuMtlUpdate(pause_rndr_mtl);
    pause_rndr_on = 0;
}
