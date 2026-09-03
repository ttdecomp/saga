#pragma once

#include "decomp.h"

enum ANIMPACKET_FLAGS : u8 {
    ANIMPACKET_FLAG_FINISHED = 0x01,
    ANIMPACKET_FLAG_LOOPED = 0x02,
    ANIMPACKET_FLAG_ANIMATION_CHANGED = 0x04,
    ANIMPACKET_FLAG_PLAYING_REVERSED = 0x08,
    ANIMPACKET_FLAG_PAUSED = 0x10,
    ANIMPACKET_FLAG_ZERO_TIMESTEP = 0x20,
    ANIMPACKET_FLAG_BLEND_FINISHED = 0x40,
    ANIMPACKET_FLAG_FORCE_RESTART = 0x80,
};

struct ANIMPACKET_s {
    union {
        f32 current_time;
        f32 field_0x00;
    };
    union {
        f32 previous_time;
        f32 field_0x04;
    };
    f32 blend_elapsed;  // 0x08
    f32 blend_duration; // 0x0c
    union {
        f32 blend_source_time;
        f32 time;
    };
    union {
        f32 blend_target_time;
        f32 time2;
    };
    u8 pad_0x18[0x20 - 0x18];
    union {
        f32 field_0x20;
        f32 time_secondary;
    }; // 0x20
    u8 pad_0x24[0x30 - 0x24];
    union {
        u8 flags;
        u8 field_0x30;
    };
    u8 blending;           // 0x31
    i16 blend_animation_a; // 0x32
    i16 blend_animation_b; // 0x34
    i16 animation_index;   // 0x36
    union {
        i16 previous_animation;
        i16 field_0x38;
    };
    union {
        i16 requested_animation;
        i16 field_0x3a;
    };
    u8 blend_source_reversed; // 0x3c
    u8 blend_target_reversed; // 0x3d
    u8 current_reversed;      // 0x3e
    u8 pad_0x3f[0x42 - 0x3f];
    union {
        i16 overlay_animation; // -1 when no overlay is active
        u16 frame;
    }; // 0x42
    f32 field_0x44; // 0x44
};

DECOMP_ASSERT(sizeof(ANIMPACKET_s) == 0x48, "ANIMPACKET_s size");

struct MINIANIMPACKET_s {
    i32 current_animation; // 0x00
    i32 animation_mode;    // 0x04
    u8 data_0x08[0x14 - 0x08];
    i32 previous_animation_mode; // 0x14
    u8 reset_state;              // 0x18
    u8 field_0x19;               // 0x19
    u8 data_0x1a[0x1e - 0x1a];
    union {
        i16 animation_index;
        i16 current_animation_id;
    }; // 0x1e
    union {
        i16 previous_animation;
        i16 previous_animation_id;
    }; // 0x20
    i16 requested_animation_id; // 0x22
};

DECOMP_ASSERT(sizeof(MINIANIMPACKET_s) == 0x24, "MINIANIMPACKET_s size");
DECOMP_ASSERT(offsetof(MINIANIMPACKET_s, previous_animation_mode) == 0x14, "MINIANIMPACKET_s previous mode offset");
DECOMP_ASSERT(offsetof(MINIANIMPACKET_s, animation_index) == 0x1e, "MINIANIMPACKET_s animation offset");
DECOMP_ASSERT(offsetof(MINIANIMPACKET_s, requested_animation_id) == 0x22,
              "MINIANIMPACKET_s requested animation offset");
