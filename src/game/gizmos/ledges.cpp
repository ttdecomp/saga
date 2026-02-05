#include "game/gizmos/ledges.h"

#include "decomp.h"

static int Ledges_GetMaxGizmos(void *ledge) {
    UNIMPLEMENTED();
}

static void Ledges_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Ledges_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Ledge_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Ledge_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Ledge_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Ledge_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Ledge_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Ledge_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *Ledges_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
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

static int Ledges_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Ledges_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Ledges_RegisterGizmo(int type_id) {
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
