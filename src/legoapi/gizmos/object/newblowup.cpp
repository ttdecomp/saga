#include "legoapi/gizmos/object/newblowup.h"

#include "decomp.h"
#include "batman.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/numath/numtx.h"

#include <string.h>

extern "C" void PlatOnOff(i32 platform_id, i32 enabled);
void GameAntinode_UnregisterAntiNode(GAMEANTINODESYS_s *system, GAMEANTINODE_s *anti_node);
i32 InitGizmoBlowupTypes(WORLDINFO_s *world);
i32 InitGizmoBlowups(WORLDINFO_s *world);
i32 InitGizmoBlowupsMtxBuffer(WORLDINFO_s *world);
GIZMOBLOWUPTYPE_s *GizmoBlowup_FindType(char *name, WORLDINFO_s *world);
void GameAntiNodeData_Init(GAMEANTINODEDATA_s *data, nuhspecial_s *special);
void GameAntiNodeData_Read(GAMEANTINODEDATA_s *data);
void GizBlowup_Respawn(GIZMOBLOWUP_s *blowup);
void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_s *blowup, NUMTX *matrix, i32 alternate);
void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_s *blowup, NUMTX *matrix);
int MatrixReflection(NUMTX *matrix, i32 axis, f32 plane, f32 height, NUMTX *result);
void PushAway(NUVEC *position, f32 radius, NUVEC *bounds_min, NUVEC *bounds_max, GameObject_s *source,
              GameObject_s *ignore, f32 strength, u32 flags);

enum GIZMOBLOWUP_OUTPUT_FLAGS : u8 {
    GIZMOBLOWUP_OUTPUT_BLOWN_UP = 1 << 0,
    GIZMOBLOWUP_OUTPUT_PUNCHED = 1 << 1,
};

enum GIZMOBLOWUP_VISIBILITY_FLAGS : u8 {
    GIZMOBLOWUP_VISIBLE = 1 << 6,
    GIZMOBLOWUP_DRAWN = 1 << 7,
};

enum GIZMOBLOWUP_STATE_FLAGS : u8 {
    GIZMOBLOWUP_STATE_ACTIVE = 1 << 0,
    GIZMOBLOWUP_STATE_DELAY_ACTIVE = 1 << 1,
    GIZMOBLOWUP_STATE_REPEAT_ANIMATION = 1 << 3,
    GIZMOBLOWUP_STATE_ANIMATION_PLAYING = 1 << 4,
    GIZMOBLOWUP_STATE_REPEATING = 1 << 6,
    GIZMOBLOWUP_STATE_ACTIVATED = 1 << 7,
};

enum GIZMOBLOWUPTYPE_RUNTIME_FLAGS : u8 {
    GIZMOBLOWUPTYPE_ANIMATION_UPDATED = 1 << 0,
};

enum GIZMOBLOWUPTYPE_ANIMATION_FLAGS : u8 {
    GIZMOBLOWUPTYPE_ANIMATION_INCLUDES_INSTANCE_TRANSFORM = 1 << 5,
};

enum GIZMOBLOWUP_DRAW_FLAGS : u32 {
    GIZMOBLOWUP_DRAW_CUSTOM_TRANSFORM = 0x00000010,
    GIZMOBLOWUP_DRAW_HIDE_DECAL = 0x00000400,
    GIZMOBLOWUP_DRAW_RESET_SHADOW_MAP = 0x00020000,
    GIZMOBLOWUP_DRAW_PROJECT_ONLY = 0x00800000,
    GIZMOBLOWUP_DRAW_REFLECTION = 0x40000000,
};

enum GIZMOBLOWUP_DRAW_STATUS_FLAGS : u32 {
    GIZMOBLOWUP_DRAW_STATUS_TRANSFORM_READY = 0x00808000,
};

static char *Blowup_OutputName[] = {"Blownup", "Punched", "Plugging"};

i32 blowup_gizmotype_id = -1;

static i32 Blowup_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_gizmo_blowups : 0;
}

static void Blowup_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->gizmo_blowup_count; ++index) {
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[index];
        if (NuStrLen(blowup->name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, blowup);
        }
    }
}

void GizmoBlowupEarlyUpdate(void *world_ptr, void *, float) {
    if (world_ptr == NULL) {
        return;
    }

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZMOBLOWUP_s *blowup = world->gizmo_blowups;
    for (i32 blowup_index = 0; blowup_index < world->gizmo_blowup_count; ++blowup_index, ++blowup) {
        if (blowup->respawn_timer > 0.0f) {
            blowup->respawn_timer -= FRAMETIME;
            if (blowup->respawn_timer <= 2.0f) {
                blowup->visibility_flags |= GIZMOBLOWUP_VISIBLE;
                if (blowup->respawn_timer <= 0.0f) {
                    GizBlowup_Respawn(blowup);
                } else if (PickupFlickerFrame % PickUpFlickerFrames >= PickUpFlickerTest) {
                    blowup->visibility_flags &= ~GIZMOBLOWUP_VISIBLE;
                }

                f32 push_radius_x = (blowup->bounds_max.x - blowup->bounds_min.x) * 0.5f;
                f32 push_radius_z = (blowup->bounds_max.z - blowup->bounds_min.z) * 0.5f;
                f32 push_radius = push_radius_x > push_radius_z ? push_radius_x : push_radius_z;
                PushAway(&blowup->mid_position, push_radius, &blowup->bounds_min, &blowup->bounds_max, NULL, NULL, 1.0f,
                         4);
            }
        }

        nuinstanim_s *animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        bool requires_update = ((blowup->state_flags & GIZMOBLOWUP_STATE_ACTIVATED) != 0 &&
                                (blowup->output_flags & GIZMOBLOWUP_OUTPUT_BLOWN_UP) == 0) ||
                               (blowup->state_flags & GIZMOBLOWUP_STATE_DELAY_ACTIVE) != 0;
        if (!requires_update && (blowup->state_flags & GIZMOBLOWUP_STATE_ACTIVATED) == 0 && animation != NULL &&
            animation->playing != 0) {
            animation->playing = 0;
        }
    }
}

void GizmoBlowupLateUpdate(void *world_ptr, void *, float) {
    if (world_ptr == NULL) {
        return;
    }

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZMOBLOWUPTYPE_s *blowup_type = world->gizmo_blowup_types;
    for (i32 type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index, ++blowup_type) {
        if (NuSpecialExistsFn(&blowup_type->animated_special) != 0) {
            nuinstanim_s *animation = NuSpecialGetInstAnim(&blowup_type->animated_special);
            if (animation != NULL && (blowup_type->animation_runtime_flags & GIZMOBLOWUPTYPE_ANIMATION_UPDATED) != 0) {
                blowup_type->animation_runtime_flags &= ~GIZMOBLOWUPTYPE_ANIMATION_UPDATED;
            }
        }
    }
}

void GizmoBlowupBurstDraw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    i32 type_index = 0;
    GIZMOBLOWUP_s *type_instances = world->gizmo_blowups;

    while (type_index < world->gizmo_blowup_type_count) {
        GIZMOBLOWUPTYPE_s *type = type_instances->type;
        GIZMOBLOWUP_s *next_type_instances = type_instances;
        if (type != NULL) {
            GIZMOBLOWUP_s *blowup = type_instances;
            for (i32 instance_index = 0; instance_index < type->instance_count; ++instance_index, ++blowup) {
                if ((blowup->draw_flags & GIZMOBLOWUP_DRAW_PROJECT_ONLY) != 0) {
                    blowup->visibility_flags |= GIZMOBLOWUP_DRAWN;
                    NuCameraTransformScreenClip(&blowup->screen_position, &blowup->position, 1, NULL);
                    continue;
                }
                if ((blowup->visibility_flags & GIZMOBLOWUP_VISIBLE) == 0) {
                    continue;
                }

                TouchHacks::TintStack tint;
                blowup->flicker_timer -= FRAMETIME;
                if (TouchHacks::ShouldFlash(blowup->flicker_timer)) {
                    NUCOLOUR3 *flash_colour = TouchHacks::GetFlashColour();
                    NuRndrLightingStateCurrent.ambient = *flash_colour;
                    NuRndrSetAmbientLightPS(flash_colour);
                }

                if ((blowup->draw_flags & GIZMOBLOWUP_DRAW_RESET_SHADOW_MAP) != 0) {
                    ResetShadowMapRendering();
                } else {
                    EnableShadowMapRendering(0);
                }

                nuhspecial_s *animated_special = &type->animated_special;
                nuinstanim_s *animation = NuSpecialGetInstAnim(animated_special);
                NUMTX animated_matrix;
                NUMTX draw_matrix;
                if (animation == NULL || (blowup->state_flags & GIZMOBLOWUP_STATE_REPEAT_ANIMATION) == 0) {
                    i32 drawn = NuSpecialDrawAt(animated_special, &blowup->transform);
                    blowup->visibility_flags =
                        static_cast<u8>((blowup->visibility_flags & ~GIZMOBLOWUP_DRAWN) | (drawn << 7));
                    draw_matrix = blowup->transform;
                    if ((blowup->visibility_flags & GIZMOBLOWUP_DRAWN) != 0) {
                        NuCameraTransformScreenClip(&blowup->screen_position, &blowup->position, 1, NULL);
                    }
                } else {
                    f32 end_frame =
                        NuAnimEndFrameOld(animated_special->scene->instance_animation_data[animation->anim_ix]);
                    f32 frame = type->animation_base_frame + blowup->animation_offset;
                    if (frame > end_frame) {
                        frame = frame - end_frame + animation->tfirst;
                    }

                    if ((type->animation_flags & GIZMOBLOWUPTYPE_ANIMATION_INCLUDES_INSTANCE_TRANSFORM) == 0) {
                        if ((blowup->state_flags & GIZMOBLOWUP_STATE_REPEATING) == 0) {
                            frame = blowup->animation_time;
                        }
                        EvalAnim(animated_special, frame, &animated_matrix, 0);
                        NuMtxMulVU0(&animated_matrix, &animated_matrix, &blowup->transform);
                    } else {
                        EvalAnim(animated_special, frame, &animated_matrix, 1);
                    }

                    i32 drawn = NuSpecialDrawAt(animated_special, &animated_matrix);
                    blowup->visibility_flags =
                        static_cast<u8>((blowup->visibility_flags & ~GIZMOBLOWUP_DRAWN) | (drawn << 7));
                    draw_matrix = animated_matrix;
                    if ((blowup->visibility_flags & GIZMOBLOWUP_DRAWN) != 0) {
                        NuCameraTransformScreenClip(&blowup->screen_position, NUMTX_GET_ROW_VEC(&animated_matrix, 3), 1,
                                                    NULL);
                    }
                }

                ResetShadowMapRendering();
                u32 draw_flags = blowup->draw_flags;
                if ((blowup->status_flags & GIZMOBLOWUP_DRAW_STATUS_TRANSFORM_READY) ==
                        GIZMOBLOWUP_DRAW_STATUS_TRANSFORM_READY &&
                    (draw_flags & GIZMOBLOWUP_DRAW_CUSTOM_TRANSFORM) != 0 && GizmoBlowup_TransformDrawFn != NULL) {
                    GizmoBlowup_TransformDrawFn(blowup);
                    draw_flags = blowup->draw_flags;
                }

                if ((draw_flags & GIZMOBLOWUP_DRAW_REFLECTION) != 0 &&
                    MatrixReflection(&draw_matrix, 2, draw_matrix.m31 + blowup->reflection_height,
                                     blowup->reflection_height, &draw_matrix) != 0) {
                    NuRndrStartReflectionRender(0);
                    NuSpecialDrawAt(animated_special, &draw_matrix);
                    NuRndrEndReflectionRender();
                }
            }

            if (NuSpecialExistsFn(&type->decal_special) != 0) {
                GIZMOBLOWUP_s *decal_blowup = type_instances;
                for (i32 instance_index = 0; instance_index < type->instance_count; ++instance_index, ++decal_blowup) {
                    if ((decal_blowup->draw_flags & GIZMOBLOWUP_DRAW_HIDE_DECAL) == 0 &&
                        (decal_blowup->output_flags & GIZMOBLOWUP_OUTPUT_BLOWN_UP) != 0) {
                        NUMTX decal_matrix;
                        GizmoBlowupGenDecalMatrix(decal_blowup, &decal_matrix, 0);
                        NuSpecialDrawAt(&type->decal_special, &decal_matrix);
                    }
                }
            }

            if (NuSpecialExistsFn(&type->shadow_special) != 0) {
                for (i32 instance_index = 0; instance_index < type->instance_count; ++instance_index) {
                    GIZMOBLOWUP_s *blowup = &type_instances[instance_index];
                    if ((blowup->state_flags & GIZMOBLOWUP_STATE_ACTIVATED) != 0 &&
                        (blowup->draw_flags & GIZMOBLOWUP_DRAW_RESET_SHADOW_MAP) == 0) {
                        NUMTX shadow_matrix;
                        GizmoBlowupGenShadowMatrix(blowup, &shadow_matrix);
                        NuSpecialDrawAt(&type->shadow_special, &shadow_matrix);
                    }
                }
            }

            if (NuSpecialExistsFn(&type->burst_special) != 0) {
                for (i32 instance_index = 0; instance_index < type->instance_count; ++instance_index) {
                    GIZMOBLOWUP_s *blowup = &type_instances[instance_index];
                    if ((blowup->output_flags & GIZMOBLOWUP_OUTPUT_BLOWN_UP) != 0) {
                        NuSpecialDrawAt(&type->burst_special, &blowup->transform);
                    }
                }
            }

            next_type_instances = blowup;
        }

        type_instances = next_type_instances;
        ++type_index;
    }
    ResetShadowMapRendering();
}

static char *Blowup_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZMOBLOWUP_s *>(gizmo->object)->name : NULL;
}

static i32 Blowup_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    GIZMOBLOWUP_s *blowup = static_cast<GIZMOBLOWUP_s *>(gizmo->object);
    switch (output_index) {
        case 0:
            return (blowup->output_flags & GIZMOBLOWUP_OUTPUT_BLOWN_UP) != 0;
        case 1:
            return (blowup->output_flags & GIZMOBLOWUP_OUTPUT_PUNCHED) != 0;
        case 2:
            return (blowup->field_0x9f & 0x10) != 0;
        default:
            return 0;
    }
}

static char *Blowup_GetOutputName(GIZMO *gizmo, i32 output_index) {
    return output_index >= 0 && output_index < 3 ? Blowup_OutputName[output_index] : NULL;
}

static i32 Blowup_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Blowup_Activate(GIZMO *gizmo, i32 enabled) {
    if (gizmo == NULL) {
        return;
    }

    GIZMOBLOWUP_s *blowup = static_cast<GIZMOBLOWUP_s *>(gizmo->object);
    i32 activated = enabled != 0;
    i32 activated_flag = activated << 7;
    blowup->state_flags = (blowup->state_flags & ~GIZMOBLOWUP_STATE_ACTIVATED) | activated_flag;

    nuinstanim_s *animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
    u8 state_flags;
    if (animation != NULL) {
        if (!activated) {
            animation->playing = 0;
            return;
        }
        state_flags = blowup->state_flags;
        if ((state_flags & GIZMOBLOWUP_STATE_ANIMATION_PLAYING) != 0) {
            animation->playing = 1;
            state_flags = blowup->state_flags;
        }
    } else if (!activated) {
        return;
    } else {
        state_flags = blowup->state_flags;
    }

    blowup->output_flags &= ~GIZMOBLOWUP_OUTPUT_BLOWN_UP;
    blowup->field_0x9f &= ~1;
    blowup->visibility_flags = (blowup->visibility_flags & 0x7f) | GIZMOBLOWUP_VISIBLE;
    blowup->state_flags = state_flags | GIZMOBLOWUP_STATE_ACTIVATED;
    blowup->saved_state_1 = blowup->initial_state_1;
    blowup->saved_state_0 = blowup->initial_state_0;

    animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
    if (animation == NULL) {
        state_flags = blowup->state_flags;
    } else {
        u8 previous_state_flags = blowup->state_flags;
        state_flags = previous_state_flags;
        if (animation->playing != 0) {
            state_flags = previous_state_flags | GIZMOBLOWUP_STATE_ANIMATION_PLAYING;
            blowup->state_flags = state_flags;
            if (animation->repeating != 0) {
                state_flags = previous_state_flags | GIZMOBLOWUP_STATE_REPEAT_ANIMATION |
                              GIZMOBLOWUP_STATE_ANIMATION_PLAYING | GIZMOBLOWUP_STATE_REPEATING;
                blowup->state_flags = state_flags;
            }
        }
    }
    blowup->state_flags = state_flags | GIZMOBLOWUP_STATE_ACTIVE;
}

static void Blowup_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL) {
        return;
    }

    GIZMOBLOWUP_s *blowup = static_cast<GIZMOBLOWUP_s *>(gizmo->object);
    u8 visibility_flag = visible != 0;
    visibility_flag <<= 6;
    blowup->visibility_flags = (blowup->visibility_flags & ~GIZMOBLOWUP_VISIBLE) | visibility_flag;

    if (blowup->platform_id != -1) {
        PlatOnOff(blowup->platform_id, visible);
    }

    if ((blowup->visibility_flags & GIZMOBLOWUP_VISIBLE) != 0) {
        if (blowup->anti_node == NULL) {
            blowup->state_flags |= GIZMOBLOWUP_STATE_ACTIVE;
        }
    } else if (blowup->anti_node != NULL) {
        GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, blowup->anti_node);
        blowup->anti_node = NULL;
    }
}

i32 Blowup_GetVisibility(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return 0;
    }
    GIZMOBLOWUP_s *blowup = static_cast<GIZMOBLOWUP_s *>(gizmo->object);
    return (blowup->visibility_flags & GIZMOBLOWUP_VISIBLE) != 0;
}

static NUVEC *GizmoBlowup_GetPos(GIZMO *gizmo) {
    return gizmo != NULL ? &static_cast<GIZMOBLOWUP_s *>(gizmo->object)->position : NULL;
}

static void *Blowup_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0x100);
}

struct BLOWUPPROGRESS_s {
    u32 blown_up[16];
    u32 activated[16];
    u32 visible[16];
    u32 secondary_output[16];
};
DECOMP_ASSERT(sizeof(BLOWUPPROGRESS_s) == 0x100, "BLOWUPPROGRESS_s ABI");

static void Blowup_ClearProgress(void *, void *progress_ptr) {
    BLOWUPPROGRESS_s *progress = static_cast<BLOWUPPROGRESS_s *>(progress_ptr);
    if (progress != NULL) {
        memset(progress->blown_up, 0, sizeof(progress->blown_up));
        memset(progress->activated, 0xff, sizeof(progress->activated));
        memset(progress->visible, 0xff, sizeof(progress->visible));
        memset(progress->secondary_output, 0, sizeof(progress->secondary_output));
    }
}

static void Blowup_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    BLOWUPPROGRESS_s *progress = static_cast<BLOWUPPROGRESS_s *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    Blowup_ClearProgress(NULL, progress);
    if (world == NULL || world->gizmo_blowups == NULL) {
        return;
    }

    i32 count = world->gizmo_blowup_count;
    if (count > 512) {
        count = 512;
    }
    for (i32 index = 0; index < count; ++index) {
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[index];
        const u32 bit = 1u << (index & 31);
        const i32 word = index >> 5;
        if ((blowup->output_flags & GIZMOBLOWUP_OUTPUT_BLOWN_UP) != 0) {
            progress->blown_up[word] |= bit;
        }
        if ((blowup->state_flags & GIZMOBLOWUP_STATE_ACTIVATED) == 0) {
            progress->activated[word] &= ~bit;
        }
        if ((blowup->visibility_flags & GIZMOBLOWUP_VISIBLE) == 0) {
            progress->visible[word] &= ~bit;
        }
        if ((blowup->field_0x9f & 0x10) != 0) {
            progress->secondary_output[word] |= bit;
        }
    }
}

static void Blowups_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    BLOWUPPROGRESS_s *progress = static_cast<BLOWUPPROGRESS_s *>(progress_ptr);
    if (world == NULL || world->gizmo_blowups == NULL) {
        return;
    }

    for (i32 index = 0; index < world->gizmo_blowup_count; ++index) {
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[index];
        blowup->state_flags |= GIZMOBLOWUP_STATE_ACTIVATED;
        blowup->visibility_flags = (blowup->visibility_flags & ~GIZMOBLOWUP_DRAWN) | GIZMOBLOWUP_VISIBLE;
        blowup->output_flags &= ~GIZMOBLOWUP_OUTPUT_BLOWN_UP;
        blowup->field_0x9f &= ~0x10;
        blowup->saved_state_0 = blowup->initial_state_0;
        blowup->saved_state_1 = blowup->initial_state_1;

        nuinstanim_s *animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        blowup->state_flags &=
            ~(GIZMOBLOWUP_STATE_REPEAT_ANIMATION | GIZMOBLOWUP_STATE_ANIMATION_PLAYING | GIZMOBLOWUP_STATE_REPEATING);
        if (animation != NULL && animation->playing != 0) {
            blowup->state_flags |= GIZMOBLOWUP_STATE_ANIMATION_PLAYING;
            if (animation->repeating != 0) {
                blowup->state_flags |= GIZMOBLOWUP_STATE_REPEAT_ANIMATION | GIZMOBLOWUP_STATE_REPEATING;
            }
        }

        if (progress != NULL && index < 512) {
            const u32 bit = 1u << (index & 31);
            const i32 word = index >> 5;
            if ((progress->blown_up[word] & bit) != 0) {
                blowup->output_flags |= GIZMOBLOWUP_OUTPUT_BLOWN_UP;
            }
            if ((progress->activated[word] & bit) == 0) {
                blowup->state_flags &= ~GIZMOBLOWUP_STATE_ACTIVATED;
            }
            if ((progress->visible[word] & bit) == 0) {
                blowup->visibility_flags &= ~GIZMOBLOWUP_VISIBLE;
            }
            if ((progress->secondary_output[word] & bit) != 0) {
                blowup->field_0x9f |= 0x10;
            }
        }
    }
}

void *gizmoblowup_reservebuffers(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    InitGizmoBlowupTypes(world);
    InitGizmoBlowups(world);
    InitGizmoBlowupsMtxBuffer(world);
    return world->gizmo_blowups;
}

static void Blowup_ReadString(char *text) {
    memset(text, 0, 0x100);
    const i32 length = static_cast<i8>(EdFileReadChar());
    if (length > 0) {
        EdFileRead(text, length);
    }
}

static void Blowup_FindSpecial(WORLDINFO *world, nuhspecial_s *special, char *name, char *label, char *type_name) {
    Gizmo_FindNuSpecial(world->current_gscn, special, name, 1, world->gizmo_sys, label, type_name);
}

i32 gizmoblowup_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->gizmo_blowup_types == NULL || world->gizmo_blowups == NULL) {
        return 0;
    }
    memset(world->gizmo_blowup_types, 0, world->current_level->max_gizmo_blowup_types * sizeof(GIZMOBLOWUPTYPE_s));
    if (world->gizmo_blowup_count != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    i32 file_type_count = 0;
    i32 file_instance_count;
    if (version < 2) {
        file_instance_count = EdFileReadInt();
    } else {
        file_type_count = EdFileReadInt();
        file_instance_count = EdFileReadInt();
    }

    char text[0x100];
    for (i32 file_type_index = 0; file_type_index < file_type_count; ++file_type_index) {
        GIZMOBLOWUPTYPE_s type;
        memset(&type, 0, sizeof(type));

        Blowup_ReadString(text);
        NuStrCpy(type.name, text);
        Blowup_ReadString(text);
        Blowup_FindSpecial(world, &type.animated_special, text, const_cast<char *>("BlowUp"), type.name);

        for (i32 particle_index = 0; particle_index < 9; ++particle_index) {
            type.particle_types[particle_index] = -1;
        }
        if (version > 16) {
            Blowup_ReadString(text);
            Blowup_ReadString(text);
        }
        for (i32 particle_index = 0; particle_index < 3; ++particle_index) {
            Blowup_ReadString(text);
        }
        if (version >= 26) {
            Blowup_ReadString(text);
            Blowup_ReadString(text);
        }
        if (version != 26) {
            Blowup_ReadString(text);
            Blowup_ReadString(text);
        }

        type.type_flags = static_cast<u32>(EdFileReadInt());
        type.effect_flags = static_cast<u32>(EdFileReadInt());
        type.field_0xfb = static_cast<u8>(EdFileReadChar());
        type.animation_time_scale = EdFileReadFloat();

        Blowup_ReadString(text);
        Blowup_FindSpecial(world, &type.decal_special, text, const_cast<char *>("BlowUpDecal"), type.name);
        type.animation_start_frame = EdFileReadFloat();
        type.animation_end_frame = EdFileReadFloat();
        type.field_0xf8 = static_cast<u8>(EdFileReadChar());
        type.field_0xf9 = static_cast<u8>(EdFileReadChar());
        GameAntiNodeData_Init(&type.anti_node_data, &type.animated_special);
        if (version >= 16) {
            GameAntiNodeData_Read(&type.anti_node_data);
        }
        if (version >= 22) {
            for (i32 alternate_index = 0; alternate_index < 4; ++alternate_index) {
                Blowup_ReadString(text);
                Blowup_FindSpecial(world, &type.alternate_specials[alternate_index], text,
                                   const_cast<char *>("BlowUpEmitObj"), type.name);
            }
        }
        type.field_0xfa = static_cast<u8>(EdFileReadChar());
        type.field_0x94 = EdFileReadFloat();
        type.field_0x98 = EdFileReadFloat();
        Blowup_ReadString(text);
        Blowup_FindSpecial(world, &type.shadow_special, text, const_cast<char *>("BlowUpShadow"), type.name);
        if (version >= 20) {
            Blowup_ReadString(text);
            Blowup_FindSpecial(world, &type.burst_special, text, const_cast<char *>("BlowUpSwap"), type.name);
        }
        if (version >= 23) {
            type.field_0x9c = EdFileReadFloat();
        }
        if (version >= 24) {
            type.field_0x84 = EdFileReadFloat();
        }

        if (NuSpecialExistsFn(&type.animated_special) != 0) {
            world->gizmo_blowup_types[world->gizmo_blowup_type_count++] = type;
        }
    }

    for (i32 file_instance_index = 0; file_instance_index < file_instance_count; ++file_instance_index) {
        GIZMOBLOWUP_s blowup;
        memset(&blowup, 0, sizeof(blowup));

        Blowup_ReadString(text);
        GIZMOBLOWUPTYPE_s *type = GizmoBlowup_FindType(text, world);
        if (version > 1) {
            Blowup_ReadString(text);
            NuStrNCpy(blowup.name, text, sizeof(blowup.name));
        }
        blowup.type = type;
        EdFileReadNuVec(&blowup.position);
        blowup.field_0xf0 = EdFileReadShort();
        blowup.field_0xf2 = EdFileReadShort();
        blowup.field_0xf4 = EdFileReadShort();
        blowup.draw_flags = static_cast<u32>(EdFileReadInt());
        if (version >= 30) {
            blowup.secondary_flags = static_cast<u32>(EdFileReadInt());
        }
        blowup.field_0xa8 = static_cast<u32>(EdFileReadInt());
        blowup.field_0x115 = static_cast<u8>(EdFileReadChar());
        blowup.field_0x114 = static_cast<u8>(EdFileReadChar());
        blowup.initial_state_0 = static_cast<u8>(EdFileReadChar());
        blowup.field_0xb4 = EdFileReadFloat();
        blowup.field_0xb8 = EdFileReadFloat();
        blowup.field_0xc0 = EdFileReadFloat();
        blowup.field_0xe4 = EdFileReadShort();
        blowup.field_0xe6 = EdFileReadShort();
        blowup.field_0xe8 = EdFileReadShort();
        blowup.field_0x74 = EdFileReadFloat();
        blowup.field_0x78 = EdFileReadFloat();
        blowup.field_0x7c = EdFileReadFloat();
        blowup.field_0xc8 = EdFileReadFloat();
        blowup.field_0x80 = EdFileReadFloat();
        blowup.field_0x84 = EdFileReadFloat();
        blowup.field_0x88 = EdFileReadFloat();
        blowup.saved_state_1 = static_cast<u8>(EdFileReadChar());
        blowup.initial_state_1 = blowup.saved_state_1;
        blowup.field_0xf6 = EdFileReadShort();
        blowup.field_0xf8 = EdFileReadShort();
        blowup.field_0xea = EdFileReadShort();
        blowup.field_0xec = EdFileReadShort();
        blowup.field_0xee = EdFileReadShort();
        blowup.field_0x8c = EdFileReadFloat();
        blowup.field_0x90 = EdFileReadFloat();
        blowup.field_0x94 = EdFileReadFloat();
        blowup.field_0xcc = EdFileReadFloat();
        blowup.animation_offset = EdFileReadFloat();
        if (version > 22) {
            blowup.field_0xd8 = EdFileReadFloat();
        }
        if (version > 30) {
            blowup.reflection_height = EdFileReadFloat();
        }

        if (type != NULL) {
            world->gizmo_blowups[world->gizmo_blowup_count++] = blowup;
        }
    }

    for (i32 type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
        GIZMOBLOWUPTYPE_s *type = &world->gizmo_blowup_types[type_index];
        type->instance_count = 0;
        for (i32 instance_index = 0; instance_index < world->gizmo_blowup_count; ++instance_index) {
            if (world->gizmo_blowups[instance_index].type == type) {
                ++type->instance_count;
            }
        }
    }

    for (i32 instance_index = 0; instance_index < world->gizmo_blowup_count; ++instance_index) {
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[instance_index];
        NUVEC *draw_position = NuSpecialGetDrawPos(&blowup->type->animated_special);
        if (draw_position != NULL && draw_position->x == blowup->position.x && draw_position->y == blowup->position.y &&
            draw_position->z == blowup->position.z) {
            blowup->type->type_flags |= 0x2000;
        }
        blowup->platform_id = -1;
        blowup->field_0x10c = -1;
        if ((blowup->draw_flags & GIZMOBLOWUP_DRAW_REFLECTION) == 0) {
            blowup->reflection_height = 0.0f;
        }
        if ((blowup->draw_flags & 0x00018080) == 0x00018000) {
            blowup->draw_flags |= 0x80;
        }
    }
    return 1;
}

ADDGIZMOTYPE *NewBlowup_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "blowup";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x100;
    addtype.fns.early_update_fn = GizmoBlowupEarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = Blowup_GetVisibility;
    addtype.fns.get_max_gizmos_fn = Blowup_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBlowup_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Blowup_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizmoBlowupLateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizmoBlowupBurstDraw;
    addtype.fns.get_gizmo_name_fn = Blowup_GetGizmoName;
    addtype.fns.get_output_fn = Blowup_GetOutput;
    addtype.fns.get_output_name_fn = Blowup_GetOutputName;
    addtype.fns.get_num_outputs_fn = Blowup_GetNumOutputs;
    addtype.fns.activate_fn = Blowup_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Blowup_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Blowup_AllocateProgressData;
    addtype.fns.clear_progress_fn = Blowup_ClearProgress;
    addtype.fns.store_progress_fn = Blowup_StoreProgress;
    addtype.fns.reset_fn = Blowups_Reset;
    addtype.fns.reserve_buffer_space_fn = gizmoblowup_reservebuffers;
    addtype.fns.load_fn = gizmoblowup_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    blowup_gizmotype_id = type_id;

    return &addtype;
}
