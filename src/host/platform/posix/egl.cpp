#include "host/platform/graphics.hpp"

#include <dlfcn.h>

extern "C" EGLBoolean eglSwapBuffers(EGLDisplay display, EGLSurface surface) {
    typedef EGLBoolean (*HostEglSwapBuffers)(EGLDisplay, EGLSurface);
    static HostEglSwapBuffers host_real_egl_swap_buffers =
        reinterpret_cast<HostEglSwapBuffers>(dlsym(RTLD_NEXT, "eglSwapBuffers"));

    HostCaptureCurrentSurface(display, surface);
    return host_real_egl_swap_buffers != nullptr ? host_real_egl_swap_buffers(display, surface) : EGL_FALSE;
}
