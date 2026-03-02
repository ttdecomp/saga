#include "nu2api/nu3d/NuRenderDevice.h"

#include "decomp.h"
#include "nu2api/nucore/common.h"

NuRenderDevice g_renderDevice;

thread_local i32 gt_glContextIndex;

void NuRenderDevice::SetThisTreadAsRender() {
    gt_glContextIndex = 3;
}

int NuRenderDevice::BeginCriticalSection(const char *file, i32 line) {
    UNIMPLEMENTED();
    return 0;
}

void NuRenderDevice::EndCriticalSection(const char *file, i32 line) {
    UNIMPLEMENTED();
}

void NuRenderDevice::SwapBuffers() {
    UNIMPLEMENTED();
}

void NuRenderSetThisTreadAsRender() {
    g_renderDevice.SetThisTreadAsRender();
}

void BeginCriticalSectionGL(const char *file, i32 line) {
    g_renderDevice.BeginCriticalSection(file, line);
}

void EndCriticalSectionGL(const char *file, i32 line) {
    g_renderDevice.EndCriticalSection(file, line);
}

void NuRenderDeviceSwapBuffers() {
    g_renderDevice.SwapBuffers();
}
