#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" i32 NuPortalWhichRoom(NUGSCN *scene, NUVEC *position);
extern i32 LEGOCONTEXT_BUILDIT;
void GameCam_Blend(GAMECAMERA_s *camera, f32 duration, f32 curve, i32 mode);

void ReleaseBuildIt(GameObject_s *player, i32 completed) {
    const i32 build_context = LEGOCONTEXT_BUILDIT;
    if (build_context == -1 || build_context != player->build_context) {
        return;
    }

    player->build_context = -1;
    const f32 completion_blend = completed != 0 ? 0.6f : 0.0f;
    GameCam_Blend(GameCam, 0.5f, completion_blend, 1);
}

void BuildIt_MoveCode(GameObject_s *) {
}

void GizBuildit_Reset(GIZBUILDIT_s *buildit, void *world_ptr) {
    buildit->availability_flags |= GIZBUILDIT_AVAILABILITY_ACTIVE | GIZBUILDIT_AVAILABILITY_VISIBLE;
    buildit->field_0x83 &= static_cast<u8>(~GIZBUILDIT_RUNTIME_REWARD_RELEASED);

    buildit->start_position = buildit->file_position;
    buildit->position = buildit->file_position;
    buildit->bounds_radius = NuTrigTable[0x3000];

    if (buildit->anim_set != NULL && buildit->anim_set->objects != NULL) {
        NUVEC start_min = {1000000000.0f, 1000000000.0f, 1000000000.0f};
        NUVEC start_max = {-1000000000.0f, -1000000000.0f, -1000000000.0f};
        NUVEC end_min = start_min;
        NUVEC end_max = start_max;

        for (GAMEANIMOBJ_s *object = buildit->anim_set->objects; object != NULL; object = object->next) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(object->object_data);
            memset(data, 0, sizeof(*data));

            NUMTX evaluated;
            EvalAnim(&object->special, object->start_frame, &evaluated, 1);
            data->start_mtx = evaluated;
            if (buildit->linked_buildit != NULL) {
                data->start_mtx.m30 -= buildit->linked_buildit->file_position.x;
                data->start_mtx.m31 -= buildit->linked_buildit->file_position.y;
                data->start_mtx.m32 -= buildit->linked_buildit->file_position.z;
                NuMtxRotateY(&data->start_mtx, buildit->field_0x7c);
                data->start_mtx.m30 += buildit->file_position.x;
                data->start_mtx.m31 += buildit->file_position.y;
                data->start_mtx.m32 += buildit->file_position.z;
                data->draw_mtx = data->start_mtx;
            }

            EvalAnim(&object->special, object->end_frame, &evaluated, 1);
            data->end_mtx = evaluated;
            if (buildit->linked_buildit != NULL) {
                data->end_mtx.m30 -= buildit->linked_buildit->file_position.x;
                data->end_mtx.m31 -= buildit->linked_buildit->file_position.y;
                data->end_mtx.m32 -= buildit->linked_buildit->file_position.z;
                NuMtxRotateY(&data->end_mtx, buildit->field_0x7c);
                data->end_mtx.m30 += buildit->file_position.x;
                data->end_mtx.m31 += buildit->file_position.y;
                data->end_mtx.m32 += buildit->file_position.z;
            }

            if (data->start_mtx.m30 < start_min.x)
                start_min.x = data->start_mtx.m30;
            if (data->start_mtx.m31 < start_min.y)
                start_min.y = data->start_mtx.m31;
            if (data->start_mtx.m32 < start_min.z)
                start_min.z = data->start_mtx.m32;
            if (data->start_mtx.m30 > start_max.x)
                start_max.x = data->start_mtx.m30;
            if (data->start_mtx.m31 > start_max.y)
                start_max.y = data->start_mtx.m31;
            if (data->start_mtx.m32 > start_max.z)
                start_max.z = data->start_mtx.m32;

            if (data->end_mtx.m30 < end_min.x)
                end_min.x = data->end_mtx.m30;
            if (data->end_mtx.m31 < end_min.y)
                end_min.y = data->end_mtx.m31;
            if (data->end_mtx.m32 < end_min.z)
                end_min.z = data->end_mtx.m32;
            if (data->end_mtx.m30 > end_max.x)
                end_max.x = data->end_mtx.m30;
            if (data->end_mtx.m31 > end_max.y)
                end_max.y = data->end_mtx.m31;
            if (data->end_mtx.m32 > end_max.z)
                end_max.z = data->end_mtx.m32;

            data->wobble_time = 0.2f;
        }

        buildit->start_position.x = (start_min.x + start_max.x) * 0.5f;
        buildit->start_position.y = (start_min.y + start_max.y) * 0.5f;
        buildit->start_position.z = (start_min.z + start_max.z) * 0.5f;
        buildit->position.x = (end_min.x + end_max.x) * 0.5f;
        buildit->position.y = (end_min.y + end_max.y) * 0.5f;
        buildit->position.z = (end_min.z + end_max.z) * 0.5f;

        f32 span = start_max.x - start_min.x;
        if (span < start_max.z - start_min.z) {
            span = start_max.z - start_min.z;
        }
        buildit->bounds_radius = span * NuTrigTable[0x3000];
    }

    if (buildit->field_0x5c == 0) {
        buildit->state_flags &= static_cast<u16>(~GIZBUILDIT_STATE_ROTATING_WOBBLE);
    } else {
        buildit->state_flags |= GIZBUILDIT_STATE_ROTATING_WOBBLE;
    }
    buildit->build_state = GIZBUILDIT_BUILD_IDLE;
    buildit->built_object_count = 0;

    if (world_ptr != NULL) {
        WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
        buildit->room_index = world->current_gscn != NULL
                                  ? static_cast<i16>(NuPortalWhichRoom(world->current_gscn, &buildit->file_position))
                                  : -1;
    }
}

void GizDrawBuildItPiece(GameObject_s *, i32) {
}

void ForceBuildItToUseNext(GIZBUILDIT_s &) {
}

void GizGetBuildItPlayerPos(GameObject_s *, nuvec_s *, nuvec_s *) {
}

void GizBuildit_SetVisibility(GIZBUILDIT_s *buildit, i32 visible) {
    if (buildit == NULL) {
        return;
    }

    GameAnimSet_SetVisibility(buildit->anim_set, visible);
    buildit->availability_flags = static_cast<u8>((buildit->availability_flags & ~GIZBUILDIT_AVAILABILITY_VISIBLE) |
                                                  (visible != 0 ? GIZBUILDIT_AVAILABILITY_VISIBLE : 0));
}

void GizMoveAttractoBuildItPiece(GIZBUILDIT_s *, GAMEANIMOBJ_s *) {
}
