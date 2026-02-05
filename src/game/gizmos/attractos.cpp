#include "game/gizmos/attractos.h"

#include "decomp.h"

static int Attractos_GetMaxGizmos(void *attracto) {
    UNIMPLEMENTED();
}

static void Attractos_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Attractos_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Attractos_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Attracto_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Attracto_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Attracto_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Attracto_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Attracto_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Attracto_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Attracto_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *Attractos_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Attractos_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Attractos_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Attractos_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Attractos_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Attractos_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Attractos_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Attracto";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x2c;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Attractos_GetMaxGizmos;
    addtype.fns.get_pos_fn = Attracto_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Attractos_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Attractos_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Attractos_Draw;
    addtype.fns.get_gizmo_name_fn = Attracto_GetGizmoName;
    addtype.fns.get_output_fn = Attracto_GetOutput;
    addtype.fns.get_output_name_fn = Attracto_GetOutputName;
    addtype.fns.get_num_outputs_fn = Attracto_GetNumOutputs;
    addtype.fns.activate_fn = Attracto_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Attracto_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Attractos_AllocateProgressData;
    addtype.fns.clear_progress_fn = Attractos_ClearProgress;
    addtype.fns.store_progress_fn = Attractos_StoreProgress;
    addtype.fns.reset_fn = Attractos_Reset;
    addtype.fns.reserve_buffer_space_fn = Attractos_ReserveBufferSpace;
    addtype.fns.load_fn = Attractos_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
