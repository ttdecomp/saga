#include "game/gizmos/tubes.h"

#include "decomp.h"

static int Tubes_GetMaxGizmos(void *tube) {
    UNIMPLEMENTED();
}

static void Tubes_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Tubes_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Tube_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Tube_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Tube_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Tube_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Tube_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Tube_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void Tube_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Tube_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *Tubes_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Tubes_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Tubes_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Tubes_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Tubes_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Tube";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Tubes_GetMaxGizmos;
    addtype.fns.get_pos_fn = Tube_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Tubes_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Tubes_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Tubes_Draw;
    addtype.fns.get_gizmo_name_fn = Tube_GetGizmoName;
    addtype.fns.get_output_fn = Tube_GetOutput;
    addtype.fns.get_output_name_fn = Tube_GetOutputName;
    addtype.fns.get_num_outputs_fn = Tube_GetNumOutputs;
    addtype.fns.activate_fn = Tube_Activate;
    addtype.fns.activate_rev_fn = Tube_ActivateRev;
    addtype.fns.set_visibility_fn = Tube_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Tubes_AllocateProgressData;
    addtype.fns.clear_progress_fn = Tubes_ClearProgress;
    addtype.fns.store_progress_fn = Tubes_StoreProgress;
    addtype.fns.reset_fn = Tubes_Reset;
    addtype.fns.reserve_buffer_space_fn = Tubes_ReserveBufferSpace;
    addtype.fns.load_fn = Tubes_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
