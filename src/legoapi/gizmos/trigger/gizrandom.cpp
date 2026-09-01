#include "legoapi/gizmos/trigger/gizrandom.h"

#include "decomp.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

i32 gizrandom_gizmotype_id = -1;

i32 GizRandom_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    return world != NULL && world->current_level != NULL ? world->current_level->max_giz_randoms : 0;
}

void GizRandom_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

char *GizRandom_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZRANDOM *>(gizmo->object)->name : NULL;
}

i32 GizRandom_GetOutput(GIZMO *gizmo, i32 output, i32) {
    GIZRANDOM *random = static_cast<GIZRANDOM *>(gizmo->object);
    return (random->flags & 1) != 0 && random->selected_output == output;
}

char *GizRandom_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

i32 GizRandom_GetNumOutputs(GIZMO *gizmo) {
    return static_cast<GIZRANDOM *>(gizmo->object)->output_count;
}

void GizRandom_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

void *GizRandom_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizRandom_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizRandom";
    addtype.prefix = "rnd_";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizRandom_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizRandom_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizRandom_GetGizmoName;
    addtype.fns.get_output_fn = GizRandom_GetOutput;
    addtype.fns.get_output_name_fn = GizRandom_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizRandom_GetNumOutputs;
    addtype.fns.activate_fn = GizRandom_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = GizRandom_ReserveBufferSpace;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizrandom_gizmotype_id = type_id;

    return &addtype;
}
