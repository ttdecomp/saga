#include "game/gizmos/edgizshadowmachine.h"

#include "decomp.h"

int edGizShadow_gizmotype_id;

static int edGizShadow_GetMaxGizmos(void *shadow) {
    UNIMPLEMENTED();
}

static void edGizShadow_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *edGizShadow_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int edGizShadow_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *edGizShadow_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int edGizShadow_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *edGizShadow_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int edGizShadow_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *EdGizShadowMachine_RegisterGizmo(int type_id) {
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
