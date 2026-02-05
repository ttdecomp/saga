#include "game/gizmos/edgizshadowmachine.h"

#include "decomp.h"

int edGizShadow_gizmotype_id = -1;

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

static int edGizShadow_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int edGizShadow_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* EdGizShadowMachine_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ShadowEditor";
    addtype.prefix = "";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = edGizShadow_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = edGizShadow_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = edGizShadow_GetGizmoName;
    addtype.get_output_fn = edGizShadow_GetOutput;
    addtype.get_output_name_fn = edGizShadow_GetOutputName;
    addtype.get_num_outputs_fn = edGizShadow_GetNumOutputs;
    addtype.activate_fn = NULL;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = NULL;
    addtype.reserve_buffer_space_fn = edGizShadow_ReserveBufferSpace;
    addtype.load_fn = edGizShadow_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    edGizShadow_gizmotype_id = type_id;

    return &addtype;
}
