#include "legoapi/gizmos/traps/gizbombgen.h"

#include "decomp.h"

i32 bombgen_gizmotype_id = -1;

static i32 GizBombGens_GetMaxGizmos(void *bombgen) {
    UNIMPLEMENTED();
    return {};
}

static void GizBombGens_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoBombGen_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoBombGen_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoBombGen_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoBombGen_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void GizmoBombGen_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizmoBombGen_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoBombGen_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *GizBombGens_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void GizBombGens_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_SetResetFlag(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizBombGens_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizBombGens_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "BombGenerator";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x18;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBombGens_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBombGen_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBombGens_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBombGens_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizmoBombGen_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBombGen_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBombGen_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBombGen_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBombGen_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBombGen_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBombGens_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBombGens_ClearProgress;
    addtype.fns.store_progress_fn = GizBombGens_StoreProgress;
    addtype.fns.reset_fn = GizBombGens_SetResetFlag;
    addtype.fns.reserve_buffer_space_fn = GizBombGens_ReserveBufferSpace;
    addtype.fns.load_fn = GizBombGens_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    bombgen_gizmotype_id = type_id;

    return &addtype;
}
