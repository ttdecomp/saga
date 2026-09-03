#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"

struct GameObject_s;

struct LEVER_CONFIG {
    i16 floor_target_object_id;
    i16 reserved;
};

DECOMP_ASSERT(sizeof(LEVER_CONFIG) == 0x4, "LEVER_CONFIG size");

extern LEVER_CONFIG LeverSys;
extern i32 lever_gizmotype_id;

#ifdef __cplusplus

typedef struct LEVER_s LEVER;

ADDGIZMOTYPE *Levers_RegisterGizmo(i32 type_id);
void Levers_InitTerrain(WORLDINFO_s *world);
void Lever_MoveCode(WORLDINFO_s *world, GameObject_s *object);
LEVER_s *Lever_FindNearest(WORLDINFO_s *world, nuvec_s *position, GameObject_s *object, f32 *distance_squared);
bool Lever_BeingPulled(LEVER_s *lever);
bool Lever_FullyPulledDown(LEVER_s *lever);
void Lever_GetAbsTargetPos(LEVER_s *lever, nuvec_s *target_position);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
