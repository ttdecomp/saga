#pragma once

#include "decomp.h"

struct ANIMPACKET_s {
    i32 current_animation;
    i32 animation_mode;
    f32 blend_elapsed;  // 0x08
    f32 blend_duration; // 0x0c
    f32 time;           // 0x10
    i32 previous_animation_mode;
    u8 pad_0x18[0x20 - 0x18];
    f32 time_secondary; // 0x20
    u8 pad_0x24[0x30 - 0x24];
    u8 field_0x30;
    u8 blending;           // 0x31
    i16 blend_animation_a; // 0x32
    i16 blend_animation_b; // 0x34
    i16 animation_index;   // 0x36
    i16 field_0x38;
    i16 field_0x3a; // 0x3a
    u8 reset_flags; // 0x3c
    u8 field_0x3d;
    u8 field_0x3e;
    u8 pad_0x3f[0x42 - 0x3f];
    u16 frame;      // 0x42
    f32 field_0x44; // 0x44
};

DECOMP_ASSERT(sizeof(ANIMPACKET_s) == 0x48, "ANIMPACKET_s size");
