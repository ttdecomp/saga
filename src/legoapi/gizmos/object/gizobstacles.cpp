#include "legoapi/gizmos/object/gizobstacles.h"

#include "decomp.h"

i32 obstacle_gizmotype_id = -1;

static i32 GizObstacles_GetMaxGizmos(void *obstacle) {
    UNIMPLEMENTED();
    return {};
}

static void GizObstacles_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizObstacles_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoObstacle_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoObstacle_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoObstacle_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoObstacle_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoObstacle_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoObstacle_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoObstacle_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoObstacle_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizObstacles_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizObstacles_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float,
                                           float, i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizObstacles_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                                unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizObstacles_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void GizObstacles_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizObstacles_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizObstacles_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

static void GizObstacles_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_AddLevelSfx(void *, void *, i32 *, i32 *, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizObstacle";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoObstacle_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizObstacles_Update;
    addtype.fns.bolt_hit_fn = GizObstacles_BoltHit;
    addtype.fns.draw_fn = GizObstacles_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype.fns.get_output_fn = GizmoObstacle_GetOutput;
    addtype.fns.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype.fns.activate_fn = GizmoObstacle_Activate;
    addtype.fns.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizObstacles_ClearProgress;
    addtype.fns.store_progress_fn = GizObstacles_StoreProgress;
    addtype.fns.reset_fn = GizObstacles_Reset;
    addtype.fns.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype.fns.load_fn = GizObstacles_Load;
    addtype.fns.post_load_fn = GizObstacles_PostLoad;
    addtype.fns.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;

    return &addtype;
}
