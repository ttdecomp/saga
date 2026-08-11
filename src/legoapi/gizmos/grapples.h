#pragma once

#include "legoapi/gameobject.h"
#include "legoapi/gizmo.h"

extern int grapple_gizmotype_id;

#ifdef __cplusplus

typedef struct GRAPPLE_s {
    u8 unknown_00[0x30];
    NUVEC look_at_position;
    NUVEC target_position;
    u8 unknown_48[4];
    f32 swing_angle_scale;
} GRAPPLE;

#ifndef __x86_64__
static_assert(offsetof(GRAPPLE, look_at_position) == 0x30, "GRAPPLE look_at_position offset");
static_assert(offsetof(GRAPPLE, target_position) == 0x3c, "GRAPPLE target_position offset");
static_assert(offsetof(GRAPPLE, swing_angle_scale) == 0x4c, "GRAPPLE swing_angle_scale offset");
#endif

ADDGIZMOTYPE *Grapples_RegisterGizmo(int type_id);
i32 Grapple_ReachedTop(GameObject_s *object);
void Grapple_SetPlayerTargetPoint(GameObject_s *object, NUVEC *target);
i32 Grapple_SetTargetMom(GameObject_s *object);
void Grapple_SetRotOrder(GameObject_s *object);
i32 Grapple_LookAtPos(GameObject_s *object, NUVEC *position);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
