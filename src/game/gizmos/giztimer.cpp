#include "game/gizmos/giztimer.h"

#include "decomp.h"
#include "gameapi.saga/edtools/edfile.h"
#include "lostandfound/qrand.h"

int giztimer_gizmotype_id = -1;

int GizTimer_GetMaxGizmos(void *timer) {
    UNIMPLEMENTED();
}

void GizTimer_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *) {
    UNIMPLEMENTED();
}

void GizTimer_Update(void *world_info, void *, float delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_info;
    for (int i = 0; i < world->giz_timers_count; i++) {
        GIZTIMER *timer = &world->giz_timers[i];

        if (timer->time_remaining >= 0.0f) {
            timer->time_remaining -= delta_time;
        }
    }
}

char *GizTimer_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return 0;
    }

    return gizmo->object.timer->name;
}

int GizTimer_GetOutput(GIZMO *gizmo, int, int) {
    if (gizmo->object.timer->flags & 1) {
        return gizmo->object.timer->time_remaining <= 0.0f;
    }

    return 0;
}

char *GizTimer_GetOutputName(GIZMO *gizmo, int output_index) {
    return "Ping";
}

int GizTimer_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

void GizTimer_Activate(GIZMO *gizmo, int unknown) {
    // if ((gizmo->object.timer->flags & 2) == 0) {
    //     gizmo->object.timer->time_remaining = gizmo->object.timer->start_time;
    // } else {
    //     gizmo->object.timer->time_remaining = QRAND_FLOAT() * gizmo->object.timer->start_time;
    // }

    // gizmo->object.timer->flags = gizmo->object.timer->flags & 0xfe | ~-(unknown == 0) & 1U;

    UNIMPLEMENTED();
}

int GizTimer_ReserveBufferSpace(void *, int) {
    UNIMPLEMENTED();
}

int GizTimer_Load(void *world_info, void *) {
    NUVEC vec;
    char buffer[16];

    WORLDINFO *world = (WORLDINFO *)world_info;
    if (world->giz_timers_count == 0) {
        EdFileReadInt();
        int count = EdFileReadInt();

        for (int i = 0; i < count; i++) {
            int length = EdFileReadInt();
            EdFileRead(buffer, length);
            EdFileReadFloat();
            EdFileReadUnsignedShort();
            EdFileReadNuVec(&vec);
        }

        return 1;
    }

    return 0;
}

ADDGIZMOTYPE *GizTimer_RegisterGizmo(int type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTimer";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = (GIZMOEARLYUPDATEFN)GizTimer_Update;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTimer_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTimer_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizTimer_GetGizmoName;
    addtype.fns.get_output_fn = GizTimer_GetOutput;
    addtype.fns.get_output_name_fn = GizTimer_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizTimer_GetNumOutputs;
    addtype.fns.activate_fn = GizTimer_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = GizTimer_ReserveBufferSpace;
    addtype.fns.load_fn = GizTimer_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    giztimer_gizmotype_id = type_id;

    return &addtype;
}
