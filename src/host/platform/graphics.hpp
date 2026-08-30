#pragma once

#include "decomp.h"

// Host platform boundary used by the harness.
void HostSetDocumentsPath(const char *path);
void HostSetReadbackEnabled(bool enabled);
i32 HostReadbackPixels(u32 max_width, u32 max_height, u8 *rgba);
