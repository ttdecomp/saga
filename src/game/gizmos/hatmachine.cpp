#include "game/gizmos/hatmachine.h"

#include "decomp.h"

int hatmachine_gizmotype_id = -1;

static int HatMachine_GetMaxGizmos(void *hatmachine) {
    UNIMPLEMENTED();
}

static void HatMachine_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachine_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void HatMachine_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *HatMachine_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int HatMachine_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *HatMachine_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int HatMachine_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void HatMachine_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void HatMachine_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *HatMachines_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void HatMachines_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachines_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachines_Reset(void *, void *, void*) {
    UNIMPLEMENTED();
}

static int HatMachines_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int HatMachine_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* HatMachine_RegisterGizmo(int type_id) {
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
