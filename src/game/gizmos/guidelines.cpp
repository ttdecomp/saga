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

static void GuideLines_AllocateProgressData(VARIPTR *, VARIPTR *) {
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
    addtype.unknown1 = 8;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GuideLines_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GuideLines_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = GuideLines_Draw;
    addtype.get_gizmo_name_fn = GuideLine_GetGizmoName;
    addtype.get_output_fn = GuideLine_GetOutput;
    addtype.get_output_name_fn = GuideLine_GetOutputName;
    addtype.get_num_outputs_fn = GuideLine_GetNumOutputs;
    addtype.activate_fn = GuideLine_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = GuideLine_SetVisibility;
    addtype.allocate_progress_data_fn = GuideLines_AllocateProgressData;
    addtype.clear_progress_fn = GuideLines_ClearProgress;
    addtype.store_progress_fn = GuideLines_StoreProgress;
    addtype.reset_fn = GuideLines_Reset;
    addtype.reserve_buffer_space_fn = GuideLines_ReserveBufferSpace;
    addtype.load_fn = GuideLines_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;

    return &addtype;
}
