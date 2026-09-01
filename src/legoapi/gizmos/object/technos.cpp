#include "legoapi/gizmos/object/technos.h"

#include "decomp.h"
#include "legoapi/legoapi_types.h"

struct TECHNOPROGRESS {
    i32 state[2];
};

i32 techno_gizmotype_id = -1;

static i32 Technos_GetMaxGizmos(void *techno) {
    UNIMPLEMENTED();
    return {};
}

static void Technos_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
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
    TECHNO_s *techno = gizmo != NULL ? static_cast<TECHNO_s *>(gizmo->object) : NULL;
    return techno != NULL ? techno->name : NULL;
}

static i32 Techno_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Techno_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Techno_GetNumOutputs(GIZMO *) {
    return 1;
}

static void Techno_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static void Techno_SetVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Techno_GetPos(GIZMO *gizmo) {
    TECHNO_s *techno = gizmo != NULL ? static_cast<TECHNO_s *>(gizmo->object) : NULL;
    return techno != NULL ? static_cast<i32>(reinterpret_cast<usize>(&techno->position)) : 0;
}

static void *Technos_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 8);
}

static void Technos_ClearProgress(void *, void *progress_data) {
    TECHNOPROGRESS *progress = (TECHNOPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state[0] = -1;
    progress->state[1] = -1;
}

static void Technos_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Technos_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Technos_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Technos_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Technos_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Techno";
    addtype.prefix = "";
    addtype.fns.unknown1 = 8;
    addtype.fns.early_update_fn = Technos_EarlyUpdate;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Technos_GetMaxGizmos;
    addtype.fns.get_pos_fn = Techno_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Technos_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Technos_LateUpdate;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Technos_Draw;
    addtype.fns.get_gizmo_name_fn = Techno_GetGizmoName;
    addtype.fns.get_output_fn = Techno_GetOutput;
    addtype.fns.get_output_name_fn = Techno_GetOutputName;
    addtype.fns.get_num_outputs_fn = Techno_GetNumOutputs;
    addtype.fns.activate_fn = Techno_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Techno_SetVisibility;
    addtype.fns.allocate_progress_data_fn = Technos_AllocateProgressData;
    addtype.fns.clear_progress_fn = Technos_ClearProgress;
    addtype.fns.store_progress_fn = Technos_StoreProgress;
    addtype.fns.reset_fn = Technos_Reset;
    addtype.fns.reserve_buffer_space_fn = Technos_ReserveBufferSpace;
    addtype.fns.load_fn = Technos_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    techno_gizmotype_id = type_id;

    return &addtype;
}
