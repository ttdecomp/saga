#include "legoapi/gizmos/gizobstacles.h"

#include "decomp.h"

int obstacle_gizmotype_id = -1;

typedef struct GIZOBSTACLEPROGRESS_s {
    u32 active[8];
    u32 used[20];
} GIZOBSTACLEPROGRESS;

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

static void GizmoObstacle_Activate(GIZMO *gizmo, int active) {
    if (gizmo == NULL) {
        return;
    }

    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if (active == 0) {
        obstacle->flags_098 &= ~1;
        obstacle->unknown_09c = 0;
        return;
    }

    if ((obstacle->flags_098 & 1) == 0) {
        GameAnimSet_JumpToStart(obstacle->anim_set);
    }

    obstacle->flags_098 |= 1;
    obstacle->unknown_09c = 0;
    obstacle->flags_0a0 &= ~0x80;
    obstacle->flags_099 &= ~1;
    GameAnimSet_EvaluateState(obstacle->anim_set);
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

static void *GizObstacles_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizObstacles_ClearProgress(void *, void *progress_data) {
    GIZOBSTACLEPROGRESS *progress = (GIZOBSTACLEPROGRESS *)progress_data;

    if (progress == NULL) {
        return;
    }

    progress->active[0] = ~0u;
    progress->active[1] = ~0u;
    progress->active[2] = ~0u;
    progress->active[3] = ~0u;
    progress->active[4] = ~0u;
    progress->active[5] = ~0u;
    progress->active[6] = ~0u;
    progress->active[7] = ~0u;
    progress->used[0] = 0;
    progress->used[1] = 0;
    progress->used[2] = 0;
    progress->used[3] = 0;
    progress->used[4] = 0;
    progress->used[5] = 0;
    progress->used[6] = 0;
    progress->used[7] = 0;
    progress->used[8] = 0;
    progress->used[9] = 0;
    progress->used[10] = 0;
    progress->used[11] = 0;
    progress->used[12] = 0;
    progress->used[13] = 0;
    progress->used[14] = 0;
    progress->used[15] = 0;
    progress->used[16] = 0;
    progress->used[17] = 0;
    progress->used[18] = 0;
    progress->used[19] = 0;
}

static void GizObstacles_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizObstacles_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizObstacles_ReserveBufferSpace(void *) {
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

ADDGIZMOTYPE *GizObstacles_RegisterGizmo(int type_id) {
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
