#include "game/gizmos/gizspecial.h"

#include "decomp.h"

int gizspecial_gizmotype_id = -1;

static char *gizSpec_prefix = "Spec_";

static int GizSpecial_GetMaxGizmos(void *special) {
    UNIMPLEMENTED();
}

static void GizSpecial_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizSpecial_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizSpecial_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizSpecial_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizSpecial_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizSpecial_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizSpecial_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void GizSpecial_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizSpecial_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizSpecial_UsingSpecial(GIZMO **, void *, int, char *) {
    UNIMPLEMENTED();
}

static void *GizSpecial_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizSpecial_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizSpecial_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizSpecial_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

void *GizSpecial_ReserveBuffer(void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *GizSpecial_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "NuSpecial";
    addtype.prefix = gizSpec_prefix;
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizSpecial_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizSpecial_GetPos;
    addtype.fns.using_special_fn = GizSpecial_UsingSpecial;
    addtype.fns.add_gizmos_fn = GizSpecial_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizSpecial_GetGizmoName;
    addtype.fns.get_output_fn = GizSpecial_GetOutput;
    addtype.fns.get_output_name_fn = GizSpecial_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizSpecial_GetNumOutputs;
    addtype.fns.activate_fn = GizSpecial_Activate;
    addtype.fns.activate_rev_fn = GizSpecial_ActivateRev;
    addtype.fns.set_visibility_fn = GizSpecial_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizSpecial_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizSpecial_ClearProgress;
    addtype.fns.store_progress_fn = GizSpecial_StoreProgress;
    addtype.fns.reset_fn = GizSpecial_Reset;
    addtype.fns.reserve_buffer_space_fn = GizSpecial_ReserveBuffer;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizspecial_gizmotype_id = type_id;

    return &addtype;
}
