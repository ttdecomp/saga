#include "legoapi/gizmos/traps/giztorpmachine.h"

#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

void EnableShadowMapRendering(i32 enable);
void ResetShadowMapRendering();
void Draw3DObject(WORLDINFO_s *world, i32 object_index, NUVEC *position, u16 x_rotation, u16 y_rotation, u16 z_rotation,
                  f32 x_scale, f32 y_scale, f32 z_scale, i32 flags);

enum GIZTORP_LEVEL_OBJECT {
    GIZTORP_LEVEL_OBJECT_GLOW = 0x121,
    GIZTORP_LEVEL_OBJECT_PRIMARY = 0x122,
    GIZTORP_LEVEL_OBJECT_ALTERNATE = 0x123,
};

i32 giztorpmachine_gizmotype_id;

static i32 GizTorp_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_torp_machines : 0;
}

static void GizTorp_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world != NULL && world->giz_torp_machine_sys != NULL && world->giz_torp_machine_sys->count > 0) {
        i32 index = 0;
        do {
            GIZTORPMACHINE *machine = &world->giz_torp_machine_sys->machines[index];
            if (NuStrLen(machine->name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, machine);
            }
            ++index;
        } while (world->giz_torp_machine_sys->count > index);
    }
}

static void GizTorp_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTORPMACHINESYS *system = world->giz_torp_machine_sys;
    if (system == NULL || system->count <= 0) {
        return;
    }

    for (i32 index = 0; index < system->count; ++index) {
        GIZTORPMACHINE &machine = system->machines[index];
        if (machine.activation_time < 0.5f) {
            machine.activation_time = MIN(0.5f, machine.activation_time + FRAMETIME);
        }
    }
}

static void GizTorp_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->giz_torp_machine_sys == NULL || world->giz_torp_machine_sys->count == 0 ||
        world->lev_objs[GIZTORP_LEVEL_OBJECT_PRIMARY].active == 0 ||
        world->lev_objs[GIZTORP_LEVEL_OBJECT_ALTERNATE].active == 0) {
        return;
    }

    GIZTORPMACHINESYS *system = world->giz_torp_machine_sys;
    EnableShadowMapRendering(0);
    for (i32 index = 0; index < system->count; ++index) {
        GIZTORPMACHINE &machine = system->machines[index];
        if ((machine.flags & GIZTORPMACHINE_FLAG_VISIBLE) == 0 && editor_active == 0) {
            continue;
        }

        NUMTX matrix;
        NuMtxSetRotationY(&matrix, machine.y_rotation);
        NuMtxTranslate(&matrix, &machine.position);
        NuMtxPreScaleU(&matrix, system->scale);

        const i32 model_index = (machine.flags & GIZTORPMACHINE_FLAG_ALTERNATE_MODEL) != 0
                                    ? GIZTORP_LEVEL_OBJECT_ALTERNATE
                                    : GIZTORP_LEVEL_OBJECT_PRIMARY;
        if (world->lev_objs[model_index].active != 0) {
            NuSpecialDrawAt(&world->lev_objs[model_index].special, &matrix);
        }

        if ((machine.flags & GIZTORPMACHINE_FLAG_ACTIVE) != 0 && machine.activation_time > 0.0f) {
            const f32 activation_phase = machine.activation_time * 2.0f;
            if (activation_phase >= 0.3f) {
                const i32 opening_angle = (static_cast<i32>((activation_phase - 0.3f) / 0.7f * 16384.0f) >> 1) & 0x7fff;
                const f32 opening_scale = NuTrigTable[opening_angle] * (system->scale * 4.0f);
                const i32 pulse_angle =
                    (static_cast<i32>(GameTimer.time_elapsed_mod_seconds / 0.2f * 65536.0f) >> 1) & 0x7fff;
                const f32 effect_scale = (NuTrigTable[pulse_angle] * 0.1f + 1.0f) * opening_scale;

                Draw3DObject(world, GIZTORP_LEVEL_OBJECT_GLOW, &machine.position, 0, 0, 0, effect_scale, effect_scale,
                             effect_scale, 2);
                Draw3DObject(world, GIZTORP_LEVEL_OBJECT_ALTERNATE, &machine.position, 0, 0, 0, effect_scale,
                             effect_scale, effect_scale, 2);
            }
        }
    }
    ResetShadowMapRendering();
}

static char *GizTorp_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<GIZTORPMACHINE *>(gizmo->object)->name : NULL;
}

static i32 GizTorp_GetOutput(GIZMO *, i32, i32) {
    return 0;
}

static char *GizTorp_GetOutputName(GIZMO *, i32 output_index) {
    return output_index == 0 ? const_cast<char *>("Active") : NULL;
}

static i32 GizTorp_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizTorp_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        GIZTORPMACHINE *machine = static_cast<GIZTORPMACHINE *>(gizmo->object);
        const u8 active_flag = active != 0;
        machine->flags = static_cast<u8>((machine->flags & ~GIZTORPMACHINE_FLAG_ACTIVE) | active_flag);
    }
}

static void GizTorp_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        GIZTORPMACHINE *machine = static_cast<GIZTORPMACHINE *>(gizmo->object);
        const u8 visible_flag = visible != 0;
        machine->flags = static_cast<u8>((machine->flags & ~GIZTORPMACHINE_FLAG_VISIBLE) | (visible_flag << 1));
    }
}

static void GizTorps_Reset(void *world_ptr, void *, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return;
    }

    GIZTORPMACHINESYS *system = world->giz_torp_machine_sys;
    if (system == NULL || system->machines == NULL || system->count <= 0) {
        return;
    }

    for (i32 index = 0; index < system->count; ++index) {
        system->machines[index].flags |= GIZTORPMACHINE_FLAG_ACTIVE | GIZTORPMACHINE_FLAG_VISIBLE;
    }
}

static void *GizTorps_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    world->giz_torp_machine_sys = NULL;
    if (world->current_level->max_torp_machines == 0) {
        return NULL;
    }

    GIZTORPMACHINESYS *system = reinterpret_cast<GIZTORPMACHINESYS *>((world->giz_buffer.addr + 3) & ~3u);
    world->giz_torp_machine_sys = system;
    world->giz_buffer.addr = reinterpret_cast<usize>(system + 1);
    memset(system, 0, sizeof(*system));

    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    system->machines = reinterpret_cast<GIZTORPMACHINE *>(world->giz_buffer.addr);
    world->giz_buffer.addr +=
        world->current_level->max_torp_machines * static_cast<i32>(sizeof(GIZTORPMACHINE));
    memset(system->machines, 0,
           world->current_level->max_torp_machines * static_cast<i32>(sizeof(GIZTORPMACHINE)));
    system->scale = 1.0f;
    return world->giz_torp_machine_sys;
}

static i32 GizTorp_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->giz_torp_machine_sys == NULL || world->giz_torp_machine_sys->count != 0) {
        return 0;
    }

    GIZTORPMACHINESYS *system = world->giz_torp_machine_sys;
    const i32 version = EdFileReadInt();
    system->count = EdFileReadInt();
    system->scale = version > 2 ? EdFileReadFloat() : 1.0f;

    for (i32 index = 0; index < system->count; ++index) {
        GIZTORPMACHINE &machine = system->machines[index];
        const i32 name_length = EdFileReadInt();
        EdFileRead(machine.name, name_length);
        EdFileReadNuVec(&machine.position);
        machine.y_rotation = static_cast<u16>(EdFileReadShort());
        if (version > 1) {
            const u8 alternate_model = EdFileReadUnsignedChar() != 0;
            machine.flags =
                static_cast<u8>((machine.flags & ~GIZTORPMACHINE_FLAG_ALTERNATE_MODEL) | (alternate_model << 2));
        } else {
            machine.flags &= ~GIZTORPMACHINE_FLAG_ALTERNATE_MODEL;
        }
    }
    return 1;
}

ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Torp Machine";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTorp_GetMaxGizmos;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTorp_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizTorp_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizTorp_Draw;
    addtype.fns.get_gizmo_name_fn = GizTorp_GetGizmoName;
    addtype.fns.get_output_fn = GizTorp_GetOutput;
    addtype.fns.get_output_name_fn = GizTorp_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizTorp_GetNumOutputs;
    addtype.fns.activate_fn = GizTorp_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizTorp_SetVisibility;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = GizTorps_Reset;
    addtype.fns.reserve_buffer_space_fn = GizTorps_ReserveBufferSpace;
    addtype.fns.load_fn = GizTorp_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    giztorpmachine_gizmotype_id = type_id;

    return &addtype;
}
