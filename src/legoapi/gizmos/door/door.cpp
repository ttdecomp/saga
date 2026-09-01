#include "legoapi/gizmos/door/door.h"

#include "decomp.h"
#include "legoapi/props/doors/door.h"

static char door_gizmotype_id = -1;

static i32 Door_GetMaxGizmos(void *door) {
    UNIMPLEMENTED();
    return {};
}

static void Door_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static char *Door_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL && gizmo->object != NULL ? ((DOOR *)gizmo->object)->gizmo_name : NULL;
}

static i32 Door_GetOutput(GIZMO *gizmo, i32, i32) {
    return gizmo != NULL && gizmo->object != NULL ? ((DOOR *)gizmo->object)->active == 0 : 0;
}

char *Door_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Door_GetNumOutputs(GIZMO *) {
    return 1;
}

void Door_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL && gizmo->object != NULL) {
        ((DOOR *)gizmo->object)->active = active == 0;
    }
}

ADDGIZMOTYPE *Door_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Door";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Door_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Door_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Door_GetGizmoName;
    addtype.fns.get_output_fn = Door_GetOutput;
    addtype.fns.get_output_name_fn = Door_GetOutputName;
    addtype.fns.get_num_outputs_fn = Door_GetNumOutputs;
    addtype.fns.activate_fn = Door_Activate;
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
    door_gizmotype_id = (char)type_id;

    return &addtype;
}
