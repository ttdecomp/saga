#include "legoapi/gizmos/ai.h"
#include "legoapi/world.h"
#include "decomp.h"
#include "nu2api/nucore/nustring.h"

static int AI_GetMaxGizmos(void *ai) {
    return 0x20;
}

static void AI_AddGizmos(GIZMOSYS *gizmo_sys, int gizmo_type, void *world_base_ptr, void *unused) {
    int current_offset;
    intptr_t index;

    if ((world_base_ptr != (void *)0xffffd510) && (0 < WORLD->something)) {
        index = 0;
        do {
            current_offset = index * 0xdc;
            // (91.66% match)
            //__asm__("" : "+r" (index));
            current_offset = current_offset + (intptr_t)world_base_ptr;
            index = index + 1;
            void *gizmo_addr = (void *)(current_offset + 0x2af0);
            AddGizmo(gizmo_sys, gizmo_type, NULL, gizmo_addr);
        } while (index < WORLD->something);
    }
    return;
}

static char *AI_GetGizmoName(GIZMO *gizmo) {
    if (!gizmo || !gizmo->object.void_ptr) {
        return 0;
    }

    GIZMOINTERNAL *data = (GIZMOINTERNAL *)gizmo->object.void_ptr;
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
        return (*((unsigned char *)gizmo + 0x60) >> 1 ^ 1) & 1;
    }
    //increases accuracy
    //__asm__("");

    return 0;
}

char *AI_GetOutputName(GIZMO *gizmo, int output_index) {
    static char name[] = "Processing";
    return name;
}

static int AI_GetNumOutputs(GIZMO *gizmo) {
    return 0x1;
}

void AI_Activate(GIZMO *gizmo, int param_2) {
    int gizmo_internal;

    if ((gizmo) && (gizmo_internal = *(int *)gizmo)) {
        int flag = (param_2 == 0);
        unsigned int val = *(unsigned char *)(gizmo_internal + 0x60);
        flag += flag;
        *(unsigned char *)(gizmo_internal + 0x60) = (val & 0xfd) | flag;
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
