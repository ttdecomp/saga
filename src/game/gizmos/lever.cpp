#include "game/gizmos/lever.h"

#include "decomp.h"

int lever_gizmotype_id = -1;

static int Levers_GetMaxGizmos(void *lever) {
    UNIMPLEMENTED();
}

static void Levers_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
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
}

static int Lever_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Lever_GetOutputName(GIZMO *gizmo, int output_index) {
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

static int Lever_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Lever_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int LeverActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static void Lever_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Levers_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Levers_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Levers_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Levers_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Levers_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Levers_Load(void *, void *) {
    UNIMPLEMENTED();
}

void Levers_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Lever";
    addtype.id = 12;
    addtype.early_update_fn = NULL;
    addtype.prefix = "";
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Levers_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Levers_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Levers_Update;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Levers_Draw;
    addtype.get_gizmo_name_fn = Lever_GetGizmoName;
    addtype.get_output_fn = Lever_GetOutput;
    addtype.get_output_name_fn = Lever_GetOutputName;
    addtype.get_num_outputs_fn = Lever_GetNumOutputs;
    addtype.activate_fn = Lever_Activate;
    addtype.activate_rev_fn = LeverActivateRev;
    addtype.set_visibility_fn = Lever_SetVisibility;
    addtype.allocate_progress_data_fn = Levers_AllocateProgressData;
    addtype.clear_progress_fn = Levers_ClearProgress;
    addtype.store_progress_fn = Levers_StoreProgress;
    addtype.reset_fn = Levers_Reset;
    addtype.reserve_buffer_space_fn = Levers_ReserveBufferSpace;
    addtype.load_fn = Levers_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    lever_gizmotype_id = type_id;
}
