#include "legoapi/gizmos/traps/shards.h"

#include "decomp.h"

static i32 Shards_GetMaxGizmos(void *shard) {
    UNIMPLEMENTED();
    return {};
}

static void Shards_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Shards_UpdateBeforeCharacters(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Shards_UpdateAfterCharacters(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Shards_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Shard_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 Shard_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Shard_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Shard_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void Shard_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Shard_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *Shards_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void Shards_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Shards_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Shards_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Shards_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Shards_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Shards_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Shard";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x30;
    addtype.fns.early_update_fn = Shards_UpdateBeforeCharacters;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Shards_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Shards_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Shards_UpdateAfterCharacters;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Shards_Draw;
    addtype.fns.get_gizmo_name_fn = Shard_GetGizmoName;
    addtype.fns.get_output_fn = Shard_GetOutput;
    addtype.fns.get_output_name_fn = Shard_GetOutputName;
    addtype.fns.get_num_outputs_fn = Shard_GetNumOutputs;
    addtype.fns.activate_fn = Shard_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Shard_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Shards_AllocateProgressData;
    addtype.fns.clear_progress_fn = Shards_ClearProgress;
    addtype.fns.store_progress_fn = Shards_StoreProgress;
    addtype.fns.reset_fn = Shards_Reset;
    addtype.fns.reserve_buffer_space_fn = Shards_ReserveBufferSpace;
    addtype.fns.load_fn = Shards_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
