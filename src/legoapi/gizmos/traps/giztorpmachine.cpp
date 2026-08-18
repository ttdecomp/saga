#include "legoapi/gizmos/traps/giztorpmachine.h"

#include "decomp.h"

i32 giztorpmachine_gizmotype_id;

static i32 GizTorp_GetMaxGizmos(void *torp) {
    UNIMPLEMENTED();
    return {};
}

static void GizTorp_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTorp_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizTorp_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizTorp_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTorp_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizTorp_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTorp_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizTorp_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizTorp_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizTorps_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizTorps_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTorp_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Torp Machine";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTorp_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTorp_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizTorp_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizTorp_Draw;
    addtype.fns.get_gizmo_name_fn = GizTorp_GetGizmoName;
    addtype.fns.get_output_fn = GizTorp_GetOutput;
    addtype.fns.get_output_name_fn = GizTorp_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizTorp_GetNumOutputs;
    addtype.fns.activate_fn = GizTorp_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizTorp_SetVisibility;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = GizTorps_Reset;
    addtype.fns.reserve_buffer_space_fn = GizTorps_ReserveBufferSpace;
    addtype.fns.load_fn = GizTorp_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    giztorpmachine_gizmotype_id = type_id;

    return &addtype;
}
