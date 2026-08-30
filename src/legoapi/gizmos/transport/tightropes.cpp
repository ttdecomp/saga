#include "legoapi/gizmos/transport/tightropes.h"

#include "decomp.h"

struct TIGHTROPEPROGRESS {
    i32 state[2];
};

static i32 TightRopes_GetMaxGizmos(void *tightrope) {
    UNIMPLEMENTED();
    return {};
}

static void TightRopes_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void TightRopes_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *TightRope_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 TightRope_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *TightRope_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 TightRope_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void TightRope_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void TightRope_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *TightRopes_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void TightRopes_ClearProgress(void *, void *progress_data) {
    TIGHTROPEPROGRESS *progress = (TIGHTROPEPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state[0] = -1;
    progress->state[1] = -1;
}

static void TightRopes_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void TightRopes_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *TightRopes_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 TightRopes_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *TightRopes_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "TightRope";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = TightRopes_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = TightRopes_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = TightRopes_Draw;
    addtype.fns.get_gizmo_name_fn = TightRope_GetGizmoName;
    addtype.fns.get_output_fn = TightRope_GetOutput;
    addtype.fns.get_output_name_fn = TightRope_GetOutputName;
    addtype.fns.get_num_outputs_fn = TightRope_GetNumOutputs;
    addtype.fns.activate_fn = TightRope_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = TightRope_SetVisibility;
    addtype.fns.allocate_progress_data_fn = TightRopes_AllocateProgressData;
    addtype.fns.clear_progress_fn = TightRopes_ClearProgress;
    addtype.fns.store_progress_fn = TightRopes_StoreProgress;
    addtype.fns.reset_fn = TightRopes_Reset;
    addtype.fns.reserve_buffer_space_fn = TightRopes_ReserveBufferSpace;
    addtype.fns.load_fn = TightRopes_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
