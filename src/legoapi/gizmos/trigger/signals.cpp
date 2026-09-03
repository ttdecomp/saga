#include "legoapi/gizmos/trigger/signals.h"

#include "decomp.h"
#include "legoapi/world/level.h"

#include <string.h>

static i32 Signals_GetMaxGizmos(void *world_info) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
    return world != NULL ? world->current_level->max_signals : 0;
}

static void Signals_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    UNIMPLEMENTED();
}

static void Signals_Update(void *, void *, float) {
    UNIMPLEMENTED();
}

static void Signals_Draw(void *, void *, float) {
    UNIMPLEMENTED();
}

static char *Signal_GetGizmoName(GIZMO *gizmo) {
    SIGNAL *signal = gizmo != NULL ? static_cast<SIGNAL *>(gizmo->object) : NULL;
    return signal != NULL ? signal->name : NULL;
}

static i32 Signal_GetOutput(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static char *Signal_GetOutputName(GIZMO *gizmo, i32 output_index) {
    UNIMPLEMENTED();
    return {};
}

static i32 Signal_GetNumOutputs(GIZMO *) {
    return 1;
}

static void Signal_Activate(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static i32 Signal_ActivateRev(GIZMO *gizmo, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static void Signal_SetGizmoVisibility(GIZMO *gizmo, i32) {
    UNIMPLEMENTED();
}

static NUVEC *Signal_GetPos(GIZMO *gizmo) {
    SIGNAL *signal = gizmo != NULL ? static_cast<SIGNAL *>(gizmo->object) : NULL;
    return signal != NULL ? &signal->position : NULL;
}

static void *Signals_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(SIGNALPROGRESS));
}

static void Signals_ClearProgress(void *, void *progress_data) {
    SIGNALPROGRESS *progress = static_cast<SIGNALPROGRESS *>(progress_data);
    if (progress != NULL) {
        memset(progress->suit_letters, 0, sizeof(progress->suit_letters));
        progress->active_mask = 0xffffffff;
        progress->visible_mask = 0xffffffff;
    }
}

static void Signals_StoreProgress(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void Signals_Reset(void *, void *, void *) {
    UNIMPLEMENTED();
}

static void *Signals_ReserveBufferSpace(void *) {
    UNIMPLEMENTED();
    return {};
}

static i32 Signals_Load(void *, void *) {
    UNIMPLEMENTED();
    return {};
}

ADDGIZMOTYPE *Signals_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Signal";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x28;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = Signals_GetMaxGizmos;
    addtype.fns.get_pos_fn = Signal_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = Signals_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = Signals_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = Signals_Draw;
    addtype.fns.get_gizmo_name_fn = Signal_GetGizmoName;
    addtype.fns.get_output_fn = Signal_GetOutput;
    addtype.fns.get_output_name_fn = Signal_GetOutputName;
    addtype.fns.get_num_outputs_fn = Signal_GetNumOutputs;
    addtype.fns.activate_fn = Signal_Activate;
    addtype.fns.activate_rev_fn = Signal_ActivateRev;
    addtype.fns.set_visibility_fn = Signal_SetGizmoVisibility;
    addtype.fns.allocate_progress_data_fn = Signals_AllocateProgressData;
    addtype.fns.clear_progress_fn = Signals_ClearProgress;
    addtype.fns.store_progress_fn = Signals_StoreProgress;
    addtype.fns.reset_fn = Signals_Reset;
    addtype.fns.reserve_buffer_space_fn = Signals_ReserveBufferSpace;
    addtype.fns.load_fn = Signals_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;

    return &addtype;
}
