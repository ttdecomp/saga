#include "legoapi/gizmos/traps/gizbombgen.h"

#include "decomp.h"
#include "legoapi/world/level.h"

struct GIZBOMBGEN_RESET_s {
    u8 data[0x0c];
    u8 flags;
    u8 pad_0d[3];
    u32 reset_value;
};

struct GIZBOMBGEN_RESET_CONTEXT_s {
    u8 data[0x118];
    i32 editor_flag;
};

extern i32 come_from_an_editor;

i32 bombgen_gizmotype_id = -1;

static i32 GizBombGens_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    return world != NULL ? world->current_level->max_bombgens : 0;
}

static void GizBombGens_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizmoBombGen_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZBOMBGEN *>(gizmo->object)->name : NULL;
}

static i32 GizmoBombGen_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizmoBombGen_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizmoBombGen_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void GizmoBombGen_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizmoBombGen_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 GizmoBombGen_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static void *GizBombGens_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void GizBombGens_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizBombGens_SetResetFlag(void *context_ptr, void *bomb_ptr, void *value_ptr) {
    GIZBOMBGEN_RESET_s *bomb = static_cast<GIZBOMBGEN_RESET_s *>(bomb_ptr);
    GIZBOMBGEN_RESET_CONTEXT_s *context = static_cast<GIZBOMBGEN_RESET_CONTEXT_s *>(context_ptr);
    i32 editor_flag = 0;
    u8 flags = bomb->flags;
    flags = static_cast<u8>(flags | 1);
    bomb->flags = flags;
    if (come_from_an_editor == 0) {
        editor_flag = context->editor_flag;
    }
    flags = static_cast<u8>((flags & static_cast<u8>(~2)) | static_cast<u8>((editor_flag & 1) << 1));
    bomb->flags = flags;
    bomb->reset_value = reinterpret_cast<u32>(value_ptr);
}

static void *GizBombGens_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizBombGens_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "BombGenerator";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x18;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizBombGens_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoBombGen_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizBombGens_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizBombGens_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizmoBombGen_GetGizmoName;
    addtype.fns.get_output_fn = GizmoBombGen_GetOutput;
    addtype.fns.get_output_name_fn = GizmoBombGen_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoBombGen_GetNumOutputs;
    addtype.fns.activate_fn = GizmoBombGen_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoBombGen_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizBombGens_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizBombGens_ClearProgress;
    addtype.fns.store_progress_fn = GizBombGens_StoreProgress;
    addtype.fns.reset_fn = GizBombGens_SetResetFlag;
    addtype.fns.reserve_buffer_space_fn = GizBombGens_ReserveBufferSpace;
    addtype.fns.load_fn = GizBombGens_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    bombgen_gizmotype_id = type_id;

    return &addtype;
}
