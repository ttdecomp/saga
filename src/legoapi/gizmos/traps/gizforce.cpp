#include "legoapi/gizmos/traps/gizforce.h"

#include "decomp.h"
#include "legoapi/world/level.h"

i32 force_gizmotype_id = -1;

static i32 GizForces_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    return world != NULL ? world->current_level->max_force : 0;
}

static void GizForces_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizForces_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoForce_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZFORCE *>(gizmo->object)->name : NULL;
}

static i32 GizmoForce_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoForce_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoForce_GetNumOutputs(GIZMO *gizmo) {
    return 8;
}

static void GizmoForce_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoForce_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoForce_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoForce_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizForces_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizForces_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                        i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizForces_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                             unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizForces_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *GizForces_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizForces_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

static void GizForces_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

static void GizForces_AddLevelSfx(void *, void *, i32 *, i32 *, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizForce_RegisterGizmo(i32 type_id) {
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
