#include "game/gizmos/door.h"

#include "decomp.h"

char door_gizmotype_id = -1;

static int Door_GetMaxGizmos(void *door) {
    UNIMPLEMENTED();
}

static void Door_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *Door_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Door_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Door_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Door_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Door_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Door_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Door";
    addtype.prefix = "";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Door_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Door_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = Door_GetGizmoName;
    addtype.get_output_fn = Door_GetOutput;
    addtype.get_output_name_fn = Door_GetOutputName;
    addtype.get_num_outputs_fn = Door_GetNumOutputs;
    addtype.activate_fn = Door_Activate;
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
    door_gizmotype_id = (char)type_id;

    return &addtype;
}
