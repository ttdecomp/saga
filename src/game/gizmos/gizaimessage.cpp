#include "game/gizmos/gizaimessage.h"

#include "decomp.h"

int gizaimessage_gizmotype_id = -1;

static char *gizaimessage_prefix = "msg_";

static int GizAIMessage_GetMaxGizmos(void *message) {
    UNIMPLEMENTED();
}

static void GizAIMessage_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizAIMessage_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizAIMessage_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizAIMessage_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizAIMessage_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

void GizAIMessage_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Message";
    addtype.prefix = gizaimessage_prefix;
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizAIMessage_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizAIMessage_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizAIMessage_GetGizmoName;
    addtype.get_output_fn = GizAIMessage_GetOutput;
    addtype.get_output_name_fn = GizAIMessage_GetOutputName;
    addtype.get_num_outputs_fn = GizAIMessage_GetNumOutputs;
    addtype.activate_fn = NULL;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = NULL;
    addtype.reserve_buffer_space_fn = NULL;
    addtype.load_fn = NULL;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    gizaimessage_gizmotype_id = type_id;
}
