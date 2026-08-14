#include "legoapi/gizmos/gizaimessage.h"

#include "decomp.h"

i32 gizaimessage_gizmotype_id = -1;

static char *gizaimessage_prefix = "msg_";

static i32 GizAIMessage_GetMaxGizmos(void *message) {
    UNIMPLEMENTED();
    return {};
}

static void GizAIMessage_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizAIMessage_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizAIMessage_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizAIMessage_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizAIMessage_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizAIMessage_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Message";
    addtype.prefix = gizaimessage_prefix;
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizAIMessage_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizAIMessage_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizAIMessage_GetGizmoName;
    addtype.fns.get_output_fn = GizAIMessage_GetOutput;
    addtype.fns.get_output_name_fn = GizAIMessage_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizAIMessage_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizaimessage_gizmotype_id = type_id;

    return &addtype;
}
