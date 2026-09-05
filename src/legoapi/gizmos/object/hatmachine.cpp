#include "legoapi/gizmos/object/hatmachine.h"

#include "decomp.h"
#include "globals.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "gamelib/util/gamelib_util_types.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/input/qrand.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

void Hat_GetAbsTargetPos(HATMACHINE_s *machine, NUVEC *target_position);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);
void EnableShadowMapRendering(i32 enable);
void ResetShadowMapRendering(void);

static const NUVEC HatMachine_HatOffset = {0.0f, 0.3f, 0.0f};

enum HATMACHINE_ANIMATION_STATE {
    HATMACHINE_ANIMATION_IDLE = 0,
    HATMACHINE_ANIMATION_PLAY_HAT_SFX = 2,
    HATMACHINE_ANIMATION_COMPLETE = 7,
};

enum HATMACHINE_PLATFORM_TYPE {
    HATMACHINE_PLATFORM_COLLISION = 2,
};

extern "C" {
    void DeletePlatinst(i32 platform_id);
    i16 NewPlatPickupInst(void *object, i32 object_type);
    void PlatInstRotate(i32 platform_id, i32 enabled);
}

struct HATMACHINEPROGRESS {
    i32 preserved_state;
    u32 enabled_mask;
    u32 visible_mask;
};

i32 hatmachine_gizmotype_id = -1;

static void HatMachine_Reset(HATMACHINE_s *machine);

static i32 HatMachine_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_hat_machines : 0;
}

static void HatMachine_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->hat_machine_sys == NULL || world->hat_machine_sys->count <= 0) {
        return;
    }

    for (i32 index = 0; index < world->hat_machine_sys->count; ++index) {
        HATMACHINE *machine = &world->hat_machine_sys->machines[index];
        if (NuStrLen(machine->name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, machine);
        }
    }
}

static void HatMachine_Update(void *world_ptr, void *, float elapsed) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    HATMACHINESYS_s *system = world->hat_machine_sys;
    if (system == NULL || system->count <= 0) {
        return;
    }

    for (i32 index = 0; index < system->count; ++index) {
        HATMACHINE *machine = &system->machines[index];

        if (machine->hat_delay > 0.0f) {
            machine->hat_delay -= FRAMETIME;
            if (machine->hat_delay <= 0.0f && machine->displayed_hat == 0) {
                if (machine->configured_hat_count != 0) {
                    machine->displayed_hat = machine->configured_hat_count;
                } else {
                    machine->displayed_hat = static_cast<u8>(NuFloatRand(NULL) * 4.0f) + 1;
                }
                machine->hat_delay = 1.0f;
            }
        }

        if ((machine->flags & HATMACHINE_FLAG_ANIMATING) != 0 && machine->animation_state > 0) {
            machine->state_elapsed += elapsed;
            if (machine->state_elapsed > machine->state_duration) {
                ++machine->animation_state;
                if (machine->animation_state == HATMACHINE_ANIMATION_PLAY_HAT_SFX) {
                    PlaySfx("HatOn", &machine->position);
                }
                machine->state_elapsed = 0.0f;

                if (machine->animation_state >= HATMACHINE_ANIMATION_COMPLETE) {
                    machine->state_duration = 0.0f;
                    machine->hat_delay = 0.6f;
                    machine->animation_time = 0.0f;
                    machine->animation_state = HATMACHINE_ANIMATION_IDLE;
                    machine->flags = static_cast<HATMACHINE_FLAGS>(machine->flags & ~HATMACHINE_FLAG_ANIMATING);
                }
            }
        }
    }
}

static void HatMachine_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->hat_machine_sys == NULL || world->hat_machine_sys->count == 0) {
        return;
    }

    const u16 spin_angle = static_cast<u16>(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f);
    const f32 pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
    const f32 target_pulse = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.2f + 0.8f;
    const f32 ready_alpha = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.15f + 0.85f;

    EnableShadowMapRendering(0);

    // The four machine effects share the same animation playback convention.  End
    // their current instance animation once per draw, before evaluating a frame.
    nuhspecial_s *animated_special = &world->lev_objs[288].special;
    nuinstanim_s *animated_instance_animation = NuSpecialGetInstAnim(animated_special);
    f32 animated_end_frame = 0.0f;
    if (animated_instance_animation != NULL && animated_special->scene != NULL) {
        animated_end_frame =
            NuAnimEndFrameOld(animated_special->scene->instance_animation_data[animated_instance_animation->anim_ix]);
    }

    nuhspecial_s *effect_special_a = &world->lev_objs[285].special;
    nuinstanim_s *effect_instance_animation_a = NuSpecialGetInstAnim(effect_special_a);
    f32 effect_end_frame_a = 0.0f;
    if (effect_instance_animation_a != NULL && effect_special_a->scene != NULL) {
        effect_end_frame_a =
            NuAnimEndFrameOld(effect_special_a->scene->instance_animation_data[effect_instance_animation_a->anim_ix]);
    }

    nuhspecial_s *effect_special_b = &world->lev_objs[289].special;
    nuinstanim_s *effect_instance_animation_b = NuSpecialGetInstAnim(effect_special_b);
    f32 effect_end_frame_b = 0.0f;
    if (effect_instance_animation_b != NULL && effect_special_b->scene != NULL) {
        effect_end_frame_b =
            NuAnimEndFrameOld(effect_special_b->scene->instance_animation_data[effect_instance_animation_b->anim_ix]);
    }

    nuhspecial_s *effect_special_c = &world->lev_objs[284].special;
    nuinstanim_s *effect_instance_animation_c = NuSpecialGetInstAnim(effect_special_c);
    f32 effect_end_frame_c = 0.0f;
    if (effect_instance_animation_c != NULL && effect_special_c->scene != NULL) {
        effect_end_frame_c =
            NuAnimEndFrameOld(effect_special_c->scene->instance_animation_data[effect_instance_animation_c->anim_ix]);
    }

    HATMACHINESYS_s *system = world->hat_machine_sys;
    for (i32 index = 0; index < system->count; ++index) {
        HATMACHINE_s *machine = &system->machines[index];
        if ((machine->flags & HATMACHINE_FLAG_VISIBLE) == 0 && !TouchHacks::TouchControlsActive) {
            continue;
        }

        TouchHacks::TintStack tint;
        machine->flash_timer -= FRAMETIME;
        if (TouchHacks::ShouldFlash(machine->flash_timer)) {
            NUCOLOUR3 *flash_colour = TouchHacks::GetFlashColour();
            NuRndrLightingStateCurrent.ambient = *flash_colour;
            NuRndrSetAmbientLightPS(flash_colour);
        }

        f32 animation_frame = machine->state_elapsed;
        if (animation_frame < 0.0f) {
            animation_frame = 0.0f;
        }

        // The inactive model is still useful to touch controls.  The selected
        // model is drawn below after the animated machine body.
        if (world->lev_objs[283].active != 0) {
            NuSpecialDrawAt(&world->lev_objs[283].special, &machine->transform);
        }

        if ((machine->flags & HATMACHINE_FLAG_ENABLED) == 0) {
            if (world->lev_objs[287].active != 0) {
                NuSpecialDrawAt(&world->lev_objs[287].special, &machine->transform);
            }
        } else if (world->lev_objs[286].active != 0) {
            NuSpecialDrawAt(&world->lev_objs[286].special, &machine->transform);
        }

        if ((machine->flags & HATMACHINE_FLAG_HIDE_MACHINE) == 0 && machine->target_offset.y != 2000000.0f &&
            world->lev_objs[85].active != 0) {
            NUVEC target_position;
            Hat_GetAbsTargetPos(machine, &target_position);
            NUMTX target_matrix;
            NuMtxSetRotationY(&target_matrix, spin_angle);
            if (machine->terrain_roll != 0) {
                NuMtxRotateZ(&target_matrix, machine->terrain_roll);
            }
            if (machine->terrain_pitch != 0) {
                NuMtxRotateX(&target_matrix, machine->terrain_pitch);
            }
            NuMtxScaleU(&target_matrix, 0.8f);
            NuMtxTranslate(&target_matrix, &target_position);
            NuMtxPreScaleU(&target_matrix, machine->scale);
            const f32 target_alpha = machine->state_bit0 != 0 ? 0.0f : target_pulse;
            NuSpecialDrawAtAlpha(&world->lev_objs[85].special, &target_matrix, target_alpha);
        }

        // The animated machine body is evaluated in its own space and then
        // composed with the machine transform.
        if (animated_instance_animation != NULL) {
            if (machine->animation_state > 0) {
                machine->animation_time += FRAMETIME;
                if (machine->animation_state <= 3 && machine->animation_time >= animated_end_frame) {
                    machine->animation_state = 4;
                    machine->state_elapsed = 0.0f;
                    machine->state_duration = 2.0f;
                }
                animation_frame = machine->animation_time * animated_instance_animation->tfactor * 0.1f;
                if (animation_frame > animated_end_frame) {
                    animation_frame = animated_end_frame;
                }
            } else {
                machine->animation_time = 0.0f;
                if ((machine->flags & (HATMACHINE_FLAG_ANIMATING | HATMACHINE_FLAG_FINISHED |
                                       HATMACHINE_FLAG_ENABLED)) == HATMACHINE_FLAG_ENABLED) {
                    if (machine->idle_bounce_timer > 0.0f) {
                        machine->idle_bounce_timer -= FRAMETIME;
                        if (machine->idle_bounce_timer > 0.0f) {
                            const f32 idle_phase = machine->idle_bounce_timer / 0.15f * 50.0f + 16384.0f;
                            animation_frame =
                                (1.0f - NuFabs(NuTrigTable[(static_cast<i32>(idle_phase) >> 1) & 0x7fff])) * 1.5f +
                                16.0f;
                        }
                    } else if (qrand() <= 0x7ff) {
                        machine->idle_bounce_timer = 0.15f;
                    }
                }
            }

            NUMTX animated_matrix;
            EvalAnim(animated_special, animation_frame, &animated_matrix, 0);
            NuMtxMulVU0(&animated_matrix, &animated_matrix, &machine->transform);
            NuSpecialDrawAt(animated_special, &animated_matrix);
            if (world->lev_objs[machine->target_special_index].active != 0) {
                NuSpecialDrawAt(&world->lev_objs[machine->target_special_index].special, &animated_matrix);
            }
            if ((machine->flags & (HATMACHINE_FLAG_ANIMATING | HATMACHINE_FLAG_FINISHED | HATMACHINE_FLAG_ENABLED)) ==
                    HATMACHINE_FLAG_ENABLED &&
                world->lev_objs[41].active != 0) {
                NuSpecialDrawAtAlpha(&world->lev_objs[41].special, &animated_matrix, ready_alpha);
            }
        }

        // Draw the three optional animation effects in order.  Their generated
        // translations are carried forward for the hat's final placement.
        NUVEC draw_position = machine->position;
        NUMTX effect_matrix;
        if (effect_instance_animation_a != NULL) {
            f32 frame = machine->animation_time * effect_instance_animation_a->tfactor * 0.1f;
            if (frame > effect_end_frame_a)
                frame = effect_end_frame_a;
            EvalAnim(effect_special_a, frame, &effect_matrix, 0);
            NuMtxMulVU0(&effect_matrix, &effect_matrix, &machine->transform);
            NuSpecialDrawAt(effect_special_a, &effect_matrix);
            draw_position.x = effect_matrix.m30;
            draw_position.z = effect_matrix.m32;
        }
        if (effect_instance_animation_b != NULL) {
            f32 frame = machine->animation_time * effect_instance_animation_b->tfactor * 0.1f;
            if (frame > effect_end_frame_b)
                frame = effect_end_frame_b;
            EvalAnim(effect_special_b, frame, &effect_matrix, 0);
            NuMtxMulVU0(&effect_matrix, &effect_matrix, &machine->transform);
            NuSpecialDrawAt(effect_special_b, &effect_matrix);
            draw_position.x = effect_matrix.m30;
            draw_position.z = effect_matrix.m32;
        }
        if (effect_instance_animation_c != NULL) {
            f32 frame = machine->animation_time * effect_instance_animation_c->tfactor * 0.1f;
            if (frame > effect_end_frame_c)
                frame = effect_end_frame_c;
            EvalAnim(effect_special_c, frame, &effect_matrix, 0);
            NuMtxMulVU0(&effect_matrix, &effect_matrix, &machine->transform);
            NuSpecialDrawAt(effect_special_c, &effect_matrix);
            draw_position.x = effect_matrix.m30;
            draw_position.z = effect_matrix.m32;
        }

        if ((machine->flags & HATMACHINE_FLAG_ANIMATING) != 0 && machine->displayed_hat != 0) {
            const i32 hat_index = machine->displayed_hat + 249;
            if (machine->animation_time < 2.35f && world->lev_objs[hat_index].active != 0) {
                const NUVEC *volatile hat_offset = &HatMachine_HatOffset;
                NUVEC hat_position = *hat_offset;
                const f32 hat_phase = machine->hat_delay * 32768.0f + 16384.0f;
                const f32 hat_sine = NuTrigTable[(static_cast<i32>(hat_phase) >> 1) & 0x7fff];
                const f32 hat_scale = (1.0f - hat_sine) * 0.5f;
                hat_position.y += hat_scale * 0.1f;

                f32 scale = 1.0f;
                if (machine->animation_state <= 2) {
                    const f32 frame_fade = animation_frame / 50.0f;
                    scale = frame_fade <= 1.0f ? (1.0f - frame_fade) * 0.01f : 1.0f;
                }
                hat_position.y += NuTrigTable[static_cast<i32>(FRAMETIME * 32768.0f) >> 1 & 0x7fff] * scale;

                NuVecRotateY(&hat_position, &hat_position, machine->yaw);
                NuMtxSetRotationY(&effect_matrix, machine->yaw + 0x8000);
                NuMtxTranslate(&effect_matrix, &draw_position);
                NuMtxTranslate(&effect_matrix, &hat_position);
                NuMtxPreScaleU(&effect_matrix, hat_scale);
                NuSpecialDrawAt(&world->lev_objs[hat_index].special, &effect_matrix);
            }
        }
    }

    ResetShadowMapRendering();
}

static char *HatMachine_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return static_cast<HATMACHINE *>(gizmo->object)->name;
}

static i32 HatMachine_GetOutput(GIZMO *gizmo, i32, i32) {
    HATMACHINE *machine = static_cast<HATMACHINE *>(gizmo->object);
    return (static_cast<u8>(machine->flags) >> 1) & 1;
}

static char *HatMachine_GetOutputName(GIZMO *, i32 output_index) {
    return output_index == 0 ? const_cast<char *>("Finished") : NULL;
}

static i32 HatMachine_GetNumOutputs(GIZMO *) {
    return 1;
}

static void HatMachine_Activate(GIZMO *gizmo, i32 enabled) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    HATMACHINE *machine = static_cast<HATMACHINE *>(gizmo->object);
    if (enabled != 0) {
        machine->flags = static_cast<HATMACHINE_FLAGS>(machine->flags | HATMACHINE_FLAG_ENABLED);
        HatMachine_Reset(machine);
    } else {
        machine->flags = static_cast<HATMACHINE_FLAGS>(machine->flags & ~HATMACHINE_FLAG_ENABLED);
    }
}

static void HatMachine_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    HATMACHINE *machine = static_cast<HATMACHINE *>(gizmo->object);
    u8 flags = machine->flags;
    u8 previous_visibility = flags;
    previous_visibility >>= 2;
    const i32 was_visible = previous_visibility & 1;
    visible = visible != 0;
    flags = static_cast<u8>((flags & ~HATMACHINE_FLAG_VISIBLE) | (visible << 2));
    machine->flags = static_cast<HATMACHINE_FLAGS>(flags);

    if ((flags & HATMACHINE_FLAG_VISIBLE) != 0) {
        if (was_visible == 0) {
            machine->platform_id = NewPlatPickupInst(machine, HATMACHINE_PLATFORM_COLLISION);
            PlatInstRotate(machine->platform_id, 1);
        }
    } else if (was_visible != 0) {
        DeletePlatinst(machine->platform_id);
    }
}

static void *HatMachines_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(HATMACHINEPROGRESS));
}

static void HatMachines_ClearProgress(void *, void *progress_data) {
    HATMACHINEPROGRESS *progress = (HATMACHINEPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->enabled_mask = ~0u;
    progress->visible_mask = ~0u;
}

static void HatMachines_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    HATMACHINEPROGRESS *progress = static_cast<HATMACHINEPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->enabled_mask = ~0u;
    progress->visible_mask = ~0u;
    if (world == NULL || world->hat_machine_sys == NULL || world->hat_machine_sys->machines == NULL) {
        return;
    }

    HATMACHINESYS_s *system = world->hat_machine_sys;
    for (i32 index = 0; index < system->count && index < 32; ++index) {
        const u32 mask = 1u << index;
        HATMACHINE *machine = &system->machines[index];
        if ((machine->flags & HATMACHINE_FLAG_VISIBLE) == 0) {
            progress->visible_mask &= ~mask;
        }
        if ((machine->flags & HATMACHINE_FLAG_ENABLED) == 0) {
            progress->enabled_mask &= ~mask;
        }
    }
}

static void HatMachine_Reset(HATMACHINE_s *machine) {
    machine->player_position.y = 0.0f;
    machine->player_position.x = 0.0f;
    machine->player_position.z = 0.1441f;
    NuVecRotateY(&machine->player_position, &machine->player_position, machine->yaw + 0x8000);
    NuVecAdd(&machine->player_position, &machine->player_position, &machine->position);

    NUVEC target_position;
    Hat_GetAbsTargetPos(machine, &target_position);
    target_position.y = machine->position.y;

    machine->player_position.y = GameShadow(NULL, &machine->player_position, 1.0f, -1);
    const f32 target_y = GameShadow(NULL, &target_position, 1.0f, -1);
    if (target_y == 2000000.0f) {
        machine->target_offset.y = target_y;
    } else {
        machine->target_offset.y = target_y + 0.005f;
        FindAnglesZX(&ShadNorm, &machine->terrain_pitch, &machine->terrain_roll);
    }

    machine->animation_state = 0;
    machine->progress_state1 = 1;
    machine->progress_state0 = 1;
    machine->state_bit0 = 0;
    machine->state_bit1 = 0;
    machine->hat_refresh_timer = 0.0f;
    machine->animation_duration = 0.0f;
    machine->render_animation_time = 0.0f;

    if (machine->model_letter == 'r') {
        machine->model_special_index = 0x22;
    } else {
        machine->model_special_index = (machine->model_letter == 'o') + 0x20;
    }

    if (machine->configured_hat != 0) {
        machine->current_hat = machine->configured_hat;
    } else {
        machine->current_hat = static_cast<u8>(NuFloatRand(reinterpret_cast<NURAND *>(&GAMERAND)) * 4.0f) + 1;
    }

    NuMtxSetRotationY(&machine->matrix, machine->yaw);
    NuMtxTranslate(&machine->matrix, &machine->position);
}

static void HatMachines_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return;
    }

    HATMACHINESYS_s *system = world->hat_machine_sys;
    if (system == NULL || system->machines == NULL || system->count <= 0) {
        return;
    }

    HATMACHINEPROGRESS *progress = static_cast<HATMACHINEPROGRESS *>(progress_ptr);
    for (i32 index = 0; index < system->count; ++index) {
        HATMACHINE_s *machine = &system->machines[index];
        HatMachine_Reset(machine);

        if (index <= 31 && progress != NULL) {
            const u32 mask = 1u << index;
            machine->progress_state1 = (progress->visible_mask & mask) != 0;
            machine->progress_state0 = (progress->enabled_mask & mask) != 0;
        }
    }
}

static void *HatMachines_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->hat_machine_sys = NULL;
    if (world->current_level->max_levers == 0) {
        return NULL;
    }

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    world->hat_machine_sys = static_cast<HATMACHINESYS_s *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += sizeof(HATMACHINESYS_s);
    memset(world->hat_machine_sys, 0, sizeof(*world->hat_machine_sys));

    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 16);
    world->hat_machine_sys->machines = static_cast<HATMACHINE *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += world->current_level->max_hat_machines * sizeof(HATMACHINE);
    memset(world->hat_machine_sys->machines, 0, world->current_level->max_hat_machines * sizeof(HATMACHINE));
    return world->hat_machine_sys;
}

static i32 HatMachine_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }

    HATMACHINESYS_s *system = world->hat_machine_sys;
    if (system == NULL || system->count != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    system->count = EdFileReadInt();
    if (system->count > 0) {
        i32 index = 0;
        do {
            HATMACHINE *machine = &system->machines[index];
            EdFileRead(machine->name, EdFileReadInt());
            EdFileReadNuVec(&machine->position);
            machine->yaw = EdFileReadShort();
            machine->configured_hat = static_cast<u8>(EdFileReadChar());

            if (version <= 2) {
                machine->model_letter = 'r';
            } else {
                machine->model_letter = static_cast<char>(EdFileReadChar());
                if (version != 3) {
                    EdFileReadNuVec(&machine->target_offset);
                    machine->scale = EdFileReadFloat();
                    if (version != 4) {
                        const u8 hidden = static_cast<u8>(EdFileReadChar()) & 1;
                        machine->flags = static_cast<HATMACHINE_FLAGS>(
                            (machine->flags & ~HATMACHINE_FLAG_HIDE_MACHINE) | (hidden << 5));
                    }
                    machine->platform_id = -1;
                    ++index;
                    continue;
                }
            }

            machine->target_offset.x = 0.0f;
            machine->target_offset.y = 0.0f;
            machine->target_offset.z = -0.1441f;
            machine->scale = 1.0f;
            machine->platform_id = -1;
            ++index;
        } while (system->count > index);
    }
    return 1;
}

ADDGIZMOTYPE *HatMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "HatMachine";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0xc;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = HatMachine_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = HatMachine_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = HatMachine_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = HatMachine_Draw;
    addtype.fns.get_gizmo_name_fn = HatMachine_GetGizmoName;
    addtype.fns.get_output_fn = HatMachine_GetOutput;
    addtype.fns.get_output_name_fn = HatMachine_GetOutputName;
    addtype.fns.get_num_outputs_fn = HatMachine_GetNumOutputs;
    addtype.fns.activate_fn = HatMachine_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = HatMachine_SetVisibility;
    addtype.fns.allocate_progress_data_fn = HatMachines_AllocateProgressData;
    addtype.fns.clear_progress_fn = HatMachines_ClearProgress;
    addtype.fns.store_progress_fn = HatMachines_StoreProgress;
    addtype.fns.reset_fn = HatMachines_Reset;
    addtype.fns.reserve_buffer_space_fn = HatMachines_ReserveBufferSpace;
    addtype.fns.load_fn = HatMachine_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    hatmachine_gizmotype_id = type_id;

    return &addtype;
}
