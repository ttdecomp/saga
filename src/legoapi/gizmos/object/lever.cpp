#include "legoapi/gizmos/object/lever.h"

#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" {
    void DeletePlatinst(i32 platform_id);
    i16 NewPlatPickupInst(void *object, i32 object_type);
    void PlatInstRotate(i32 platform_id, i32 enabled);
}

f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 platform_id);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);

struct LEVERPROGRESS {
    u32 pulled_down;
    u32 enabled;
    u32 visible;
};

DECOMP_ASSERT(sizeof(LEVERPROGRESS) == 0xc, "LEVER progress ABI");

LEVER_CONFIG LeverSys = {0x55, 0};

i32 lever_gizmotype_id = -1;

static i32 Levers_GetMaxGizmos(void *lever) {
    WORLDINFO *world = static_cast<WORLDINFO *>(lever);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_levers;
}

static void Levers_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->levers == NULL) {
        return;
    }

    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        if (NuStrLen(lever.name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &lever);
        }
    }
}

static void Levers_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->levers == NULL) {
        return;
    }

    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        if ((lever.flags & (LEVER_FLAG_BEING_PULLED | LEVER_FLAG_INTERACTION_FINISHED | LEVER_FLAG_PULLED_DOWN)) == 0) {
            lever.animation_frame = 0;
            continue;
        }

        lever.flags |= LEVER_FLAG_BEING_PULLED;
        if (lever.auto_reset_timer > 0.0f) {
            lever.auto_reset_timer -= FRAMETIME;
            if (lever.auto_reset_timer <= 0.0f) {
                lever.flags |= LEVER_FLAG_RETURNING | LEVER_FLAG_PULLED_DOWN;
                lever.flags &= ~LEVER_FLAG_INTERACTION_FINISHED;
                lever.animation_frame = 0;
                lever.auto_reset_timer = 0.0f;
                lever.pull_progress = 0.6f;
            } else {
                lever.animation_frame = static_cast<u16>(lever.pull_progress * 32768.0f);
            }
            continue;
        }

        if ((lever.flags & (LEVER_FLAG_RETURNING | LEVER_FLAG_PULLED_DOWN)) == 0) {
            if (lever.pull_progress < 1.0f) {
                lever.pull_progress += FRAMETIME;
                if (lever.pull_progress > 1.0f) {
                    lever.pull_progress = 1.0f;
                    PlaySfx(const_cast<char *>("SwLever"), &lever.position);
                    if ((lever.flags & LEVER_FLAG_AUTO_RESET) != 0) {
                        lever.auto_reset_timer = lever.auto_reset_delay;
                    }
                }
            }
        } else {
            lever.flags |= LEVER_FLAG_RETURNING;
            lever.pull_progress -= FRAMETIME;
            if (lever.pull_progress <= 0.0f) {
                lever.pull_progress = 0.0f;
                lever.auto_reset_timer = 0.0f;
                lever.flags &= ~LEVER_FLAG_RETURN_RESET_MASK;
                lever.idle_animation_timer = 0.0f;
                lever.animation_frame = 0;
                continue;
            }
        }

        lever.animation_frame = static_cast<u16>(lever.pull_progress * 32768.0f);
    }
}

static void Levers_Draw(void *world_ptr, void *, float) {
    enum LEVER_SPECIAL_INDEX {
        LEVER_SPECIAL_ANIMATED = 30,
        LEVER_SPECIAL_BASE = 31,
        LEVER_SPECIAL_MODEL_FIRST = 32,
        LEVER_SPECIAL_READY = 41,
        LEVER_SPECIAL_COMPLETE = 42,
        LEVER_SPECIAL_INCOMPLETE = 43,
        LEVER_SPECIAL_TARGET = 85,
    };

    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->lev_objs == NULL || world->levers == NULL ||
        world->lev_objs[LEVER_SPECIAL_ANIMATED].active == 0) {
        return;
    }

    const u16 target_spin_angle = static_cast<u16>(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f);
    const f32 pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
    const f32 target_pulse = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.2f + 0.8f;
    const f32 ready_alpha = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.15f + 0.85f;

    EnableShadowMapRendering(0);

    LEVEL_OBJECT_RUNTIME *animated_entry = &world->lev_objs[LEVER_SPECIAL_ANIMATED];
    nuhspecial_s *animated_special = animated_entry->active != 0 ? &animated_entry->special : NULL;
    if (animated_special != NULL) {
        nuinstanim_s *instance_animation = NuSpecialGetInstAnim(animated_special);
        if (instance_animation != NULL && animated_special->scene != NULL) {
            NuAnimEndFrameOld(animated_special->scene->instance_animation_data[instance_animation->anim_ix]);
        }
    }

    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        if ((lever.flags & LEVER_FLAG_VISIBLE) == 0) {
            continue;
        }

        TouchHacks::TintStack tint;
        lever.flash_timer -= FRAMETIME;
        if (TouchHacks::ShouldFlash(lever.flash_timer)) {
            NUCOLOUR3 *flash_colour = TouchHacks::GetFlashColour();
            NuRndrLightingStateCurrent.ambient = *flash_colour;
            NuRndrSetAmbientLightPS(flash_colour);
        }

        NUMTX animated_matrix;
        if (animated_special == NULL) {
            NuMtxSetRotationX(&animated_matrix, 0);
            NuMtxRotateY(&animated_matrix, lever.y_rotation);
            NuMtxTranslate(&animated_matrix, &lever.position);
            NuSpecialDrawAt(&animated_entry->special, &animated_matrix);
        } else {
            f32 animation_frame = 0.0f;
            if (lever.pull_progress <= 0.0f) {
                if ((lever.flags & LEVER_FLAG_IDLE_STATE_MASK) == LEVER_FLAG_ENABLED) {
                    if (lever.idle_animation_timer > 0.0f) {
                        lever.idle_animation_timer -= FRAMETIME;
                        if (lever.idle_animation_timer < 0.0f) {
                            lever.idle_animation_timer = 0.0f;
                        } else {
                            const f32 idle_phase = lever.idle_animation_timer / 0.15f * 32768.0f + 16384.0f;
                            animation_frame =
                                (1.0f - NuFabs(NuTrigTable[(static_cast<i32>(idle_phase) >> 1) & 0x7fff])) * 1.5f +
                                16.0f;
                        }
                    } else if (qrand() < 0x800) {
                        lever.idle_animation_timer = 0.15f;
                    }
                }
            } else if (lever.auto_reset_timer > 0.0f) {
                animation_frame = 32.0f;
            } else if ((lever.flags & LEVER_FLAG_RETURNING) == 0) {
                animation_frame = lever.pull_progress * 31.0f + 1.0f;
            } else {
                animation_frame = 32.0f + ((0.6f - lever.pull_progress) * 32.0f) / 0.6f;
            }

            EvalAnim(animated_special, animation_frame, &animated_matrix, 0);
            NuMtxMulVU0(&animated_matrix, &animated_matrix, &lever.transform);
            NuSpecialDrawAt(animated_special, &animated_matrix);
        }

        NUMTX target_matrix = animated_matrix;
        NUMTX base_matrix;
        NuMtxSetRotationY(&base_matrix, lever.y_rotation);
        NuMtxTranslate(&base_matrix, &lever.position);

        LEVEL_OBJECT_RUNTIME &model_entry = world->lev_objs[lever.model_special_index];
        if (model_entry.active != 0) {
            NuSpecialDrawAt(&model_entry.special, &animated_matrix);
        }

        if (world->lev_objs[LEVER_SPECIAL_BASE].active != 0 && (lever.flags & LEVER_FLAG_HIDE_BASE) == 0) {
            NuSpecialDrawAt(&world->lev_objs[LEVER_SPECIAL_BASE].special, &lever.transform);
        }

        const i32 status_special = lever.pull_progress < 1.0f ? LEVER_SPECIAL_INCOMPLETE : LEVER_SPECIAL_COMPLETE;
        if ((lever.flags & LEVER_FLAG_ENABLED) != 0) {
            bool draw_target = false;
            if ((lever.flags & LEVER_FLAG_HIDE_TARGET_INDICATOR) == 0 && lever.target_offset.y != 2000000.0f) {
                draw_target = world->lev_objs[LEVER_SPECIAL_TARGET].active != 0;
            }

            if ((lever.flags & LEVER_FLAG_IDLE_STATE_MASK) == LEVER_FLAG_ENABLED &&
                world->lev_objs[LEVER_SPECIAL_READY].active != 0) {
                NuSpecialDrawAtAlpha(&world->lev_objs[LEVER_SPECIAL_READY].special, &target_matrix, ready_alpha);
            } else if (!draw_target) {
                goto draw_status;
            }

            GameObject_s *nearest_player;
            f32 distance_squared;
            if (FindNearestPlayerToVec(&lever.position, &nearest_player, distance_squared, false, 0)) {
                const f32 distance_factor = NuFmin(distance_squared / 6.0f, 1.0f);
                const f32 target_alpha =
                    (1.0f -
                     NuTrigTable[(static_cast<i32>(distance_factor * 16384.0f + 49152.0f + 16384.0f) >> 1) & 0x7fff]) *
                    target_pulse;
                if (target_alpha > 0.0f && draw_target) {
                    NUVEC target_position;
                    Lever_GetAbsTargetPos(&lever, &target_position);
                    NuMtxSetRotationY(&animated_matrix, target_spin_angle);
                    if (lever.target_z_rotation != 0) {
                        NuMtxRotateZ(&animated_matrix, lever.target_z_rotation);
                    }
                    if (lever.target_x_rotation != 0) {
                        NuMtxRotateX(&animated_matrix, lever.target_x_rotation);
                    }
                    NuMtxScaleU(&animated_matrix, 0.8f);
                    NuMtxTranslate(&animated_matrix, &target_position);
                    NuMtxPreScaleU(&animated_matrix, lever.target_indicator_scale);
                    NuSpecialDrawAtAlpha(&world->lev_objs[LEVER_SPECIAL_TARGET].special, &animated_matrix,
                                         (1.0f - lever.pull_progress) * target_alpha);
                }
            }
        } else if (lever.pull_progress == 0.0f && ParticlesPerSecond(0.5f, FRAMETIME) > 0) {
            AddGameDebris(WORLD->debris_sys, 0x57, &lever.position);
        }

    draw_status:
        if (world->lev_objs[status_special].active != 0 && (lever.flags & LEVER_FLAG_HIDE_BASE) == 0) {
            NuSpecialDrawAt(&world->lev_objs[status_special].special, &base_matrix);
        }
    }

    ResetShadowMapRendering();
}

static char *Lever_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return static_cast<LEVER_s *>(gizmo->object)->name;
}

static i32 Lever_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    LEVER_s *lever = static_cast<LEVER_s *>(gizmo->object);
    if ((lever->flags & LEVER_FLAG_ENABLED) == 0) {
        return 0;
    }
    if (output_index == 0) {
        return Lever_FullyPulledDown(lever);
    }
    if (output_index == 1) {
        return (lever->flags & LEVER_FLAG_GOODIE) != 0 && Lever_FullyPulledDown(lever);
    }
    if (output_index == 2) {
        return (lever->flags & LEVER_FLAG_BADDIE) != 0 && Lever_FullyPulledDown(lever);
    }
    return 0;
}

static char *Lever_GetOutputName(GIZMO *gizmo, i32 output_index) {
    if (output_index == 1) {
        return "Down(Goodie)";
    }

    if (output_index != 2) {
        if (output_index == 0) {
            return "Down";
        }

        return NULL;
    }

    return "Down(Baddie)";
}

static i32 Lever_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Lever_Reset(LEVER_s *lever);

static void Lever_Activate(GIZMO *gizmo, i32 enabled) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    LEVER_s *lever = static_cast<LEVER_s *>(gizmo->object);
    if (enabled != 0) {
        lever->flags |= LEVER_FLAG_ENABLED;
        Lever_Reset(lever);
    } else {
        lever->flags &= ~LEVER_FLAG_ENABLED;
    }
}

static i32 Lever_ActivateRev(GIZMO *gizmo, i32 value, i32 query) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 1;
    }

    LEVER_s *lever = static_cast<LEVER_s *>(gizmo->object);
    if ((query & 1) == 0) {
        if (value == 0) {
            lever->flags |= LEVER_FLAG_ENABLED;
        } else {
            lever->flags &= ~LEVER_FLAG_ENABLED;
        }
        return 1;
    }
    if ((lever->flags & LEVER_FLAG_ENABLED) == 0) {
        return value == 0;
    }
    return value;
}

static void Lever_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    LEVER_s *lever = static_cast<LEVER_s *>(gizmo->object);
    const bool was_visible = (lever->flags & LEVER_FLAG_VISIBLE) != 0;
    if (visible != 0) {
        lever->flags |= LEVER_FLAG_VISIBLE;
        if (!was_visible) {
            lever->platform_id = NewPlatPickupInst(lever, 3);
            PlatInstRotate(lever->platform_id, 1);
        }
    } else {
        lever->flags &= ~LEVER_FLAG_VISIBLE;
        if (was_visible) {
            DeletePlatinst(lever->platform_id);
        }
    }
}

static void *Levers_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(LEVERPROGRESS));
}

static void Levers_ClearProgress(void *, void *progress_data) {
    LEVERPROGRESS *progress = (LEVERPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->pulled_down = 0;
    progress->enabled = 0xffffffff;
    progress->visible = 0xffffffff;
}

static void Levers_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    LEVERPROGRESS *progress = static_cast<LEVERPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    Levers_ClearProgress(NULL, progress);
    if (world == NULL || world->levers == NULL) {
        return;
    }

    const i32 count = world->nlevers < 32 ? world->nlevers : 32;
    for (i32 index = 0; index < count; ++index) {
        const LEVER_s &lever = world->levers[index];
        const u32 bit = 1u << index;
        if ((lever.flags & LEVER_FLAG_BEING_PULLED) != 0 && lever.pull_progress >= 1.0f) {
            progress->pulled_down |= bit;
        }
        if ((lever.flags & LEVER_FLAG_VISIBLE) == 0) {
            progress->visible &= ~bit;
        }
        if ((lever.flags & LEVER_FLAG_ENABLED) == 0) {
            progress->enabled &= ~bit;
        }
    }
}

static void Levers_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    LEVERPROGRESS *progress = static_cast<LEVERPROGRESS *>(progress_ptr);
    if (world == NULL || world->levers == NULL) {
        return;
    }

    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        Lever_Reset(&lever);
        if (progress == NULL || index >= 32) {
            continue;
        }

        const u32 bit = 1u << index;
        if ((progress->pulled_down & bit) != 0) {
            lever.pull_progress = 1.0f;
            lever.flags |= LEVER_FLAG_BEING_PULLED;
            lever.animation_frame = 0x8000;
        }
        if ((progress->visible & bit) != 0) {
            lever.flags |= LEVER_FLAG_VISIBLE;
        } else {
            lever.flags &= ~LEVER_FLAG_VISIBLE;
        }
        if ((progress->enabled & bit) != 0) {
            lever.flags |= LEVER_FLAG_ENABLED;
        } else {
            lever.flags &= ~LEVER_FLAG_ENABLED;
        }
    }
}

static void *Levers_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->levers = NULL;
    world->nlevers = 0;
    if (world->current_level->max_levers == 0) {
        return NULL;
    }

    world->levers = static_cast<LEVER_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, world->current_level->max_levers * sizeof(LEVER_s)));
    return world->levers;
}

static i32 Levers_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->nlevers != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    world->nlevers = EdFileReadInt();
    for (i32 index = 0; index < world->nlevers; ++index) {
        LEVER_s &lever = world->levers[index];
        lever.flags = 0;
        EdFileRead(lever.name, sizeof(lever.name));
        EdFileReadNuVec(&lever.position);
        lever.y_rotation = static_cast<u16>(EdFileReadShort());
        lever.model_variant = static_cast<char>(EdFileReadChar());

        if (version < 2) {
            lever.auto_reset_delay = 2.0f;
        } else {
            if (EdFileReadChar() != 0) {
                lever.flags |= LEVER_FLAG_AUTO_RESET;
            }
            if (version == 2) {
                lever.auto_reset_delay = 2.0f;
            } else {
                lever.auto_reset_delay = EdFileReadFloat();
                if (version == 3) {
                    lever.flags &= ~LEVER_FLAG_HIDE_BASE;
                } else {
                    if (EdFileReadChar() != 0) {
                        lever.flags |= LEVER_FLAG_HIDE_BASE;
                    }
                    if (version != 4) {
                        EdFileReadNuVec(&lever.target_offset);
                        lever.target_indicator_scale = EdFileReadFloat();
                        if (version != 5 && EdFileReadChar() != 0) {
                            lever.flags |= LEVER_FLAG_HIDE_TARGET_INDICATOR;
                        }
                        lever.platform_id = -1;
                        continue;
                    }
                }
            }
        }

        lever.target_offset.x = 0.0f;
        lever.target_offset.y = 0.0f;
        lever.target_offset.z = -0.1441f;
        lever.target_indicator_scale = 1.0f;
        lever.platform_id = -1;
    }
    return 1;
}

static void Lever_Reset(LEVER_s *lever) {
    switch (lever->model_variant) {
        case 'r':
            lever->model_special_index = 0x22;
            break;
        case 'o':
            lever->model_special_index = 0x21;
            break;
        case 'y':
            lever->model_special_index = 0x20;
            break;
        case 'b':
            lever->model_special_index = 0x23;
            break;
        case 'g':
            lever->model_special_index = 0x24;
            break;
        case 'p':
            lever->model_special_index = 0x25;
            break;
        case 'w':
            lever->model_special_index = 0x26;
            break;
        case 'u':
            lever->model_special_index = 0x27;
            break;
        case 'l':
            lever->model_special_index = 0x28;
            break;
    }

    lever->floor_position = {0.0f, 0.0f, 0.1441f};
    NuVecRotateY(&lever->floor_position, &lever->floor_position, static_cast<u16>(lever->y_rotation + 0x8000));
    NuVecAdd(&lever->floor_position, &lever->floor_position, &lever->position);

    NUVEC target_position;
    Lever_GetAbsTargetPos(lever, &target_position);
    target_position.y = lever->position.y;
    lever->floor_position.y = GameShadow(NULL, &lever->floor_position, 0.2f, -1);
    const f32 target_floor = GameShadow(NULL, &target_position, 0.2f, -1);
    if (target_floor == -1.0f) {
        lever->target_offset.y = -1.0f;
    } else {
        lever->target_offset.y = target_floor + 0.02f;
        FindAnglesZX(&ShadNorm, &lever->target_x_rotation, &lever->target_z_rotation);
    }

    lever->animation_frame = 0;
    const u16 old_flags = lever->flags;
    lever->flags |= LEVER_FLAG_VISIBLE | LEVER_FLAG_ENABLED;
    if ((old_flags & (LEVER_FLAG_AUTO_RESET | LEVER_FLAG_BEING_PULLED)) !=
        (LEVER_FLAG_AUTO_RESET | LEVER_FLAG_BEING_PULLED)) {
        lever->pull_progress = 0.0f;
        lever->auto_reset_timer = 0.0f;
        lever->flags &= ~LEVER_FLAG_PULLED_DOWN;
        lever->flags =
            static_cast<u16>((lever->flags & ~LEVER_FLAG_LOW_STATE_MASK) | LEVER_FLAG_VISIBLE | LEVER_FLAG_ENABLED);
    }

    NuMtxSetRotationY(&lever->transform, lever->y_rotation);
    NuMtxTranslate(&lever->transform, &lever->position);
}

ADDGIZMOTYPE *Levers_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Lever";
    addtype.fns.unknown1 = 12;
    addtype.fns.early_update_fn = NULL;
    addtype.prefix = "";
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Levers_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Levers_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Levers_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Levers_Draw;
    addtype.fns.get_gizmo_name_fn = Lever_GetGizmoName;
    addtype.fns.get_output_fn = Lever_GetOutput;
    addtype.fns.get_output_name_fn = Lever_GetOutputName;
    addtype.fns.get_num_outputs_fn = Lever_GetNumOutputs;
    addtype.fns.activate_fn = Lever_Activate;
    addtype.fns.activate_rev_fn = Lever_ActivateRev;
    addtype.fns.set_visibility_fn = Lever_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Levers_AllocateProgressData;
    addtype.fns.clear_progress_fn = Levers_ClearProgress;
    addtype.fns.store_progress_fn = Levers_StoreProgress;
    addtype.fns.reset_fn = Levers_Reset;
    addtype.fns.reserve_buffer_space_fn = Levers_ReserveBufferSpace;
    addtype.fns.load_fn = Levers_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    lever_gizmotype_id = type_id;

    return &addtype;
}
