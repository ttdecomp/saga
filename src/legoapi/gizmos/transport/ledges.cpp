#include "legoapi/gizmos/transport/ledges.h"

#include "decomp.h"

static i32 Ledges_GetMaxGizmos(void *ledge) {
    UNIMPLEMENTED();
    return {};
}

static void Ledges_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Ledges_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Ledge_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 Ledge_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Ledge_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Ledge_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void Ledge_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Ledge_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *Ledges_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void Ledges_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Ledges_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Ledges_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Ledges_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Ledges_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Ledges_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Ledge";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x20;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Ledges_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Ledges_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Ledges_Draw;
    addtype.fns.get_gizmo_name_fn = Ledge_GetGizmoName;
    addtype.fns.get_output_fn = Ledge_GetOutput;
    addtype.fns.get_output_name_fn = Ledge_GetOutputName;
    addtype.fns.get_num_outputs_fn = Ledge_GetNumOutputs;
    addtype.fns.activate_fn = Ledge_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Ledge_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Ledges_AllocateProgressData;
    addtype.fns.clear_progress_fn = Ledges_ClearProgress;
    addtype.fns.store_progress_fn = Ledges_StoreProgress;
    addtype.fns.reset_fn = Ledges_Reset;
    addtype.fns.reserve_buffer_space_fn = Ledges_ReserveBufferSpace;
    addtype.fns.load_fn = Ledges_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
