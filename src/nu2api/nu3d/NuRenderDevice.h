#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus

class NuRenderDevice {
  public:
    void SetThisTreadAsRender();
    int BeginCriticalSection(const char *file, i32 line);
    void EndCriticalSection(const char *file, i32 line);
    void SwapBuffers();
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
