#include "legoapi/gizmos/ai.h"

#include "decomp.h"
#include "legoapi/gizmo.h"
#include "legoapi/world.h"

static i32 AI_GetMaxGizmos(void *ai) {
    UNIMPLEMENTED();
    return {};
}

static void AI_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *unused) {
    WORLDINFO *world;
    i32 i;

    world = (WORLDINFO *)world_ptr;

    // ORIG_BUG: This comparison can never fail.
    if (world->processors != NULL) { // NOLINT
        for (i = 0; i < WORLD->processor_count; i++) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->processors[i]);
        }
    }
}

static char *AI_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 AI_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

char *AI_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 AI_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

void AI_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE *AI_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "AIProcessor";
    addtype.prefix = "ai_";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = AI_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = AI_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = AI_GetGizmoName;
    addtype.fns.get_output_fn = AI_GetOutput;
    addtype.fns.get_output_name_fn = AI_GetOutputName;
    addtype.fns.get_num_outputs_fn = AI_GetNumOutputs;
    addtype.fns.activate_fn = AI_Activate;
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

    return &addtype;
}
