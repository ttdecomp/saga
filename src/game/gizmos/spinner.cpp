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

static void *GizSpinner_AllocateProgressData(VARIPTR *, VARIPTR *) {
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

ADDGIZMOTYPE* Spinner_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Spinner";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x40;
    addtype.fns.early_update_fn = GizSpinners_Update;
    addtype.fns.panel_draw_fn = GizSpinner_PanelDraw;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizSpinner_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizSpinner_GetPos;
    addtype.fns.using_special_fn = GizSpinner_UsingSpecial;
    addtype.fns.add_gizmos_fn = GizSpinner_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizSpinner_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizSpinner_GetBestBoltTarget;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = GizSpinner_HasBoltHit;
    addtype.fns.draw_fn = GizSpinner_Draw;
    addtype.fns.get_gizmo_name_fn = GizSpinner_GetGizmoName;
    addtype.fns.get_output_fn = GizSpinner_GetOutput;
    addtype.fns.get_output_name_fn = GizSpinner_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizSpinner_GetNumOutputs;
    addtype.fns.activate_fn = GizSpinner_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizSpinner_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizSpinner_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizSpinner_ClearProgress;
    addtype.fns.store_progress_fn = GizSpinner_StoreProgressData;
    addtype.fns.reset_fn = GizSpinner_ResetAll;
    addtype.fns.reserve_buffer_space_fn = GizSpinner_ReserveBufferSpace;
    addtype.fns.load_fn = GizSpinner_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    spinner_gizmotype_id = type_id;

    return &addtype;
}
