#include "game/gizmos/plugs.h"

#include "decomp.h"

int plug_gizmotype_id = -1;

static int Plugs_GetMaxGizmos(void *plug) {
    UNIMPLEMENTED();
}

static void Plugs_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Plugs_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Plugs_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Plug_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Plug_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Plug_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Plug_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Plug_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Plug_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Plug_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *Plugs_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Plugs_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Plugs_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Plugs_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Plugs_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Plugs_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Plugs_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Plug";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0xc;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Plugs_GetMaxGizmos;
    addtype.fns.get_pos_fn = Plug_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Plugs_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Plugs_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Plugs_Draw;
    addtype.fns.get_gizmo_name_fn = Plug_GetGizmoName;
    addtype.fns.get_output_fn = Plug_GetOutput;
    addtype.fns.get_output_name_fn = Plug_GetOutputName;
    addtype.fns.get_num_outputs_fn = Plug_GetNumOutputs;
    addtype.fns.activate_fn = Plug_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Plug_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Plugs_AllocateProgressData;
    addtype.fns.clear_progress_fn = Plugs_ClearProgress;
    addtype.fns.store_progress_fn = Plugs_StoreProgress;
    addtype.fns.reset_fn = Plugs_Reset;
    addtype.fns.reserve_buffer_space_fn = Plugs_ReserveBufferSpace;
    addtype.fns.load_fn = Plugs_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    plug_gizmotype_id = type_id;

    return &addtype;
}
