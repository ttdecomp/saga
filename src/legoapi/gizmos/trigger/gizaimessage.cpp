#include "legoapi/gizmos/trigger/gizaimessage.h"

#include <stdio.h>

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nulist.h"

extern GIZAIMESSAGESYS_s *gizaimessagesys;

char *GizAIMessage_GetName(GIZAIMESSAGE_s *message);

i32 gizaimessage_gizmotype_id = -1;

static char *gizaimessage_prefix = "msg_";

static i32 GizAIMessage_GetMaxGizmos(void *) {
    return 0x40;
}

static void GizAIMessage_AddGizmos(GIZMOSYS *gizmo_sys, i32, void *, void *) {
    if (gizaimessagesys == NULL) {
        return;
    }

    GIZAIMESSAGE_s *message = (GIZAIMESSAGE_s *)NuLinkedListGetHead(&gizaimessagesys->active_list);
    while (message != NULL) {
        if ((message->flags & GIZAIMESSAGE_FLAG_ADD_GIZMO) != 0) {
            AddGizmo(gizmo_sys, gizaimessage_gizmotype_id, NULL, message);
            message->flags |= GIZAIMESSAGE_FLAG_GIZMO_ADDED;
        }
        message = (GIZAIMESSAGE_s *)NuLinkedListGetNext(&gizaimessagesys->active_list, &message->links);
    }
}

static char *GizAIMessage_GetGizmoName(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return GizAIMessage_GetName((GIZAIMESSAGE_s *)gizmo->object);
}

static i32 GizAIMessage_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    if ((gizmo == NULL) || (gizmo->object == NULL) || ((u32)output_index > 7)) {
        return 0;
    }

    GIZAIMESSAGE_s *message = (GIZAIMESSAGE_s *)gizmo->object;
    return message->value == (f32)message->output_values[output_index];
}

static char *GizAIMessage_GetOutputName(GIZMO *gizmo, i32 output_index) {
    static char returnstr[4];

    if ((gizmo == NULL) || (gizmo->object == NULL) || ((u32)output_index > 7)) {
        return NULL;
    }

    GIZAIMESSAGE_s *message = (GIZAIMESSAGE_s *)gizmo->object;
    sprintf(returnstr, "%i", (i32)message->output_values[output_index]);
    return returnstr;
}

static i32 GizAIMessage_GetNumOutputs(GIZMO *gizmo) {
    if ((gizmo == NULL) || (gizmo->object == NULL)) {
        return 0;
    }
    return ((GIZAIMESSAGE_s *)gizmo->object)->output_count;
}

ADDGIZMOTYPE *GizAIMessage_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Message";
    addtype.prefix = gizaimessage_prefix;
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizAIMessage_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizAIMessage_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizAIMessage_GetGizmoName;
    addtype.fns.get_output_fn = GizAIMessage_GetOutput;
    addtype.fns.get_output_name_fn = GizAIMessage_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizAIMessage_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizaimessage_gizmotype_id = type_id;

    return &addtype;
}
