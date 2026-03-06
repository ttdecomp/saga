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
    u8 enabled_extensions[26]; //NEEDS TO BE AT 0x28

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
