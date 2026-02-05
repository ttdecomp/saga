#include "game/gizmos/newblowup.h"

#include "decomp.h"

int blowup_gizmotype_id = -1;

static int Blowup_GetMaxGizmos(void *blowup) {
    UNIMPLEMENTED();
}

static void Blowup_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizmoBlowupEarlyUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizmoBlowupLateUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizmoBlowupBurstDraw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Blowup_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Blowup_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Blowup_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Blowup_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Blowup_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Blowup_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Blowup_GetVisibility(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoBlowup_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Blowup_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Blowup_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Blowup_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Blowups_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int gizmoblowup_reservebuffers(void *, int) {
    UNIMPLEMENTED();
}

static int gizmoblowup_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* NewBlowup_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "blowup";
    addtype.prefix = "";
    addtype.unknown1 = 0x100;
    addtype.early_update_fn = GizmoBlowupEarlyUpdate;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = Blowup_GetVisibility;
    addtype.get_max_gizmos_fn = Blowup_GetMaxGizmos;
    addtype.get_pos_fn = GizmoBlowup_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Blowup_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = GizmoBlowupLateUpdate;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = GizmoBlowupBurstDraw;
    addtype.get_gizmo_name_fn = Blowup_GetGizmoName;
    addtype.get_output_fn = Blowup_GetOutput;
    addtype.get_output_name_fn = Blowup_GetOutputName;
    addtype.get_num_outputs_fn = Blowup_GetNumOutputs;
    addtype.activate_fn = Blowup_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Blowup_SetVisibility;
    addtype.allocate_progress_data_fn = Blowup_AllocateProgressData;
    addtype.clear_progress_fn = Blowup_ClearProgress;
    addtype.store_progress_fn = Blowup_StoreProgress;
    addtype.reset_fn = Blowups_Reset;
    addtype.reserve_buffer_space_fn = gizmoblowup_reservebuffers;
    addtype.load_fn = gizmoblowup_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    blowup_gizmotype_id = type_id;

    return &addtype;
}
