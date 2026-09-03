#include "legoapi/gizmos/object/gizobstacles.h"

#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"

extern "C" {
    i16 FindPlatInst(i32 instance_ix);
    i32 NuPortalWhichRoom(NUGSCN *scene, NUVEC *position);
    i32 IsSfxLooping(i32 sfx_id);
}

i32 GizmoBlowupGetNameTableId(char *name);
i32 GizmoBlowupGetTypeFromNameTableId(WORLDINFO_s *world, i32 name_table_id);
void GizmoBlowUpTypeBlowUp(WORLDINFO_s *world, i32 blowup_type, NUVEC *position);
void AddShoveObject(nuhspecial_s *special, i16 character_id);
void AddPickups(i32 count, i32 pickup_type, i32 param3, i32 param4, NUVEC *position, NUVEC *direction, f32 speed,
                i32 source, f32 scatter_height, f32 lifetime, GameObject_s *owner, i32 param12, i32 param13,
                bool param14);
void GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 flags, i32 volume);
void GameAnimSet_GetAveragePos(GAMEANIMSET_s *set, NUVEC *position, i32 include_static, i32 include_animated,
                               i32 include_hidden);
void GameAnimSet_DrawReflection(GAMEANIMSET_s *set, i32 flags, f32 alpha, numtx_s *matrix);

namespace {

    enum : i32 {
        GIZOBSTACLE_PROGRESS_CAPACITY = 128,
        GIZOBSTACLE_PROGRESS_WORDS = GIZOBSTACLE_PROGRESS_CAPACITY / 32,
    };

    enum GIZOBSTACLE_OUTPUT : i32 {
        GIZOBSTACLE_OUTPUT_AT_END = 0,
        GIZOBSTACLE_OUTPUT_NOT_AT_START = 1,
        GIZOBSTACLE_OUTPUT_PROXIMITY = 2,
        GIZOBSTACLE_OUTPUT_AT_START = 3,
        GIZOBSTACLE_OUTPUT_PLAYING_FORWARD = 4,
    };

    enum GIZOBSTACLE_ACTIVATE_REVERSE_FLAGS : u32 {
        GIZOBSTACLE_ACTIVATE_REVERSE_VALIDATE = 0x01,
        GIZOBSTACLE_ACTIVATE_REVERSE_ALLOW_MATCHING_STATE = 0x04,
    };

    struct GIZOBSTACLEPROGRESS_s {
        u32 progress_flag_0[GIZOBSTACLE_PROGRESS_WORDS];
        u32 progress_flag_1[GIZOBSTACLE_PROGRESS_WORDS];
        u32 runtime_flag_0[GIZOBSTACLE_PROGRESS_WORDS];
        u32 runtime_flag_2[GIZOBSTACLE_PROGRESS_WORDS];
        u32 runtime_flag_3[GIZOBSTACLE_PROGRESS_WORDS];
        u32 runtime_flag_6[GIZOBSTACLE_PROGRESS_WORDS];
        u32 runtime_flag_7[GIZOBSTACLE_PROGRESS_WORDS];
    };

    DECOMP_ASSERT(sizeof(GIZOBSTACLEPROGRESS_s) == 0x70, "GIZOBSTACLE progress ABI");

    static void ClearObstacleProgress(GIZOBSTACLEPROGRESS_s *progress) {
        if (progress == NULL) {
            return;
        }

        for (i32 word = 0; word < GIZOBSTACLE_PROGRESS_WORDS; ++word) {
            progress->progress_flag_0[word] = 0xffffffff;
            progress->progress_flag_1[word] = 0xffffffff;
            progress->runtime_flag_0[word] = 0;
            progress->runtime_flag_2[word] = 0;
            progress->runtime_flag_3[word] = 0;
            progress->runtime_flag_6[word] = 0;
            progress->runtime_flag_7[word] = 0;
        }
    }

} // namespace

static void Gizobstacle_ReadAnimSetData(GAMEANIMOBJ_s *object, unsigned char version);

NUVEC *gizobstacletriggers[16];
i32 ngizobstacletriggers;

i32 obstacle_gizmotype_id = -1;

static i32 GizObstacles_GetMaxGizmos(void *obstacle) {
    WORLDINFO *world = static_cast<WORLDINFO *>(obstacle);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_obstacles;
}

static void GizObstacles_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *data) {
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    if (obstacle_sys != NULL) {
        if (obstacle_sys->count != 0) {
            for (i32 i = 0; i < obstacle_sys->count; ++i) {
                if (NuStrLen(obstacle_sys->obstacles[i].name) != 0) {
                    AddGizmo(gizmo_sys, type_id, NULL, &obstacle_sys->obstacles[i]);
                }
            }
        }
    }
}

static void GizObstacles_Update(void *world_ptr, void *data, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    if (obstacle_sys == NULL || world == NULL || world->gizmo_sys == NULL || world->gizmo_sys->sets == NULL) {
        return;
    }

    GIZMOSET &gizmo_set = world->gizmo_sys->sets[obstacle_gizmotype_id];
    obstacle_sys->active_gizmo_count = 0;

    for (i32 index = 0; index < gizmo_set.count; ++index) {
        GIZMO *gizmo = &gizmo_set.gizmos[index];
        GIZOBSTACLE_s *obstacle = static_cast<GIZOBSTACLE_s *>(gizmo->object);
        if (obstacle == NULL) {
            continue;
        }

        GameObject_s *previous_triggering_object = obstacle->triggering_object;
        obstacle->triggering_object = NULL;
        if ((obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) == 0 ||
            (obstacle->progress_flags &
             (GIZOBSTACLE_PROGRESS_FLAG_ENABLED | GIZOBSTACLE_PROGRESS_FLAG_EXTERNAL_CONTROL)) == 0 ||
            obstacle->anim_set == NULL || (obstacle->control_flags & GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE) != 0 ||
            (obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_DESTROYED) != 0) {
            continue;
        }

        GAMEANIMSET_s *anim_set = obstacle->anim_set;
        if (obstacle->mode == 7) {
            if ((obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_PUSH_CONTROL) == 0) {
                if (obstacle->state == 1) {
                    obstacle->animation_speed = -1.0f;
                    GizObstacle_PlayForwards(obstacle);
                    anim_set = obstacle->anim_set;
                } else {
                    obstacle->animation_speed = 0.0f;
                }
            } else {
                obstacle->triggering_object = previous_triggering_object;
                if (obstacle->animation_speed != 0.0f) {
                    GizObstacle_PlayForwards(obstacle);
                    anim_set = obstacle->anim_set;
                }
            }
            GizObstacle_Stop(obstacle);
            anim_set = obstacle->anim_set;
        } else if ((obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_EXTERNAL_CONTROL) == 0) {
            GIZOBSTACLEUPDATEFN update_fn = gizobstacleupdatefns[obstacle->mode];
            if (update_fn != NULL) {
                update_fn(obstacle);
                anim_set = obstacle->anim_set;
            }
        } else if (obstacle->animation_speed == 0.0f) {
            GizObstacle_Stop(obstacle);
            anim_set = obstacle->anim_set;
        } else {
            GizObstacle_PlayForwards(obstacle);
            anim_set = obstacle->anim_set;
        }

        if ((anim_set->flags & 7) != 0) {
            GizObstacle_EvalAveragePosAndRadius(obstacle, 2);

            if ((obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_ANIM_OBJECT_BIT_1) != 0) {
                for (GAMEANIMOBJ_s *object = anim_set->objects; object != NULL; object = object->next) {
                    i16 *object_data = static_cast<i16 *>(object->object_data);
                    if (object_data != NULL && (object_data[0] & 2) != 0) {
                        AddShoveObject(&object->special, object_data[1]);
                    }
                }
                anim_set = obstacle->anim_set;
            }

            const bool playing_reverse = obstacle->animation_speed < 0.0f;
            const i16 sfx_id = playing_reverse ? obstacle->stop_sfx_id : obstacle->start_sfx_id;
            const GAMEANIMSET_STATE endpoint = playing_reverse ? GAMEANIMSET_STATE_AT_END : GAMEANIMSET_STATE_AT_START;
            if (sfx_id != -1 && (anim_set->state == endpoint || IsSfxLooping(sfx_id) != 0)) {
                GameAudio_PlaySfxById(sfx_id, &obstacle->evaluated_position, 0, 0);
                anim_set = obstacle->anim_set;
            }
        }

        if (anim_set->state == GAMEANIMSET_STATE_AT_END) {
            obstacle->runtime_flags &= static_cast<u8>(~GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED);
        }

        const bool completion_mode = obstacle->state == 0 || obstacle->state == 3 ||
                                     (obstacle->config_flags & GIZOBSTACLE_CONFIG_RESET_AFTER_COMPLETION) != 0;
        const bool animation_finished =
            ((anim_set->flags & GAMEANIMSET_FLAG_STOP_REQUESTED) != 0 || anim_set->animated_object_count == 0) &&
            anim_set->state == GAMEANIMSET_STATE_AT_END;
        if (completion_mode && animation_finished) {
            if (obstacle->blowup_type != -1) {
                if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_BLOWUP_AT_ANIM_OBJECTS) == 0) {
                    GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, &obstacle->evaluated_position);
                } else {
                    for (GAMEANIMOBJ_s *object = anim_set->objects; object != NULL; object = object->next) {
                        NUVEC *draw_position = NuSpecialGetDrawPos(&object->special);
                        if (draw_position != NULL) {
                            GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, draw_position);
                        }
                    }
                }
                GameAnimSet_SetVisibility(obstacle->anim_set, 0);
                obstacle->runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_DESTROYED;
            }

            if (obstacle->pickup_count != 0 &&
                ((obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED) == 0 ||
                 (obstacle->config_flags & GIZOBSTACLE_CONFIG_RESET_AFTER_COMPLETION) != 0)) {
                NUVEC pickup_position;
                NUVEC pickup_direction;
                NuVecAdd(&pickup_position, &obstacle->evaluated_position, &obstacle->pickup_offset);
                NuVecRotateX(&pickup_direction, &v010, obstacle->pickup_direction_x);
                NuVecRotateY(&pickup_direction, &pickup_direction, obstacle->pickup_direction_y);
                AddPickups(static_cast<u16>(obstacle->pickup_count), 0, 0, 0, &pickup_position, &pickup_direction, 2.0f,
                           -1, obstacle->pickup_scatter_height, 2000000.0f, NULL, 1, 0, true);
                obstacle->runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED;
            }

            if (obstacle->state == 3) {
                GameAnimSet_JumpToStart(obstacle->anim_set);
                GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
                GameAnimSet_SetVisibility(obstacle->anim_set, 1);
                obstacle->runtime_flags &= static_cast<u8>(~GIZOBSTACLE_RUNTIME_FLAG_DESTROYED);
            }
        }

        if ((obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_DESTROYED) == 0 &&
            obstacle->anim_set->state != GAMEANIMSET_STATE_AT_END &&
            (obstacle->config_flags &
             (GIZOBSTACLE_CONFIG_ALWAYS_RUN_PROXIMITY | GIZOBSTACLE_CONFIG_ADD_TO_ACTIVE_LIST)) != 0) {
            obstacle_sys->active_gizmos[obstacle_sys->active_gizmo_count++] = gizmo;
        }

        obstacle->animation_speed = 1.0f;
        obstacle->progress_flags &=
            static_cast<u8>(~(GIZOBSTACLE_PROGRESS_FLAG_EXTERNAL_CONTROL | GIZOBSTACLE_PROGRESS_FLAG_PUSH_CONTROL));
    }

    ngizobstacletriggers = 0;
}

static void GizObstacles_Draw(void *world_ptr, void *data, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);

    if (obstacle_sys != NULL) {
        GIZOBSTACLE_s *obstacle = obstacle_sys->obstacles;
        if (obstacle_sys->count != 0) {
            for (i32 index = 0; obstacle_sys->count > index; ++index, ++obstacle) {
                if ((obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) != 0 &&
                    (obstacle->room_id < 0 || world->rooms_visible_ptr[obstacle->room_id] != 0) &&
                    static_cast<i8>(obstacle->runtime_flags) >= 0 &&
                    (obstacle->config_flags & GIZOBSTACLE_CONFIG_DRAW_REFLECTION) != 0) {
                    GameAnimSet_DrawReflection(obstacle->anim_set, 2, obstacle->reflection_alpha, NULL);
                }
            }
        }
    }
}

static char *GizmoObstacle_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZOBSTACLE_s *obstacle = static_cast<GIZOBSTACLE_s *>(gizmo->object);
        if (obstacle != NULL) {
            return obstacle->name;
        }
    }
    return NULL;
}

static i32 GizmoObstacle_GetOutput(GIZMO *gizmo, i32 output_index, i32 ignore_activation_state) {
    GIZOBSTACLE_s *obstacle = gizmo != NULL ? static_cast<GIZOBSTACLE_s *>(gizmo->object) : NULL;
    if (obstacle == NULL) {
        return 0;
    }
    if ((obstacle->progress_flags & (GIZOBSTACLE_PROGRESS_FLAG_ENABLED | GIZOBSTACLE_PROGRESS_FLAG_VISIBLE)) !=
            (GIZOBSTACLE_PROGRESS_FLAG_ENABLED | GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) &&
        ignore_activation_state == 0) {
        return 0;
    }

    switch (output_index) {
        case GIZOBSTACLE_OUTPUT_AT_END:
            return obstacle->anim_set != NULL && obstacle->anim_set->state == GAMEANIMSET_STATE_AT_END;
        case GIZOBSTACLE_OUTPUT_NOT_AT_START:
            return obstacle->anim_set != NULL && obstacle->anim_set->state != GAMEANIMSET_STATE_AT_START;
        case GIZOBSTACLE_OUTPUT_PROXIMITY:
            return obstacle->anim_set != NULL && obstacle->proximity_output != 0;
        case GIZOBSTACLE_OUTPUT_AT_START:
            return obstacle->anim_set != NULL && obstacle->anim_set->state == GAMEANIMSET_STATE_AT_START;
        case GIZOBSTACLE_OUTPUT_PLAYING_FORWARD:
            return obstacle->anim_set != NULL && obstacle->anim_set->state == GAMEANIMSET_STATE_ACTIVE_FORWARD;
        default:
            return 0;
    }
}

static char *GizmoObstacle_GetOutputName(GIZMO *, i32 output_index) {
    switch (output_index) {
        case GIZOBSTACLE_OUTPUT_AT_END:
            return const_cast<char *>("AtEnd");
        case GIZOBSTACLE_OUTPUT_NOT_AT_START:
            return const_cast<char *>("NotAtStart");
        case GIZOBSTACLE_OUTPUT_PROXIMITY:
            return const_cast<char *>("Proximity");
        case GIZOBSTACLE_OUTPUT_AT_START:
            return const_cast<char *>("AtStart");
        case GIZOBSTACLE_OUTPUT_PLAYING_FORWARD:
            return const_cast<char *>("PlayingForward");
        default:
            return NULL;
    }
}

static i32 GizmoObstacle_GetNumOutputs(GIZMO *) {
    return 5;
}

static void GizmoObstacle_Activate(GIZMO *gizmo, i32 activate) {
    if (gizmo == NULL) {
        return;
    }
    GIZOBSTACLE_s *obstacle = static_cast<GIZOBSTACLE_s *>(gizmo->object);
    if (obstacle == NULL) {
        return;
    }

    if (activate == 0) {
        obstacle->progress_flags &= static_cast<u8>(~GIZOBSTACLE_PROGRESS_FLAG_ENABLED);
        obstacle->proximity_output = 0;
        return;
    }

    if ((obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_ENABLED) == 0) {
        GameAnimSet_JumpToStart(obstacle->anim_set);
    }
    obstacle->proximity_output = 0;
    obstacle->progress_flags |= GIZOBSTACLE_PROGRESS_FLAG_ENABLED;
    obstacle->runtime_flags &= static_cast<u8>(~GIZOBSTACLE_RUNTIME_FLAG_DESTROYED);
    obstacle->control_flags &= static_cast<u8>(~GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE);
    GameAnimSet_EvaluateState(obstacle->anim_set);
}

static i32 GizmoObstacle_ActivateRev(GIZMO *gizmo, i32 activate, i32 flags) {
    GIZOBSTACLE_s *obstacle = gizmo != NULL ? static_cast<GIZOBSTACLE_s *>(gizmo->object) : NULL;
    if (obstacle == NULL) {
        return 0;
    }

    const u8 initial_runtime_flags = obstacle->runtime_flags;
    if ((obstacle->control_flags & GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE) != 0 && obstacle->anim_set != NULL &&
        (obstacle->anim_set->state == GAMEANIMSET_STATE_ACTIVE_FORWARD ||
         obstacle->anim_set->state == GAMEANIMSET_STATE_AT_END)) {
        obstacle->progress_flags |= GIZOBSTACLE_PROGRESS_FLAG_ENABLED;
        obstacle->control_flags &= static_cast<u8>(~GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE);
    }
    if ((initial_runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE) != 0) {
        obstacle->progress_flags |= GIZOBSTACLE_PROGRESS_FLAG_ENABLED;
        obstacle->control_flags &= static_cast<u8>(~GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE);
    }

    if ((flags & GIZOBSTACLE_ACTIVATE_REVERSE_VALIDATE) == 0) {
        if (activate == 0) {
            obstacle->progress_flags |= GIZOBSTACLE_PROGRESS_FLAG_ENABLED;
            obstacle->control_flags &= static_cast<u8>(~GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE);
            return 1;
        }
        GizObstacle_PlayBackwards(obstacle);
        obstacle->control_flags |= GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE;
        obstacle->progress_flags &= static_cast<u8>(~GIZOBSTACLE_PROGRESS_FLAG_ENABLED);
        return 1;
    }

    const i32 reverse_active = (obstacle->control_flags & GIZOBSTACLE_CONTROL_FLAG_REVERSE_ACTIVE) != 0 ? 1 : 0;
    if (activate == reverse_active) {
        if ((flags & GIZOBSTACLE_ACTIVATE_REVERSE_ALLOW_MATCHING_STATE) == 0) {
            return 0;
        }
        const i32 visible = (obstacle->progress_flags & GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) != 0 ? 1 : 0;
        if (visible != activate) {
            return 0;
        }
    }
    if (activate == 1 && (initial_runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE) != 0) {
        return 0;
    }
    return 1;
}

static void GizmoObstacle_SetVisibility(GIZMO *gizmo, i32 visibility) {
    GIZOBSTACLE_s *obstacle = gizmo != NULL ? static_cast<GIZOBSTACLE_s *>(gizmo->object) : NULL;
    if (obstacle == NULL) {
        return;
    }
    GameAnimSet_SetVisibility(obstacle->anim_set, visibility);
    obstacle->progress_flags = static_cast<u8>((obstacle->progress_flags & ~GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) |
                                               (visibility != 0 ? GIZOBSTACLE_PROGRESS_FLAG_VISIBLE : 0));
}

static NUVEC *GizmoObstacle_GetPos(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZOBSTACLE_s *obstacle = static_cast<GIZOBSTACLE_s *>(gizmo->object);
        if (obstacle != NULL) {
            return &obstacle->position;
        }
    }
    return NULL;
}

static i32 GizObstacles_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizObstacles_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float,
                                           float, i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizObstacles_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                                unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizObstacles_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZOBSTACLEPROGRESS_s));
}

static void GizObstacles_ClearProgress(void *, void *progress_ptr) {
    ClearObstacleProgress(static_cast<GIZOBSTACLEPROGRESS_s *>(progress_ptr));
}

static void GizObstacles_StoreProgress(void *, void *data, void *progress_ptr) {
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    GIZOBSTACLEPROGRESS_s *progress = static_cast<GIZOBSTACLEPROGRESS_s *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    ClearObstacleProgress(progress);
    if (obstacle_sys == NULL) {
        return;
    }

    i32 count = obstacle_sys->count;
    if (count > GIZOBSTACLE_PROGRESS_CAPACITY) {
        count = GIZOBSTACLE_PROGRESS_CAPACITY;
    }

    for (i32 index = 0; index < count; ++index) {
        const GIZOBSTACLE_s &obstacle = obstacle_sys->obstacles[index];
        const i32 word = index >> 5;
        const u32 bit = 1u << (index & 31);

        if ((obstacle.progress_flags & GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) == 0) {
            progress->progress_flag_1[word] &= ~bit;
        }
        if ((obstacle.progress_flags & GIZOBSTACLE_PROGRESS_FLAG_ENABLED) == 0) {
            progress->progress_flag_0[word] &= ~bit;
        }
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED) != 0) {
            progress->runtime_flag_0[word] |= bit;
        }
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE) != 0) {
            progress->runtime_flag_2[word] |= bit;
        }
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_BLOCKED) != 0) {
            progress->runtime_flag_3[word] |= bit;
        }
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED) != 0) {
            progress->runtime_flag_6[word] |= bit;
        }
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_DESTROYED) != 0) {
            progress->runtime_flag_7[word] |= bit;
        }
    }
}

static void GizObstacles_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    GIZOBSTACLEPROGRESS_s *progress = static_cast<GIZOBSTACLEPROGRESS_s *>(progress_ptr);
    for (i32 index = 0; index < obstacle_sys->count; ++index) {
        GIZOBSTACLE_s &obstacle = obstacle_sys->obstacles[index];
        obstacle.progress_flags = static_cast<u8>(
            (obstacle.progress_flags | GIZOBSTACLE_PROGRESS_FLAG_ENABLED | GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) &
            ~(GIZOBSTACLE_PROGRESS_FLAG_EXTERNAL_CONTROL | GIZOBSTACLE_PROGRESS_FLAG_PUSH_CONTROL));
        obstacle.runtime_flags &=
            static_cast<u8>(GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED | GIZOBSTACLE_RUNTIME_FLAG_PENDING_BLOWUP_TYPE);
        obstacle.animation_speed = 1.0f;

        GAMEANIMSET_s *anim_set = obstacle.anim_set;
        if (anim_set != NULL) {
            for (GAMEANIMOBJ_s *object = anim_set->objects; object != NULL; object = object->next) {
                i16 fallback_object_data[2] = {};
                i16 *object_data = static_cast<i16 *>(object->object_data);
                if (object_data == NULL) {
                    object_data = fallback_object_data;
                }

                object_data[1] = -1;
                if (world->terrain != NULL && (static_cast<u32 *>(world->terrain)[0] & 1) == 0 &&
                    NuSpecialExistsFn(&object->special) != 0) {
                    object_data[1] = FindPlatInst(NuSpecialGetInstanceix(&object->special));
                }
                if (object_data[1] != -1) {
                    obstacle.runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_HAS_PLATFORM;
                }
                if ((object_data[0] & 2) != 0) {
                    obstacle.runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_ANIM_OBJECT_BIT_1;
                }
            }

            obstacle.room_id = world->current_gscn != NULL
                                   ? static_cast<i16>(NuPortalWhichRoom(world->current_gscn, &obstacle.position))
                                   : -1;
            if ((obstacle.config_flags & GIZOBSTACLE_CONFIG_USE_ANIM_AVERAGE_POSITION) != 0) {
                GameAnimSet_GetAveragePos(anim_set, &obstacle.secondary_position, 0, 1, 1);
            }
            GameAnimSet_EvaluateState(anim_set);
            GizObstacle_EvalAveragePosAndRadius(&obstacle, 2);
        }

        if (progress != NULL && index < GIZOBSTACLE_PROGRESS_CAPACITY) {
            const i32 word = index >> 5;
            const u32 bit = 1u << (index & 31);

            obstacle.progress_flags =
                static_cast<u8>((obstacle.progress_flags & ~GIZOBSTACLE_PROGRESS_FLAG_VISIBLE) |
                                ((progress->progress_flag_1[word] & bit) != 0 ? GIZOBSTACLE_PROGRESS_FLAG_VISIBLE : 0));
            obstacle.progress_flags =
                static_cast<u8>((obstacle.progress_flags & ~GIZOBSTACLE_PROGRESS_FLAG_ENABLED) |
                                ((progress->progress_flag_0[word] & bit) != 0 ? GIZOBSTACLE_PROGRESS_FLAG_ENABLED : 0));
            obstacle.runtime_flags = static_cast<u8>(
                (obstacle.runtime_flags & ~GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED) |
                ((progress->runtime_flag_0[word] & bit) != 0 ? GIZOBSTACLE_RUNTIME_FLAG_PICKUPS_SPAWNED : 0));
            obstacle.runtime_flags =
                static_cast<u8>((obstacle.runtime_flags & ~GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE) |
                                ((progress->runtime_flag_2[word] & bit) != 0 ? GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE : 0));
            obstacle.runtime_flags =
                static_cast<u8>((obstacle.runtime_flags & ~GIZOBSTACLE_RUNTIME_FLAG_BLOCKED) |
                                ((progress->runtime_flag_3[word] & bit) != 0 ? GIZOBSTACLE_RUNTIME_FLAG_BLOCKED : 0));
            obstacle.runtime_flags = static_cast<u8>(
                (obstacle.runtime_flags & ~GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED) |
                ((progress->runtime_flag_6[word] & bit) != 0 ? GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED : 0));
            obstacle.runtime_flags =
                static_cast<u8>((obstacle.runtime_flags & ~GIZOBSTACLE_RUNTIME_FLAG_DESTROYED) |
                                ((progress->runtime_flag_7[word] & bit) != 0 ? GIZOBSTACLE_RUNTIME_FLAG_DESTROYED : 0));
        }
    }
    ngizobstacletriggers = 0;
}

static void *GizObstacles_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZOBSTACLESYS_s)));

    obstacle_sys->capacity = world->current_level->max_obstacles;
    obstacle_sys->obstacles = static_cast<GIZOBSTACLE_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, obstacle_sys->capacity * sizeof(GIZOBSTACLE_s)));
    obstacle_sys->active_gizmos = static_cast<GIZMO **>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, obstacle_sys->capacity * sizeof(GIZMO *)));
    obstacle_sys->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 4,
                                                           world->current_level->max_obstacle_objs);

    for (i32 i = 0; i < obstacle_sys->capacity; ++i) {
        obstacle_sys->obstacles[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108, obstacle_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_obstacle_sys = obstacle_sys;
    return obstacle_sys;
}

static i32 GizObstacles_Load(void *world_ptr, void *data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    const u8 version = static_cast<u8>(EdFileReadChar());
    obstacle_sys->count = static_cast<u16>(EdFileReadShort());

    for (i32 index = 0; index < obstacle_sys->count; ++index) {
        GIZOBSTACLE_s &obstacle = obstacle_sys->obstacles[index];
        EdFileRead(obstacle.name, sizeof(obstacle.name));
        EdFileReadNuVec(&obstacle.position);

        if (version <= 1) {
            obstacle.secondary_position = obstacle.position;
        } else {
            EdFileReadNuVec(&obstacle.secondary_position);
        }

        obstacle.auto_return_delay = EdFileReadFloat();
        obstacle.trigger_radius = EdFileReadFloat();
        if (version != 2) {
            EdFileReadNuVec(&obstacle.trigger_box_half_extents);
            obstacle.trigger_box_yaw = EdFileReadShort();
            obstacle.config_flags = static_cast<u32>(EdFileReadInt());
            if (version > 11) {
                obstacle.field_0x6c = static_cast<u32>(EdFileReadInt());
            }
        }

        if (version == 6) {
            EdFileReadShort();
            EdFileReadChar();
        }
        obstacle.state = static_cast<u8>(EdFileReadChar());
        obstacle.mode = static_cast<u8>(EdFileReadChar());
        if (version <= 6) {
            obstacle.trigger_mode = 0xff;
        } else {
            obstacle.trigger_mode = static_cast<u8>(EdFileReadChar());
        }

        GizmoFileReadGameAnimSet(obstacle.anim_set, world, Gizobstacle_ReadAnimSetData, version, const_cast<char *>(""),
                                 obstacle.name);

        if (version <= 3) {
            obstacle.field_0x4c = 1.0f;
            obstacle.field_0x50 = 1.0f;
        } else {
            obstacle.field_0x4c = EdFileReadFloat();
            if (version <= 4) {
                obstacle.field_0x50 = obstacle.field_0x4c;
            } else {
                obstacle.field_0x50 = EdFileReadFloat();
            }
        }

        obstacle.blowup_type = -1;
        if (version > 7) {
            obstacle.reflection_alpha = EdFileReadFloat();

            if (version == 9) {
                obstacle.blowup_type = EdFileReadShort();
                obstacle.pickup_count = EdFileReadShort();
                obstacle.pickup_direction_x = EdFileReadShort();
                obstacle.pickup_direction_y = EdFileReadShort();
                EdFileReadNuVec(&obstacle.pickup_offset);
            } else if (version > 9) {
                char blowup_name[32];
                const i32 name_length = static_cast<i8>(EdFileReadChar());
                if (name_length != 0) {
                    EdFileRead(blowup_name, name_length);
                    obstacle.blowup_type = static_cast<i16>(GizmoBlowupGetNameTableId(blowup_name));
                    if (obstacle.blowup_type != -1) {
                        obstacle.runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_PENDING_BLOWUP_TYPE;
                    }
                }

                obstacle.pickup_count = EdFileReadShort();
                obstacle.pickup_direction_x = EdFileReadShort();
                obstacle.pickup_direction_y = EdFileReadShort();
                EdFileReadNuVec(&obstacle.pickup_offset);
            }
        }

        if (version <= 9 || version == 10) {
            obstacle.pickup_scatter_height = world->area != NULL && (world->area->flags & 1) != 0 ? -105.0f : -999.0f;
            obstacle.start_sfx_id = -1;
            obstacle.stop_sfx_id = -1;
        } else {
            obstacle.pickup_scatter_height = EdFileReadFloat();
            obstacle.start_sfx_id = -1;
            obstacle.stop_sfx_id = -1;
            if (version > 12) {
                char sfx_name[32];
                if (GizmoFileReadName(sfx_name) != 0) {
                    obstacle.start_sfx_id = static_cast<i16>(GetSfxId(sfx_name));
                }
                if (version > 13 && GizmoFileReadName(sfx_name) != 0) {
                    obstacle.stop_sfx_id = static_cast<i16>(GetSfxId(sfx_name));
                }
            }
        }

        if (GizObstacle_SetDefaultSFXFn != NULL) {
            GizObstacle_SetDefaultSFXFn(world, &obstacle);
        }
    }

    return 1;
}

static void Gizobstacle_ReadAnimSetData(GAMEANIMOBJ_s *object, unsigned char version) {
    if (object == NULL) {
        return;
    }

    u16 fallback_data = 0;
    u16 *object_data = static_cast<u16 *>(object->object_data);
    if (object_data == NULL) {
        object_data = &fallback_data;
    }

    if (version > 7) {
        object_data[0] = static_cast<u16>(EdFileReadShort());
    } else if ((object->flags & 2) != 0) {
        object_data[0] |= 1;
    }
}

static void GizObstacles_PostLoad(void *world_ptr, void *data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZOBSTACLESYS_s *obstacle_sys = static_cast<GIZOBSTACLESYS_s *>(data);
    if (obstacle_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < obstacle_sys->count; ++index) {
        GIZOBSTACLE_s &obstacle = obstacle_sys->obstacles[index];
        if ((obstacle.runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_PENDING_BLOWUP_TYPE) != 0) {
            obstacle.blowup_type = static_cast<i16>(GizmoBlowupGetTypeFromNameTableId(world, obstacle.blowup_type));
            obstacle.runtime_flags &= static_cast<u8>(~GIZOBSTACLE_RUNTIME_FLAG_PENDING_BLOWUP_TYPE);
        }
    }
}

static void GizObstacles_AddLevelSfx(void *, void *, i32 *, i32 *, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizObstacle";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoObstacle_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizObstacles_Update;
    addtype.fns.bolt_hit_fn = GizObstacles_BoltHit;
    addtype.fns.draw_fn = GizObstacles_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype.fns.get_output_fn = GizmoObstacle_GetOutput;
    addtype.fns.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype.fns.activate_fn = GizmoObstacle_Activate;
    addtype.fns.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizObstacles_ClearProgress;
    addtype.fns.store_progress_fn = GizObstacles_StoreProgress;
    addtype.fns.reset_fn = GizObstacles_Reset;
    addtype.fns.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype.fns.load_fn = GizObstacles_Load;
    addtype.fns.post_load_fn = GizObstacles_PostLoad;
    addtype.fns.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;

    return &addtype;
}
