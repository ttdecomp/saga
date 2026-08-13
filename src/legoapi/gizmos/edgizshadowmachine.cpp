#include "legoapi/gizmos/edgizshadowmachine.h"

#include "decomp.h"

i32 edGizShadow_gizmotype_id;

static i32 edGizShadow_GetMaxGizmos(void *shadow) {
    UNIMPLEMENTED();
    return {};
}

static void edGizShadow_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static char *edGizShadow_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 edGizShadow_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *edGizShadow_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 edGizShadow_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *edGizShadow_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 edGizShadow_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ShadowEditor";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = edGizShadow_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = edGizShadow_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = edGizShadow_GetGizmoName;
    addtype.fns.get_output_fn = edGizShadow_GetOutput;
    addtype.fns.get_output_name_fn = edGizShadow_GetOutputName;
    addtype.fns.get_num_outputs_fn = edGizShadow_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = edGizShadow_ReserveBufferSpace;
    addtype.fns.load_fn = edGizShadow_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    edGizShadow_gizmotype_id = type_id;

    return &addtype;
}
