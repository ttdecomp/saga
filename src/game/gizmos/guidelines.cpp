#include "game/gizmos/guidelines.h"

#include "decomp.h"

static int GuideLines_GetMaxGizmos(void *guideline) {
    UNIMPLEMENTED();
}

static void GuideLines_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GuideLines_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GuideLine_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GuideLine_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GuideLine_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GuideLine_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GuideLine_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GuideLine_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *GuideLines_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GuideLines_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GuideLines_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GuideLines_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GuideLines_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GuideLines_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* GuideLines_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GuideLine";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GuideLines_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GuideLines_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GuideLines_Draw;
    addtype.fns.get_gizmo_name_fn = GuideLine_GetGizmoName;
    addtype.fns.get_output_fn = GuideLine_GetOutput;
    addtype.fns.get_output_name_fn = GuideLine_GetOutputName;
    addtype.fns.get_num_outputs_fn = GuideLine_GetNumOutputs;
    addtype.fns.activate_fn = GuideLine_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GuideLine_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GuideLines_AllocateProgressData;
    addtype.fns.clear_progress_fn = GuideLines_ClearProgress;
    addtype.fns.store_progress_fn = GuideLines_StoreProgress;
    addtype.fns.reset_fn = GuideLines_Reset;
    addtype.fns.reserve_buffer_space_fn = GuideLines_ReserveBufferSpace;
    addtype.fns.load_fn = GuideLines_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
