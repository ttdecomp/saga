#pragma once

#include "decomp.h"

struct ANIMPACKET_s {
    f32 field_0x00;
    f32 field_0x04;
    f32 blend_elapsed;  // 0x08
    f32 blend_duration; // 0x0c
    f32 time;           // 0x10
    f32 time2;          // 0x14
    u8 pad_0x18[0x20 - 0x18];
    f32 field_0x20; // 0x20
    u8 pad_0x24[0x30 - 0x24];
    u8 field_0x30;
    u8 blending;           // 0x31
    i16 blend_animation_a; // 0x32
    i16 blend_animation_b; // 0x34
    u16 animation_index;   // 0x36
    i16 field_0x38;
    i16 field_0x3a; // 0x3a
    u8 pad_0x3c[0x42 - 0x3c];
    u16 frame;      // 0x42
    f32 field_0x44; // 0x44
};

DECOMP_ASSERT(sizeof(ANIMPACKET_s) == 0x48, "ANIMPACKET_s size");
