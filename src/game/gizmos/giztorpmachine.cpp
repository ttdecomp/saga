#include "game/gizmos/giztorpmachine.h"

#include "decomp.h"

int giztorpmachine_gizmotype_id = -1;

static int GizTorp_GetMaxGizmos(void *torp) {
    UNIMPLEMENTED();
}

static void GizTorp_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
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
}

static int GizTorp_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizTorp_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizTorp_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizTorp_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizTorp_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizTorps_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizTorps_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizTorp_Load(void *, void *) {
    UNIMPLEMENTED();
}

void GizTorpMachine_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Torp Machine";
    addtype.prefix = "";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizTorp_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizTorp_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = GizTorp_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = GizTorp_Draw;
    addtype.get_gizmo_name_fn = GizTorp_GetGizmoName;
    addtype.get_output_fn = GizTorp_GetOutput;
    addtype.get_output_name_fn = GizTorp_GetOutputName;
    addtype.get_num_outputs_fn = GizTorp_GetNumOutputs;
    addtype.activate_fn = GizTorp_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = GizTorp_SetVisibility;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = GizTorps_Reset;
    addtype.reserve_buffer_space_fn = GizTorps_ReserveBufferSpace;
    addtype.load_fn = GizTorp_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    giztorpmachine_gizmotype_id = type_id;
}
