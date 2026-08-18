#include "legoapi/gizmos/door/securitydoors.h"

#include "decomp.h"

static i32 SecurityDoors_GetMaxGizmos(void *door) {
    UNIMPLEMENTED();
    return {};
}

static void SecurityDoors_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
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
    return {};
}

static i32 SecurityDoor_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *SecurityDoor_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 SecurityDoor_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void SecurityDoor_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void SecurityDoor_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 SecurityDoor_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *SecurityDoors_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *SecurityDoors_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 SecurityDoors_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "SecurityDoor";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0xc;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = SecurityDoors_GetMaxGizmos;
    addtype.fns.get_pos_fn = SecurityDoor_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = SecurityDoors_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = SecurityDoors_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = SecurityDoors_Draw;
    addtype.fns.get_gizmo_name_fn = SecurityDoor_GetGizmoName;
    addtype.fns.get_output_fn = SecurityDoor_GetOutput;
    addtype.fns.get_output_name_fn = SecurityDoor_GetOutputName;
    addtype.fns.get_num_outputs_fn = SecurityDoor_GetNumOutputs;
    addtype.fns.activate_fn = SecurityDoor_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = SecurityDoor_SetVisibility;
    addtype.fns.allocate_progress_data_fn = SecurityDoors_AllocateProgressData;
    addtype.fns.clear_progress_fn = SecurityDoors_ClearProgress;
    addtype.fns.store_progress_fn = SecurityDoors_StoreProgress;
    addtype.fns.reset_fn = SecurityDoors_Reset;
    addtype.fns.reserve_buffer_space_fn = SecurityDoors_ReserveBufferSpace;
    addtype.fns.load_fn = SecurityDoors_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
