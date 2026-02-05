#include "game/gizmos/tightropes.h"

#include "decomp.h"

static int TightRopes_GetMaxGizmos(void *tightrope) {
    UNIMPLEMENTED();
}

static void TightRopes_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void TightRopes_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *TightRope_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int TightRope_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *TightRope_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int TightRope_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void TightRope_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void TightRope_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void TightRopes_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void TightRopes_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void TightRopes_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void TightRopes_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int TightRopes_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int TightRopes_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* TightRopes_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "TightRope";
    addtype.prefix = "";
    addtype.unknown1 = 8;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = TightRopes_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = TightRopes_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = TightRopes_Draw;
    addtype.get_gizmo_name_fn = TightRope_GetGizmoName;
    addtype.get_output_fn = TightRope_GetOutput;
    addtype.get_output_name_fn = TightRope_GetOutputName;
    addtype.get_num_outputs_fn = TightRope_GetNumOutputs;
    addtype.activate_fn = TightRope_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = TightRope_SetVisibility;
    addtype.allocate_progress_data_fn = TightRopes_AllocateProgressData;
    addtype.clear_progress_fn = TightRopes_ClearProgress;
    addtype.store_progress_fn = TightRopes_StoreProgress;
    addtype.reset_fn = TightRopes_Reset;
    addtype.reserve_buffer_space_fn = TightRopes_ReserveBufferSpace;
    addtype.load_fn = TightRopes_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;

    return &addtype;
}
