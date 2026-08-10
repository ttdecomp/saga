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
    i32 lock_count;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex2;
    u32 field10_0x10;
    u32 field11_0x14;
    const char *extensions;
    bool is_not_amazon_kf;
    u8 field13_0x1d[3];
    i32 max_texture_units;
    i32 max_texture_size;

  public:
    u8 enabled_extensions[26];

  private:
    bool oes_packed_depth_stencil;
    bool oes_depth24;
    bool oes_depth_texture;
    bool field48_0x45;
    bool focus;
    u8 field50_0x47;
    EGLNativeWindowType native_window;
    u32 field52_0x4c;
    i32 field53_0x50;
    bool field54_0x54;
    u8 field55_0x55[3];
    EGLint attrib_list[8];
    EGLDisplay egl_display;
    EGLConfig egl_config;
    i32 field61_0x80;

    EGLSurface pbuffers[4];
    EGLContext contexts[4];

    volatile bool context_valid;
    u8 field76_0xa5[3];
    EGLint surface_width;
    EGLint surface_height;
    u32 width, height;
    u32 backing_width, backing_height;

    i32 nominal_aspect_ratio;
    f32 aspect_ratio;

  private:
    i32 DetermineNominalAspectRatio(u32 width, u32 height);
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

    void InitialiseOpenGLContext(ANativeWindow *window);
    void CheckForRenderWindowInitialisation();
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
