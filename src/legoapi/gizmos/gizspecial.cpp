#include "legoapi/gizmos/gizspecial.h"

#include "decomp.h"

i32 gizspecial_gizmotype_id = -1;

static char *gizSpec_prefix = "Spec_";

static i32 GizSpecial_GetMaxGizmos(void *special) {
    UNIMPLEMENTED();
    return {};
}

static void GizSpecial_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizSpecial_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizSpecial_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizSpecial_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizSpecial_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizSpecial_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizSpecial_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void GizSpecial_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizSpecial_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizSpecial_UsingSpecial(GIZMO **, void *, i32, char *) {
    UNIMPLEMENTED();
}

static void *GizSpecial_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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
    return {};
}

ADDGIZMOTYPE *GizSpecial_RegisterGizmo(i32 type_id) {
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
