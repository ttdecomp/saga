#include "game/gizmos/gizpanel.h"

#include "decomp.h"

int gizpanel_gizmotype_id = -1;

static int GizPanel_GetMaxGizmos(void *panel) {
    UNIMPLEMENTED();
}

static void GizPanel_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizPanel_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizPanel_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizPanel_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizPanel_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizPanel_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizPanel_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizPanel_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizPanel_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *GizPanels_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizPanels_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizPanels_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizPanels_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizPanels_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizPanel_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* GizPanel_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;

    addtype.name = "Panel";
    addtype.fns.unknown1 = 0xc;
    addtype.prefix = "";
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizPanel_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizPanel_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizPanel_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizPanel_Draw;
    addtype.fns.get_gizmo_name_fn = GizPanel_GetGizmoName;
    addtype.fns.get_output_fn = GizPanel_GetOutput;
    addtype.fns.get_output_name_fn = GizPanel_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizPanel_GetNumOutputs;
    addtype.fns.activate_fn = GizPanel_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizPanel_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizPanels_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizPanels_ClearProgress;
    addtype.fns.store_progress_fn = GizPanels_StoreProgress;
    addtype.fns.reset_fn = GizPanels_Reset;
    addtype.fns.reserve_buffer_space_fn = GizPanels_ReserveBufferSpace;
    addtype.fns.load_fn = GizPanel_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;

    return &addtype;
}
