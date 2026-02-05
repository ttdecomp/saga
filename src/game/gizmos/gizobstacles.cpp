#include "game/gizmos/gizobstacles.h"

#include "decomp.h"

int obstacle_gizmotype_id = -1;

static int GizObstacles_GetMaxGizmos(void *obstacle) {
    UNIMPLEMENTED();
}

static void GizObstacles_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
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
}

static int GizmoObstacle_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoObstacle_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoObstacle_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoObstacle_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoObstacle_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void GizmoObstacle_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoObstacle_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizObstacles_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static int *GizObstacles_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float,
                                           float, int, int, int) {
    UNIMPLEMENTED();
}

static int GizObstacles_BoltHit(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                                unsigned char *) {
    UNIMPLEMENTED();
}

static void GizObstacles_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
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

static int GizObstacles_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizObstacles_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_AddLevelSfx(void *, void *, int *, int *, int) {
    UNIMPLEMENTED();
}

void GizObstacles_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizObstacle";
    addtype.prefix = "";
    addtype.unknown1 = 0x70;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype.get_pos_fn = GizmoObstacle_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype.late_update_fn = GizObstacles_Update;
    addtype.bolt_hit_fn = GizObstacles_BoltHit;
    addtype.draw_fn = GizObstacles_Draw;
    addtype.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype.get_output_fn = GizmoObstacle_GetOutput;
    addtype.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype.activate_fn = GizmoObstacle_Activate;
    addtype.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype.clear_progress_fn = GizObstacles_ClearProgress;
    addtype.store_progress_fn = GizObstacles_StoreProgress;
    addtype.reset_fn = GizObstacles_Reset;
    addtype.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype.load_fn = GizObstacles_Load;
    addtype.post_load_fn = GizObstacles_PostLoad;
    addtype.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;
}
