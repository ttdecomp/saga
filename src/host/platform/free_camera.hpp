#pragma once

#include "decomp.h"

enum HostFreeCameraControl : u32 {
    HOST_FREE_CAMERA_NUMPAD_4 = 1U << 0,
    HOST_FREE_CAMERA_NUMPAD_5 = 1U << 1,
    HOST_FREE_CAMERA_NUMPAD_6 = 1U << 2,
    HOST_FREE_CAMERA_NUMPAD_8 = 1U << 3,
    HOST_FREE_CAMERA_SHIFT = 1U << 4,
};

void HostFreeCameraConfigure(bool enabled);
void HostFreeCameraSetReady(bool ready);
void HostFreeCameraSetControls(u32 controls);
bool HostFreeCameraActive(void);
