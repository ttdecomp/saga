#include "decomp.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"

void (*GizBuildIt_FinishFn)(GIZBUILDIT_s *) = NULL;
i32 (*GizBuildit_AutoBuildPosFn)(void *, NUVEC *, NUVEC *, u16 *) = NULL;
i16 GizBuilditGDeb[6] = {0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51};
i32 LEGOCONTEXT_BUILDIT = 0x2d;

void GizBuildIt_TurnOff(GIZBUILDIT_s *buildit);
void GizBuildIt_SetStepTime(GIZBUILDIT_s *buildit, GameObject_s *player);

f32 GizBuildItMul(GameObject_s *player) {
    if (static_cast<i8>(player->apiobj.field_0x1f8) < 0 && Player_HasFastBuild(player)) {
        return 3.0f;
    }

    GIZBUILDIT_s *buildit = static_cast<GIZBUILDIT_s *>(player->field_0x788);
    i32 build_amount = player->build_button_taps;
    const i32 completed_count = buildit->built_object_count;
    const i32 total_count = buildit->anim_object_count;
    if ((completed_count - build_amount + total_count) / 2 <= completed_count) {
        build_amount = total_count - completed_count;
    }
    if (build_amount < 0) {
        build_amount = 0;
    } else if (build_amount > 10) {
        build_amount = 10;
    }
    return static_cast<f32>(build_amount) / 10.0f + 1.0f;
}

GIZBUILDIT_s *GizBuildIt_Find(WORLDINFO_s *world, char *name) {
    GIZBUILDITSYS_s *buildit_sys = world->giz_buildit_sys;
    if (buildit_sys != NULL && buildit_sys->count != 0) {
        GIZBUILDIT_s *buildit = buildit_sys->buildits;
        for (i32 i = 0; i < world->giz_buildit_sys->count; ++i, ++buildit) {
            if (NuStrICmp(buildit->name, name) == 0) {
                return buildit;
            }
        }
    }

    return NULL;
}

i32 GizBuildIt_AtEnd(GIZBUILDIT_s *buildit) {
    return buildit != NULL && buildit->build_state == GIZBUILDIT_BUILD_COMPLETE;
}

void GizBuildIt_Finish(GIZBUILDIT_s *buildit) {
    buildit->build_state = GIZBUILDIT_BUILD_COMPLETE;
    if ((buildit->state_flags & GIZBUILDIT_STATE_TURN_OFF_WHEN_COMPLETE) != 0) {
        GizBuildIt_TurnOff(buildit);
    }
    if (GizBuildIt_FinishFn != NULL) {
        GizBuildIt_FinishFn(buildit);
    }
}

i32 GizBuildIt_AtStart(GIZBUILDIT_s *buildit) {
    return buildit != NULL && buildit->builders_active == 0 && buildit->built_object_count == 0;
}

void GizBuildIt_TurnOff(GIZBUILDIT_s *buildit) {
    if (buildit == NULL) {
        return;
    }
    if (buildit->linked_buildit == NULL) {
        GameAnimSet_SetVisibility(buildit->anim_set, 0);
    }
    buildit->availability_flags &= static_cast<u8>(~GIZBUILDIT_AVAILABILITY_VISIBLE);
}

void GizBuildIt_SetToEnd(GIZBUILDIT_s *buildit) {
    if (buildit == NULL) {
        return;
    }

    buildit->availability_flags |= GIZBUILDIT_AVAILABILITY_VISIBLE;
    if (buildit->linked_buildit == NULL) {
        GameAnimSet_SetVisibility(buildit->anim_set, 1);
        GameAnimSet_JumpToEnd(buildit->anim_set);
        GizBuildIt_SetStepTime(buildit, NULL);
    } else {
        for (i32 index = 0; index < buildit->anim_object_count; ++index) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(buildit->anim_objects[index]->object_data);
            data->draw_mtx = data->start_mtx;
        }
    }

    buildit->built_object_count = buildit->anim_set != NULL ? buildit->anim_object_count : 0;
    buildit->builders_active = 0;
    buildit->availability_flags &= static_cast<u8>(~GIZBUILDIT_AVAILABILITY_INTERACTING);
    GizBuildIt_Finish(buildit);
}

void GizBuildIt_KillParts(GIZBUILDIT_s *) {
}

void GizBuildIt_SetToStart(GIZBUILDIT_s *buildit, i32 emit_debris, i32 keep_built_pieces) {
    if (buildit == NULL || buildit->anim_set == NULL) {
        return;
    }

    const u8 restored_built_count = keep_built_pieces != 0 ? buildit->built_object_count : 0;
    const i32 first_piece_to_reset = restored_built_count;
    buildit->availability_flags |= GIZBUILDIT_AVAILABILITY_VISIBLE;

    if (emit_debris != 0) {
        for (i32 index = 0; index < buildit->built_object_count; ++index) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(buildit->anim_objects[index]->object_data);
            NUVEC position = {data->end_mtx.m30, data->end_mtx.m31, data->end_mtx.m32};
            AddGameDebris(WORLD->debris_sys, GizBuilditGDeb[qrand() / 0x2aab], &position);
        }
    }

    if (buildit->linked_buildit != NULL) {
        for (i32 index = first_piece_to_reset; index < buildit->anim_object_count; ++index) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(buildit->anim_objects[index]->object_data);
            data->draw_mtx = data->start_mtx;
        }
    } else {
        GameAnimSet_SetVisibility(buildit->anim_set, 1);
        for (i32 index = first_piece_to_reset; index < buildit->anim_object_count; ++index) {
            GAMEANIMOBJ_s *object = buildit->anim_objects[index];
            object->instance_animation->playing = 0;
            object->instance_animation->ltime = object->start_frame;
        }
        GizBuildIt_SetStepTime(buildit, NULL);
    }

    buildit->builders_active = 0;
    buildit->built_object_count = restored_built_count;
    buildit->availability_flags &= static_cast<u8>(~GIZBUILDIT_AVAILABILITY_INTERACTING);
    buildit->build_state = GIZBUILDIT_BUILD_IDLE;
}

void GizBuildIt_AnyReacting(WORLDINFO_s *) {
}

GIZBUILDIT_s *GizBuildIt_FindNearest(WORLDINFO_s *, GameObject_s *, BUILDIT_FIND_ENUM, i32) {
    return NULL;
}

void GizBuildIt_SetStepTime(GIZBUILDIT_s *buildit, GameObject_s *player) {
    buildit->step_duration = 0.3f;
    if (player != NULL) {
        buildit->step_duration /= GizBuildItMul(player);
    }
    buildit->step_timer = buildit->step_duration;
}

void GizBuildIts_TotalScore(void *) {
}

void GizBuildIts_UpdateHint(HINT_s *) {
}

void GizBuildIt_SetHeadTarget(GIZBUILDIT_s *, GameObject_s *) {
}

void GizBuildItPushAwayFromEnd(GameObject_s *) {
}

void GizBuildItPushAwayFromStart(GameObject_s *, GIZBUILDIT_s *) {
}

void GIZBUILDIT_s::ClearMechObjectInterface() {
}

void GIZBUILDIT_s::GetMechObjectInterface() {
}

// Static build-it helper callbacks. Moved from gizmisc_stubs.cpp.

static __used__ void GizBuildIt_FinishFn_Game(GIZBUILDIT_s *) {
}

static __used__ bool GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT_s *, GameObject_s *) {
    return false;
}

static __used__ int GizBuildit_AutoBuildPos_Game(void *, nuvec_s *, nuvec_s *, u16 *) {
    return 0;
}
