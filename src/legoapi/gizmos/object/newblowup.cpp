#include "legoapi/gizmos/object/newblowup.h"

#include "decomp.h"
#include "batman.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
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

extern "C" void PlatOnOff(i32 platform_id, i32 enabled);
void GameAntinode_UnregisterAntiNode(GAMEANTINODESYS_s *system, GAMEANTINODE_s *anti_node);
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

static void Blowup_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
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

static void Blowup_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Blowup_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Blowups_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

void *gizmoblowup_reservebuffers(void *) {
    UNIMPLEMENTED();
    return {};
}

i32 gizmoblowup_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
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
