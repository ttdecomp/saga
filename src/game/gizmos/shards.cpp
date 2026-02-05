#include "game/gizmos/shards.h"

#include "decomp.h"

static int Shards_GetMaxGizmos(void *shard) {
    UNIMPLEMENTED();
}

static void Shards_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
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
}

static int Shard_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Shard_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Shard_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Shard_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Shard_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Shards_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
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

static int Shards_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Shards_Load(void *, void *) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Shards_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Shard";
    addtype.prefix = "";
    addtype.unknown1 = 0x30;
    addtype.early_update_fn = Shards_UpdateBeforeCharacters;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Shards_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Shards_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Shards_UpdateAfterCharacters;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Shards_Draw;
    addtype.get_gizmo_name_fn = Shard_GetGizmoName;
    addtype.get_output_fn = Shard_GetOutput;
    addtype.get_output_name_fn = Shard_GetOutputName;
    addtype.get_num_outputs_fn = Shard_GetNumOutputs;
    addtype.activate_fn = Shard_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Shard_SetVisibility;
    addtype.allocate_progress_data_fn = Shards_AllocateProgressData;
    addtype.clear_progress_fn = Shards_ClearProgress;
    addtype.store_progress_fn = Shards_StoreProgress;
    addtype.reset_fn = Shards_Reset;
    addtype.reserve_buffer_space_fn = Shards_ReserveBufferSpace;
    addtype.load_fn = Shards_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;

    return &addtype;
}
