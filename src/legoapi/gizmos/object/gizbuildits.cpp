#include "legoapi/gizmos/object/gizbuildits.h"

#include "decomp.h"

i32 gizbuildit_gizmotype_id = -1;

static i32 GizBuildIts_GetMaxGizmos(void *buildit) {
    UNIMPLEMENTED();
    return {};
}

static void GizBuildIts_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
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
    return {};
}

static i32 GizmoBuildit_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoBuildit_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoBuildit_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoBuildit_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizmoBuildit_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoBuildit_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *GizBuildIts_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *GizBuildIts_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizBuildIts_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

static void GizBuildIts_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizBuildit";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x60;
    addtype.fns.early_update_fn = GizBuildIts_EarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBuildIts_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBuildit_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBuildIts_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBuildIts_LateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizBuildIts_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoBuildit_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBuildit_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBuildit_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBuildit_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBuildit_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBuildit_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBuildIts_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBuildIts_ClearProgress;
    addtype.fns.store_progress_fn = GizBuildIts_StoreProgress;
    addtype.fns.reset_fn = GizBuildIts_Reset;
    addtype.fns.reserve_buffer_space_fn = GizBuildIts_ReserveBufferSpace;
    addtype.fns.load_fn = GizBuildIts_Load;
    addtype.fns.post_load_fn = GizBuildIts_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizbuildit_gizmotype_id = type_id;

    return &addtype;
}
