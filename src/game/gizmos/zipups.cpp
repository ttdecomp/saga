#include "game/gizmos/zipups.h"

#include "decomp.h"

int zipup_gizmotype_id = -1;

static int ZipUps_GetMaxGizmos(void *zipup) {
    UNIMPLEMENTED();
}

static void ZipUps_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void ZipUps_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void ZipUps_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *ZipUp_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int ZipUp_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *ZipUp_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int ZipUp_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void ZipUp_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int ZipUp_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void ZipUp_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void *ZipUps_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void ZipUps_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void ZipUps_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

void ZipUps_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *ZipUps_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
}

static int ZipUps_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *ZipUps_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ZipUp";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = ZipUps_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = ZipUps_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = ZipUps_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = ZipUps_Draw;
    addtype.fns.get_gizmo_name_fn = ZipUp_GetGizmoName;
    addtype.fns.get_output_fn = ZipUp_GetOutput;
    addtype.fns.get_output_name_fn = ZipUp_GetOutputName;
    addtype.fns.get_num_outputs_fn = ZipUp_GetNumOutputs;
    addtype.fns.activate_fn = ZipUp_Activate;
    addtype.fns.activate_rev_fn = ZipUp_ActivateRev;
    addtype.fns.set_visibility_fn = ZipUp_SetVisibility;
    addtype.fns.allocate_progress_data_fn = ZipUps_AllocateProgressData;
    addtype.fns.clear_progress_fn = ZipUps_ClearProgress;
    addtype.fns.store_progress_fn = ZipUps_StoreProgress;
    addtype.fns.reset_fn = ZipUps_Reset;
    addtype.fns.reserve_buffer_space_fn = ZipUps_ReserveBufferSpace;
    addtype.fns.load_fn = ZipUps_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    zipup_gizmotype_id = type_id;

    return &addtype;
}
