#pragma once

#include "nu2api/nucore/common.h"

// Selected host input backend used by the harness.
void HostInputReset();
void HostInputTap(i32 port, u32 buttons);
void HostInputSetHeld(i32 port, u32 buttons);
