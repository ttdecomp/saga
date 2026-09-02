#pragma once

#include "nu2api/nucore/fixed_width.h"

struct NuInputTouchData;

struct NuTouchInputElement {
    struct TYPE {};
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32);
    NuTouchInputElement(NuTouchInputElement::TYPE, i32, u32, float, float, float, float);

    virtual ~NuTouchInputElement();
    virtual void Render() = 0;
    virtual void UpdateButtons(i32);
    virtual void Update(NuInputTouchData const *) = 0;
    virtual bool IsPressed() const;
    virtual float GetStickX() const;
    virtual float GetStickY() const;
    virtual void Deactivate();
    virtual void Activate();

    float x;
    float y;
    float width;
    float height;
    u32 field_0x14;
    u32 field_0x18;
    u32 field_0x1c;
    u32 field_0x20;
    u32 id;
    i32 index;
    u32 type;
};
