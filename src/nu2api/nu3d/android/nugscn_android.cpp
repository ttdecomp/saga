#include "nu2api/nu3d/android/nugscn_android.h"

#include <GLES2/gl2.h>

#include "legoapi/character.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numath.h"

i32 g_vaoLifetimeMutex = -1;
u32 g_lastBoundVAO;

static void NuIOSBindVAO(u32 vao_handle) {
    if (vao_handle != g_lastBoundVAO) {
        g_lastBoundVAO = vao_handle;
    }
}

static u32 UploadDataToGLBuffer(NUFILE file, u32 size, GLenum target, GLuint *gl_buf, VARIPTR *buf, VARIPTR buf_end) {
    u32 chunk_limit;
    u32 n;
    u32 largest_chunk;
    u32 chunk_size;
    u32 buf_size;
    u32 max_chunk_size;

    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x56);

    glGenBuffers(1, gl_buf);

    NuIOSBindVAO(0);

    glBindBuffer(target, *gl_buf);
    glBufferData(target, size, 0, GL_STATIC_DRAW);

    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x5d);

    if (bgProcIsBgThread()) {
        NuIOS_YieldThread();
    }

    chunk_limit = g_loadingCharacterInHub != 0 ? 0x4000 : 0x10000;
    buf_size = buf_end.char_ptr - buf->char_ptr;

    max_chunk_size = NuMin(chunk_limit, buf_size);

    largest_chunk = 0;

    for (n = 0; n < size; n += chunk_size) {
        chunk_size = NuMin(max_chunk_size, size - n);

        NuFileRead(file, buf->void_ptr, chunk_size);

        BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x73);

        NuIOSBindVAO(0);

        glBindBuffer(target, *gl_buf);

        if (chunk_size == size) {
            glBufferData(target, chunk_size, buf->void_ptr, GL_STATIC_DRAW);
        } else {
            glBufferSubData(target, n, chunk_size, buf->void_ptr);
        }

        EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nu3d/android/nugscn_android.c", 0x80);

        if (bgProcIsBgThread()) {
            NuIOS_YieldThread();
        }

        largest_chunk = NuMax(largest_chunk, chunk_size);
    }

    return largest_chunk;
}
