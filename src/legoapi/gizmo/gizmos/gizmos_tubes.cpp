#include "legoapi/legoapi_types.h"

#include "gamelib/util/gamelib_util_types.h"
#include "legoapi/gizmos/transport/tubes.h"

void TorpedoCode(GameObject_s *, i32, float) {
}

void Torpedo_Scale(BOLT_s *) {
}

void Torpedo_Shoot(GameObject_s *) {
}

void Tube_MoveCode(GameObject_s *, WORLDINFO_s *) {
}

void Tube_SetObjBit(TUBE *tube, i32 object_index) {
    tube->occupied_object_masks[object_index / 32] |= 1U << object_index;
}

void Torpedo_EndBolt(BOLT_s *) {
}

void Tube_FindByName(WORLDINFO_s *, char *) {
}

i32 Tube_InCylinder(GameObject_s *object, TUBE *tube, f32 *horizontal_distance_squared, i32 ignore_height) {
    if (tube == NULL || object == NULL) {
        return 0;
    }

    if (ignore_height == 0) {
        if (tube->position.y > object->apiobj.collision_max.y || object->apiobj.collision_min.y > tube->top) {
            return 0;
        }
    }

    const f32 delta_x = object->apiobj.collision_position.x - tube->position.x;
    const f32 delta_z = object->apiobj.collision_position.z - tube->position.z;
    const f32 distance_squared = delta_x * delta_x + delta_z * delta_z;

    f32 radius_squared = tube->radius_squared;
    if ((tube->flags & TUBE_FLAG_TOUCH_RADIUS) != 0 && TouchHacks::TouchControlsActive) {
        radius_squared *= 0.8f;
    }

    if (distance_squared > radius_squared) {
        return 0;
    }
    if (horizontal_distance_squared != NULL) {
        *horizontal_distance_squared = distance_squared;
    }
    return 1;
}

void TorpedoHitTarget(BOLT_s *) {
}

void Torpedo_InitBolt(BOLT_s *) {
}

void Torpedo_Ricochet(BOLT_s *, TORPEDOPACKET_s *) {
}

i32 Tube_IsObjBitSet(TUBE *tube, i32 object_index) {
    return tube->occupied_object_masks[object_index / 32] >> object_index & 1;
}

void Torpedo_UpdateBolt(BOLT_s *) {
}

void Tube_InAnyCylinder(WORLDINFO_s *, GameObject_s *, i32) {
}

void Torpedo_InitRicochet(BOLT_s *, nuvec_s *) {
}

void Torpedo_UpdateJobbies(GameObject_s *) {
}
