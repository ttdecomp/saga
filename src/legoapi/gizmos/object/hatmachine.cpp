#include "legoapi/gizmos/object/hatmachine.h"

#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nurand.h"

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
    UNIMPLEMENTED();
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

static void HatMachines_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *HatMachines_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 HatMachine_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
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
