#include "game/gizmos/newblowup.h"

#include "decomp.h"

int blowup_gizmotype_id = -1;

static int Blowup_GetMaxGizmos(void *blowup) {
    UNIMPLEMENTED();
}

static void Blowup_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

void GizmoBlowupEarlyUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

void GizmoBlowupLateUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

void GizmoBlowupBurstDraw(void *, void *, float) {
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

int Blowup_GetVisibility(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizmoBlowup_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void *Blowup_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

void *gizmoblowup_reservebuffers(void *) {
    UNIMPLEMENTED();
}

int gizmoblowup_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *NewBlowup_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "blowup";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x100;
    addtype.fns.early_update_fn = GizmoBlowupEarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = Blowup_GetVisibility;
    addtype.fns.get_max_gizmos_fn = Blowup_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBlowup_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Blowup_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizmoBlowupLateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizmoBlowupBurstDraw;
    addtype.fns.get_gizmo_name_fn = Blowup_GetGizmoName;
    addtype.fns.get_output_fn = Blowup_GetOutput;
    addtype.fns.get_output_name_fn = Blowup_GetOutputName;
    addtype.fns.get_num_outputs_fn = Blowup_GetNumOutputs;
    addtype.fns.activate_fn = Blowup_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Blowup_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Blowup_AllocateProgressData;
    addtype.fns.clear_progress_fn = Blowup_ClearProgress;
    addtype.fns.store_progress_fn = Blowup_StoreProgress;
    addtype.fns.reset_fn = Blowups_Reset;
    addtype.fns.reserve_buffer_space_fn = gizmoblowup_reservebuffers;
    addtype.fns.load_fn = gizmoblowup_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    blowup_gizmotype_id = type_id;

    return &addtype;
}
