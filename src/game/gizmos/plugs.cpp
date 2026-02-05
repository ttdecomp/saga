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

static void Plugs_AllocateProgressData(VARIPTR *, VARIPTR *) {
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
    addtype.unknown1 = 0xc;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Plugs_GetMaxGizmos;
    addtype.get_pos_fn = Plug_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Plugs_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Plugs_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Plugs_Draw;
    addtype.get_gizmo_name_fn = Plug_GetGizmoName;
    addtype.get_output_fn = Plug_GetOutput;
    addtype.get_output_name_fn = Plug_GetOutputName;
    addtype.get_num_outputs_fn = Plug_GetNumOutputs;
    addtype.activate_fn = Plug_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Plug_SetVisibility;
    addtype.allocate_progress_data_fn = Plugs_AllocateProgressData;
    addtype.clear_progress_fn = Plugs_ClearProgress;
    addtype.store_progress_fn = Plugs_StoreProgress;
    addtype.reset_fn = Plugs_Reset;
    addtype.reserve_buffer_space_fn = Plugs_ReserveBufferSpace;
    addtype.load_fn = Plugs_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    plug_gizmotype_id = type_id;

    return &addtype;
}
