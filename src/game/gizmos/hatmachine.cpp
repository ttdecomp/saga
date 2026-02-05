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

static void HatMachines_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void HatMachines_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachines_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void HatMachines_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int HatMachines_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int HatMachine_Load(void *, void *) {
    UNIMPLEMENTED();
}

void HatMachine_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "HatMachine";
    addtype.prefix = "";
    addtype.unknown1 = 0xc;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = HatMachine_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = HatMachine_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = HatMachine_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = HatMachine_Draw;
    addtype.get_gizmo_name_fn = HatMachine_GetGizmoName;
    addtype.get_output_fn = HatMachine_GetOutput;
    addtype.get_output_name_fn = HatMachine_GetOutputName;
    addtype.get_num_outputs_fn = HatMachine_GetNumOutputs;
    addtype.activate_fn = HatMachine_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = HatMachine_SetVisibility;
    addtype.allocate_progress_data_fn = HatMachines_AllocateProgressData;
    addtype.clear_progress_fn = HatMachines_ClearProgress;
    addtype.store_progress_fn = HatMachines_StoreProgress;
    addtype.reset_fn = HatMachines_Reset;
    addtype.reserve_buffer_space_fn = HatMachines_ReserveBufferSpace;
    addtype.load_fn = HatMachine_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    hatmachine_gizmotype_id = type_id;
}
