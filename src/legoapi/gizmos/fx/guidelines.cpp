#include "legoapi/gizmos/fx/guidelines.h"

#include "decomp.h"
#include "legoapi/world/level.h"

static i32 GuideLines_GetMaxGizmos(void *guideline) {
    WORLDINFO *world = static_cast<WORLDINFO *>(guideline);
    return world != NULL ? world->current_level->max_guidelines : 0;
}

static void GuideLines_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void GuideLines_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *GuideLine_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<char *>(gizmo->object) : NULL;
}

static i32 GuideLine_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *GuideLine_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 GuideLine_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void GuideLine_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void GuideLine_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *GuideLines_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void GuideLines_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void GuideLines_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

struct GUIDELINEPROGRESS {
    u32 state[2];
};

static void GuideLines_Reset(void *world_info, void *, void *progress_data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    if (world == NULL || world->guidelines == NULL || world->guideline_count <= 0) {
        return;
    }

    GUIDELINE *guideline = world->guidelines;
    for (i32 i = 0; i < world->guideline_count; i++, guideline++) {
        guideline->active = 1;
        guideline->visible = 1;

        if (i <= 31 && progress_data != NULL) {
            u32 mask = 1u << i;
            guideline->visible = (static_cast<GUIDELINEPROGRESS *>(progress_data)->state[1] & mask) != 0;
            guideline->active = (static_cast<GUIDELINEPROGRESS *>(progress_data)->state[0] & mask) != 0;
        }
    }
}

static void *GuideLines_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 GuideLines_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *GuideLines_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GuideLine";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GuideLines_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GuideLines_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GuideLines_Draw;
    addtype.fns.get_gizmo_name_fn = GuideLine_GetGizmoName;
    addtype.fns.get_output_fn = GuideLine_GetOutput;
    addtype.fns.get_output_name_fn = GuideLine_GetOutputName;
    addtype.fns.get_num_outputs_fn = GuideLine_GetNumOutputs;
    addtype.fns.activate_fn = GuideLine_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GuideLine_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GuideLines_AllocateProgressData;
    addtype.fns.clear_progress_fn = GuideLines_ClearProgress;
    addtype.fns.store_progress_fn = GuideLines_StoreProgress;
    addtype.fns.reset_fn = GuideLines_Reset;
    addtype.fns.reserve_buffer_space_fn = GuideLines_ReserveBufferSpace;
    addtype.fns.load_fn = GuideLines_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
