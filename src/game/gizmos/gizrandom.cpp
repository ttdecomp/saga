#include "game/gizmos/gizrandom.h"

#include "decomp.h"

int gizrandom_gizmotype_id = -1;

static int GizRandom_GetMaxGizmos(void *random) {
    UNIMPLEMENTED();
}

static void GizRandom_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *GizRandom_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int GizRandom_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *GizRandom_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int GizRandom_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void GizRandom_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int GizRandom_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* GizRandom_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizRandom";
    addtype.prefix = "rnd_";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = GizRandom_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = GizRandom_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = GizRandom_GetGizmoName;
    addtype.get_output_fn = GizRandom_GetOutput;
    addtype.get_output_name_fn = GizRandom_GetOutputName;
    addtype.get_num_outputs_fn = GizRandom_GetNumOutputs;
    addtype.activate_fn = GizRandom_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = NULL;
    addtype.reserve_buffer_space_fn = GizRandom_ReserveBufferSpace;
    addtype.load_fn = NULL;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    gizrandom_gizmotype_id = type_id;

    return &addtype;
}
