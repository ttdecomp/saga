#include "game/gizmos/gizbombgen.h"

#include "decomp.h"

int bombgen_gizmotype_id = -1;

static int GizBombGens_GetMaxGizmos(void *bombgen) {
    UNIMPLEMENTED();
}

static void GizBombGens_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoBombGen_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoBombGen_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizmoBombGen_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizmoBombGen_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizmoBombGen_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizmoBombGen_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizmoBombGen_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizBombGens_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
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

static int GizBombGens_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizBombGens_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* GizBombGen_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "BombGenerator";
    addtype.prefix = "";
    addtype.unknown1 = 0x18;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizBombGens_GetMaxGizmos;
    addtype.get_pos_fn = GizmoBombGen_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizBombGens_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = GizBombGens_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizmoBombGen_GetGizmoName;
    addtype.get_output_fn = GizmoBombGen_GetOutput;
    addtype.get_output_name_fn = GizmoBombGen_GetOutputName;
    addtype.get_num_outputs_fn = GizmoBombGen_GetNumOutputs;
    addtype.activate_fn = GizmoBombGen_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = GizmoBombGen_SetVisibility;
    addtype.allocate_progress_data_fn = GizBombGens_AllocateProgressData;
    addtype.clear_progress_fn = GizBombGens_ClearProgress;
    addtype.store_progress_fn = GizBombGens_StoreProgress;
    addtype.reset_fn = GizBombGens_SetResetFlag;
    addtype.reserve_buffer_space_fn = GizBombGens_ReserveBufferSpace;
    addtype.load_fn = GizBombGens_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    bombgen_gizmotype_id = type_id;

    return &addtype;
}
