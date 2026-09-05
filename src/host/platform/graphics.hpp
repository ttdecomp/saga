#pragma once

#include <EGL/egl.h>

#include "decomp.h"
#include "nu2api/nucore/nutime.h"

// Host platform boundary used by the harness.
void HostSetDocumentsPath(const char *path);
void HostSetReadbackEnabled(bool enabled);
void HostRequestReadback(void);
void HostSetFpsOverlayEnabled(bool enabled);
void HostSetMsaaEnabled(bool enabled);
i32 HostReadbackPixels(u32 max_width, u32 max_height, u8 *rgba);
void HostCaptureCurrentSurface(EGLDisplay display, EGLSurface surface);
void HostCaptureCurrentFramebuffer(i32 width, i32 height);
void HostPaceFrame(const NUTIME *frame_start, f32 target_seconds);
