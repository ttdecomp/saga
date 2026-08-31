#include "legoapi/legoapi_types.h"

#include "legoapi/gizmos/object/lever.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

extern "C" {
    i16 NewPlatPickupInst(void *object, i32 object_type);
    void PlatInstRotate(i32 platform_id, i32 enabled);
}

void Lever_MoveCode(WORLDINFO_s *, GameObject_s *) {
}

bool Lever_BeingPulled(LEVER_s *lever) {
    return lever != NULL && (lever->flags & LEVER_FLAG_BEING_PULLED) != 0;
}

LEVER_s *Lever_FindNearest(WORLDINFO_s *world, nuvec_s *position, GameObject_s *object, f32 *distance_squared) {
    LEVER_s *nearest = NULL;
    f32 nearest_distance = 1.0f;

    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s *lever = &world->levers[index];
        NUVEC target_position;
        NUVEC *candidate_position = &lever->position;

        if (object != NULL) {
            if ((lever->flags & (LEVER_FLAG_INTERACTING | LEVER_FLAG_BEING_PULLED | LEVER_FLAG_VISIBLE |
                                 LEVER_FLAG_ENABLED)) != (LEVER_FLAG_VISIBLE | LEVER_FLAG_ENABLED) ||
                lever->pull_progress != 0.0f || lever->floor_position.y == -1.0f) {
                continue;
            }
            Lever_GetAbsTargetPos(lever, &target_position);
            candidate_position = &target_position;
        }

        const f32 candidate_distance = NuVecDistSqr(position, candidate_position, NULL);
        if (candidate_distance < nearest_distance) {
            nearest = lever;
            nearest_distance = candidate_distance;
        }
    }

    if (distance_squared != NULL) {
        *distance_squared = nearest_distance;
    }
    return nearest;
}

void Levers_InitTerrain(WORLDINFO_s *world) {
    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        lever.platform_id = NewPlatPickupInst(&lever, 3);
        PlatInstRotate(lever.platform_id, 1);
    }
}

bool Lever_FullyPulledDown(LEVER_s *lever) {
    return lever != NULL &&
           (lever->flags & (LEVER_FLAG_VISIBLE | LEVER_FLAG_BEING_PULLED)) ==
               (LEVER_FLAG_VISIBLE | LEVER_FLAG_BEING_PULLED) &&
           lever->pull_progress >= 1.0f;
}

void Lever_GetAbsTargetPos(LEVER_s *lever, nuvec_s *target_position) {
    if (lever == NULL || target_position == NULL) {
        return;
    }

    NUVEC offset = lever->target_offset;
    NuVecRotateY(&offset, &offset, lever->y_rotation);
    target_position->x = offset.x + lever->position.x;
    target_position->y = offset.y;
    target_position->z = offset.z + lever->position.z;
}

void LEVER_s::ClearMechObjectInterface() {
}

void LEVER_s::GetMechObjectInterface() {
}
