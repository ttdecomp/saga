#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

namespace {

    struct OBSTACLE_CHARACTER_DATA_FLAGS_s {
        u32 flags_0x00;
        u8 flags_0x04;
        u8 flags_0x05;
    };

    static bool ObstacleCharacterInfoAllowsTrigger(const GameObject_s &) {
        // TODO: Restore the target CInfo table (0x10-byte entries, exclusion bit
        // at +9) and enforce its bit 0x80 here. Until the real table exists, the
        // safe normal-code fallback is to avoid rejecting otherwise valid actors.
        return true;
    }

    static bool ObstacleUsesBoxTrigger(const GIZOBSTACLE_s *obstacle) {
        return obstacle->mode == 5 || obstacle->mode == 6;
    }

    static bool ObstacleCharacterDataHasFlag(const GameObject_s *object, u8 offset, u8 flag) {
        const OBSTACLE_CHARACTER_DATA_FLAGS_s *data = static_cast<const OBSTACLE_CHARACTER_DATA_FLAGS_s *>(
            static_cast<const void *>(object->apiobj.character_data));
        if (data == NULL) {
            return false;
        }
        return offset == 4 ? (data->flags_0x04 & flag) != 0 : (data->flags_0x05 & flag) != 0;
    }

} // namespace

static void GizObstacleUpdate_Proximity(GIZOBSTACLE_s *obstacle);
static i32 GizObstacle_SatisfyingTerrainChecks(GIZOBSTACLE_s *obstacle, GameObject_s *object);

static i32 GizObstacle_PosWithinBox(GIZOBSTACLE_s *obstacle, NUVEC *position) {
    if (obstacle == NULL || position == NULL) {
        return 0;
    }

    NUVEC local_position;
    NuVecSub(&local_position, position, &obstacle->secondary_position);
    NuVecRotateY(&local_position, &local_position, -obstacle->trigger_box_yaw);
    return local_position.x >= -obstacle->trigger_box_half_extents.x &&
           local_position.x <= obstacle->trigger_box_half_extents.x && local_position.y >= 0.0f &&
           local_position.y <= obstacle->trigger_box_half_extents.y &&
           local_position.z >= -obstacle->trigger_box_half_extents.z &&
           local_position.z <= obstacle->trigger_box_half_extents.z;
}

void GizObstacle_Stop(GIZOBSTACLE_s *obstacle) {
    if (obstacle != NULL) {
        GameAnimSet_Stop(obstacle->anim_set);
    }
}

void GizObstacles_Hit(void *, GIZOBSTACLE_s *, nuvec_s *, i32, i32) {
}

void GizObstacle_JumpToEnd(GIZOBSTACLE_s *obstacle) {
    if (obstacle != NULL && obstacle->anim_set != NULL) {
        GameAnimSet_JumpToEnd(obstacle->anim_set);
    }
}

GIZOBSTACLE_s *GizObstacle_FindByName(GIZOBSTACLESYS_s *system, char *name) {
    if (system == NULL || name == NULL) {
        return NULL;
    }

    for (i32 index = 0; index < system->count; ++index) {
        GIZOBSTACLE_s *obstacle = &system->obstacles[index];
        if (NuStrICmp(obstacle->name, name) == 0) {
            return obstacle;
        }
    }

    return NULL;
}

void GizObstacle_FindNearest(GIZOBSTACLESYS_s *, nuvec_s *, GameObject_s *, float *, i32) {
}

void GizObstacle_JumpToStart(GIZOBSTACLE_s *obstacle) {
    if (obstacle != NULL && obstacle->anim_set != NULL) {
        GameAnimSet_JumpToStart(obstacle->anim_set);
    }
}

void GizObstacles_AddTrigger(nuvec_s *position) {
    if (position == NULL) {
        return;
    }
    if (ngizobstacletriggers >= 16) {
        AddGameMessage(const_cast<char *>("MAXGIZOBSTACLETRIGGERS needs increasing, ask a programmer!"), &v001, 0.4f,
                       NULL, 0.4f, 200, 200, 200, 0x2080, 0.0f);
        return;
    }
    gizobstacletriggers[ngizobstacletriggers++] = position;
}

void GizObstacles_TotalScore(void *) {
}

void GizObstacle_PlayForwards(GIZOBSTACLE_s *obstacle) {
    if (obstacle != NULL) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->state == 2);
        if (obstacle->animation_speed < 0.0f) {
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->field_0x50, 0);
        } else {
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->field_0x4c, 0);
        }
    }
}

void GizObstacle_PlayBackwards(GIZOBSTACLE_s *obstacle) {
    if (obstacle != NULL) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->state == 2);
        if (obstacle->animation_speed < 0.0f) {
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->field_0x4c, 0);
        } else {
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->field_0x50, 0);
        }
    }
}

void GizObstacle_SetPushControlled(GIZOBSTACLE_s *, GameObject_s *, float) {
}

void GizObstacle_SetDefaultSFXFn_LSW(void *, GIZOBSTACLE_s *) {
}

void GizObstacle_SetTechnoControlled(GIZOBSTACLE_s *, float) {
}

// The obstacle update path treats this callback as a boolean predicate.
i32 GizObstacle_CheckExcludeFlagsFn_LSW(GIZOBSTACLE_s *, GameObject_s *) {
    return 0;
}

void GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE_s *, i32) {
}

void GIZOBSTACLE_s::ClearMechObjectInterface() {
}

void GIZOBSTACLE_s::GetMechObjectInterface() {
}

// Obstacle modes dispatch through this exact eight-entry target table.

static void GizObstacleUpdate_PushOnly(GIZOBSTACLE_s *) {
}

static void GizObstacleUpdate_AutoStart(GIZOBSTACLE_s *obstacle) {
    if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_ALWAYS_RUN_PROXIMITY) != 0) {
        obstacle->runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_BLOCKED;
        GizObstacleUpdate_Proximity(obstacle);
        return;
    }
    if (obstacle->anim_set->state == GAMEANIMSET_STATE_AT_START ||
        obstacle->anim_set->state == GAMEANIMSET_STATE_ACTIVE_BACKWARD) {
        GizObstacle_PlayForwards(obstacle);
    }
}

static void GizObstacleUpdate_NoTrigger(GIZOBSTACLE_s *obstacle) {
    if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_ALWAYS_RUN_PROXIMITY) == 0 &&
        (obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE) == 0 &&
        obstacle->anim_set->state == GAMEANIMSET_STATE_AT_START) {
        return;
    }
    obstacle->runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_BLOCKED;
    GizObstacleUpdate_Proximity(obstacle);
}

static void GizObstacleUpdate_Proximity(GIZOBSTACLE_s *obstacle) {
    obstacle->proximity_output = 0;

    if ((obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_BLOCKED) == 0) {
        const f32 radius_squared = obstacle->trigger_radius * obstacle->trigger_radius;
        f32 closest_distance_squared = radius_squared;
        NUVEC *closest_position = NULL;
        GameObject_s *closest_object = NULL;
        u32 players_not_satisfying_terrain = 0;

        const bool camera_mode = obstacle->mode == 1 || obstacle->mode == 5;
        if (camera_mode && GameCam != NULL && GameCam->mode != -1 && obstacle->trigger_mode == 0xff &&
            (obstacle->config_flags &
             (GIZOBSTACLE_CONFIG_INVERT_PROXIMITY | GIZOBSTACLE_CONFIG_EXCLUDE_NON_PLAYER |
              GIZOBSTACLE_CONFIG_CHECK_SUPPORTING_PLATFORM | GIZOBSTACLE_CONFIG_REQUIRE_ACTIVE_PLAYER |
              GIZOBSTACLE_CONFIG_REQUIRE_LINKED_OBJECT | GIZOBSTACLE_CONFIG_REQUIRE_CHARACTER_DATA_FLAG_04)) == 0) {
            const f32 distance_squared = NuVecDistSqr(&GameCam->pos, &obstacle->secondary_position, NULL);
            if (distance_squared < closest_distance_squared &&
                (!ObstacleUsesBoxTrigger(obstacle) || GizObstacle_PosWithinBox(obstacle, &GameCam->pos) != 0)) {
                closest_distance_squared = distance_squared;
                closest_position = &GameCam->pos;
            }
        }

        const bool accepts_external_triggers =
            obstacle->mode == 1 || obstacle->mode == 2 || obstacle->mode == 5 || obstacle->mode == 6;
        if (accepts_external_triggers && (obstacle->config_flags & GIZOBSTACLE_CONFIG_INVERT_PROXIMITY) == 0) {
            for (i32 index = 0; index < ngizobstacletriggers; ++index) {
                NUVEC *trigger = gizobstacletriggers[index];
                const f32 distance_squared = NuVecDistSqr(&obstacle->secondary_position, trigger, NULL);
                if (distance_squared < closest_distance_squared &&
                    (!ObstacleUsesBoxTrigger(obstacle) || GizObstacle_PosWithinBox(obstacle, trigger) != 0)) {
                    closest_distance_squared = distance_squared;
                    closest_position = trigger;
                }
            }
        }

        for (i32 index = 0; index < HIGHGAMEOBJECT; ++index) {
            GameObject_s *object = &Obj[index];
            if ((object->apiobj.field_0x1f8 & (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER)) !=
                    (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER) ||
                object->apiobj.field_0x287 != 0 ||
                (LEGOCONTEXT_DOOMED != -1 && LEGOCONTEXT_DOOMED == static_cast<i8>(object->field_0x7a5)) ||
                (object->field_0x1050 & 1) == 0 || (object->field_0xe20 & 0x20) != 0) {
                continue;
            }

            const i8 player_index = object->apiobj.field_0x27c;
            if (player_index == -1) {
                if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_EXCLUDE_NON_PLAYER) != 0) {
                    continue;
                }
            } else {
                players_not_satisfying_terrain |= 1u << (static_cast<u8>(player_index) & 31);
            }

            if (((obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_ACTIVE_PLAYER) != 0 &&
                 (object->apiobj.field_0x1f8 & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) ||
                !ObstacleCharacterInfoAllowsTrigger(*object) ||
                ((obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_LINKED_OBJECT) != 0 &&
                 object->field_0xcc0 == NULL) ||
                ((obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_CHARACTER_DATA_FLAG_04) != 0 &&
                 !ObstacleCharacterDataHasFlag(object, 4, 0x04)) ||
                ((obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_CHARACTER_DATA_FLAG_20) != 0 &&
                 !ObstacleCharacterDataHasFlag(object, 5, 0x20)) ||
                (GizObstacle_CheckExcludeFlagsFn != NULL && GizObstacle_CheckExcludeFlagsFn(obstacle, object) != 0)) {
                continue;
            }

            const bool requires_obstacle_platform =
                (obstacle->mode & 0xfb) == 2 && object->apiobj.field_0x27d == 0 &&
                (object->field_0xf03 & GAMEOBJECT_F03_FLAG_OBSTACLE_TERRAIN_VALID) == 0 && object->field_0xe31 != 1;
            if (requires_obstacle_platform) {
                bool on_obstacle_platform = false;
                if (obstacle->anim_set != NULL) {
                    for (GAMEANIMOBJ_s *anim_object = obstacle->anim_set->objects; anim_object != NULL;
                         anim_object = anim_object->next) {
                        i16 *object_data = static_cast<i16 *>(anim_object->object_data);
                        if (object_data != NULL && object_data[1] != -1 &&
                            object_data[1] == object->apiobj.supporting_platform_id) {
                            on_obstacle_platform = true;
                            break;
                        }
                    }
                }
                if (!on_obstacle_platform) {
                    continue;
                }
            }

            const f32 distance_squared =
                NuVecDistSqr(&obstacle->secondary_position, &object->apiobj.lower_position, NULL);
            NUVEC object_position = {object->apiobj.pos_x, object->apiobj.pos_y, object->apiobj.pos_z};
            const bool within_box =
                !ObstacleUsesBoxTrigger(obstacle) || GizObstacle_PosWithinBox(obstacle, &object_position) != 0;

            if (distance_squared < closest_distance_squared) {
                if (within_box && GizObstacle_SatisfyingTerrainChecks(obstacle, object) != 0) {
                    if (player_index != -1) {
                        players_not_satisfying_terrain &= ~(1u << (static_cast<u8>(player_index) & 31));
                    }
                    closest_distance_squared = distance_squared;
                    closest_position = &object->apiobj.lower_position;
                    closest_object = object;
                }
            } else if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_ALL_PLAYERS) != 0 && player_index != -1 &&
                       distance_squared < radius_squared && within_box &&
                       GizObstacle_SatisfyingTerrainChecks(obstacle, object) != 0) {
                players_not_satisfying_terrain &= ~(1u << (static_cast<u8>(player_index) & 31));
            }
        }

        const bool invert = (obstacle->config_flags & GIZOBSTACLE_CONFIG_INVERT_PROXIMITY) != 0;
        const bool all_players_satisfied = (obstacle->config_flags & GIZOBSTACLE_CONFIG_REQUIRE_ALL_PLAYERS) == 0 ||
                                           players_not_satisfying_terrain == 0;
        if ((invert && closest_position == NULL) || (!invert && closest_position != NULL && all_players_satisfied)) {
            obstacle->proximity_output = 1;
            obstacle->triggering_object = closest_object;
        }
    }

    const GAMEANIMSET_STATE state = obstacle->anim_set->state;
    if (state == GAMEANIMSET_STATE_AT_START) {
        if (obstacle->proximity_output != 0 ||
            (obstacle->runtime_flags &
             (GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE | GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED)) != 0) {
            GizObstacle_PlayForwards(obstacle);
            obstacle->auto_return_timer = obstacle->auto_return_delay;
        }
        return;
    }

    bool may_return = state == GAMEANIMSET_STATE_AT_END && obstacle->state != 0;
    if (!may_return) {
        may_return = WORLD != NULL && WORLD->current_level == BLOCKADERUNNERC_LDATA && LevGizObst[0] == obstacle &&
                     state == GAMEANIMSET_STATE_BETWEEN_ENDPOINTS;
    }
    if (!may_return || obstacle->proximity_output != 0 ||
        (obstacle->runtime_flags & (GIZOBSTACLE_RUNTIME_FLAG_AI_ACTIVE | GIZOBSTACLE_RUNTIME_FLAG_TRIGGER_LATCHED)) !=
            0) {
        return;
    }

    if (obstacle->auto_return_timer <= 0.0f) {
        GizObstacle_PlayBackwards(obstacle);
    } else if (state == GAMEANIMSET_STATE_AT_END) {
        obstacle->auto_return_timer -= FRAMETIME;
    }
}

static void GizObstacleUpdate_TechnoOnly(GIZOBSTACLE_s *) {
}

static i32 GizObstacle_SatisfyingTerrainChecks(GIZOBSTACLE_s *obstacle, GameObject_s *object) {
    if (static_cast<i8>(obstacle->trigger_mode) < 0) {
        if ((obstacle->config_flags & GIZOBSTACLE_CONFIG_CHECK_SUPPORTING_PLATFORM) == 0) {
            return 1;
        }
        if (object->apiobj.field_0x27d == 0 &&
            (object->field_0xf03 & GAMEOBJECT_F03_FLAG_OBSTACLE_TERRAIN_VALID) == 0) {
            return 0;
        }
        if (object->apiobj.supporting_platform_id >= 0) {
            if (obstacle->anim_set == NULL || (obstacle->runtime_flags & GIZOBSTACLE_RUNTIME_FLAG_HAS_PLATFORM) == 0) {
                return 0;
            }
            for (GAMEANIMOBJ_s *anim_object = obstacle->anim_set->objects; anim_object != NULL;
                 anim_object = anim_object->next) {
                i16 *object_data = static_cast<i16 *>(anim_object->object_data);
                if (object_data != NULL && object_data[1] == object->apiobj.supporting_platform_id) {
                    return 1;
                }
            }
        }
        return 0;
    }

    if ((object->apiobj.field_0x27d == 0 && (object->field_0xf03 & GAMEOBJECT_F03_FLAG_OBSTACLE_TERRAIN_VALID) == 0) ||
        obstacle->trigger_mode != object->apiobj.field_0x281) {
        return 0;
    }
    return 1;
}

GIZOBSTACLEUPDATEFN gizobstacleupdatefns[8] = {
    GizObstacleUpdate_AutoStart,  GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_NoTrigger,
    GizObstacleUpdate_TechnoOnly, GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_PushOnly,
};
