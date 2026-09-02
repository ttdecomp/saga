#include "legoapi/gizmos/trigger/ai.h"

#include "decomp.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"

static i32 AI_GetMaxGizmos(void *) {
    return 32;
}

static void AI_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *unused) {
    WORLDINFO *world;
    i32 i;

    world = (WORLDINFO *)world_ptr;

    // ORIG_BUG: This comparison can never fail.
    if (world->processors != NULL) { // NOLINT
        for (i = 0; i < WORLD->processor_count; i++) {
            AddGizmo(gizmo_sys, type_id, NULL, &world->processors[i]);
        }
    }
}

static char *AI_GetGizmoName(GIZMO *gizmo) {
    static char name[16];
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }

    LEVELSCRIPTPROCESS *level_process = static_cast<LEVELSCRIPTPROCESS *>(gizmo->object);
    if (NuStrLen(level_process->name) != 0) {
        return level_process->name;
    }

    AISCRIPTPROCESS *process = &level_process->processor;
    const char *script_name = process->script != NULL && process->script->name != NULL ? process->script->name : "";
    NuStrNCpy(name, script_name, sizeof(name));

    const char *context_name = NULL;
    if (process->unknown_a8 != NULL) {
        context_name = process->unknown_a8->name;
    } else if (process->unknown_a4 != NULL) {
        context_name = process->unknown_a4->name;
    } else if (process->unknown_a0 != NULL) {
        context_name = process->unknown_a0->name;
    }

    if (context_name != NULL && NuStrLen(name) + NuStrLen("()") + NuStrLen(context_name) <= 15) {
        NuStrCat(name, "(");
        NuStrCat(name, context_name);
        NuStrCat(name, ")");
    }
    return name;
}

static i32 AI_GetOutput(GIZMO *gizmo, i32, i32) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    LEVELSCRIPTPROCESS *process = static_cast<LEVELSCRIPTPROCESS *>(gizmo->object);
    return process->processor.is_disabled == 0;
}

char *AI_GetOutputName(GIZMO *, i32) {
    static char name[] = "Processing";
    return name;
}

static i32 AI_GetNumOutputs(GIZMO *) {
    return 1;
}

void AI_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL && gizmo->object != NULL) {
        LEVELSCRIPTPROCESS *process = static_cast<LEVELSCRIPTPROCESS *>(gizmo->object);
        process->processor.is_disabled = active == 0;
    }
}

ADDGIZMOTYPE *AI_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "AIProcessor";
    addtype.prefix = "ai_";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = AI_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = AI_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = AI_GetGizmoName;
    addtype.fns.get_output_fn = AI_GetOutput;
    addtype.fns.get_output_name_fn = AI_GetOutputName;
    addtype.fns.get_num_outputs_fn = AI_GetNumOutputs;
    addtype.fns.activate_fn = AI_Activate;
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

    return &addtype;
}
