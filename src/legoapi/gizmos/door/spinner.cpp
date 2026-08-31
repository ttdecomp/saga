#include "legoapi/gizmos/door/spinner.h"

#include "decomp.h"

i32 spinner_gizmotype_id = -1;

static i32 GizSpinner_GetMaxGizmos(void *spinner) {
    UNIMPLEMENTED();
    return {};
}

static void GizSpinner_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

void GizSpinners_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizSpinner_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizSpinner_PanelDraw(void *, void *, float) {
}

static char *GizSpinner_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

i32 GizSpinner_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

char *GizSpinner_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

i32 GizSpinner_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

void GizSpinner_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

void GizSpinner_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static NUVEC *GizSpinner_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

i32 GizSpinner_UsingSpecial(GIZMO **, void *, i32, char *) {
    UNIMPLEMENTED();
    return 0;
}

static i32 GizSpinner_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizSpinner_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizSpinner_HasBoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                                 unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizSpinner_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
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

static void *GizSpinner_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizSpinner_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Spinner_RegisterGizmo(i32 type_id) {
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
