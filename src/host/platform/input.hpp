#pragma once

#include "nu2api/nucore/common.h"

// Selected host input backend used by the harness.
void HostInputReset();
void HostInputTap(i32 port, u32 buttons);
void HostInputSetHeld(i32 port, u32 buttons);
void HostInputSetKeyboardHeld(i32 port, u32 buttons);
void HostInputTouch(i32 x, i32 y, i32 width, i32 height);

// Weak extension points used by platform input backends.
void HostInputResetPlatform();
u32 HostInputConsumePlatform(i32 port);
