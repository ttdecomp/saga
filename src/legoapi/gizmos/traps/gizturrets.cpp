#include "legoapi/gizmos/traps/gizturrets.h"

#include "decomp.h"
#include "legoapi/world/level.h"

i32 turret_gizmotype_id = -1;

static i32 GizTurrets_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    return world != NULL ? world->current_level->max_turrets : 0;
}

static void GizTurrets_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizTurrets_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoTurret_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZTURRET *>(gizmo->object)->name : NULL;
}

static i32 GizmoTurret_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoTurret_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoTurret_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void GizmoTurret_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoTurret_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoTurret_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoTurret_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTurrets_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizTurrets_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTurrets_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                              unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizTurrets_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *GizTurrets_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTurrets_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

static void GizTurrets_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizTurrets_AddLevelSfx(void *, void *, i32 *, i32 *, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTurret";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTurrets_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoTurret_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTurrets_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizTurrets_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizTurrets_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizTurrets_Update;
    addtype.fns.bolt_hit_fn = GizTurrets_BoltHit;
    addtype.fns.draw_fn = GizTurrets_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoTurret_GetGizmoName;
    addtype.fns.get_output_fn = GizmoTurret_GetOutput;
    addtype.fns.get_output_name_fn = GizmoTurret_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoTurret_GetNumOutputs;
    addtype.fns.activate_fn = GizmoTurret_Activate;
    addtype.fns.activate_rev_fn = GizmoTurret_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoTurret_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizTurrets_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizTurrets_ClearProgress;
    addtype.fns.store_progress_fn = GizTurrets_StoreProgress;
    addtype.fns.reset_fn = GizTurrets_Reset;
    addtype.fns.reserve_buffer_space_fn = GizTurrets_ReserveBufferSpace;
    addtype.fns.load_fn = GizTurrets_Load;
    addtype.fns.post_load_fn = GizTurrets_PostLoad;
    addtype.fns.add_level_sfx_fn = GizTurrets_AddLevelSfx;
    turret_gizmotype_id = type_id;

    return &addtype;
}
