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

static void Attractos_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

void Attractos_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Attracto";
    addtype.prefix = "";
    addtype.unknown1 = 0x2c;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Attractos_GetMaxGizmos;
    addtype.get_pos_fn = Attracto_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Attractos_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Attractos_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Attractos_Draw;
    addtype.get_gizmo_name_fn = Attracto_GetGizmoName;
    addtype.get_output_fn = Attracto_GetOutput;
    addtype.get_output_name_fn = Attracto_GetOutputName;
    addtype.get_num_outputs_fn = Attracto_GetNumOutputs;
    addtype.activate_fn = Attracto_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Attracto_SetVisibility;
    addtype.allocate_progress_data_fn = Attractos_AllocateProgressData;
    addtype.clear_progress_fn = Attractos_ClearProgress;
    addtype.store_progress_fn = Attractos_StoreProgress;
    addtype.reset_fn = Attractos_Reset;
    addtype.reserve_buffer_space_fn = Attractos_ReserveBufferSpace;
    addtype.load_fn = Attractos_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
}
