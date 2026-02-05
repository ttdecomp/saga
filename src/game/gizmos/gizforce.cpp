#include "game/gizmos/gizforce.h"

#include "decomp.h"

int force_gizmotype_id = -1;

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

static void GizmoForce_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
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

static void GizForces_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizForces_ReserveBufferSpace(void *, int) {
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

ADDGIZMOTYPE* GizForce_RegisterGizmo(int type_id) {
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
