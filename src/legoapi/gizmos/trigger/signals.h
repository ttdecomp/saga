#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus

typedef struct SIGNAL_s {
    char name[0x10];
    NUMTX matrix;
    NUVEC position;
    u16 yaw;
    u16 terrain_pitch;
    u16 terrain_roll;
    u16 animation_0;
    u16 animation_1;
    u16 animation_2;
    char character_letter;
    char suit_letter;
    u8 flags;
    u8 pad_6b;
    f32 radius;
    f32 scale;
    NUVEC normal;
    void *suit;
    NUVEC target_position;
    i16 platform_id;
    u16 pad_92;
} SIGNAL;

typedef struct SIGNALPROGRESS_s {
    char suit_letters[32];
    u32 active_mask;
    u32 visible_mask;
} SIGNALPROGRESS;

DECOMP_ASSERT(sizeof(SIGNAL) == 0x94, "SIGNAL size");
DECOMP_ASSERT(sizeof(SIGNALPROGRESS) == 0x28, "SIGNALPROGRESS size");

ADDGIZMOTYPE *Signals_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
