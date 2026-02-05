#include "game/gizmos/push.h"

#include "decomp.h"

int pushblock_gizmotype_id = -1;

static int Push_GetMaxGizmos(void *push) {
    UNIMPLEMENTED();
}

static void Push_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void UpdatePushBlocks(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizPush_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizPush_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizPush_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizPush_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Push_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Push_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Pushblocks_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
}

static void *Push_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Push_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Push_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Push_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int PushBlocks_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int edpush_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Push_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "PushBlocks";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x24c;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Push_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Push_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = Pushblocks_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = UpdatePushBlocks;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizPush_GetGizmoName;
    addtype.fns.get_output_fn = GizPush_GetOutput;
    addtype.fns.get_output_name_fn = GizPush_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizPush_GetNumOutputs;
    addtype.fns.activate_fn = Push_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Push_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Push_AllocateProgressData;
    addtype.fns.clear_progress_fn = Push_ClearProgress;
    addtype.fns.store_progress_fn = Push_StoreProgress;
    addtype.fns.reset_fn = Push_Reset;
    addtype.fns.reserve_buffer_space_fn = PushBlocks_ReserveBufferSpace;
    addtype.fns.load_fn = edpush_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    pushblock_gizmotype_id = type_id;

    return &addtype;
}
