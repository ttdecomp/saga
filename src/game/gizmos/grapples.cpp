#include "game/gizmos/grapples.h"

#include "decomp.h"

int grapple_gizmotype_id = -1;

static int Grapples_GetMaxGizmos(void *grapple) {
    UNIMPLEMENTED();
}

static void Grapples_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Grapples_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Grapple_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Grapple_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Grapple_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Grapple_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Grapple_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Grapple_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Grapples_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Grapples_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Grapples_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Grapples_Load(void *, void *) {
    UNIMPLEMENTED();
}

void Grapples_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Grapple";
    addtype.prefix = "";
    addtype.unknown1 = 8;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Grapples_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Grapples_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Grapples_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Grapples_Draw;
    addtype.get_gizmo_name_fn = Grapple_GetGizmoName;
    addtype.get_output_fn = Grapple_GetOutput;
    addtype.get_output_name_fn = Grapple_GetOutputName;
    addtype.get_num_outputs_fn = Grapple_GetNumOutputs;
    addtype.activate_fn = Grapple_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Grapple_SetVisibility;
    addtype.allocate_progress_data_fn = Grapples_AllocateProgressData;
    addtype.clear_progress_fn = Grapples_ClearProgress;
    addtype.store_progress_fn = Grapples_StoreProgress;
    addtype.reset_fn = Grapples_Reset;
    addtype.reserve_buffer_space_fn = Grapples_ReserveBufferSpace;
    addtype.load_fn = Grapples_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    grapple_gizmotype_id = type_id;
}
