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

static void *Grapples_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

ADDGIZMOTYPE* Grapples_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Grapple";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Grapples_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Grapples_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Grapples_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Grapples_Draw;
    addtype.fns.get_gizmo_name_fn = Grapple_GetGizmoName;
    addtype.fns.get_output_fn = Grapple_GetOutput;
    addtype.fns.get_output_name_fn = Grapple_GetOutputName;
    addtype.fns.get_num_outputs_fn = Grapple_GetNumOutputs;
    addtype.fns.activate_fn = Grapple_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Grapple_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Grapples_AllocateProgressData;
    addtype.fns.clear_progress_fn = Grapples_ClearProgress;
    addtype.fns.store_progress_fn = Grapples_StoreProgress;
    addtype.fns.reset_fn = Grapples_Reset;
    addtype.fns.reserve_buffer_space_fn = Grapples_ReserveBufferSpace;
    addtype.fns.load_fn = Grapples_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    grapple_gizmotype_id = type_id;

    return &addtype;
}
