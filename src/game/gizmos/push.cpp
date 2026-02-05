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

static void Push_AllocateProgressData(VARIPTR *, VARIPTR *) {
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
    addtype.unknown1 = 0x24c;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Push_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Push_AddGizmos;
    addtype.bolt_hit_plat_fn = Pushblocks_BoltHitPlat;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = UpdatePushBlocks;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizPush_GetGizmoName;
    addtype.get_output_fn = GizPush_GetOutput;
    addtype.get_output_name_fn = GizPush_GetOutputName;
    addtype.get_num_outputs_fn = GizPush_GetNumOutputs;
    addtype.activate_fn = Push_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Push_SetVisibility;
    addtype.allocate_progress_data_fn = Push_AllocateProgressData;
    addtype.clear_progress_fn = Push_ClearProgress;
    addtype.store_progress_fn = Push_StoreProgress;
    addtype.reset_fn = Push_Reset;
    addtype.reserve_buffer_space_fn = PushBlocks_ReserveBufferSpace;
    addtype.load_fn = edpush_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    pushblock_gizmotype_id = type_id;

    return &addtype;
}
