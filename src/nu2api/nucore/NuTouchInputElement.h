#pragma once

struct NuTouchInputElement {
    struct TYPE {};
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32);
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32, float, float, float, float);
};
