#include "game/gizmos/securitydoors.h"

#include "decomp.h"

static int SecurityDoors_GetMaxGizmos(void *door) {
    UNIMPLEMENTED();
}

static void SecurityDoors_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void SecurityDoors_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void SecurityDoors_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *SecurityDoor_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int SecurityDoor_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *SecurityDoor_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int SecurityDoor_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void SecurityDoor_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void SecurityDoor_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int SecurityDoor_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void SecurityDoors_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void SecurityDoors_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void SecurityDoors_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void SecurityDoors_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int SecurityDoors_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int SecurityDoors_Load(void *, void *) {
    UNIMPLEMENTED();
}

void SecurityDoors_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "SecurityDoor";
    addtype.prefix = "";
    addtype.unknown1 = 0xc;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = SecurityDoors_GetMaxGizmos;
    addtype.get_pos_fn = SecurityDoor_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = SecurityDoors_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = SecurityDoors_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = SecurityDoors_Draw;
    addtype.get_gizmo_name_fn = SecurityDoor_GetGizmoName;
    addtype.get_output_fn = SecurityDoor_GetOutput;
    addtype.get_output_name_fn = SecurityDoor_GetOutputName;
    addtype.get_num_outputs_fn = SecurityDoor_GetNumOutputs;
    addtype.activate_fn = SecurityDoor_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = SecurityDoor_SetVisibility;
    addtype.allocate_progress_data_fn = SecurityDoors_AllocateProgressData;
    addtype.clear_progress_fn = SecurityDoors_ClearProgress;
    addtype.store_progress_fn = SecurityDoors_StoreProgress;
    addtype.reset_fn = SecurityDoors_Reset;
    addtype.reserve_buffer_space_fn = SecurityDoors_ReserveBufferSpace;
    addtype.load_fn = SecurityDoors_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
}
