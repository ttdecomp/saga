#pragma once

#include <EGL/egl.h>

#include "decomp.h"

// Host platform boundary used by the harness.
void HostSetDocumentsPath(const char *path);
void HostSetReadbackEnabled(bool enabled);
void HostRequestReadback(void);
void HostSetFpsOverlayEnabled(bool enabled);
void HostSetMsaaEnabled(bool enabled);
i32 HostReadbackPixels(u32 max_width, u32 max_height, u8 *rgba);
void HostCaptureCurrentSurface(EGLDisplay display, EGLSurface surface);
