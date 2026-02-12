#include "game/gizmo.h"
#include "decomp.h"
#include "game/gizmos/ai.h"
#include "game/gizmos/door.h"
#include "game/gizmos/edgizshadowmachine.h"
#include "game/gizmos/gizaimessage.h"
#include "game/gizmos/gizbombgen.h"
#include "game/gizmos/gizbuildits.h"
#include "game/gizmos/gizforce.h"
#include "game/gizmos/gizmopickups.h"
#include "game/gizmos/gizobstacles.h"
#include "game/gizmos/gizpanel.h"
#include "game/gizmos/gizportal.h"
#include "game/gizmos/gizrandom.h"
#include "game/gizmos/gizspecial.h"
#include "game/gizmos/giztimer.h"
#include "game/gizmos/giztorpmachine.h"
#include "game/gizmos/gizturrets.h"
#include "game/gizmos/grapples.h"
#include "game/gizmos/hatmachine.h"
#include "game/gizmos/lever.h"
#include "game/gizmos/minicut.h"
#include "game/gizmos/newblowup.h"
#include "game/gizmos/plugs.h"
#include "game/gizmos/push.h"
#include "game/gizmos/spinner.h"
#include "game/gizmos/technos.h"
#include "game/gizmos/teleport.h"
#include "game/gizmos/tubes.h"
#include "game/gizmos/zipups.h"
#include "nu2api.saga/nucore/nustring.h"

#include <string.h>

static int DefaultGizmo_GetOutput(GIZMO *, int, int) {
    return 0;
}

static char *DefaultGizmo_GetOutputName(GIZMO *, int) {
    return "default";
}

static int DefaultGizmo_GetNumOutputs(GIZMO *) {
    return 1;
}

ADDGIZMOTYPE Default_ADDGIZMOTYPE = {
    NULL,                       // name
    NULL,                       // prefix
    -1,                         // id
    0,                          // unknown1
    0,                          // unknown2
    NULL,                       // get_max_gizmos_fn
    NULL,                       // add_gizmos_fn
    NULL,                       // early_update_fn
    NULL,                       // late_update_fn
    NULL,                       // draw_fn
    NULL,                       // panel_draw_fn
    NULL,                       // unknown_fn
    NULL,                       // get_gizmo_name_fn
    DefaultGizmo_GetOutput,     // get_output_fn
    DefaultGizmo_GetOutputName, // get_output_name_fn
    DefaultGizmo_GetNumOutputs, // get_num_outputs_fn
    NULL,                       // activate_fn
    NULL,                       // activate_rev_fn
    NULL,                       // set_visibility_fn
    NULL,                       // get_visibility_fn
    NULL,                       // get_pos_fn
    NULL,                       // using_special_fn
    NULL,                       // bolt_hit_plat_fn
    NULL,                       // get_best_bolt_target_fn
    NULL,                       // bolt_hit_fn
    NULL,                       // allocate_progress_data_fn
    NULL,                       // clear_progress_fn
    NULL,                       // store_progress_fn
    NULL,                       // reset_fn
    NULL,                       // reserve_buffer_space_fn
    NULL,                       // load_fn
    NULL,                       // post_load_fn
    NULL,                       // add_level_sfx_fn
};

// this technically has the wrong linkage, it does not have a C++ mangled name in the original binary... maybe should be
// in a different file?
static REGISTERGIZMOTYPEFN GizmoTypesLSW[] = {GizObstacles_RegisterGizmo,
                                              GizBuildIts_RegisterGizmo,
                                              GizForce_RegisterGizmo,
                                              NewBlowup_RegisterGizmo,
                                              GizmoPickups_RegisterGizmo,
                                              Levers_RegisterGizmo,
                                              Spinner_RegisterGizmo,
                                              MiniCut_RegisterGizmo,
                                              Tubes_RegisterGizmo,
                                              ZipUps_RegisterGizmo,
                                              GizTurrets_RegisterGizmo,
                                              GizBombGen_RegisterGizmo,
                                              AI_RegisterGizmo,
                                              GizSpecial_RegisterGizmo,
                                              GizAIMessage_RegisterGizmo,
                                              GizTimer_RegisterGizmo,
                                              GizRandom_RegisterGizmo,
                                              GizPanel_RegisterGizmo,
                                              HatMachine_RegisterGizmo,
                                              Push_RegisterGizmo,
                                              Door_RegisterGizmo,
                                              Teleport_RegisterGizmo,
                                              GizTorpMachine_RegisterGizmo,
                                              EdGizShadowMachine_RegisterGizmo,
                                              Portal_RegisterGizmo,
                                              Grapples_RegisterGizmo,
                                              Plugs_RegisterGizmo,
                                              Technos_RegisterGizmo,
                                              NULL};

#define GIZMO_TYPES_LSW_COUNT ((sizeof(GizmoTypesLSW) / sizeof(REGISTERGIZMOTYPEFN)) - 1)

GIZMOTYPES *gizmotypes;

VARIPTR *GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *buffer_end, int size) {
    VARIPTR *ptr = NULL;

    if (buffer_end != NULL && buffer != NULL) {
        if (buffer_end->addr > buffer->addr + size) {
            ptr = (VARIPTR *)ALIGN(buffer->addr, 16);
            buffer->addr = (usize)ptr + size;
            memset(ptr, 0, size);
        }
    }

    return ptr;
}

void RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *buffer_end, REGISTERGIZMOTYPEFN *register_gizmo_type_fns,
                        int unknown) {
    ADDGIZMOTYPE *addgizmo;
    GIZMOTYPES *types;
    GIZMOTYPE *gizmo;
    void *pvVar4;
    VARIPTR *pvVar3;
    int i;

    if (gizmotypes != NULL || register_gizmo_type_fns == NULL || *register_gizmo_type_fns == NULL) {
        return;
    }

    int ntypes = 0;
    for (; register_gizmo_type_fns[ntypes] != NULL; ntypes++) {
    }

    types = (GIZMOTYPES *)GizmoBufferAlloc(buffer, buffer_end, 0xc);
    gizmotypes = types;
    if (types == NULL) {
        return;
    }

    types->unknown = unknown;
    types->types = (GIZMOTYPE *)GizmoBufferAlloc(buffer, buffer_end, ntypes * sizeof(GIZMOTYPE));
    gizmo = gizmotypes->types;

    if (gizmo == NULL) {
        return;
    }

    gizmotypes->count = ntypes;

    for (int n = 0; n < gizmotypes->count; n++, gizmo++) {
        addgizmo = register_gizmo_type_fns[n](n);

        if (*addgizmo->prefix != '\0' && n != 0) {
            i = 0;
            do {
                while (gizmotypes->types[i].prefix[0] == '\0') {
                    i = i + 1;
                    if (i == n)
                        goto copy_data;
                }
                NuStrICmp(addgizmo->prefix, gizmotypes->types[i].prefix);
                i = i + 1;
            } while (i != n);
        }

    copy_data:
        NuStrCpy(gizmo->name, addgizmo->name);
        NuStrNCpy(gizmo->prefix, addgizmo->prefix, sizeof(gizmo->prefix));
        gizmo->fns.unknown1 = addgizmo->fns.unknown1;
        gizmo->fns.get_max_gizmos_fn = addgizmo->fns.get_max_gizmos_fn;
        gizmo->fns.add_gizmos_fn = addgizmo->fns.add_gizmos_fn;
        gizmo->fns.early_update_fn = addgizmo->fns.early_update_fn;
        gizmo->fns.late_update_fn = addgizmo->fns.late_update_fn;
        gizmo->fns.draw_fn = addgizmo->fns.draw_fn;
        gizmo->fns.panel_draw_fn = addgizmo->fns.panel_draw_fn;
        gizmo->fns.get_gizmo_name_fn = addgizmo->fns.get_gizmo_name_fn;
        gizmo->fns.get_output_fn = addgizmo->fns.get_output_fn;
        gizmo->fns.get_output_name_fn = addgizmo->fns.get_output_name_fn;
        gizmo->fns.get_num_outputs_fn = addgizmo->fns.get_num_outputs_fn;
        gizmo->fns.activate_fn = addgizmo->fns.activate_fn;
        gizmo->fns.activate_rev_fn = addgizmo->fns.activate_rev_fn;
        gizmo->fns.set_visibility_fn = addgizmo->fns.set_visibility_fn;
        gizmo->fns.get_visibility_fn = addgizmo->fns.get_visibility_fn;
        gizmo->fns.get_pos_fn = addgizmo->fns.get_pos_fn;
        gizmo->fns.using_special_fn = addgizmo->fns.using_special_fn;
        gizmo->fns.bolt_hit_plat_fn = addgizmo->fns.bolt_hit_plat_fn;
        gizmo->fns.get_best_bolt_target_fn = addgizmo->fns.get_best_bolt_target_fn;
        gizmo->fns.bolt_hit_fn = addgizmo->fns.bolt_hit_fn;
        gizmo->fns.allocate_progress_data_fn = addgizmo->fns.allocate_progress_data_fn;
        gizmo->fns.clear_progress_fn = addgizmo->fns.clear_progress_fn;
        gizmo->fns.store_progress_fn = addgizmo->fns.store_progress_fn;
        gizmo->fns.reset_fn = addgizmo->fns.reset_fn;
        gizmo->fns.reserve_buffer_space_fn = addgizmo->fns.reserve_buffer_space_fn;
        gizmo->fns.load_fn = addgizmo->fns.load_fn;
        gizmo->fns.post_load_fn = addgizmo->fns.post_load_fn;
        gizmo->fns.add_level_sfx_fn = addgizmo->fns.add_level_sfx_fn;

        pvVar4 = (void *)addgizmo->fns.allocate_progress_data_fn;

        if (pvVar4 != NULL && gizmotypes->unknown != 0) {
            pvVar3 = GizmoBufferAlloc(buffer, buffer_end, gizmotypes->unknown << 2);
            gizmo->buffer = pvVar3;
            if (types != NULL && gizmotypes->unknown > 0) {
                i = 0;
                while (1) {
                    void *pvVar5 = gizmo->fns.allocate_progress_data_fn(buffer, buffer_end);
                    pvVar3[i].void_ptr = pvVar5;
                    i = i + 1;
                    if (gizmotypes->unknown <= i)
                        break;
                    pvVar3 = gizmo->buffer;
                }
            }
        }
    }
}

void RegisterGizmoTypes_LSW(VARIPTR *buffer, VARIPTR *buffer_end) {
    REGISTERGIZMOTYPEFN gizmo_types[GIZMO_TYPES_LSW_COUNT + 1];
    memcpy(gizmo_types, GizmoTypesLSW, sizeof(GizmoTypesLSW));
    RegisterGizmoTypes(buffer, buffer_end, gizmo_types, 12);
}

GIZMO *AddGizmo(GIZMOSYS *gizmo_sys, int type_id, char *name, void *object) {
    if (gizmotypes == NULL || gizmo_sys == NULL) {
        return NULL;
    }

    if (name != NULL) {
        type_id = GizmoGetTypeIDByName(gizmo_sys, name);
    }

    if (type_id > -1 && gizmotypes->count > type_id) {
        GIZMOSET *set = &gizmo_sys->sets[type_id];
        if (set->count < set->max_count) {
            GIZMO *gizmo = &set->gizmos[set->count];
            if (gizmo != NULL) {
                if (object != NULL) {
                    gizmo->object.void_ptr = object;
                }

                gizmo->unknown = 0;
                gizmo->type_id = (u8)type_id;
                set->count++;

                return gizmo;
            }
        }
    }

    return NULL;
}

int GizmoGetTypeIDByName(GIZMOSYS_s *gizmo_sys, char *name) {
    if (gizmotypes == NULL || name == NULL || gizmo_sys == NULL || gizmotypes->count <= 0) {
        return -1;
    }

    for (int i = 0; i < gizmotypes->count; i++) {
        if (NuStrCmp(gizmotypes->types[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

void GizmoSetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int visibility, int unknown) {
    if (gizmotypes == NULL || gizmo == NULL || gizmo_sys == NULL || gizmo->type_id >= gizmotypes->count ||
        gizmotypes->types[gizmo->type_id].fns.set_visibility_fn == NULL) {
        return;
    }

    if (visibility == 0 && gizmotypes->types[gizmo->type_id].fns.activate_fn != NULL) {
        gizmotypes->types[gizmo->type_id].fns.activate_fn(gizmo, 0);
    }

    gizmotypes->types[gizmo->type_id].fns.set_visibility_fn(gizmo, visibility);
}

int GizmoGetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo) {

    if (gizmotypes == NULL || gizmo == NULL || gizmo_sys == NULL || gizmo->type_id >= gizmotypes->count ||
        gizmotypes->types[gizmo->type_id].fns.get_visibility_fn == NULL) {
        return 0;
    }

    return gizmotypes->types[gizmo->type_id].fns.get_visibility_fn(gizmo);
}

void GizmoActivate(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int unknown1, int unknown2) {
    if (gizmotypes == NULL || gizmo == NULL || gizmo_sys == NULL || gizmo->type_id >= gizmotypes->count) {
        return;
    }

    GIZMOTYPE *type = &gizmotypes->types[gizmo->type_id];
    if (type->fns.activate_fn == NULL) {
        return;
    }

    if (unknown1 != 0 && type->fns.set_visibility_fn != NULL) {
        type->fns.set_visibility_fn(gizmo, 1);
    }

    type->fns.activate_fn(gizmo, unknown1);
}

char *GizmoGetOutputName(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int output_index) {
    if (gizmotypes == NULL || gizmo == NULL || gizmo_sys == NULL) {
        return NULL;
    }

    if (gizmo->type_id >= gizmotypes->count || gizmotypes->types[gizmo->type_id].fns.get_output_name_fn == NULL) {
        return "default";
    }

    return gizmotypes->types[gizmo->type_id].fns.get_output_name_fn(gizmo, output_index);
}

int GizmoGetOutput(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int unknown1, int unknown2) {
    if (gizmotypes == NULL || gizmo == NULL || gizmo_sys == NULL) {
        return 0;
    }

    if (gizmo->type_id >= gizmotypes->count || gizmotypes->types[gizmo->type_id].fns.get_output_fn == NULL) {
        return 0;
    }

    return gizmotypes->types[gizmo->type_id].fns.get_output_fn(gizmo, unknown1, unknown2);
}

void GizmoSysEarlyUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time) {
    if (gizmotypes == NULL || gizmo_sys == NULL) {
        return;
    }

    GIZMOTYPE *type = gizmotypes->types;
    GIZMOSET *set = gizmo_sys->sets;
    for (i32 i = 0; i < gizmotypes->count; i++, set++, type++) {
        if (type->fns.early_update_fn != NULL) {
            type->fns.early_update_fn(world_info, set->unknown, delta_time);
        }
    }
}

void GizmoSysLateUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time) {
    if (gizmotypes == NULL || gizmo_sys == NULL) {
        return;
    }

    GIZMOTYPE *type = gizmotypes->types;
    GIZMOSET *set = gizmo_sys->sets;
    for (i32 i = 0; i < gizmotypes->count; i++, set++, type++) {
        if (type->fns.late_update_fn != NULL) {
            type->fns.late_update_fn(world_info, set->unknown, delta_time);
        }
    }
}

void GizmoSysDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time) {
    if (gizmotypes == NULL || gizmo_sys == NULL) {
        return;
    }

    GIZMOTYPE *type = gizmotypes->types;
    GIZMOSET *set = gizmo_sys->sets;
    for (i32 i = 0; i < gizmotypes->count; i++, set++, type++) {
        if (type->fns.draw_fn != NULL) {
            type->fns.draw_fn(world_info, set->unknown, delta_time);
        }
    }
}

void GizmoSysPanelDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time) {
    if (gizmotypes == NULL || gizmo_sys == NULL) {
        return;
    }

    GIZMOTYPE *type = gizmotypes->types;
    GIZMOSET *set = gizmo_sys->sets;
    for (i32 i = 0; i < gizmotypes->count; i++, set++, type++) {
        if (type->fns.panel_draw_fn != NULL) {
            type->fns.panel_draw_fn(world_info, set->unknown, delta_time);
        }
    }
}

int GizmoSys_BoltHitPlat(GIZMOSYS *gizmo_sys, void *world_info, BOLT *bolt, unsigned char *unknown) {
    if (gizmotypes == NULL || bolt == NULL || gizmo_sys == NULL) {
        return 0;
    }

    GIZMOTYPE *type = gizmotypes->types;
    GIZMOSET *set = gizmo_sys->sets;
    for (i32 i = 0; i < gizmotypes->count; i++, set++, type++) {
        if (type->fns.bolt_hit_plat_fn != NULL) {
            i32 result = type->fns.bolt_hit_plat_fn(world_info, set->unknown, bolt, unknown);
            if (result != 0) {
                return 1;
            }
        }
    }

    return 0;
}

int ResetGizmoType(GIZMOSYS *gizmo_sys, int type_id, char *name) {
    if (name != NULL && type_id == -1) {
        type_id = GizmoGetTypeIDByName(gizmo_sys, name);
    }

    if (type_id == -1) {
        return 0;
    }

    gizmo_sys->sets[type_id].count = 0;
    memset(gizmo_sys->sets[type_id].gizmos, 0, gizmo_sys->sets[type_id].max_count * sizeof(GIZMO));

    return 1;
}

void GizmoSysClearLevelProgress(void *unknown, int type_id) {
    GIZMOTYPE *type = gizmotypes->types;
    if (gizmotypes == NULL || gizmotypes->count <= 0) {
        return;
    }

    if (type_id < 0) {
        for (i32 i = 0; i < gizmotypes->count; i++, type++) {
            if (type->fns.clear_progress_fn != NULL) {
                type->fns.clear_progress_fn(unknown, NULL);
            }
        }
    } else {
        for (i32 i = 0; i < gizmotypes->count; i++, type++) {
            if (type->fns.clear_progress_fn != NULL) {
                void *progress = NULL;
                if (type->buffer != NULL && type_id < gizmotypes->unknown) {
                    progress = type->buffer[type_id].void_ptr;
                }

                type->fns.clear_progress_fn(unknown, progress);
            }
        }
    }
}
