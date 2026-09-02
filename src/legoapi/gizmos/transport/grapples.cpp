#include "legoapi/gizmos/transport/grapples.h"

#include "decomp.h"

#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

i32 grapple_gizmotype_id = -1;

static i32 Grapples_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = (WORLDINFO *)world_info;
    if (world == NULL) {
        return 0;
    }

    return world->current_level->max_grapples;
}

static void Grapples_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Grapples_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Grapple_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<char *>(gizmo->object) : NULL;
}

static i32 Grapple_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Grapple_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Grapple_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Grapple_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Grapple_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *Grapples_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void Grapples_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Grapples_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Grapples_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Grapples_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Grapples_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Grapple";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Grapples_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Grapples_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Grapples_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Grapples_Draw;
    addtype.fns.get_gizmo_name_fn = Grapple_GetGizmoName;
    addtype.fns.get_output_fn = Grapple_GetOutput;
    addtype.fns.get_output_name_fn = Grapple_GetOutputName;
    addtype.fns.get_num_outputs_fn = Grapple_GetNumOutputs;
    addtype.fns.activate_fn = Grapple_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Grapple_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Grapples_AllocateProgressData;
    addtype.fns.clear_progress_fn = Grapples_ClearProgress;
    addtype.fns.store_progress_fn = Grapples_StoreProgress;
    addtype.fns.reset_fn = Grapples_Reset;
    addtype.fns.reserve_buffer_space_fn = Grapples_ReserveBufferSpace;
    addtype.fns.load_fn = Grapples_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    grapple_gizmotype_id = type_id;

    return &addtype;
}
