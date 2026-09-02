#include "legoapi/gizmos/object/hatmachine.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/level.h"

#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void Hat_GetAbsTargetPos(HATMACHINE_s *machine, NUVEC *target_position);
void FindAnglesZX(NUVEC *vector, u16 *pitch, u16 *yaw);
extern "C" f32 NuFloatRand(i32 *random);

struct HATMACHINEPROGRESS {
    i32 preserved_state;
    u32 state[2];
};

i32 hatmachine_gizmotype_id = -1;

static i32 HatMachine_GetMaxGizmos(void *hatmachine) {
    WORLDINFO *world = static_cast<WORLDINFO *>(hatmachine);
    return world != NULL ? world->current_level->max_hat_machines : 0;
}

static void HatMachine_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachine_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void HatMachine_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *HatMachine_GetGizmoName(GIZMO *gizmo) {
    HATMACHINE_s *machine = gizmo != NULL ? static_cast<HATMACHINE_s *>(gizmo->object) : NULL;
    return machine != NULL ? machine->name : NULL;
}

static i32 HatMachine_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *HatMachine_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 HatMachine_GetNumOutputs(GIZMO *) {
    return 1;
}

static void HatMachine_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void HatMachine_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *HatMachines_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 12);
}

static void HatMachines_ClearProgress(void *, void *progress_data) {
    HATMACHINEPROGRESS *progress = (HATMACHINEPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state[0] = -1;
    progress->state[1] = -1;
}

static void HatMachines_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static __used__ void HatMachine_Reset(HATMACHINE_s *machine) {
    machine->player_position.y = 0.0f;
    machine->player_position.x = 0.0f;
    machine->player_position.z = 0.1441f;
    NuVecRotateY(&machine->player_position, &machine->player_position, machine->yaw + 0x8000);
    NuVecAdd(&machine->player_position, &machine->player_position, &machine->position);

    NUVEC target_position;
    Hat_GetAbsTargetPos(machine, &target_position);
    target_position.y = machine->position.y;

    machine->player_position.y = GameShadow(NULL, &machine->player_position, 1.0f, -1);
    f32 target_y = GameShadow(NULL, &target_position, 1.0f, -1);
    if (target_y == 2000000.0f) {
        machine->target_offset.y = target_y;
    } else {
        machine->target_offset.y = target_y + 0.005f;
        FindAnglesZX(&GameTimer.elapsed_components, &machine->terrain_pitch, &machine->terrain_roll);
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
        machine->current_hat = static_cast<u8>(NuFloatRand(&GAMERAND) * 4.0f) + 1;
    }

    NuMtxSetRotationY(&machine->matrix, machine->yaw);
    NuMtxTranslate(&machine->matrix, &machine->position);
}

static void HatMachines_Reset(void *world_info, void *, void *progress_data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    if (world == NULL) {
        return;
    }

    HATMACHINESYS_s *system = world->hat_sys;
    if (system == NULL || system->machines == NULL || system->count <= 0) {
        return;
    }

    for (i32 i = 0; i < system->count; i++) {
        HATMACHINE *machine = &system->machines[i];
        HatMachine_Reset(machine);

        if (i <= 31 && progress_data != NULL) {
            u32 mask = 1u << i;
            machine->progress_state1 = (static_cast<HATMACHINEPROGRESS *>(progress_data)->state[1] & mask) != 0;
            machine->progress_state0 = (static_cast<HATMACHINEPROGRESS *>(progress_data)->state[0] & mask) != 0;
        }
    }
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
