#include "legoapi/gizmos/traps/attractos.h"

#include "decomp.h"

static i32 Attractos_GetMaxGizmos(void *attracto) {
    UNIMPLEMENTED();
    return {};
}

static void Attractos_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
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
    return {};
}

static i32 Attracto_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Attracto_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Attracto_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void Attracto_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Attracto_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Attracto_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *Attractos_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *Attractos_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Attractos_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Attractos_RegisterGizmo(i32 type_id) {
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
