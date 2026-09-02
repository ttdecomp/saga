#include "decomp.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

namespace {
    enum : u8 {
        GIZFORCE_CHARACTER_CONTEXT = 8,
    };
}

void GizForce_Throw(GameObject_s *, GIZFORCE_s *, float, float, i32) {
}

i32 GizForce_Complete(GIZFORCE_s *force) {
    if ((force->config_flags & GIZFORCE_CONFIG_WAIT_FOR_FORCE_RANGE) != 0 ||
        (force->force_range > 0.0f && (force->runtime_flags & GIZFORCE_RUNTIME_FORCE_RANGE_COMPLETE) == 0)) {
        return 0;
    }
    if ((force->config_flags & GIZFORCE_CONFIG_ALONG_SOCKET) != 0 &&
        (force->runtime_flags & GIZFORCE_RUNTIME_ALONG_SOCKET_HIDDEN) == 0) {
        return 0;
    }
    return GizForce_AnimComplete(force);
}

void GizForce_ResetLOS(GameObject_s *object) {
    if (object != NULL && object->gizforce_los_info != NULL) {
        memset(object->gizforce_los_info, 0, 0x630);
    }
}

GIZFORCE_s *GizForce_FindByName(GIZFORCESYS_s *force_sys, char *name) {
    GIZFORCE_s *force = NULL;
    if (name == NULL || force_sys == NULL) {
        return force;
    }
    force = force_sys->forces;
    for (i32 index = 0; index < force_sys->count; ++index, ++force) {
        if (NuStrICmp(force->name, name) == 0) {
            return force;
        }
    }
    return force;
}

void GizForce_TotalScore(void *) {
}

void GizForce_UpdateHint(HINT_s *) {
}

GIZFORCE_s *GizForces_FindForce(WORLDINFO_s *world, char *name) {
    GIZMO *gizmo = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? static_cast<GIZFORCE_s *>(gizmo->object) : NULL;
}

i32 GizForce_AnimComplete(GIZFORCE_s *force) {
    if (force != NULL && force->anim_set != NULL) {
        if ((force->progress_flags & GIZFORCE_PROGRESS_ANIMATION_REVERSED) == 0) {
            if (force->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                return 1;
            }
        } else if (force->anim_set->state == GAMEANIMSET_STATE_AT_START) {
            return 1;
        }
        return 0;
    }
    return 1;
}

void GizForce_PlayForwards(GIZFORCE_s *force) {
    if (force == NULL) {
        return;
    }
    if ((force->progress_flags & GIZFORCE_PROGRESS_ANIMATION_REVERSED) == 0) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        f32 speed = force->animation_speed;
        if (speed < 0.0f) {
            GameAnimSet_Play(force->anim_set, speed * force->end_frame, 0);
            return;
        }
        GameAnimSet_Play(force->anim_set, speed * force->start_frame, 0);
        return;
    }
    GameAnimSet_SetRepeating(force->anim_set, 0);
    f32 speed = -force->animation_speed;
    if (force->animation_speed >= 0.0f) {
        GameAnimSet_Play(force->anim_set, speed * force->end_frame, 0);
        return;
    }
    GameAnimSet_Play(force->anim_set, speed * force->start_frame, 0);
}

i32 GizForce_StoodOnForce(GIZFORCE_s *force, GameObject_s *object) {
    i32 result = 0;
    GAMEANIMOBJ_s *anim_object;
    if ((force->runtime_flags & GIZFORCE_RUNTIME_HAS_PLATFORM) != 0 && object->field_0x1078 != -1 &&
        (anim_object = force->anim_set->objects) != NULL) {
        while (object->field_0x1078 != static_cast<GIZFORCEANIMDATA_s *>(anim_object->object_data)->platform_id) {
            anim_object = anim_object->next;
            if (anim_object == NULL) {
                return 0;
            }
        }
        result = 1;
    }
    return result;
}

void GizForce_PlayBackwards(GIZFORCE_s *force) {
    if (force == NULL) {
        return;
    }
    if ((force->progress_flags & GIZFORCE_PROGRESS_ANIMATION_REVERSED) == 0) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        f32 speed = -force->animation_speed;
        if (force->animation_speed < 0.0f) {
            GameAnimSet_Play(force->anim_set, speed * force->start_frame, 0);
            return;
        }
        GameAnimSet_Play(force->anim_set, speed * force->end_frame, 0);
        return;
    }
    GameAnimSet_SetRepeating(force->anim_set, 0);
    f32 speed = force->animation_speed;
    if (speed >= 0.0f) {
        GameAnimSet_Play(force->anim_set, speed * force->start_frame, 0);
        return;
    }
    GameAnimSet_Play(force->anim_set, speed * force->end_frame, 0);
}

void GizForce_SetVisibility(GIZFORCE_s *force, i32 visibility) {
    if (force != NULL) {
        GameAnimSet_SetVisibility(force->anim_set, visibility);
        force->progress_flags =
            static_cast<u8>((force->progress_flags & ~GIZFORCE_PROGRESS_VISIBLE) | ((visibility != 0) << 1));
    }
}

void GizForces_AngleToForce(nuvec_s *position, GIZFORCE_s *force) {
    NuAtan2D(force->position.x - position->x, force->position.z - position->z);
}

i32 GizForce_GameObjUsingForce(GameObject_s *object, GIZFORCE_s *force) {
    return force != NULL && object != NULL && object->field_0x7a5 == GIZFORCE_CHARACTER_CONTEXT &&
           object->gizforce_target == force;
}

void GizForce_FindBestForceTarget(GIZFORCESYS_s *force_sys, GameObject_s *object) {
    if (force_sys == NULL || object == NULL) {
        return;
    }

    object->gizforce_target = NULL;
    object->gizforce_target_object = NULL;
    const f32 facing_x = NU_SIN_LUT(object->apiobj.movement_facing_angle);
    const f32 facing_z = NU_COS_LUT(object->apiobj.movement_facing_angle);
    f32 best_distance = 1.0e9f;

    for (u32 index = 0; index < force_sys->visible_force_count; ++index) {
        GIZFORCE_s *force = force_sys->visible_forces[index];
        if (force == NULL || force->using_object != NULL || force->field_0x3c != 0.0f) {
            continue;
        }
        if (force->group == NULL) {
            if (GizForce_Complete(force) != 0) {
                continue;
            }
        } else if (force->group->count != 0 && force->group->forces[force->group->count - 1] != force &&
                   (force->group->field_0x24 & GIZFORCE_GROUP_ACTIVE) != 0) {
            continue;
        }
        if (GizForce_StoodOnForce(force, object) != 0) {
            continue;
        }

        NUVEC object_position = {object->apiobj.pos_x, object->apiobj.pos_y, object->apiobj.pos_z};
        if ((force->config_flags & GIZFORCE_CONFIG_TARGET_ANIMATION_OBJECTS) != 0) {
            for (GAMEANIMOBJ_s *anim_object = force->anim_set->objects; anim_object != NULL;
                 anim_object = anim_object->next) {
                NUVEC *target_position = NuSpecialGetDrawPos(&anim_object->special);
                if (target_position == NULL) {
                    continue;
                }
                NUVEC delta;
                const f32 distance = NuVecDistSqr(target_position, &object_position, &delta);
                if (distance <= force->interaction_radius * force->interaction_radius &&
                    delta.x * facing_x + delta.z * facing_z >= 0.0f && distance < best_distance) {
                    best_distance = distance;
                    object->gizforce_target = force;
                    object->gizforce_target_object = anim_object;
                }
            }
            continue;
        }

        NUVEC delta;
        const f32 distance = NuVecDistSqr(&force->position, &object_position, &delta);
        if (distance <= force->interaction_radius * force->interaction_radius &&
            delta.x * facing_x + delta.z * facing_z >= 0.0f && distance < best_distance) {
            best_distance = distance;
            object->gizforce_target = force;
            object->gizforce_target_object = NULL;
        }
    }
}

void GIZFORCE_s::ClearMechObjectInterface() {
}

void GIZFORCE_s::GetMechObjectInterface() {
}

// Static force SFX nufpar helpers. Moved from gizmisc_stubs.cpp.

static __used__ void GizForceSFX_forcename(nufpar_s *) {
}

static __used__ void GizForceSFX_returnsfx(nufpar_s *) {
}

static __used__ void GizForceSFX_processsfx(nufpar_s *) {
}

static __used__ void GizForceSFX_completesfx(nufpar_s *) {
}
