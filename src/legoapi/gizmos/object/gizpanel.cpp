#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/world/level.h"

#include "decomp.h"
#include "legoapi/legoapi_types.h"

struct GIZPANELPROGRESS {
    i32 state;
    i32 goodie_state;
    i32 baddie_state;
};

i32 gizpanel_gizmotype_id = -1;

static i32 GizPanel_GetMaxGizmos(void *panel) {
    WORLDINFO *world = static_cast<WORLDINFO *>(panel);
    return world != NULL ? world->current_level->max_giz_panels : 0;
}

static void GizPanel_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GizPanel_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void GizPanel_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GizPanel_GetGizmoName(GIZMO *gizmo) {
    GIZPANEL_s *panel = gizmo != NULL ? static_cast<GIZPANEL_s *>(gizmo->object) : NULL;
    return panel != NULL ? panel->name : NULL;
}

static i32 GizPanel_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GizPanel_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizPanel_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizPanel_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GizPanel_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *GizPanels_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0xc);
}

static void GizPanels_ClearProgress(void *, void *progress_data) {
    GIZPANELPROGRESS *progress = (GIZPANELPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state = 0;
    progress->goodie_state = -1;
    progress->baddie_state = -1;
}

static void GizPanels_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void GizPanels_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *GizPanels_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizPanel_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;

    addtype.name = "Panel";
    addtype.fns.unknown1 = 0xc;
    addtype.prefix = "";
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizPanel_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizPanel_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizPanel_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizPanel_Draw;
    addtype.fns.get_gizmo_name_fn = GizPanel_GetGizmoName;
    addtype.fns.get_output_fn = GizPanel_GetOutput;
    addtype.fns.get_output_name_fn = GizPanel_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizPanel_GetNumOutputs;
    addtype.fns.activate_fn = GizPanel_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizPanel_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizPanels_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizPanels_ClearProgress;
    addtype.fns.store_progress_fn = GizPanels_StoreProgress;
    addtype.fns.reset_fn = GizPanels_Reset;
    addtype.fns.reserve_buffer_space_fn = GizPanels_ReserveBufferSpace;
    addtype.fns.load_fn = GizPanel_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizpanel_gizmotype_id = type_id;

    return &addtype;
}
