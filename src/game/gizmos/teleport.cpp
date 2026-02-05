#include "game/gizmos/teleport.h"

#include "decomp.h"

int teleport_gizmotype_id = -1;

static int Teleport_GetMaxGizmos(void *teleport) {
    UNIMPLEMENTED();
}

static void Teleport_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static char *Teleport_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Teleport_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Teleport_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Teleport_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Teleport_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Teleport_ActivateRev(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

ADDGIZMOTYPE* Teleport_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Teleport";
    addtype.prefix = "TLT_";
    addtype.unknown1 = 0;
    addtype.early_update_fn = NULL;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Teleport_GetMaxGizmos;
    addtype.get_pos_fn = NULL;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Teleport_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = NULL;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = NULL;
    addtype.get_gizmo_name_fn = Teleport_GetGizmoName;
    addtype.get_output_fn = Teleport_GetOutput;
    addtype.get_output_name_fn = Teleport_GetOutputName;
    addtype.get_num_outputs_fn = Teleport_GetNumOutputs;
    addtype.activate_fn = Teleport_Activate;
    addtype.activate_rev_fn = Teleport_ActivateRev;
    addtype.set_visibility_fn = NULL;
    addtype.allocate_progress_data_fn = NULL;
    addtype.clear_progress_fn = NULL;
    addtype.store_progress_fn = NULL;
    addtype.reset_fn = NULL;
    addtype.reserve_buffer_space_fn = NULL;
    addtype.load_fn = NULL;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    teleport_gizmotype_id = type_id;

    return &addtype;
}
