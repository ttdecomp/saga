#include "game/gizmos/gizturrets.h"

#include "decomp.h"

int turret_gizmotype_id = -1;

static int GizTurrets_GetMaxGizmos(void *turret) {
    UNIMPLEMENTED();
}

static void GizTurrets_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizTurrets_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoTurret_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoTurret_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoTurret_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoTurret_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoTurret_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoTurret_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void GizmoTurret_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoTurret_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizTurrets_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static int *GizTurrets_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         int, int, int) {
    UNIMPLEMENTED();
}

static int GizTurrets_BoltHit(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                              unsigned char *) {
    UNIMPLEMENTED();
}

static void GizTurrets_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizTurrets_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizTurrets_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizTurrets_Load(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_AddLevelSfx(void *, void *, int *, int *, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* GizTurrets_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTurret";
    addtype.prefix = "";
    addtype.unknown1 = 0x70;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizTurrets_GetMaxGizmos;
    addtype.get_pos_fn = GizmoTurret_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizTurrets_AddGizmos;
    addtype.bolt_hit_plat_fn = GizTurrets_BoltHitPlat;
    addtype.get_best_bolt_target_fn = GizTurrets_GetBestBoltTarget;
    addtype.late_update_fn = GizTurrets_Update;
    addtype.bolt_hit_fn = GizTurrets_BoltHit;
    addtype.draw_fn = GizTurrets_Draw;
    addtype.get_gizmo_name_fn = GizmoTurret_GetGizmoName;
    addtype.get_output_fn = GizmoTurret_GetOutput;
    addtype.get_output_name_fn = GizmoTurret_GetOutputName;
    addtype.get_num_outputs_fn = GizmoTurret_GetNumOutputs;
    addtype.activate_fn = GizmoTurret_Activate;
    addtype.activate_rev_fn = GizmoTurret_ActivateRev;
    addtype.set_visibility_fn = GizmoTurret_SetVisibility;
    addtype.allocate_progress_data_fn = GizTurrets_AllocateProgressData;
    addtype.clear_progress_fn = GizTurrets_ClearProgress;
    addtype.store_progress_fn = GizTurrets_StoreProgress;
    addtype.reset_fn = GizTurrets_Reset;
    addtype.reserve_buffer_space_fn = GizTurrets_ReserveBufferSpace;
    addtype.load_fn = GizTurrets_Load;
    addtype.post_load_fn = GizTurrets_PostLoad;
    addtype.add_level_sfx_fn = GizTurrets_AddLevelSfx;
    turret_gizmotype_id = type_id;

    return &addtype;
}
