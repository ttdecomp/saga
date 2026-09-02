#include "legoapi/gizmos/object/technos.h"

#include "decomp.h"
#include "batman.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"

struct TECHNOPROGRESS {
    i32 state[2];
};

extern "C" void NewTerrPlatformsOff(void);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);

TECHNO_CONFIG TechnoSys = {
    0.2f,
    0x55,
    0x2c,
    -1,
    -1,
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    0x2d,
    0x2e,
    {0.0991f, 0.0f, 0.0762f},
    {-0.0991f, 0.0f, 0.0762f},
};

i32 techno_gizmotype_id = -1;

static i32 Technos_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_technos;
}

static void Technos_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->ntechnos; ++index) {
        TECHNO &techno = world->technos[index];
        if (NuStrLen(techno.name) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &techno);
        }
    }
}

static void Technos_EarlyUpdate(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->technos == NULL) {
        return;
    }
    TECHNO *techno = world->technos;
    for (i32 index = 0; index < world->ntechnos; ++index, ++techno) {
        techno->flags &= ~TECHNO_FLAG_USED_THIS_FRAME;
    }
}

static void Technos_LateUpdate(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world != NULL) {
        TECHNO *techno = world->technos;
        if (techno != NULL && world->ntechnos > 0) {
            for (i32 index = 0; index < world->ntechnos; ++index, ++techno) {
                techno->ground_offset =
                    SeekLinearF(techno->ground_offset, (techno->flags & TECHNO_FLAG_USED_THIS_FRAME) != 0 ? 1.0f : 0.0f,
                                FRAMETIME * 2.5f);
            }
        }
    }
}

static void Technos_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (TechnoSys.active_effect_id == -1 || world->lev_objs[TechnoSys.active_effect_id].active == 0 ||
        world->technos == NULL) {
        return;
    }

    const u16 spin_angle = static_cast<u16>(NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0f * 65536.0f);
    const f32 pulse_phase = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0f;
    const f32 pulse = NuTrigTable[(static_cast<i32>(pulse_phase) >> 1) & 0x7fff] * 0.2f + 0.8f;

    TECHNO *techno = world->technos;
    for (i32 index = 0; index < world->ntechnos; ++index, ++techno) {
        if ((techno->flags & TECHNO_FLAG_VISIBLE) == 0) {
            continue;
        }

        NUMTX matrix;
        if (TechnoSys.active_effect_id != -1) {
            NuMtxSetTranslation(&matrix, &TechnoSys.idle_offset);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuMtxTranslate(&matrix, &techno->position);
            NuSpecialDrawAt(&world->lev_objs[TechnoSys.active_effect_id].special, &matrix);
        }
        if (TechnoSys.success_effect_id != -1) {
            NuMtxSetTranslation(&matrix, &TechnoSys.active_offset);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuMtxTranslate(&matrix, &techno->position);
            NuSpecialDrawAt(&world->lev_objs[TechnoSys.success_effect_id].special, &matrix);
        }
        if (TechnoSys.failure_effect_id != -1) {
            NuMtxSetTranslation(&matrix, &TechnoSys.complete_offset);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuMtxTranslate(&matrix, &techno->position);
            NuSpecialDrawAt(&world->lev_objs[TechnoSys.failure_effect_id].special, &matrix);
        }

        if (world->lev_objs[TechnoSys.activation_effect_id].active != 0) {
            const f32 hand_phase = techno->ground_offset * 32768.0f + 16384.0f;
            const i32 hand_angle = static_cast<i32>(
                (1.0f - (NuTrigTable[(static_cast<i32>(hand_phase) >> 1) & 0x7fff] + 1.0f) * 0.5f) * 29127.0f);
            NuMtxSetRotationY(&matrix, hand_angle);
            NuMtxTranslate(&matrix, &TechnoSys.left_hand_offset);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NUVEC *translation = NUMTX_GET_ROW_VEC(&matrix, 3);
            NuVecAdd(translation, translation, &techno->position);
            NuSpecialDrawAt(&world->lev_objs[TechnoSys.activation_effect_id].special, &matrix);
        }
        if (world->lev_objs[TechnoSys.completion_effect_id].active != 0) {
            const f32 hand_phase = techno->ground_offset * 32768.0f + 16384.0f;
            const i32 hand_angle = static_cast<i32>(
                (1.0f - (NuTrigTable[(static_cast<i32>(hand_phase) >> 1) & 0x7fff] + 1.0f) * 0.5f) * 29127.0f);
            NuMtxSetRotationY(&matrix, -hand_angle);
            NuMtxTranslate(&matrix, &TechnoSys.right_hand_offset);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NUVEC *translation = NUMTX_GET_ROW_VEC(&matrix, 3);
            NuVecAdd(translation, translation, &techno->position);
            NuSpecialDrawAt(&world->lev_objs[TechnoSys.completion_effect_id].special, &matrix);
        }

        if ((techno->flags & TECHNO_FLAG_ACTIVE) != 0 && techno->ground_position.y != 2000000.0f &&
            world->lev_objs[TechnoSys.floor_target_object_id].active != 0 && pulse > 0.0f) {
            NuMtxSetRotationY(&matrix, spin_angle);
            if (techno->ground_z_rotation != 0) {
                NuMtxRotateZ(&matrix, techno->ground_z_rotation);
            }
            if (techno->ground_x_rotation != 0) {
                NuMtxRotateX(&matrix, techno->ground_x_rotation);
            }
            NuMtxTranslate(&matrix, &techno->ground_position);
            NuSpecialDrawAtAlpha(&world->lev_objs[TechnoSys.floor_target_object_id].special, &matrix, pulse);
        }
    }
}

static char *Techno_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? static_cast<TECHNO *>(gizmo->object)->name : NULL;
}

static i32 Techno_GetOutput(GIZMO *gizmo, i32, i32) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }

    TECHNO *techno = static_cast<TECHNO *>(gizmo->object);
    if ((techno->flags & (TECHNO_FLAG_ACTIVE | TECHNO_FLAG_VISIBLE)) != (TECHNO_FLAG_ACTIVE | TECHNO_FLAG_VISIBLE) ||
        techno->controlled_object == NULL) {
        return 0;
    }
    if (techno->target_mode == 1) {
        return (techno->flags & TECHNO_FLAG_COMPLETE) != 0;
    }
    return 0;
}

static char *Techno_GetOutputName(GIZMO *gizmo, i32 output_index) {
    return const_cast<char *>("Active");
}

static i32 Techno_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static void Techno_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL) {
        return;
    }
    TECHNO *techno = static_cast<TECHNO *>(gizmo->object);
    techno->flags = static_cast<u8>((techno->flags & ~TECHNO_FLAG_ACTIVE) | (active != 0 ? TECHNO_FLAG_ACTIVE : 0));
}

static void Techno_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL) {
        return;
    }
    TECHNO *techno = static_cast<TECHNO *>(gizmo->object);
    const u8 visible_flag = visible != 0;
    techno->flags = static_cast<u8>((techno->flags & ~TECHNO_FLAG_VISIBLE) | (visible_flag << 1));
}

static NUVEC *Techno_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &static_cast<TECHNO *>(gizmo->object)->position;
}

static void *Technos_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(TECHNOPROGRESS));
}

static void Technos_ClearProgress(void *, void *progress_data) {
    TECHNOPROGRESS *progress = (TECHNOPROGRESS *)progress_data;
    if (progress == NULL) {
        return;
    }

    progress->state[0] = -1;
    progress->state[1] = -1;
}

static void Technos_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    TECHNOPROGRESS *progress = static_cast<TECHNOPROGRESS *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress->state[0] = -1;
    progress->state[1] = -1;
    if (world == NULL || world->technos == NULL) {
        return;
    }

    const i32 count = world->ntechnos < 32 ? world->ntechnos : 32;
    for (i32 index = 0; index < count; ++index) {
        const u32 bit = 1u << index;
        if ((world->technos[index].flags & TECHNO_FLAG_VISIBLE) == 0) {
            progress->state[1] &= ~bit;
        }
        if ((world->technos[index].flags & TECHNO_FLAG_ACTIVE) == 0) {
            progress->state[0] &= ~bit;
        }
    }
}

static void Technos_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    TECHNOPROGRESS *progress = static_cast<TECHNOPROGRESS *>(progress_ptr);
    if (world == NULL || world->technos == NULL) {
        return;
    }

    for (i32 index = 0; index < world->ntechnos; ++index) {
        TECHNO &techno = world->technos[index];
        techno.ground_position.x = 0.0f;
        techno.ground_position.y = 0.0f;
        techno.ground_position.z = TechnoSys.interaction_time;
        NuVecRotateY(&techno.ground_position, &techno.ground_position, techno.y_rotation);
        NuVecAdd(&techno.ground_position, &techno.ground_position, &techno.position);

        NewTerrPlatformsOff();
        const f32 floor_height = GameShadow(NULL, &techno.ground_position, 5.0f, -1);
        if (floor_height == floor_height) {
            techno.ground_position.y = 2000000.0f;
            techno.ground_offset = 0.0f;
        } else {
            techno.ground_position.y = floor_height + 0.005f;
            FindAnglesZX(&ShadNorm, &techno.ground_x_rotation, &techno.ground_z_rotation);
        }

        techno.flags =
            static_cast<u8>((techno.flags | TECHNO_FLAG_ACTIVE | TECHNO_FLAG_VISIBLE) & ~TECHNO_FLAG_USED_THIS_FRAME);
        NuStrCpy(techno.target_name, techno.target_object_name);

        if (progress != NULL && index <= 31) {
            const u32 bit = 1u << index;
            const u8 visible = (progress->state[1] & bit) != 0;
            techno.flags = static_cast<u8>((techno.flags & ~TECHNO_FLAG_VISIBLE) | (visible << 1));
            const u8 active = (progress->state[0] & bit) != 0;
            techno.flags = static_cast<u8>((techno.flags & ~TECHNO_FLAG_ACTIVE) | active);
        }
    }
}

static void *Technos_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    TECHNO *result = NULL;
    world->technos = NULL;
    world->ntechnos = 0;
    if (world->current_level->max_technos != 0) {
        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        result = static_cast<TECHNO *>(world->giz_buffer.void_ptr);
        world->technos = result;
        world->giz_buffer.addr += world->current_level->max_technos * sizeof(TECHNO);
    }
    return result;
}

static i32 Technos_Load(void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->ntechnos != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    world->ntechnos = EdFileReadInt();
    for (i32 index = 0; index < world->ntechnos; ++index) {
        TECHNO &techno = world->technos[index];
        EdFileRead(techno.name, sizeof(techno.name));
        EdFileReadNuVec(&techno.position);
        techno.y_rotation = static_cast<i16>(EdFileReadShort());

        if (version <= 1) {
            techno.enabled = 1;
            techno.scale = 1.0f;
            techno.output = 0;
            continue;
        }

        techno.target_mode = static_cast<u8>(EdFileReadChar());
        const i32 target_name_length = EdFileReadInt();
        EdFileRead(techno.target_object_name, target_name_length);
        NuStrCpy(techno.target_name, techno.target_object_name);
        if (version == 2) {
            techno.enabled = 1;
            techno.scale = 1.0f;
            techno.output = 0;
            continue;
        }

        techno.enabled = static_cast<u8>(EdFileReadChar());
        if (version == 3) {
            techno.scale = 1.0f;
            techno.output = 0;
            continue;
        }

        techno.scale = EdFileReadFloat();
        techno.output = version == 4 ? 0 : EdFileReadInt();
    }
    return 1;
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
