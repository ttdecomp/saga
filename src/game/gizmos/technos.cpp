#include "game/gizmos/technos.h"

#include "decomp.h"

int techno_gizmotype_id = -1;

static int Technos_GetMaxGizmos(void *techno) {
    UNIMPLEMENTED();
}

static void Technos_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

static void Technos_EarlyUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Technos_LateUpdate(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Technos_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Techno_GetGizmoName(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static int Techno_GetOutput(GIZMO *gizmo, int, int) {
    UNIMPLEMENTED();
}

static char *Techno_GetOutputName(GIZMO *gizmo, int output_index) {
    UNIMPLEMENTED();
}

static int Techno_GetNumOutputs(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Techno_Activate(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static void Techno_SetVisibility(GIZMO *gizmo, int) {
    UNIMPLEMENTED();
}

static int Techno_GetPos(GIZMO *gizmo) {
    UNIMPLEMENTED();
}

static void Technos_AllocateProgressData(VARIPTR *, VARIPTR *) {
    UNIMPLEMENTED();
}

static void Technos_ClearProgress(void *, void *) {
    UNIMPLEMENTED();
}

static void Technos_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Technos_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static int Technos_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

static int Technos_Load(void *, void *) {
    UNIMPLEMENTED();
}

void Technos_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Techno";
    addtype.prefix = "";
    addtype.unknown1 = 8;
    addtype.early_update_fn = Technos_EarlyUpdate;
    addtype.panel_draw_fn = NULL;
    addtype.get_visibility_fn = NULL;
    addtype.get_max_gizmos_fn = Technos_GetMaxGizmos;
    addtype.get_pos_fn = Techno_GetPos;
    addtype.using_special_fn = NULL;
    addtype.add_gizmos_fn = Technos_AddGizmos;
    addtype.bolt_hit_plat_fn = NULL;
    addtype.get_best_bolt_target_fn = NULL;
    addtype.late_update_fn = Technos_LateUpdate;
    addtype.bolt_hit_fn = NULL;
    addtype.draw_fn = Technos_Draw;
    addtype.get_gizmo_name_fn = Techno_GetGizmoName;
    addtype.get_output_fn = Techno_GetOutput;
    addtype.get_output_name_fn = Techno_GetOutputName;
    addtype.get_num_outputs_fn = Techno_GetNumOutputs;
    addtype.activate_fn = Techno_Activate;
    addtype.activate_rev_fn = NULL;
    addtype.set_visibility_fn = Techno_SetVisibility;
    addtype.allocate_progress_data_fn = Technos_AllocateProgressData;
    addtype.clear_progress_fn = Technos_ClearProgress;
    addtype.store_progress_fn = Technos_StoreProgress;
    addtype.reset_fn = Technos_Reset;
    addtype.reserve_buffer_space_fn = Technos_ReserveBufferSpace;
    addtype.load_fn = Technos_Load;
    addtype.post_load_fn = NULL;
    addtype.add_level_sfx_fn = NULL;
    techno_gizmotype_id = type_id;
}
