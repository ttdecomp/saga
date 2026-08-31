#include "legoapi/gizmos/object/gizbuildits.h"

#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nurand.h"

#include <string.h>

i32 gizbuildit_gizmotype_id = -1;

void GizBuildit_Reset(GIZBUILDIT_s *buildit, void *world);
void GizBuildit_SetVisibility(GIZBUILDIT_s *buildit, i32 visible);
i32 GizmoBlowupGetNameTableId(char *name);
i32 GizmoBlowupGetTypeFromNameTableId(WORLDINFO_s *world, i32 name_table_id);
i32 MatrixReflection(NUMTX *matrix, i32 axis, f32 plane, f32 scale, NUMTX *result);
void GizBuildIt_Finish(GIZBUILDIT_s *buildit);
void GizBuildIt_SetStepTime(GIZBUILDIT_s *buildit, GameObject_s *player);
void ReleaseBuildIt(GameObject_s *player, i32 completed);
void GameAudio_PlaySfx(i32 sfx, NUVEC *position, i32 flags, i32 volume);
void NewRumbleAllPlayers(f32 intensity, f32 duration, i32 flags, i32 player_index);
void GizmoBlowUpTypeBlowUp(WORLDINFO_s *world, i32 blowup_type, NUVEC *position);
extern i16 GizBuilditGDeb[6];
extern i32 LEGOCONTEXT_BUILDIT;
extern i32 (*GizBuildit_AutoBuildPosFn)(void *, NUVEC *, NUVEC *, u16 *);

namespace {

    enum : i32 {
        GIZBUILDIT_PROGRESS_CAPACITY = 64,
        GIZBUILDIT_PROGRESS_WORDS = GIZBUILDIT_PROGRESS_CAPACITY / 32,
    };

    struct GIZBUILDITPROGRESS_s {
        u32 completed[GIZBUILDIT_PROGRESS_WORDS];
        u32 active[GIZBUILDIT_PROGRESS_WORDS];
        u32 visible[GIZBUILDIT_PROGRESS_WORDS];
        u32 reward_released[GIZBUILDIT_PROGRESS_WORDS];
        u8 built_object_count[GIZBUILDIT_PROGRESS_CAPACITY];
    };

    DECOMP_ASSERT(sizeof(GIZBUILDITPROGRESS_s) == 0x60, "BuildIt progress ABI");

    static void ClearBuildItProgress(GIZBUILDITPROGRESS_s *progress) {
        if (progress == NULL) {
            return;
        }

        memset(progress, 0, sizeof(*progress));
        for (i32 word = 0; word < GIZBUILDIT_PROGRESS_WORDS; ++word) {
            progress->active[word] = 0xffffffff;
            progress->visible[word] = 0xffffffff;
        }
    }

    enum BUILDIT_GAMEPLAY_CONSTANTS : i32 {
        BUILDIT_HINT_ID = 0x25c,
        BUILDIT_SFX_COMPLETE = 0x3a,
        BUILDIT_SFX_PLACE_PIECE = 0x3b,
    };

    static const f32 BUILDIT_FINISH_DURATION = 0.6f;

    static GameObject_s *FindActiveBuilder(GIZBUILDIT_s *buildit) {
        if (LEGOCONTEXT_BUILDIT == -1) {
            return NULL;
        }

        for (i32 index = 0; index < 8; ++index) {
            GameObject_s *player = Player[index];
            if (player != NULL && static_cast<i8>(player->apiobj.field_0x1f8) < 0 &&
                player->build_context == LEGOCONTEXT_BUILDIT && player->field_0x788 == buildit) {
                return player;
            }
        }
        return NULL;
    }

    static NUVEC BuildItPieceEndPosition(GIZBUILDIT_s *buildit, GAMEANIMOBJ_s *object) {
        if (buildit->linked_buildit != NULL) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(object->object_data);
            data->draw_mtx = data->end_mtx;
            return {data->draw_mtx.m30, data->draw_mtx.m31, data->draw_mtx.m32};
        }

        NuSpecialSetVisibility(&object->special, 1);
        if (object->instance_animation != NULL) {
            object->instance_animation->playing = 0;
            object->instance_animation->ltime = object->end_frame;
            EvalAnim2(&object->special, object->end_frame);
        }
        NUVEC *position = NuSpecialGetDrawPos(&object->special);
        return position != NULL ? *position : buildit->position;
    }

    static void EmitBuildItDebris(WORLDINFO *world, GIZBUILDIT_s *buildit) {
        if (world == NULL || world->debris_sys == NULL) {
            return;
        }

        for (i32 index = 0; index < buildit->anim_object_count; ++index) {
            GIZBUILDITANIMDATA_s *data = static_cast<GIZBUILDITANIMDATA_s *>(buildit->anim_objects[index]->object_data);
            const i16 debris_type = GizBuilditGDeb[qrand() / 0x2aab];
            if (debris_type >= 0) {
                NUVEC position = {data->end_mtx.m30, data->end_mtx.m31, data->end_mtx.m32};
                AddGameDebris(world->debris_sys, debris_type, &position);
            }
        }
    }

    static void CompleteBuildIt(WORLDINFO *world, GIZBUILDIT_s *buildit) {
        Hint_SetComplete(BUILDIT_HINT_ID);
        GizBuildIt_Finish(buildit);

        NUVEC effect_position = {
            buildit->position.x + buildit->effect_position.x,
            buildit->position.y + buildit->effect_position.y,
            buildit->position.z + buildit->effect_position.z,
        };
        GameAudio_PlaySfx(BUILDIT_SFX_COMPLETE, &effect_position, 0, 0);
        NewRumbleAllPlayers(0.75f, 0.1f, 0, 0);

        buildit->field_0x83 |= GIZBUILDIT_RUNTIME_REWARD_RELEASED;
        if (buildit->blowup_type != -1) {
            GizmoBlowUpTypeBlowUp(world, buildit->blowup_type, &buildit->position);
            GameAnimSet_SetVisibility(buildit->anim_set, 0);
        }
    }

} // namespace

static i32 GizBuildIts_GetMaxGizmos(void *buildit) {
    WORLDINFO *world = static_cast<WORLDINFO *>(buildit);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_buildits;
}

static void GizBuildIts_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *data) {
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    if (buildit_sys != NULL) {
        if (buildit_sys->count != 0) {
            i32 i = 0;
            do {
                if (NuStrLen(buildit_sys->buildits[i].name) != 0) {
                    AddGizmo(gizmo_sys, type_id, NULL, &buildit_sys->buildits[i]);
                }
                ++i;
            } while (buildit_sys->count > i);
        }
    }
}

static void GizBuildIts_EarlyUpdate(void *, void *data, float) {
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    if (buildit_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < buildit_sys->count; ++index) {
        GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        buildit.availability_flags &= static_cast<u8>(~GIZBUILDIT_AVAILABILITY_INTERACTING);
        buildit.builders_active = 0;
    }
}

static void GizBuildIts_LateUpdate(void *world_ptr, void *data, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    if (buildit_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < buildit_sys->count; ++index) {
        GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        if ((buildit.availability_flags & GIZBUILDIT_AVAILABILITY_LINKED) != GIZBUILDIT_AVAILABILITY_LINKED ||
            buildit.anim_set == NULL || buildit.anim_object_count == 0 || buildit.anim_objects == NULL) {
            continue;
        }

        if (buildit.build_state == GIZBUILDIT_BUILD_FINISHING) {
            buildit.step_timer += FRAMETIME;
            if (buildit.step_timer >= BUILDIT_FINISH_DURATION) {
                CompleteBuildIt(world, &buildit);
            }
            continue;
        }
        if (buildit.build_state != GIZBUILDIT_BUILD_IDLE || buildit.built_object_count >= buildit.anim_object_count) {
            continue;
        }

        const bool automatic_build = (buildit.state_flags & GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS) != 0;
        if (!automatic_build && buildit.builders_active == 0) {
            GizBuildIt_SetStepTime(&buildit, NULL);
            if (buildit.linked_buildit == NULL) {
                GAMEANIMOBJ_s *next_piece = buildit.anim_objects[buildit.built_object_count];
                NuSpecialSetVisibility(&next_piece->special, 1);
            }
            continue;
        }

        GameObject_s *builder = automatic_build ? NULL : FindActiveBuilder(&buildit);
        buildit.step_timer -= FRAMETIME;
        GAMEANIMOBJ_s *piece = buildit.anim_objects[buildit.built_object_count];

        if (buildit.step_timer > 0.0f) {
            if (buildit.linked_buildit == NULL) {
                NuSpecialSetVisibility(&piece->special, 0);
            }
            continue;
        }

        NUVEC piece_position = BuildItPieceEndPosition(&buildit, piece);
        GameAudio_PlaySfx(BUILDIT_SFX_PLACE_PIECE, &piece_position, 0, 0);
        ++buildit.built_object_count;

        if (buildit.built_object_count == buildit.anim_object_count) {
            buildit.build_state = GIZBUILDIT_BUILD_FINISHING;
            buildit.step_timer = 0.0f;
            EmitBuildItDebris(world, &buildit);
        } else {
            GizBuildIt_SetStepTime(&buildit, builder);
        }

        if (builder != NULL && buildit.build_state != GIZBUILDIT_BUILD_IDLE) {
            ReleaseBuildIt(builder, buildit.build_state);
        }
    }
}

static void GizBuildIts_Draw(void *world_ptr, void *data, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    if (buildit_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < buildit_sys->count; ++index) {
        GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        if ((buildit.availability_flags & GIZBUILDIT_AVAILABILITY_VISIBLE) == 0 ||
            (buildit.room_index >= 0 && world->rooms_visible_ptr[buildit.room_index] == 0)) {
            continue;
        }

        if ((buildit.state_flags & GIZBUILDIT_STATE_DISABLE_SHADOW_RENDERING) != 0) {
            EnableShadowMapRendering(0);
        } else {
            ResetShadowMapRendering();
        }

        if (buildit.linked_buildit == NULL) {
            ResetShadowMapRendering();
            if ((buildit.state_flags & GIZBUILDIT_STATE_DRAW_REFLECTION) != 0) {
                GameAnimSet_DrawReflection(buildit.anim_set, 2, buildit.field_0x54, NULL);
            }
            continue;
        }

        if (buildit.anim_set == NULL) {
            continue;
        }

        f32 reflection_plane = 0.0f;
        GAMEANIMOBJ_s *object = buildit.anim_set->objects;
        if ((buildit.state_flags & GIZBUILDIT_STATE_DRAW_REFLECTION) != 0 && object != NULL &&
            object->object_data != NULL) {
            GIZBUILDITANIMDATA_s *first_data = static_cast<GIZBUILDITANIMDATA_s *>(object->object_data);
            reflection_plane = first_data->start_mtx.m31 + buildit.field_0x54;
        }

        while (object != NULL) {
            GIZBUILDITANIMDATA_s *object_data = static_cast<GIZBUILDITANIMDATA_s *>(object->object_data);
            object_data->was_drawn = static_cast<u8>(NuSpecialDrawAt(&object->special, &object_data->draw_mtx));

            if ((buildit.state_flags & GIZBUILDIT_STATE_DRAW_REFLECTION) != 0 && (object->flags & 2) == 0) {
                NUMTX reflection_mtx;
                if (MatrixReflection(&object_data->draw_mtx, 2, reflection_plane, world->current_level->unknown_0cc,
                                     &reflection_mtx) != 0) {
                    NuSpecialDrawAt(&object->special, &reflection_mtx);
                }
            }
            object = object->next;
        }
    }
    ResetShadowMapRendering();
}

static char *GizmoBuildit_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZBUILDIT_s *buildit = static_cast<GIZBUILDIT_s *>(gizmo->object);
        if (buildit != NULL) {
            return buildit->name;
        }
    }
    return NULL;
}

static i32 GizmoBuildit_GetOutput(GIZMO *gizmo, i32, i32 force_read) {
    GIZBUILDIT_s *buildit = static_cast<GIZBUILDIT_s *>(gizmo->object);
    if ((buildit->availability_flags & GIZBUILDIT_AVAILABILITY_LINKED) == GIZBUILDIT_AVAILABILITY_LINKED ||
        force_read != 0) {
        return buildit->build_state == GIZBUILDIT_BUILD_COMPLETE;
    }
    return 0;
}

static char *GizmoBuildit_GetOutputName(GIZMO *, i32) {
    return const_cast<char *>("Finished");
}

static i32 GizmoBuildit_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizmoBuildit_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL) {
        return;
    }

    GIZBUILDIT_s *buildit = static_cast<GIZBUILDIT_s *>(gizmo->object);
    buildit->availability_flags = static_cast<u8>((buildit->availability_flags & ~GIZBUILDIT_AVAILABILITY_ACTIVE) |
                                                  (active != 0 ? GIZBUILDIT_AVAILABILITY_ACTIVE : 0));
    if (active == 0) {
        return;
    }

    if ((buildit->state_flags & GIZBUILDIT_STATE_REISSUE_REWARD_ON_ACTIVATE) != 0) {
        buildit->field_0x83 &= static_cast<u8>(~GIZBUILDIT_RUNTIME_REWARD_RELEASED);
    }
    if (buildit->linked_buildit == NULL) {
        GameAnimSet_JumpToStart(buildit->anim_set);
    }

    const u8 reward_released = buildit->field_0x83 & GIZBUILDIT_RUNTIME_REWARD_RELEASED;
    GizBuildit_Reset(buildit, NULL);
    buildit->field_0x83 =
        static_cast<u8>((buildit->field_0x83 & ~GIZBUILDIT_RUNTIME_REWARD_RELEASED) | reward_released);

    if ((buildit->state_flags & GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS) == 0 || GizBuildit_AutoBuildPosFn == NULL) {
        return;
    }

    NUVEC scatter_position;
    if (GizBuildit_AutoBuildPosFn(NULL, &buildit->start_position, &scatter_position, NULL) == 0) {
        return;
    }

    buildit->step_timer = 5.0f;
    for (i32 index = 0; index < buildit->anim_object_count; ++index) {
        scatter_position.x += NuFloatRand(NULL) / 5.0f - 0.1f;
        scatter_position.y += NuFloatRand(NULL) / 5.0f - 0.1f;
        scatter_position.z += NuFloatRand(NULL) / 5.0f - 0.1f;

        GAMEANIMOBJ_s *object = buildit->anim_objects[index];
        if (buildit->linked_buildit == NULL) {
            NuSpecialSetDrawPos(&object->special, &scatter_position);
        } else {
            GIZBUILDITANIMDATA_s *object_data = static_cast<GIZBUILDITANIMDATA_s *>(object->object_data);
            object_data->draw_mtx.m30 = scatter_position.x;
            object_data->draw_mtx.m31 = scatter_position.y;
            object_data->draw_mtx.m32 = scatter_position.z;
        }
        AddGameDebris(WORLD->debris_sys, GizBuilditGDeb[qrand() / 0x2aab], &scatter_position);
    }
}

static void GizmoBuildit_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GizBuildit_SetVisibility(static_cast<GIZBUILDIT_s *>(gizmo->object), visible);
    }
}

static NUVEC *GizmoBuildit_GetPos(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZBUILDIT_s *buildit = static_cast<GIZBUILDIT_s *>(gizmo->object);
        if (buildit != NULL) {
            return &buildit->position;
        }
    }
    return NULL;
}

static void *GizBuildIts_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZBUILDITPROGRESS_s));
}

static void GizBuildIts_ClearProgress(void *, void *progress_ptr) {
    ClearBuildItProgress(static_cast<GIZBUILDITPROGRESS_s *>(progress_ptr));
}

static void GizBuildIts_StoreProgress(void *, void *data, void *progress_ptr) {
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    GIZBUILDITPROGRESS_s *progress = static_cast<GIZBUILDITPROGRESS_s *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    ClearBuildItProgress(progress);
    i32 count = buildit_sys->count;
    if (count > GIZBUILDIT_PROGRESS_CAPACITY) {
        count = GIZBUILDIT_PROGRESS_CAPACITY;
    }

    for (i32 index = 0; index < count; ++index) {
        const GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        const i32 word = index >> 5;
        const u32 bit = 1u << (index & 31);
        if (buildit.build_state != GIZBUILDIT_BUILD_IDLE) {
            progress->completed[word] |= bit;
        }
        if ((buildit.availability_flags & GIZBUILDIT_AVAILABILITY_VISIBLE) == 0) {
            progress->visible[word] &= ~bit;
        }
        if ((buildit.availability_flags & GIZBUILDIT_AVAILABILITY_ACTIVE) == 0) {
            progress->active[word] &= ~bit;
        }
        if ((buildit.field_0x83 & GIZBUILDIT_RUNTIME_REWARD_RELEASED) != 0) {
            progress->reward_released[word] |= bit;
        }
        progress->built_object_count[index] = buildit.built_object_count;
    }
}

static void GizBuildIts_Reset(void *world_ptr, void *data, void *progress_ptr) {
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    GIZBUILDITPROGRESS_s *progress = static_cast<GIZBUILDITPROGRESS_s *>(progress_ptr);

    for (i32 index = 0; index < buildit_sys->count; ++index) {
        GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        GizBuildit_Reset(&buildit, world_ptr);

        if (index >= GIZBUILDIT_PROGRESS_CAPACITY || progress == NULL) {
            continue;
        }

        const i32 word = index >> 5;
        const u32 bit = 1u << (index & 31);
        buildit.build_state =
            (progress->completed[word] & bit) != 0 ? GIZBUILDIT_BUILD_COMPLETE : GIZBUILDIT_BUILD_IDLE;
        buildit.availability_flags =
            static_cast<u8>((buildit.availability_flags & ~GIZBUILDIT_AVAILABILITY_LINKED) |
                            ((progress->active[word] & bit) != 0 ? GIZBUILDIT_AVAILABILITY_ACTIVE : 0) |
                            ((progress->visible[word] & bit) != 0 ? GIZBUILDIT_AVAILABILITY_VISIBLE : 0));
        buildit.field_0x83 =
            static_cast<u8>((buildit.field_0x83 & ~GIZBUILDIT_RUNTIME_REWARD_RELEASED) |
                            ((progress->reward_released[word] & bit) != 0 ? GIZBUILDIT_RUNTIME_REWARD_RELEASED : 0));
        buildit.built_object_count = progress->built_object_count[index];
    }
}

static void *GizBuildIts_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZBUILDITSYS_s)));

    buildit_sys->max_animation_objects = world->current_level->max_buildit_objs;
    buildit_sys->capacity = world->current_level->max_buildits;
    buildit_sys->buildits = static_cast<GIZBUILDIT_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, buildit_sys->capacity * sizeof(GIZBUILDIT_s)));
    buildit_sys->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 0xc8,
                                                          buildit_sys->max_animation_objects);
    buildit_sys->anim_objects = static_cast<GAMEANIMOBJ_s **>(GameBufferAlloc(
        &world->giz_buffer, &world->unknown_0108, buildit_sys->max_animation_objects * sizeof(GAMEANIMOBJ_s *)));

    for (i32 i = 0; i < buildit_sys->capacity; ++i) {
        buildit_sys->buildits[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108, buildit_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_buildit_sys = buildit_sys;
    return buildit_sys;
}

static i32 GizBuildIts_Load(void *world_ptr, void *data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    const u8 version = static_cast<u8>(EdFileReadChar());
    const u16 buildit_count = static_cast<u16>(EdFileReadShort());
    static_cast<GIZBUILDITSYS_s *>(data)->count = buildit_count;
    if (buildit_count == 0) {
        return 1;
    }

    char linked_name[16];
    char blowup_name[32];
    i32 anim_object_index = 0;
    GIZBUILDIT_s *buildit = static_cast<GIZBUILDITSYS_s *>(data)->buildits;
    for (i32 buildit_index = 0; buildit_index < static_cast<GIZBUILDITSYS_s *>(data)->count;
         ++buildit_index, ++buildit) {
        EdFileRead(buildit->name, sizeof(buildit->name));
        EdFileReadNuVec(&buildit->file_position);
        GizmoFileReadGameAnimSet(buildit->anim_set, world, NULL, version, "GizBuildit", buildit->name);

        buildit->interaction_radius = EdFileReadFloat();
        if (version <= 6) {
            (void)EdFileReadFloat();
        }
        buildit->field_0x5e = EdFileReadShort();
        buildit->field_0x5c = EdFileReadShort();
        buildit->field_0x78 = static_cast<u8>(EdFileReadChar());
        buildit->progress = static_cast<u8>(EdFileReadChar());
        if (version >= 6) {
            buildit->field_0x54 = EdFileReadFloat();
        }

        buildit->blowup_type = -1;
        if (version > 6) {
            if (version == 7) {
                buildit->blowup_type = EdFileReadShort();
            } else {
                const i32 blowup_name_length = static_cast<i8>(EdFileReadChar());
                if (blowup_name_length != 0) {
                    EdFileRead(blowup_name, blowup_name_length);
                    buildit->blowup_type = static_cast<i16>(GizmoBlowupGetNameTableId(blowup_name));
                    if (buildit->blowup_type != -1) {
                        buildit->availability_flags |= GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE;
                    }
                }
            }

            buildit->field_0x60 = EdFileReadShort();
            buildit->field_0x62 = EdFileReadShort();
            EdFileReadNuVec(&buildit->effect_position);
        }

        if (version <= 8) {
            buildit->activation_radius =
                world->area != NULL && (world->area->flags & AREAFLAG_VEHICLE_AREA) != 0 ? 12.0f : 1.75f;
        } else {
            buildit->activation_radius = EdFileReadFloat();
        }

        if (version <= 3) {
            buildit->state_flags = static_cast<i16>(static_cast<i8>(EdFileReadChar()));
            if (version > 1 && EdFileReadChar() != 0) {
                buildit->state_flags |= GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS;
            }
            if (version == 3 && EdFileReadChar() != 0) {
                buildit->state_flags |= 0x80;
            }
        } else {
            buildit->state_flags = static_cast<u16>(EdFileReadShort());
        }

        if (version > 4) {
            buildit->field_0x7c = static_cast<u16>(EdFileReadShort());
            if (EdFileReadChar() != 0) {
                EdFileRead(linked_name, sizeof(linked_name));
                for (i32 linked_index = 0; linked_index < static_cast<GIZBUILDITSYS_s *>(data)->count; ++linked_index) {
                    if (NuStrICmp(static_cast<GIZBUILDITSYS_s *>(data)->buildits[linked_index].name, linked_name) ==
                        0) {
                        buildit->availability_flags |= GIZBUILDIT_AVAILABILITY_LINKED;
                        buildit->linked_buildit = &static_cast<GIZBUILDITSYS_s *>(data)->buildits[linked_index];
                        break;
                    }
                }
            }
        }

        buildit->anim_object_count = 0;
        buildit->anim_objects = &static_cast<GIZBUILDITSYS_s *>(data)->anim_objects[anim_object_index];
        for (GAMEANIMOBJ_s *object = buildit->anim_set->objects; object != NULL; object = object->next) {
            if ((object->animation != NULL && object->instance_animation != NULL) ||
                (buildit->state_flags & GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS) != 0) {
                buildit->anim_objects[buildit->anim_object_count++] = object;
                ++anim_object_index;
            }
        }

        GizBuildit_Reset(buildit, world);
    }

    return 1;
}

static void GizBuildIts_PostLoad(void *world_ptr, void *data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZBUILDITSYS_s *buildit_sys = static_cast<GIZBUILDITSYS_s *>(data);
    if (buildit_sys == NULL) {
        return;
    }

    for (i32 index = 0; index < buildit_sys->count; ++index) {
        GIZBUILDIT_s &buildit = buildit_sys->buildits[index];
        if ((buildit.availability_flags & GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE) != 0) {
            buildit.blowup_type = static_cast<i16>(GizmoBlowupGetTypeFromNameTableId(world, buildit.blowup_type));
            buildit.availability_flags &= static_cast<u8>(~GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE);
        }
    }
}

ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizBuildit";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x60;
    addtype.fns.early_update_fn = GizBuildIts_EarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBuildIts_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBuildit_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBuildIts_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBuildIts_LateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizBuildIts_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoBuildit_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBuildit_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBuildit_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBuildit_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBuildit_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBuildit_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBuildIts_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBuildIts_ClearProgress;
    addtype.fns.store_progress_fn = GizBuildIts_StoreProgress;
    addtype.fns.reset_fn = GizBuildIts_Reset;
    addtype.fns.reserve_buffer_space_fn = GizBuildIts_ReserveBufferSpace;
    addtype.fns.load_fn = GizBuildIts_Load;
    addtype.fns.post_load_fn = GizBuildIts_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizbuildit_gizmotype_id = type_id;

    return &addtype;
}
