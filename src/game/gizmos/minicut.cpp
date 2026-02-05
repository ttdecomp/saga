#include "game/gizmos/minicut.h"

#include "decomp.h"

static int GizMiniCut_GetMaxGizmos(void *minicut) {
    UNIMPLEMENTED();
}

static void GizMiniCut_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizMiniCut_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizMiniCut_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizMiniCut_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizMiniCut_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizMiniCut_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizMiniCut_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizMiniCut_UsingSpecial(GIZMO **, void *, int, char *) {
    UNIMPLEMENTED();
}

static void GizMiniCut_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizMiniCut_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizMiniCut_Load(void *, void *) {
    UNIMPLEMENTED();
}

void MiniCut_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "MiniCut";
    addtype.prefix = "";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizMiniCut_GetMaxGizmos;
    addtype.get_pos_fn = GizMiniCut_GetPos;
    addtype.using_special_fn = GizMiniCut_UsingSpecial;
    addtype.add_gizmos_fn = GizMiniCut_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizMiniCut_GetGizmoName;
    addtype.get_output_fn = GizMiniCut_GetOutput;
    addtype.get_output_name_fn = GizMiniCut_GetOutputName;
    addtype.get_num_outputs_fn = GizMiniCut_GetNumOutputs;
    addtype.activate_fn = GizMiniCut_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = GizMiniCut_Reset;
    addtype.reserve_buffer_space_fn = GizMiniCut_ReserveBufferSpace;
    addtype.load_fn = GizMiniCut_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
}
