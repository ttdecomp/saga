#include "game/gizmos/gizmopickups.h"

#include "decomp.h"

int gizmopickup_typeid = -1;

static int GizmoPickups_GetMaxGizmos(void *pickup) {
    UNIMPLEMENTED();
}

static void GizmoPickups_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoPickup_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoPickup_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoPickup_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizmoPickup_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoPickup_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *GizmoPickups_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoPickups_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizmoPickups_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int GizmoPickups_Load(void *, void *) {
    UNIMPLEMENTED();
}

void GizmoPickups_PostLoad(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizmoPickup";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x100;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizmoPickups_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoPickup_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizmoPickups_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizmoPickups_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizmoPickups_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoPickup_GetGizmoName;
    addtype.fns.get_output_fn = GizmoPickup_GetOutput;
    addtype.fns.get_output_name_fn = GizmoPickup_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoPickup_GetNumOutputs;
    addtype.fns.activate_fn = GizmoPickup_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoPickup_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizmoPickups_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizmoPickups_ClearProgress;
    addtype.fns.store_progress_fn = GizmoPickups_StoreProgress;
    addtype.fns.reset_fn = GizmoPickups_Reset;
    addtype.fns.reserve_buffer_space_fn = GizmoPickups_ReserveBufferSpace;
    addtype.fns.load_fn = GizmoPickups_Load;
    addtype.fns.post_load_fn = GizmoPickups_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizmopickup_typeid = type_id;

    return &addtype;
}
