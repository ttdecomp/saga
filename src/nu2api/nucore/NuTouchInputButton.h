#pragma once

#include "nu2api/nucore/NuTouchInputElement.h"

struct NuInputTouchData;

struct NuTouchInputButton {
    NuTouchInputButton(i32, u32);
    NuTouchInputButton(i32, u32, float, float, float, float);
    void Render();
    void Update(NuInputTouchData const *);
};
