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

static void GizSpecial_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

static int GizSpecial_ReserveBuffer(void *, int) {
    UNIMPLEMENTED();
}

void GizSpecial_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "NuSpecial";
    addtype.prefix = gizSpec_prefix;
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizSpecial_GetMaxGizmos;
    addtype.get_pos_fn = GizSpecial_GetPos;
    addtype.using_special_fn = GizSpecial_UsingSpecial;
    addtype.add_gizmos_fn = GizSpecial_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizSpecial_GetGizmoName;
    addtype.get_output_fn = GizSpecial_GetOutput;
    addtype.get_output_name_fn = GizSpecial_GetOutputName;
    addtype.get_num_outputs_fn = GizSpecial_GetNumOutputs;
    addtype.activate_fn = GizSpecial_Activate;
    addtype.activate_rev_fn = GizSpecial_ActivateRev;
    addtype.set_visibility_fn = GizSpecial_SetVisibility;
    addtype.allocate_progress_data_fn = GizSpecial_AllocateProgressData;
    addtype.clear_progress_fn = GizSpecial_ClearProgress;
    addtype.store_progress_fn = GizSpecial_StoreProgress;
    addtype.reset_fn = GizSpecial_Reset;
    addtype.reserve_buffer_space_fn = GizSpecial_ReserveBuffer;
    addtype.load_fn = NULL;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    gizspecial_gizmotype_id = type_id;
}
