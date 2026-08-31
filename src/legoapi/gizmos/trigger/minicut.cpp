#include "legoapi/gizmos/trigger/minicut.h"

#include "decomp.h"

static i32 GizMiniCut_GetMaxGizmos(void *minicut) {
    UNIMPLEMENTED();
    return {};
}

static void GizMiniCut_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizMiniCut_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizMiniCut_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizMiniCut_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizMiniCut_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

void GizMiniCut_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static NUVEC *GizMiniCut_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizMiniCut_UsingSpecial(GIZMO **, void *, i32, char *) {
    UNIMPLEMENTED();
    return 0;
}

void GizMiniCut_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizMiniCut_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

i32 GizMiniCut_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *MiniCut_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "MiniCut";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizMiniCut_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizMiniCut_GetPos;
    addtype.fns.using_special_fn = GizMiniCut_UsingSpecial;
    addtype.fns.add_gizmos_fn = GizMiniCut_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizMiniCut_GetGizmoName;
    addtype.fns.get_output_fn = GizMiniCut_GetOutput;
    addtype.fns.get_output_name_fn = GizMiniCut_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizMiniCut_GetNumOutputs;
    addtype.fns.activate_fn = GizMiniCut_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = GizMiniCut_Reset;
    addtype.fns.reserve_buffer_space_fn = GizMiniCut_ReserveBufferSpace;
    addtype.fns.load_fn = GizMiniCut_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
