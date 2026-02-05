#include "game/gizmos/gizbuildits.h"

#include "decomp.h"

int gizbuildit_gizmotype_id = -1;

static int GizBuildIts_GetMaxGizmos(void *buildit) {
    UNIMPLEMENTED();
}

static void GizBuildIts_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_EarlyUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizBuildIts_LateUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizBuildIts_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoBuildit_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoBuildit_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoBuildit_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizmoBuildit_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoBuildit_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizBuildIts_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizBuildIts_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizBuildIts_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBuildIts_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

void GizBuildIts_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizBuildit";
    addtype.prefix = "";
    addtype.unknown1 = 0x60;
    addtype.early_update_fn = GizBuildIts_EarlyUpdate;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizBuildIts_GetMaxGizmos;
    addtype.get_pos_fn = GizmoBuildit_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizBuildIts_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = GizBuildIts_LateUpdate;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = GizBuildIts_Draw;
    addtype.get_gizmo_name_fn = GizmoBuildit_GetGizmoName;
    addtype.get_output_fn = GizmoBuildit_GetOutput;
    addtype.get_output_name_fn = GizmoBuildit_GetOutputName;
    addtype.get_num_outputs_fn = GizmoBuildit_GetNumOutputs;
    addtype.activate_fn = GizmoBuildit_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = GizmoBuildit_SetVisibility;
    addtype.allocate_progress_data_fn = GizBuildIts_AllocateProgressData;
    addtype.clear_progress_fn = GizBuildIts_ClearProgress;
    addtype.store_progress_fn = GizBuildIts_StoreProgress;
    addtype.reset_fn = GizBuildIts_Reset;
    addtype.reserve_buffer_space_fn = GizBuildIts_ReserveBufferSpace;
    addtype.load_fn = GizBuildIts_Load;
    addtype.post_load_fn = GizBuildIts_PostLoad;
    addtype.add_level_sfx_fn = NULL;
    gizbuildit_gizmotype_id = type_id;
}
