#include "nu2api/nucore/common.h"

#include "host/platform/graphics.hpp"
#ifdef __EMSCRIPTEN__
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nutex_ios_ex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#endif

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <vector>

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <dlfcn.h>
#endif

#ifdef __EMSCRIPTEN__
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
#endif

// Frame capture belongs at the external EGL boundary. It must not replace the
// game's render device, framebuffer allocation, or presentation path.
namespace {
    pthread_mutex_t host_readback_mutex = PTHREAD_MUTEX_INITIALIZER;
    std::vector<u8> host_readback_pixels;
    i32 host_readback_width;
    i32 host_readback_height;
    std::atomic<bool> host_readback_enabled{false};

    void host_capture_current_surface(EGLDisplay display, EGLSurface surface) {
        if (!host_readback_enabled.load(std::memory_order_relaxed)) {
            return;
        }

        EGLint width = 0;
        EGLint height = 0;
        if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &height) || width <= 0 || height <= 0) {
            return;
        }

        std::vector<u8> pixels(static_cast<usize>(width) * static_cast<usize>(height) * 4);
        GLint old_pack_alignment = 4;
        glGetIntegerv(GL_PACK_ALIGNMENT, &old_pack_alignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glPixelStorei(GL_PACK_ALIGNMENT, old_pack_alignment);

        pthread_mutex_lock(&host_readback_mutex);
        host_readback_pixels.swap(pixels);
        host_readback_width = width;
        host_readback_height = height;
        pthread_mutex_unlock(&host_readback_mutex);
    }
} // namespace

void HostSetReadbackEnabled(bool enabled) {
    host_readback_enabled.store(enabled, std::memory_order_relaxed);
}

i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba) {
    pthread_mutex_lock(&host_readback_mutex);
    const i32 width = std::min<i32>(host_readback_width, static_cast<i32>(max_w));
    const i32 height = std::min<i32>(host_readback_height, static_cast<i32>(max_h));
    if (width <= 0 || height <= 0 || rgba == nullptr) {
        pthread_mutex_unlock(&host_readback_mutex);
        return 0;
    }

    for (i32 y = 0; y < height; ++y) {
        memcpy(rgba + static_cast<usize>(y) * width * 4,
               host_readback_pixels.data() + static_cast<usize>(y) * host_readback_width * 4,
               static_cast<usize>(width) * 4);
    }
    pthread_mutex_unlock(&host_readback_mutex);
    return width * 1000 + height;
}

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
extern "C" EGLBoolean eglSwapBuffers(EGLDisplay display, EGLSurface surface) {
    typedef EGLBoolean (*HostEglSwapBuffers)(EGLDisplay, EGLSurface);
    static HostEglSwapBuffers host_real_egl_swap_buffers =
        reinterpret_cast<HostEglSwapBuffers>(dlsym(RTLD_NEXT, "eglSwapBuffers"));

    host_capture_current_surface(display, surface);
    return host_real_egl_swap_buffers != nullptr ? host_real_egl_swap_buffers(display, surface) : EGL_FALSE;
}
#endif
