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

static void ZipUps_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void ZipUps_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void ZipUps_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void ZipUps_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int ZipUps_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int ZipUps_Load(void *, void *) {
    UNIMPLEMENTED();
}

void ZipUps_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "ZipUp";
    addtype.prefix = "";
    addtype.unknown1 = 8;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = ZipUps_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = ZipUps_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = ZipUps_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = ZipUps_Draw;
    addtype.get_gizmo_name_fn = ZipUp_GetGizmoName;
    addtype.get_output_fn = ZipUp_GetOutput;
    addtype.get_output_name_fn = ZipUp_GetOutputName;
    addtype.get_num_outputs_fn = ZipUp_GetNumOutputs;
    addtype.activate_fn = ZipUp_Activate;
    addtype.activate_rev_fn = ZipUp_ActivateRev;
    addtype.set_visibility_fn = ZipUp_SetVisibility;
    addtype.allocate_progress_data_fn = ZipUps_AllocateProgressData;
    addtype.clear_progress_fn = ZipUps_ClearProgress;
    addtype.store_progress_fn = ZipUps_StoreProgress;
    addtype.reset_fn = ZipUps_Reset;
    addtype.reserve_buffer_space_fn = ZipUps_ReserveBufferSpace;
    addtype.load_fn = ZipUps_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    zipup_gizmotype_id = type_id;
}
