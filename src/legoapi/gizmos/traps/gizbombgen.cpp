#include "legoapi/gizmos/traps/gizbombgen.h"

#include "decomp.h"
#include "globals.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

i32 bombgen_gizmotype_id = -1;

struct GIZBOMBGENPROGRESS {
    u32 active_masks[3];
    u32 visible_masks[3];
};

static i32 GizBombGens_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_bombgens : 0;
}

static void GizBombGens_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *system_ptr) {
    GIZBOMBGENSYS *system = static_cast<GIZBOMBGENSYS *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }
    for (i32 index = 0; index < system->count; ++index) {
        GIZBOMBGEN *bomb_generator = &system->bomb_generators[index];
        if (NuStrLen(bomb_generator->name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, bomb_generator);
        }
    }
}

static void GizBombGens_Update(void *world_ptr, void *system_ptr, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBOMBGENSYS *system = static_cast<GIZBOMBGENSYS *>(system_ptr);
    GameObject_s **players = Player;
    GameObject_s **player_bombs = BombGenerator_PlayerBomb;
    i16 *bomb_model_id = &id_DRAGBOMB;

    if (system != NULL) {
        if ((system->flags & GIZBOMBGENSYS_FLAG_RESET) != 0) {
            goto reset;
        }

    update_generators: {
        GIZBOMBGEN *bomb_generator = system->bomb_generators;
        for (i32 index = 0; index < system->count; ++index, ++bomb_generator) {
            if ((bomb_generator->flags & GIZBOMBGEN_FLAG_VISIBLE) == 0) {
                continue;
            }
            if ((bomb_generator->flags & GIZBOMBGEN_FLAG_ACTIVE) == 0) {
                continue;
            }
            if (bomb_generator->anim_set == NULL) {
                continue;
            }

            if (bomb_generator->generated_bomb != NULL) {
                GameAnimSet_SetVisibility(bomb_generator->anim_set, 0);
                GameObject_s *bomb = bomb_generator->generated_bomb;
                if (bomb->apiobj.field_0x287 != 0 || (bomb->apiobj.flags_high & APIOBJECT_HIGH_FLAG_CHARACTER) == 0) {
                    bomb_generator->generated_bomb = NULL;
                    GameAnimSet_Reset(bomb_generator->anim_set);
                    GameAnimSet_EvalAnim(bomb_generator->anim_set);
                    GameAnimSet_SetVisibility(bomb_generator->anim_set, 1);
                }
            } else {
                GameAnimSet_Play(bomb_generator->anim_set, 1.0f, 1);
                if (bomb_generator->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                    NUVEC bomb_position;
                    GameAnimSet_GetAveragePos(bomb_generator->anim_set, &bomb_position, 2, 1, 1);
                    if (netclient == 0) {
                        bomb_generator->generated_bomb = AddDynamicCreature(
                            *bomb_model_id, &bomb_position, 0, const_cast<char *>(""), NULL, NULL, 1, NULL, NULL, 0, 0);
                        if (Cheat_IsOn(5) != 0) {
                            AddGameDebris(world->debris_sys, 0x7d, &bomb_position);
                            AddGameDebris(world->debris_sys, 0x7e, &bomb_position);
                        }
                    }
                }
            }
        }
        goto track_player_bombs;
    }

    reset: {
        GIZBOMBGENPROGRESS *progress = static_cast<GIZBOMBGENPROGRESS *>(system->progress);
        const bool has_progress = progress != NULL;

        GIZBOMBGEN *bomb_generator = system->bomb_generators;
        for (i32 index = 0; index < system->count; ++index, ++bomb_generator) {
            bomb_generator->flags |= GIZBOMBGEN_FLAG_ACTIVE | GIZBOMBGEN_FLAG_VISIBLE;
            if (bomb_generator->anim_set != NULL) {
                GameAnimSet_EvaluateState(bomb_generator->anim_set);
            }

            GameObject_s *bomb = bomb_generator->generated_bomb;
            if (bomb != NULL && (system->flags & GIZBOMBGENSYS_FLAG_KILL_BOMBS_ON_RESET) != 0 &&
                (bomb->apiobj.field_0x1f8 & (APIOBJECT_FLAG_CHARACTER | APIOBJECT_FLAG_IN_USE)) ==
                    (APIOBJECT_FLAG_CHARACTER | APIOBJECT_FLAG_IN_USE)) {
                KillGameObject(bomb, 4, 0);
            }
            bomb_generator->generated_bomb = NULL;

            if (index < 96 && has_progress) {
                const i32 word_index = index >> 5;
                const u32 mask = 1u << (index & 31);
                bomb_generator->flags = static_cast<u8>((bomb_generator->flags & ~GIZBOMBGEN_FLAG_VISIBLE) |
                                                        (((progress->visible_masks[word_index] & mask) != 0) << 1));
                bomb_generator->flags = static_cast<u8>((bomb_generator->flags & ~GIZBOMBGEN_FLAG_ACTIVE) |
                                                        ((progress->active_masks[word_index] & mask) != 0));
            }
        }

        const bool players_share_bomb = player_bombs[0] != NULL && player_bombs[0] == player_bombs[1];
        if (netclient == 0) {
            GameObject_s *player = players[0];
            if (player != NULL) {
                if (player_bombs[0] != NULL && player->cable != NULL && player->cable->target != NULL &&
                    player->cable->target->id == *bomb_model_id) {
                    KillGameObject(player->cable->target, 4, 0);
                    player->cable = NULL;
                }

                if (static_cast<i8>(player->apiobj.flags_low) < 0 && player_bombs[0] != NULL && player->cable == NULL) {
                    NUVEC bomb_position = {0.0f, 0.0f, -2.0f};
                    NuVecRotateY(&bomb_position, &bomb_position, player->apiobj.field_0x276);
                    NuVecAdd(&bomb_position, &bomb_position, &player->apiobj.collision_position);
                    GameObject_s *bomb = AddDynamicCreature(*bomb_model_id, &bomb_position, 0, const_cast<char *>(""),
                                                            NULL, NULL, 0, NULL, NULL, 0, 0);
                    if (bomb != NULL) {
                        player->cable = CreateCable(player, bomb, 0);
                        if (players[0]->cable != NULL) {
                            players[0]->cable->max_length = 1000000000.0f;
                        }
                    }
                }
            }

            player = players[1];
            if (player != NULL) {
                if (player_bombs[1] != NULL && player->cable != NULL && player->cable->target != NULL &&
                    player->cable->target->id == *bomb_model_id) {
                    KillGameObject(player->cable->target, 4, 0);
                    player->cable = NULL;
                }

                if (static_cast<i8>(player->apiobj.flags_low) < 0 && player_bombs[1] != NULL && player->cable == NULL) {
                    GameObject_s *bomb = NULL;
                    if (players_share_bomb && players[0]->cable != NULL && players[0]->cable->target != NULL &&
                        players[0]->cable->target->id == *bomb_model_id) {
                        bomb = players[0]->cable->target;
                    } else {
                        NUVEC bomb_position = {0.0f, 0.0f, -2.0f};
                        NuVecRotateY(&bomb_position, &bomb_position, player->apiobj.field_0x276);
                        NuVecAdd(&bomb_position, &bomb_position, &player->apiobj.collision_position);
                        bomb = AddDynamicCreature(*bomb_model_id, &bomb_position, 0, const_cast<char *>(""), NULL, NULL,
                                                  0, NULL, NULL, 0, 0);
                    }
                    if (bomb != NULL) {
                        player->cable = CreateCable(player, bomb, 0);
                        if (players[1]->cable != NULL) {
                            players[1]->cable->max_length = 1000000000.0f;
                        }
                    }
                }
            }
        }
        system->flags &= static_cast<u8>(~GIZBOMBGENSYS_FLAG_RESET);
        goto update_generators;
    }
    }

track_player_bombs:
    player_bombs[0] = NULL;
    if (players[0] != NULL && players[0]->cable != NULL && players[0]->cable->target != NULL &&
        players[0]->cable->target->id == *bomb_model_id) {
        player_bombs[0] = players[0]->cable->target;
    }
    player_bombs[1] = NULL;
    if (players[1] != NULL && players[1]->cable != NULL && players[1]->cable->target != NULL &&
        players[1]->cable->target->id == *bomb_model_id) {
        player_bombs[1] = players[1]->cable->target;
    }
}

static char *GizmoBombGen_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZBOMBGEN *>(gizmo->object)->name : NULL;
}

static i32 GizmoBombGen_GetOutput(GIZMO *gizmo, i32, i32) {
    return static_cast<GIZBOMBGEN *>(gizmo->object)->flags & GIZBOMBGEN_FLAG_ACTIVE;
}

static char *GizmoBombGen_GetOutputName(GIZMO *, i32) {
    return const_cast<char *>("Active");
}

static i32 GizmoBombGen_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizmoBombGen_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        GIZBOMBGEN *bomb_generator = static_cast<GIZBOMBGEN *>(gizmo->object);
        bomb_generator->flags =
            (bomb_generator->flags & ~GIZBOMBGEN_FLAG_ACTIVE) | (active != 0 ? GIZBOMBGEN_FLAG_ACTIVE : 0);
    }
}

static void GizmoBombGen_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL && gizmo->object != NULL) {
        GIZBOMBGEN *bomb_generator = static_cast<GIZBOMBGEN *>(gizmo->object);
        GameAnimSet_SetVisibility(bomb_generator->anim_set, visible);
        const u8 visible_flag = visible != 0;
        bomb_generator->flags =
            static_cast<u8>((bomb_generator->flags & ~GIZBOMBGEN_FLAG_VISIBLE) | (visible_flag << 1));
    }
}

static NUVEC *GizmoBombGen_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &static_cast<GIZBOMBGEN *>(gizmo->object)->position;
}

static void *GizBombGens_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZBOMBGENPROGRESS));
}

static void GizBombGens_ClearProgress(void *, void *progress_ptr) {
    GIZBOMBGENPROGRESS *progress = static_cast<GIZBOMBGENPROGRESS *>(progress_ptr);
    if (progress != NULL) {
        progress->active_masks[0] = ~0u;
        progress->active_masks[1] = ~0u;
        progress->active_masks[2] = ~0u;
        progress->visible_masks[0] = ~0u;
        progress->visible_masks[1] = ~0u;
        progress->visible_masks[2] = ~0u;
    }
}

static void GizBombGens_StoreProgress(void *, void *system_ptr, void *progress_ptr) {
    GIZBOMBGENSYS *system = static_cast<GIZBOMBGENSYS *>(system_ptr);
    GIZBOMBGENPROGRESS *progress = static_cast<GIZBOMBGENPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->active_masks[0] = ~0u;
    progress->active_masks[1] = ~0u;
    progress->active_masks[2] = ~0u;
    progress->visible_masks[0] = ~0u;
    progress->visible_masks[1] = ~0u;
    progress->visible_masks[2] = ~0u;

    for (i32 index = 0; index < system->count && index < 96; ++index) {
        const u32 mask = 1u << (index & 31);
        const i32 word_index = index >> 5;
        if ((system->bomb_generators[index].flags & GIZBOMBGEN_FLAG_VISIBLE) == 0) {
            progress->visible_masks[word_index] &= ~mask;
        }
        if ((system->bomb_generators[index].flags & GIZBOMBGEN_FLAG_ACTIVE) == 0) {
            progress->active_masks[word_index] &= ~mask;
        }
    }
}

static void GizBombGens_SetResetFlag(void *world_ptr, void *system_ptr, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBOMBGENSYS *system = static_cast<GIZBOMBGENSYS *>(system_ptr);

    system->flags |= GIZBOMBGENSYS_FLAG_RESET;
    const u8 visible_flag = come_from_an_editor == 0 && (world->reset_flags & 1) != 0;
    system->flags = static_cast<u8>((system->flags & ~GIZBOMBGENSYS_FLAG_KILL_BOMBS_ON_RESET) | (visible_flag << 1));
    system->progress = progress_ptr;
}

static void *GizBombGens_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBOMBGENSYS *system =
        static_cast<GIZBOMBGENSYS *>(GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZBOMBGENSYS)));

    system->capacity = world->current_level->max_bombgens;
    system->bomb_generators = static_cast<GIZBOMBGEN *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, system->capacity * sizeof(GIZBOMBGEN)));
    system->anim_object_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 0,
                                                            world->current_level->max_bombgen_objs);

    for (i32 index = 0; index < system->capacity; ++index) {
        system->bomb_generators[index].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108,
                               static_cast<GAMEANIMOBJPOOL_s *>(system->anim_object_pool), world->game_anim_sys);
    }

    world->giz_bombgen_sys = system;
    return system;
}

static i32 GizBombGens_Load(void *world_ptr, void *system_ptr) {
    const u8 version = static_cast<u8>(EdFileReadChar());
    GIZBOMBGENSYS *system = static_cast<GIZBOMBGENSYS *>(system_ptr);
    system->count = static_cast<u16>(EdFileReadShort());
    if (system->count != 0) {
        GIZBOMBGEN *bomb_generator = system->bomb_generators;
        i32 index = 0;
        do {
            EdFileRead(bomb_generator->name, sizeof(bomb_generator->name));
            EdFileReadNuVec(&bomb_generator->position);
            bomb_generator->interval = EdFileReadInt();
            bomb_generator->flags |= GIZBOMBGEN_FLAG_ACTIVE | GIZBOMBGEN_FLAG_VISIBLE;
            GizmoFileReadGameAnimSet(bomb_generator->anim_set, world_ptr, NULL, version,
                                     const_cast<char *>("BombGenerator"), bomb_generator->name);
            ++index;
            ++bomb_generator;
        } while (system->count > index);
    }
    return 1;
}

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "BombGenerator";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x18;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBombGens_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBombGen_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBombGens_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBombGens_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizmoBombGen_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBombGen_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBombGen_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBombGen_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBombGen_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBombGen_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBombGens_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBombGens_ClearProgress;
    addtype.fns.store_progress_fn = GizBombGens_StoreProgress;
    addtype.fns.reset_fn = GizBombGens_SetResetFlag;
    addtype.fns.reserve_buffer_space_fn = GizBombGens_ReserveBufferSpace;
    addtype.fns.load_fn = GizBombGens_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    bombgen_gizmotype_id = type_id;

    return &addtype;
}
