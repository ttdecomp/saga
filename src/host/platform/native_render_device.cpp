#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"

#include <EGL/egl.h>
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
    eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red);
    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green);
    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue);
    eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &alpha);
    eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth);
    eglGetConfigAttrib(display, config, EGL_STENCIL_SIZE, &stencil);
    eglGetConfigAttrib(display, config, EGL_SAMPLE_BUFFERS, &sample_buffers);
    eglGetConfigAttrib(display, config, EGL_SAMPLES, &samples);
    LOG_INFO("EGL framebuffer: rgba=%d/%d/%d/%d depth=%d stencil=%d sample-buffers=%d samples=%d", red, green, blue,
             alpha, depth, stencil, sample_buffers, samples);
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
