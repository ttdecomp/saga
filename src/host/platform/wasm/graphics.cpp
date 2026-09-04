#include "host/platform/compressed_texture.hpp"

#include <GLES2/gl2.h>

#include <cstring>
#include <vector>

#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nuandroid/ios_graphics.h"

void NuIOS_AllocateSystemFramebuffers(void) {
    BeginCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 106);

    memset(g_lastBound2DTexIds, 0, sizeof(g_lastBound2DTexIds));
    memset(g_lastBoundCubeTexIds, 0, sizeof(g_lastBoundCubeTexIds));

    glGenFramebuffers(1, &g_earlyColorFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_earlyColorFramebuffer);

    glGenTextures(1, &g_earlyColorTexture);
    glBindTexture(GL_TEXTURE_2D, g_earlyColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_backingWidth, g_backingHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_earlyColorTexture, 0);

    GLuint depth_buffer = 0;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, g_backingWidth, g_backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    g_defaultFramebuffer = 0;
    g_currentFramebuffer = g_earlyColorFramebuffer;

    EndCriticalSectionGL("i:/SagaTouch-Android_9176564/nu2api.saga/nuandroid/ios_graphics.cpp", 260);
}

extern "C" void NuIOS_UploadCompressedTexture(GLenum target, GLint level, GLenum internal_format, GLsizei width,
                                              GLsizei height, GLint border, GLsizei image_size, const void *data) {
    std::vector<u8> rgba;
    if (HostDecodeCompressedTexture(internal_format, width, height, image_size, data, rgba)) {
        glTexImage2D(target, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    } else if (internal_format != 0x8d64 && internal_format != 0x8c00 && internal_format != 0x8c02) {
        glCompressedTexImage2D(target, level, internal_format, width, height, border, image_size, data);
    }
}

bool NuIOS_TextureFormatSupported(i32 format) {
    return format == NUTEX_ETC1 || g_renderDevice.enabled_extensions[format];
}

GLenum NuIOS_PlatformVertexAttributeType(GLenum type) {
    // WebGL 2 exposes half floats under the core GL_HALF_FLOAT enum rather
    // than the OES token stored in the original Android vertex formats.
    return type == 0x8d61 ? 0x140b : type;
}

isize NuIOS_PlatformPrepareImmediateVertexData(isize data_address, usize data_size) {
    static GLuint immediate_vertex_buffer = 0;
    if (immediate_vertex_buffer == 0) {
        glGenBuffers(1, &immediate_vertex_buffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, immediate_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, reinterpret_cast<const void *>(data_address), GL_STREAM_DRAW);
    return 0;
}
