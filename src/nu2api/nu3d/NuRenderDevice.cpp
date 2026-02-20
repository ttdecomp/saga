#include "decomp.h"

#include "nu2api/nu3d/NuRenderDevice.h"

NuRenderDevice g_renderDevice;

__thread i32 gt_glContextIndex;

void NuRenderDevice::SetThisTreadAsRender() {
    gt_glContextIndex = 3;
}

int NuRenderDevice::BeginCriticalSection(const char *file, int line) {
    UNIMPLEMENTED();
    return 0;
}

void NuRenderDevice::EndCriticalSection(const char *file, int line) {
    UNIMPLEMENTED();
}

void NuRenderDevice::SwapBuffers() {
    UNIMPLEMENTED();
}

void NuRenderSetThisTreadAsRender() {
    g_renderDevice.SetThisTreadAsRender();
}

void BeginCriticalSectionGL(const char *file, int line) {
    g_renderDevice.BeginCriticalSection(file, line);
}

void EndCriticalSectionGL(const char *file, int line) {
    g_renderDevice.EndCriticalSection(file, line);
}

void NuRenderDeviceSwapBuffers() {
    g_renderDevice.SwapBuffers();
}
