#include "game/gizmos/ai.h"

#include "decomp.h"

static int AI_GetMaxGizmos(void *ai) {
    UNIMPLEMENTED();
}

static void AI_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *AI_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int AI_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *AI_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int AI_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void AI_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* AI_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "AIProcessor";
    addtype.prefix = "ai_";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = AI_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = AI_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = AI_GetGizmoName;
    addtype.get_output_fn = AI_GetOutput;
    addtype.get_output_name_fn = AI_GetOutputName;
    addtype.get_num_outputs_fn = AI_GetNumOutputs;
    addtype.activate_fn = AI_Activate;
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

    return &addtype;
}
