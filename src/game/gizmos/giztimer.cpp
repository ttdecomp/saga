#include "game/gizmos/giztimer.h"

#include "decomp.h"

int giztimer_gizmotype_id = -1;

static int GizTimer_GetMaxGizmos(void *timer) {
    UNIMPLEMENTED();
}

static void GizTimer_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTimer_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizTimer_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizTimer_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizTimer_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizTimer_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizTimer_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizTimer_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizTimer_Load(void *, void *) {
    UNIMPLEMENTED();
}

void GizTimer_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTimer";
    addtype.prefix = "";
    addtype.unknown1 = 0;
    addtype.early_update_fn = GizTimer_Update;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizTimer_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizTimer_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizTimer_GetGizmoName;
    addtype.get_output_fn = GizTimer_GetOutput;
    addtype.get_output_name_fn = GizTimer_GetOutputName;
    addtype.get_num_outputs_fn = GizTimer_GetNumOutputs;
    addtype.activate_fn = GizTimer_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = NULL;
    addtype.reserve_buffer_space_fn = GizTimer_ReserveBufferSpace;
    addtype.load_fn = GizTimer_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    giztimer_gizmotype_id = type_id;
}
