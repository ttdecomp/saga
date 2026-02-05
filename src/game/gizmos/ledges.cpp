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

static void Ledges_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

void Ledges_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Ledge";
    addtype.prefix = "";
    addtype.unknown1 = 0x20;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Ledges_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Ledges_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Ledges_Draw;
    addtype.get_gizmo_name_fn = Ledge_GetGizmoName;
    addtype.get_output_fn = Ledge_GetOutput;
    addtype.get_output_name_fn = Ledge_GetOutputName;
    addtype.get_num_outputs_fn = Ledge_GetNumOutputs;
    addtype.activate_fn = Ledge_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Ledge_SetVisibility;
    addtype.allocate_progress_data_fn = Ledges_AllocateProgressData;
    addtype.clear_progress_fn = Ledges_ClearProgress;
    addtype.store_progress_fn = Ledges_StoreProgress;
    addtype.reset_fn = Ledges_Reset;
    addtype.reserve_buffer_space_fn = Ledges_ReserveBufferSpace;
    addtype.load_fn = Ledges_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
}
