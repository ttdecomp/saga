#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/numath/nuvec.h"

struct TECHNO_CONFIG {
    f32 interaction_time;
    i16 floor_target_object_id;
    i16 active_effect_id;
    i16 success_effect_id;
    i16 failure_effect_id;
    NUVEC idle_offset;
    NUVEC active_offset;
    NUVEC complete_offset;
    i16 activation_effect_id;
    i16 completion_effect_id;
    NUVEC left_hand_offset;
    NUVEC right_hand_offset;
};

DECOMP_ASSERT(sizeof(TECHNO_CONFIG) == 0x4c, "TECHNO_CONFIG size");

enum TECHNO_FLAGS : u8 {
    TECHNO_FLAG_ACTIVE = 1 << 0,
    TECHNO_FLAG_VISIBLE = 1 << 1,
    TECHNO_FLAG_USED_THIS_FRAME = 1 << 2,
    TECHNO_FLAG_COMPLETE = 1 << 3,
};

typedef struct TECHNO_s {
    char name[16];
    NUVEC position;
    NUVEC ground_position;
    f32 ground_offset;
    f32 scale;
    u16 y_rotation;
    u16 ground_x_rotation;
    u16 ground_z_rotation;
    u8 enabled;
    u8 target_mode;
    i32 output;
    u8 flags;
    char target_name[16];
    char target_object_name[31];
    void *controlled_object;
} TECHNO;

DECOMP_ASSERT(sizeof(TECHNO) == 0x70, "TECHNO size");
DECOMP_ASSERT(offsetof(TECHNO, position) == 0x10, "TECHNO position offset");
DECOMP_ASSERT(offsetof(TECHNO, flags) == 0x3c, "TECHNO flags offset");
DECOMP_ASSERT(offsetof(TECHNO, controlled_object) == 0x6c, "TECHNO controlled-object offset");

extern TECHNO_CONFIG TechnoSys;

extern i32 techno_gizmotype_id;

#ifdef __cplusplus

ADDGIZMOTYPE *Technos_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
