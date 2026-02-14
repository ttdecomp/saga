#include "game/gizmos/giztorpmachine.h"

#include "decomp.h"

int giztorpmachine_gizmotype_id;

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

static void *GizTorps_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizTorp_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(int type_id) {
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
