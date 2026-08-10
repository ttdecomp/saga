#include "legoapi/gizmos/gizforce.h"

#include "decomp.h"

int force_gizmotype_id = -1;

typedef struct GIZFORCEPROGRESS_s {
    u32 active[8];
    u32 used[20];
    u32 available[16];
} GIZFORCEPROGRESS;

static int GizForces_GetMaxGizmos(void *force) {
    UNIMPLEMENTED();
}

static void GizForces_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizForces_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoForce_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoForce_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoForce_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoForce_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoForce_Activate(GIZMO *gizmo, int active) {
    if (gizmo == NULL) {
        return;
    }

    GIZFORCE *force = (GIZFORCE *)gizmo->object;
    if (active == 0) {
        force->flags_0a8 &= ~1;
        return;
    }

    if ((force->flags_0a8 & 1) == 0) {
        GameAnimSet_JumpToStart(force->anim_set);
    }

    force->flags_0a8 |= 1;
    force->flags_0aa &= ~1;
    force->flags_0a8 &= ~0x50;
    force->unknown_048 = 0;
    force->unknown_050 = 0;
    if (force->unknown_078 & 8) {
        force->flags_0a9 &= ~2;
    }
    force->flags_0a9 &= ~0x78;
    GameAnimSet_EvaluateState(force->anim_set);
    force->radius = 1.0f;
    force->centre = force->position;
    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->centre, &force->radius, 2, 1, 1);
}

static int GizmoForce_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void GizmoForce_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoForce_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizForces_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static int *GizForces_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                        int, int, int) {
    UNIMPLEMENTED();
}

static int GizForces_BoltHit(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                             unsigned char *) {
    UNIMPLEMENTED();
}

static void *GizForces_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizForces_ClearProgress(void *, void *progress_data) {
    GIZFORCEPROGRESS *progress = (GIZFORCEPROGRESS *)progress_data;

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
    progress->available[0] = ~0u;
    progress->available[1] = ~0u;
    progress->available[2] = ~0u;
    progress->available[3] = ~0u;
    progress->available[4] = ~0u;
    progress->available[5] = ~0u;
    progress->available[6] = ~0u;
    progress->available[7] = ~0u;
    progress->available[8] = ~0u;
    progress->available[9] = ~0u;
    progress->available[10] = ~0u;
    progress->available[11] = ~0u;
    progress->available[12] = ~0u;
    progress->available[13] = ~0u;
    progress->available[14] = ~0u;
    progress->available[15] = ~0u;
}

static void GizForces_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizForces_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizForces_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_AddLevelSfx(void *, void *, int *, int *, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizForce_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizForce";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0xb0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizForces_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoForce_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizForces_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizForces_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizForces_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizForces_Update;
    addtype.fns.bolt_hit_fn = GizForces_BoltHit;
    addtype.fns.draw_fn = GizForces_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoForce_GetGizmoName;
    addtype.fns.get_output_fn = GizmoForce_GetOutput;
    addtype.fns.get_output_name_fn = GizmoForce_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoForce_GetNumOutputs;
    addtype.fns.activate_fn = GizmoForce_Activate;
    addtype.fns.activate_rev_fn = GizmoForce_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoForce_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizForces_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizForces_ClearProgress;
    addtype.fns.store_progress_fn = GizForces_StoreProgress;
    addtype.fns.reset_fn = GizForces_Reset;
    addtype.fns.reserve_buffer_space_fn = GizForces_ReserveBufferSpace;
    addtype.fns.load_fn = GizForces_Load;
    addtype.fns.post_load_fn = GizForces_PostLoad;
    addtype.fns.add_level_sfx_fn = GizForces_AddLevelSfx;
    force_gizmotype_id = type_id;

    return &addtype;
}
