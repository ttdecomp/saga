#include "legoapi/gizmos/ai.h"
#include "decomp.h"
#include "legoapi/world.h"
#include "nu2api/nucore/nustring.h"

static int AI_GetMaxGizmos(void *ai) {
    return 0x20;
}

static void AI_AddGizmos(GIZMOSYS *gizmo_sys, int gizmo_type, void *world_base_ptr, void *unused) {
    if ((world_base_ptr != INVALID_WORLD_PTR) && (0 < WORLD->gizmo_count)) {
        WORLDINFO *local_world = (WORLDINFO *)world_base_ptr;
        for (int index = 0; index < WORLD->gizmo_count; index++) {
            AddGizmo(gizmo_sys, gizmo_type, NULL, &local_world->gizmos[index]);
        }
    }
    return;
}

static char *AI_GetGizmoName(GIZMO *gizmo) {
    if (!gizmo || !gizmo->object.void_ptr) {
        return 0;
    }

    AIGIZMOINTERNAL *data = (AIGIZMOINTERNAL *)gizmo->object.void_ptr;
    if (NuStrLen((char *)data)) {
        return (char *)data;
    }

    static char name[16];

    if (data->def && data->def->name_ptr) {
        NuStrNCpy(name, data->def->name_ptr, 16);
    } else {
        NuStrCpy(name, "");
    }

    int current_len = NuStrLen(name);
    char *suffix = 0;
    int check_len = 0;
    if ((suffix = data->suffix_1) != 0) {
        check_len = current_len + NuStrLen("()");
        if (check_len + NuStrLen(suffix) > 15) {
            return name;
        }
        NuStrCat(name, "(");
    } else {
        if ((suffix = data->suffix_2) != 0) {
            check_len = current_len + NuStrLen("()");
            if ((char)check_len + NuStrLen(suffix) > 15) {
                return name;
            }
            NuStrCat(name, "(");
        } else {
            if ((suffix = data->suffix_3) == 0) {
                return name;
            }

            check_len = (char)current_len + NuStrLen("()");
            if (check_len + NuStrLen(suffix) > 15) {
                return name;
            }
            NuStrCat(name, "(");
        }
    }

    NuStrCat(name, suffix);
    NuStrCat(name, ")");

    return name;
}

static int AI_GetOutput(GIZMO *gizmo, int param_2, int param_3) {
    if (gizmo && (gizmo = *(GIZMO **)gizmo)) {
        return ((gizmo->output) >> 1 ^ 1) & 1;
    }
    // increases accuracy to 100%
    //__asm__("");

    return 0;
}

char *AI_GetOutputName(GIZMO *gizmo, int output_index) {
    static char name[0x40] = "Processing";
    return name;
}

static int AI_GetNumOutputs(GIZMO *gizmo) {
    return 0x1;
}

void AI_Activate(GIZMO *gizmo, int param_2) {
    GIZMO *gizmo_internal;

    if ((gizmo) && (gizmo_internal = gizmo)) {
        int flag = (param_2 == 0);
        unsigned int val = (gizmo_internal->output);
        flag += flag;
        (gizmo_internal->output) = (val & 0xfd) | flag;
    }
}

ADDGIZMOTYPE *AI_RegisterGizmo(int type_id) {
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
