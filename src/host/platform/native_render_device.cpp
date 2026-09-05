#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <SDL3/SDL_video.h>
#include <pthread.h>

namespace {
    bool host_msaa_enabled = true;
}

void HostSetMsaaEnabled(bool enabled) {
    host_msaa_enabled = enabled;
}

void NuRenderInspectEGLConfig(EGLDisplay display, EGLConfig config) {
    EGLint red = 0;
    EGLint green = 0;
    EGLint blue = 0;
    EGLint alpha = 0;
    EGLint depth = 0;
    EGLint stencil = 0;
    EGLint sample_buffers = 0;
    EGLint samples = 0;
    const char *egl_vendor = nullptr;
    const char *egl_version = nullptr;
    if (display != EGL_NO_DISPLAY && config != nullptr) {
        eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red);
        eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green);
        eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue);
        eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &alpha);
        eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth);
        eglGetConfigAttrib(display, config, EGL_STENCIL_SIZE, &stencil);
        eglGetConfigAttrib(display, config, EGL_SAMPLE_BUFFERS, &sample_buffers);
        eglGetConfigAttrib(display, config, EGL_SAMPLES, &samples);
        egl_vendor = eglQueryString(display, EGL_VENDOR);
        egl_version = eglQueryString(display, EGL_VERSION);
    } else {
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red);
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green);
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue);
        SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha);
        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth);
        SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil);
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &sample_buffers);
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
    }
    const char *gl_vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    const char *gl_renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    const char *gl_version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    if (egl_vendor != nullptr) {
        LOG_INFO("EGL vendor: %s (%s)", egl_vendor, egl_version != nullptr ? egl_version : "unknown");
    } else {
        LOG_INFO("Native graphics backend: SDL3 OpenGL ES");
    }
    LOG_INFO("OpenGL ES renderer: %s / %s (%s)", gl_vendor != nullptr ? gl_vendor : "unknown",
             gl_renderer != nullptr ? gl_renderer : "unknown", gl_version != nullptr ? gl_version : "unknown");
    LOG_INFO("Framebuffer: rgba=%d/%d/%d/%d depth=%d stencil=%d sample-buffers=%d samples=%d", red, green, blue, alpha,
             depth, stencil, sample_buffers, samples);
}

EGLConfig NuRenderDevice::SelectEGLConfig() {
    EGLint attrib_list[] = {
        EGL_DEPTH_SIZE,
        24,
        EGL_LEVEL,
        0,
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_CONFORMANT,
        EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE,
        5,
        EGL_GREEN_SIZE,
        6,
        EGL_RED_SIZE,
        5,
        EGL_ALPHA_SIZE,
        0,
        EGL_STENCIL_SIZE,
        0,
        EGL_SAMPLE_BUFFERS,
        host_msaa_enabled ? 1 : 0,
        EGL_SAMPLES,
        host_msaa_enabled ? 4 : 0,
        EGL_NONE,
    };

    pthread_mutex_lock(&this->mutex);

    EGLConfig configs[32];
    EGLint num_configs = 0;
    EGLBoolean ok = eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_configs);
    if (num_configs == 0 || ok == EGL_FALSE) {
        attrib_list[21] = 0;
        attrib_list[23] = 0;
        ok = eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_configs);
    }
    if (num_configs == 0 || ok == EGL_FALSE) {
        attrib_list[1] = 16;
        eglChooseConfig(this->egl_display, attrib_list, configs, 32, &num_configs);
    }

    pthread_mutex_unlock(&this->mutex);
    return configs[0];
}
