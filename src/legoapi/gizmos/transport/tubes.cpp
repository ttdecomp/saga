#include "legoapi/gizmos/transport/tubes.h"

#include "decomp.h"

#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

static i32 Tubes_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = (WORLDINFO *)world_info;
    if (world == NULL) {
        return 0;
    }

    return world->current_level->max_tubes;
}

static void Tubes_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Tubes_Draw(void *, void *, float) {
}

static char *Tube_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<char *>(gizmo->object) : NULL;
}

static i32 Tube_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Tube_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Tube_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void Tube_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Tube_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void Tube_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Tube_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *Tubes_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void Tubes_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Tubes_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

struct TUBEPROGRESS {
    u32 state[2];
};

static void Tubes_Reset(void *world_info, void *, void *progress_data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    if (world == NULL || world->tubes == NULL || world->tube_count <= 0) {
        return;
    }

    TUBE *tube = world->tubes;
    for (i32 i = 0; i < world->tube_count; i++, tube++) {
        tube->target_y = tube->position.y + tube->initial_y_offset;
        tube->travel_speed_squared = tube->travel_speed * tube->travel_speed;
        tube->active = 1;
        tube->visible = 1;

        if (i <= 31 && progress_data != NULL) {
            u32 mask = 1u << i;
            tube->visible = (static_cast<TUBEPROGRESS *>(progress_data)->state[0] & mask) != 0;
            tube->active = (static_cast<TUBEPROGRESS *>(progress_data)->state[1] & mask) != 0;
        }
    }
}

static void *Tubes_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Tubes_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Tubes_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Tube";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Tubes_GetMaxGizmos;
    addtype.fns.get_pos_fn = Tube_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Tubes_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Tubes_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Tubes_Draw;
    addtype.fns.get_gizmo_name_fn = Tube_GetGizmoName;
    addtype.fns.get_output_fn = Tube_GetOutput;
    addtype.fns.get_output_name_fn = Tube_GetOutputName;
    addtype.fns.get_num_outputs_fn = Tube_GetNumOutputs;
    addtype.fns.activate_fn = Tube_Activate;
    addtype.fns.activate_rev_fn = Tube_ActivateRev;
    addtype.fns.set_visibility_fn = Tube_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Tubes_AllocateProgressData;
    addtype.fns.clear_progress_fn = Tubes_ClearProgress;
    addtype.fns.store_progress_fn = Tubes_StoreProgress;
    addtype.fns.reset_fn = Tubes_Reset;
    addtype.fns.reserve_buffer_space_fn = Tubes_ReserveBufferSpace;
    addtype.fns.load_fn = Tubes_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
