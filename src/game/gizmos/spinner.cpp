#include "game/gizmos/spinner.h"

#include "decomp.h"

int spinner_gizmotype_id = -1;

static int GizSpinner_GetMaxGizmos(void *spinner) {
    UNIMPLEMENTED();
}

static void GizSpinner_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void GizSpinners_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizSpinner_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizSpinner_PanelDraw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizSpinner_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizSpinner_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizSpinner_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizSpinner_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizSpinner_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void GizSpinner_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizSpinner_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizSpinner_UsingSpecial(GIZMO **, void *, int, char *) {
    UNIMPLEMENTED();
}

static int GizSpinner_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static int *GizSpinner_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         int, int, int) {
    UNIMPLEMENTED();
}

static int GizSpinner_HasBoltHit(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                                 unsigned char *) {
    UNIMPLEMENTED();
}

static void GizSpinner_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void GizSpinner_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizSpinner_StoreProgressData(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizSpinner_ResetAll(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int GizSpinner_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int GizSpinner_Load(void *, void *) {
    UNIMPLEMENTED();
}

void Spinner_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Spinner";
    addtype.prefix = "";
    addtype.unknown1 = 0x40;
    addtype.early_update_fn = GizSpinners_Update;
    addtype.panel_draw_fn = GizSpinner_PanelDraw;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizSpinner_GetMaxGizmos;
    addtype.get_pos_fn = GizSpinner_GetPos;
    addtype.using_special_fn = GizSpinner_UsingSpecial;
    addtype.add_gizmos_fn = GizSpinner_AddGizmos;
    addtype.bolt_hit_plat_fn = GizSpinner_BoltHitPlat;
    addtype.get_best_bolt_target_fn = GizSpinner_GetBestBoltTarget;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = GizSpinner_HasBoltHit;
    addtype.draw_fn = GizSpinner_Draw;
    addtype.get_gizmo_name_fn = GizSpinner_GetGizmoName;
    addtype.get_output_fn = GizSpinner_GetOutput;
    addtype.get_output_name_fn = GizSpinner_GetOutputName;
    addtype.get_num_outputs_fn = GizSpinner_GetNumOutputs;
    addtype.activate_fn = GizSpinner_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = GizSpinner_SetVisibility;
    addtype.allocate_progress_data_fn = GizSpinner_AllocateProgressData;
    addtype.clear_progress_fn = GizSpinner_ClearProgress;
    addtype.store_progress_fn = GizSpinner_StoreProgressData;
    addtype.reset_fn = GizSpinner_ResetAll;
    addtype.reserve_buffer_space_fn = GizSpinner_ReserveBufferSpace;
    addtype.load_fn = GizSpinner_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    spinner_gizmotype_id = type_id;
}
