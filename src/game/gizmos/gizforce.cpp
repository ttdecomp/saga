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

static void GizForces_AllocateProgressData(VARIPTR *, VARIPTR *) {
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
    addtype.unknown1 = 0xb0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizForces_GetMaxGizmos;
    addtype.get_pos_fn = GizmoForce_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizForces_AddGizmos;
    addtype.bolt_hit_plat_fn = GizForces_BoltHitPlat;
    addtype.get_best_bolt_target_fn = GizForces_GetBestBoltTarget;
    addtype.late_update_fn = GizForces_Update;
    addtype.bolt_hit_fn = GizForces_BoltHit;
    addtype.draw_fn = GizForces_Draw;
    addtype.get_gizmo_name_fn = GizmoForce_GetGizmoName;
    addtype.get_output_fn = GizmoForce_GetOutput;
    addtype.get_output_name_fn = GizmoForce_GetOutputName;
    addtype.get_num_outputs_fn = GizmoForce_GetNumOutputs;
    addtype.activate_fn = GizmoForce_Activate;
    addtype.activate_rev_fn = GizmoForce_ActivateRev;
    addtype.set_visibility_fn = GizmoForce_SetVisibility;
    addtype.allocate_progress_data_fn = GizForces_AllocateProgressData;
    addtype.clear_progress_fn = GizForces_ClearProgress;
    addtype.store_progress_fn = GizForces_StoreProgress;
    addtype.reset_fn = GizForces_Reset;
    addtype.reserve_buffer_space_fn = GizForces_ReserveBufferSpace;
    addtype.load_fn = GizForces_Load;
    addtype.post_load_fn = GizForces_PostLoad;
    addtype.add_level_sfx_fn = GizForces_AddLevelSfx;
    force_gizmotype_id = type_id;

    return &addtype;
}
