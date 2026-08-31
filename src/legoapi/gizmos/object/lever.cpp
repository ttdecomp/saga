#include "legoapi/gizmos/object/lever.h"

#include "decomp.h"

struct LEVERPROGRESS {
    i32 state;
    i32 goodie_state;
    i32 baddie_state;
};

i32 lever_gizmotype_id = -1;

static i32 Levers_GetMaxGizmos(void *lever) {
    UNIMPLEMENTED();
    return {};
}

static void Levers_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Levers_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Levers_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Lever_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
    return {};
}

static i32 Lever_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Lever_GetOutputName(GIZMO *gizmo, i32 output_index) {
    if (output_index == 1) {
        return "Down(Goodie)";
    }

    if (output_index != 2) {
        if (output_index == 0) {
            return "Down";
        }

        return NULL;
    }

    return "Down(Baddie)";
}

static i32 Lever_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Lever_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Lever_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void Lever_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void *Levers_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
    return {};
}

static void Levers_ClearProgress(void *, void *progress_data) {
    LEVERPROGRESS *progress = (LEVERPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state = 0;
    progress->goodie_state = -1;
    progress->baddie_state = -1;
}

static void Levers_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Levers_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Levers_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Levers_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Levers_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Lever";
    addtype.fns.unknown1 = 12;
    addtype.fns.early_update_fn = NULL;
    addtype.prefix = "";
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Levers_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Levers_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Levers_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Levers_Draw;
    addtype.fns.get_gizmo_name_fn = Lever_GetGizmoName;
    addtype.fns.get_output_fn = Lever_GetOutput;
    addtype.fns.get_output_name_fn = Lever_GetOutputName;
    addtype.fns.get_num_outputs_fn = Lever_GetNumOutputs;
    addtype.fns.activate_fn = Lever_Activate;
    addtype.fns.activate_rev_fn = Lever_ActivateRev;
    addtype.fns.set_visibility_fn = Lever_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Levers_AllocateProgressData;
    addtype.fns.clear_progress_fn = Levers_ClearProgress;
    addtype.fns.store_progress_fn = Levers_StoreProgress;
    addtype.fns.reset_fn = Levers_Reset;
    addtype.fns.reserve_buffer_space_fn = Levers_ReserveBufferSpace;
    addtype.fns.load_fn = Levers_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    lever_gizmotype_id = type_id;

    return &addtype;
}
