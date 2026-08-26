#pragma once

#include "nu2api/nucore/common.h"

#include <EGL/egl.h>
#include <pthread.h>

#ifdef __cplusplus

struct ANativeWindow;

class NuRenderDeviceGen {
  protected:
    bool value;
    NuRenderDeviceGen();
};

class NuRenderDevice : NuRenderDeviceGen {
    bool focus;
    bool field48_0x45;
    EGLDisplay egl_display;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex2;
    bool is_not_amazon_kf;
    volatile bool context_valid;
    EGLConfig egl_config;
    EGLNativeWindowType native_window;

    EGLSurface pbuffers[4];
    EGLContext contexts[4];
#ifdef HOST_BUILD
    // HOST-ONLY: dedicated surface+context for test-thread frame readbacks so
    // they never contend with the game thread's SwapBuffers on pbuffers[3].
    EGLSurface pbuffer_readback = EGL_NO_SURFACE;
    EGLContext context_readback = EGL_NO_CONTEXT;
#endif

    u32 backing_width, backing_height;

    i32 nominal_aspect_ratio;
    f32 aspect_ratio;

    i32 max_texture_units;
    i32 max_texture_size;

    const char *extensions;

    bool field54_0x54;

    EGLint attrib_list[16];

    u32 width, height;

    bool oes_packed_depth_stencil;
    bool oes_depth24;
    bool oes_depth_texture;

    i32 lock_count;

  private:
    void DetermineBackBufferResolution(i32 width, i32 height);
    EGLConfig SelectEGLConfig();
    bool IsExtensionSupported(const char *exts);

    void FrameEnd();

  public:
    NuRenderDevice();

    void Initialize();

    void SetThisTreadAsRender();
    void BeginCriticalSection(const char *file, i32 line);
    void EndCriticalSection(const char *file, i32 line);
    void SwapBuffers();

    void OnWindowCreated(ANativeWindow *window);

#ifdef HOST_BUILD
    // Host-only: make the presented window surface current and read its backbuffer
    // into the caller's RGBA buffer (width*height*4). Returns the surface
    // dimensions read, or 0 if no surface/context is present.
    i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba);
#endif

    void InitialiseOpenGLContext(ANativeWindow *window);
    void CheckForRenderWindowInitialisation();
    u8 enabled_extensions[26]; // NEEDS TO BE AT 0x28

    void OnAppPaused();
    void OnAppRestarted();
    void OnAppResume();
    void OnAppStarted();
    void OnAppStopped();
    void OnGainedFocus();
    void OnLostFocus();
    void OnWindowDestroy();
    void PreInitialize();
    void ResizeDevice(i32 width, i32 height, i32 _a, bool _b, bool _c, bool _d, bool _e);

    void IsContextValid() const;
    void MultiThreadRender() const;
    void DetermineNominalAspectRatio(u32 width, u32 height) const;
    void OpenglErrorCallback(u32 source, u32 type, u32 id, u32 severity, i32 len, char const *msg, void *user_param);
};

extern NuRenderDevice g_renderDevice;

extern "C" {
#endif
    void NuRenderSetThisTreadAsRender(void);
    void BeginCriticalSectionGL(const char *file, i32 line);
    void EndCriticalSectionGL(const char *file, i32 line);
    void NuRenderDeviceSwapBuffers(void);
#ifdef __cplusplus
}
#endif
