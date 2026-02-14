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

char *Teleport_GetOutputName(GIZMO *gizmo, int output_index) {
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

ADDGIZMOTYPE *Teleport_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Teleport";
    addtype.prefix = "TLT_";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Teleport_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Teleport_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Teleport_GetGizmoName;
    addtype.fns.get_output_fn = Teleport_GetOutput;
    addtype.fns.get_output_name_fn = Teleport_GetOutputName;
    addtype.fns.get_num_outputs_fn = Teleport_GetNumOutputs;
    addtype.fns.activate_fn = Teleport_Activate;
    addtype.fns.activate_rev_fn = Teleport_ActivateRev;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    teleport_gizmotype_id = type_id;

    return &addtype;
}
