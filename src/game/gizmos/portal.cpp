#include "game/gizmos/portal.h"

#include "decomp.h"

int portal_gizmotype_id = -1;

static int Portal_GetMaxGizmos(void *portal) {
    UNIMPLEMENTED();
}

static void Portal_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *Portal_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Portal_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Portal_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Portal_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Portal_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Portal_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void Portals_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Portals_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Portals_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Portals_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Portal_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Portal";
    addtype.prefix = "portal_";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Portal_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Portal_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = Portal_GetGizmoName;
    addtype.get_output_fn = Portal_GetOutput;
    addtype.get_output_name_fn = Portal_GetOutputName;
    addtype.get_num_outputs_fn = Portal_GetNumOutputs;
    addtype.activate_fn = Portal_Activate;
    addtype.activate_rev_fn = Portal_ActivateRev;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = Portals_AllocateProgressData;
    addtype.clear_progress_fn = Portals_ClearProgress;
    addtype.store_progress_fn = Portals_StoreProgress;
    addtype.reset_fn = Portals_Reset;
    addtype.reserve_buffer_space_fn = NULL;
    addtype.load_fn = NULL;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    portal_gizmotype_id = type_id;

    return &addtype;
}
