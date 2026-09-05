#include "legoapi/gizmos/traps/gizturrets.h"

#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numusic/sfx.h"

#include <stdio.h>
#include <string.h>

i32 GizmoBlowupGetTypeFromNameTableId(WORLDINFO_s *world, i32 name_table_id);
i32 GizmoBlowupGetNameTableId(char *name);
void AddLevelSfxFromId(i32 sfx_id, i32 *sfx_ids, i32 *sfx_count, i32 max_sfx);
extern "C" i16 FindPlatInst(i32 instance_ix);

static void GizTurret_ReadAnimSetData(GAMEANIMOBJ_s *object, unsigned char version) {
    if (version <= 2 || object == NULL) {
        return;
    }

    u8 fallback[2] = {};
    u8 *object_data = static_cast<u8 *>(object->object_data);
    if (object_data == NULL) {
        object_data = fallback;
    }
    object_data[0] = static_cast<u8>(EdFileReadChar());
    object_data[1] = static_cast<u8>(EdFileReadChar());
}

i32 turret_gizmotype_id = -1;

static i32 GizTurrets_GetMaxGizmos(void *turret) {
    WORLDINFO *world = static_cast<WORLDINFO *>(turret);
    if (world == NULL) {
        return 0;
    }
    return world->current_level->max_turrets;
}

static void GizTurrets_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *data) {
    GIZTURRETSYS_s *turret_sys = static_cast<GIZTURRETSYS_s *>(data);
    if (turret_sys != NULL) {
        if (turret_sys->count != 0) {
            i32 i = 0;
            do {
                if (NuStrLen(turret_sys->turrets[i].name) != 0) {
                    AddGizmo(gizmo_sys, type_id, NULL, &turret_sys->turrets[i]);
                }
                ++i;
            } while (turret_sys->count > i);
        }
    }
}

static void GizTurrets_Update(void *, void *system_ptr, float frame_time) {
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }

    GIZTURRET_s *turret = system->turrets;
    for (i32 index = 0; index < system->count; ++index, ++turret) {
        const u8 frame_flags = turret->flags;
        turret->flags = static_cast<u8>(frame_flags & ~GIZTURRET_FLAG_FIRED_THIS_FRAME);
        turret->runtime_flags &= ~GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING;

        const u8 update_mask = GIZTURRET_FLAG_VISIBLE | GIZTURRET_FLAG_ACTIVE | GIZTURRET_FLAG_UPDATE_DISABLED;
        if ((frame_flags & update_mask) == (GIZTURRET_FLAG_VISIBLE | GIZTURRET_FLAG_ACTIVE)) {
            turret->fire_cooldown -= frame_time;
            if (turret->fire_cooldown < 0.0f) {
                turret->fire_cooldown = 0.0f;
            }
            // Target selection, aiming, animation, and firing continue here in the original.
        }
    }
}

static void GizTurrets_Draw(void *world_ptr, void *system_ptr, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }

    for (i32 index = 0; index < system->count; ++index) {
        GIZTURRET_s &turret = system->turrets[index];
        if ((turret.flags & GIZTURRET_FLAG_VISIBLE) == 0) {
            continue;
        }
        if (turret.room_id >= 0 && world->rooms_visible_ptr[turret.room_id] == 0) {
            continue;
        }
        if ((turret.animation_flags & GIZTURRET_ANIMATION_FLAG_DRAW_REFLECTION) == 0) {
            continue;
        }
        GameAnimSet_DrawReflection(turret.anim_set, 2, turret.reflection_alpha, NULL);
    }
}

static char *GizmoTurret_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
        return turret->name;
    }
    return NULL;
}

static i32 GizmoTurret_GetOutput(GIZMO *gizmo, i32 output_index, i32) {
    GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
    switch (output_index) {
        case 0:
            return (turret->flags & 0x30) != 0;
        case 1:
            return static_cast<i8>(turret->flags) < 0;
        case 2:
            return turret->field_0x132[0] >= turret->field_0x131;
        default:
            return 0;
    }
}

static char *GizmoTurret_GetOutputName(GIZMO *gizmo, i32 output_index) {
    static char str[32];
    switch (output_index) {
        case 0:
            return const_cast<char *>("destroyed");
        case 1:
            return const_cast<char *>("fired");
        case 2: {
            GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
            if (turret == NULL) {
                return NULL;
            }
            sprintf(str, "Fired %d Shots", turret->field_0x131);
            return str;
        }
        default:
            return NULL;
    }
}

static i32 GizmoTurret_GetNumOutputs(GIZMO *) {
    return 3;
}

static void GizmoTurret_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo == NULL) {
        return;
    }
    GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
    u8 active_flag = active != 0;
    active_flag += active_flag;
    turret->flags = static_cast<u8>((turret->flags & ~GIZTURRET_FLAG_ACTIVE) | active_flag);
    if ((turret->flags & GIZTURRET_FLAG_ACTIVE) != 0) {
        turret->flags &= ~0x30;
        turret->field_0x132[0] = 0;
    }
}

static i32 GizmoTurret_ActivateRev(GIZMO *gizmo, i32 active, i32 reverse) {
    if (gizmo == NULL) {
        return 0;
    }
    GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
    if (turret == NULL) {
        return 0;
    }
    if ((reverse & 1) != 0) {
        if ((turret->flags & GIZTURRET_FLAG_ACTIVE) != 0) {
            return 0;
        }
        turret->flags &= ~0x10;
        return active == 0;
    }
    u8 active_flag = active == 0;
    active_flag += active_flag;
    turret->flags = static_cast<u8>((turret->flags & ~GIZTURRET_FLAG_ACTIVE) | active_flag);
    return 1;
}

static void GizmoTurret_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL) {
        return;
    }
    GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
    if (turret == NULL) {
        return;
    }
    GameAnimSet_SetVisibility(turret->anim_set, visible);
    u8 visibility_flag = visible != 0;
    visibility_flag <<= 2;
    turret->flags = static_cast<u8>((turret->flags & ~GIZTURRET_FLAG_VISIBLE) | visibility_flag);
}

static NUVEC *GizmoTurret_GetPos(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZTURRET_s *turret = static_cast<GIZTURRET_s *>(gizmo->object);
        if (turret != NULL) {
            return &turret->position;
        }
    }
    return NULL;
}

static i32 GizTurrets_BoltHitPlat(void *, void *, BOLT *, unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static i32 *GizTurrets_GetBestBoltTarget(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32) {
    UNIMPLEMENTED();
    return {};
}

static i32 GizTurrets_BoltHit(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                              unsigned char *) {
    UNIMPLEMENTED();
    return {};
}

static void *GizTurrets_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, 0x70);
}

static void GizTurrets_ClearProgress(void *, void *progress_ptr) {
    volatile u32 *progress = static_cast<u32 *>(progress_ptr);
    if (progress == NULL) {
        return;
    }

    progress[0] = 0xffffffff;
    progress[1] = 0xffffffff;
    progress[2] = 0xffffffff;
    progress[3] = 0xffffffff;
    progress[4] = 0;
    progress[5] = 0;
    progress[6] = 0;
    progress[7] = 0;
    progress[8] = 0;
    progress[9] = 0;
    progress[10] = 0;
    progress[11] = 0;
    progress[12] = 0xffffffff;
    progress[13] = 0xffffffff;
    progress[14] = 0xffffffff;
    progress[15] = 0xffffffff;
    progress[16] = 0xffffffff;
    progress[17] = 0xffffffff;
    progress[18] = 0xffffffff;
    progress[19] = 0xffffffff;
    progress[20] = 0xffffffff;
    progress[21] = 0xffffffff;
    progress[22] = 0xffffffff;
    progress[23] = 0xffffffff;
    progress[24] = 0xffffffff;
    progress[25] = 0xffffffff;
    progress[26] = 0xffffffff;
    progress[27] = 0xffffffff;
}

static void GizTurrets_StoreProgress(void *, void *system_ptr, void *progress_ptr) {
    if (progress_ptr == NULL) {
        return;
    }

    GizTurrets_ClearProgress(NULL, progress_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system->count == 0) {
        return;
    }

    u32 *progress = static_cast<u32 *>(progress_ptr);
    GIZTURRET_s *turret = system->turrets;
    for (i32 index = 0; index < system->count && index != 64; ++index, ++turret) {
        const i32 word = index >> 5;
        const u32 mask = 1u << index;
        if ((turret->flags & GIZTURRET_FLAG_VISIBLE) == 0) {
            progress[word + 2] &= ~mask;
        }
        if ((turret->flags & GIZTURRET_FLAG_ACTIVE) == 0) {
            progress[word] &= ~mask;
        }
        if ((turret->flags & 0x08) != 0) {
            progress[word + 4] |= mask;
        }
        if ((turret->flags & 0x10) != 0) {
            progress[word + 6] |= mask;
        }
        if ((turret->flags & GIZTURRET_FLAG_UPDATE_DISABLED) != 0) {
            progress[word + 8] |= mask;
        }
        if ((turret->runtime_flags & 0x04) != 0) {
            progress[word + 10] |= mask;
        }
        static_cast<u8 *>(progress_ptr)[0x30 + index] = turret->field_0x12e;
    }
}

static void GizTurrets_Reset(void *world_ptr, void *system_ptr, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system->count == 0) {
        return;
    }

    GIZTURRET_s *turret = system->turrets;
    const i32 has_progress = progress_ptr != NULL;
    for (i32 index = 0; index < system->count; ++index, ++turret) {
        turret->field_0x12e = turret->field_0x12f;
        turret->flags |= GIZTURRET_FLAG_ACTIVE | GIZTURRET_FLAG_VISIBLE;
        turret->field_0x132[0] = 0;
        turret->primary_anim_obj = NULL;
        turret->secondary_anim_obj = NULL;

        if (turret->anim_set != NULL) {
            GAMEANIMOBJ_s *object = turret->anim_set->objects;
            while (object != NULL) {
                u8 *object_data = static_cast<u8 *>(object->object_data);
                if (object_data[1] == 1) {
                    turret->primary_anim_obj = object;
                    object_data[2] = 0xff;
                    if (world->terrain != NULL && NuSpecialExistsFn(&object->special) != 0) {
                        object_data[2] = static_cast<u8>(FindPlatInst(NuSpecialGetInstanceix(&object->special)));
                        if (object_data[2] != 0xff) {
                            turret->runtime_flags |= 2;
                        }
                    }
                } else if (object_data[1] == 2) {
                    turret->secondary_anim_obj = object;
                    object_data[2] = 0xff;
                }
                object = object->next;
            }
        }

        turret->room_id = world->current_gscn != NULL
                              ? static_cast<i16>(NuPortalWhichRoom(world->current_gscn, &turret->position))
                              : -1;
        if ((turret->behavior_flags & 2) != 0) {
            turret->field_0x3c = turret->position;
            GameAnimSet_GetAveragePos(turret->anim_set, &turret->field_0x30, 0, 1, 1);
            turret->field_0x3c = turret->field_0x30;
        }
        if ((turret->behavior_flags & 0x800) != 0) {
            turret->field_0xf0 = turret->field_0xec;
            turret->field_0x3c = turret->field_0x30;
        }
        GameAnimSet_EvaluateState(turret->anim_set);

        if (turret->primary_anim_obj != NULL && turret->secondary_anim_obj != NULL &&
            NuSpecialExistsFn(&turret->primary_anim_obj->special) != 0 &&
            NuSpecialExistsFn(&turret->secondary_anim_obj->special) != 0) {
            NUMTX *primary_mtx = NuSpecialGetMtx(&turret->primary_anim_obj->special);
            NUMTX *secondary_mtx = NuSpecialGetMtx(&turret->secondary_anim_obj->special);
            NuMtxInv(&turret->field_0xa4, primary_mtx);
            NuMtxMul(&turret->field_0xa4, secondary_mtx, &turret->field_0xa4);
        }

        if (index <= 0x3f && has_progress != 0) {
            u32 *progress = static_cast<u32 *>(progress_ptr);
            const i32 word = index >> 5;
            const u32 bit = 1u << (index & 31);
            turret->flags = static_cast<u8>((turret->flags & ~GIZTURRET_FLAG_VISIBLE) |
                                            ((progress[word + 2] & bit) != 0 ? GIZTURRET_FLAG_VISIBLE : 0));
            turret->flags = static_cast<u8>((turret->flags & ~GIZTURRET_FLAG_ACTIVE) |
                                            ((progress[word] & bit) != 0 ? GIZTURRET_FLAG_ACTIVE : 0));
            turret->flags = static_cast<u8>((turret->flags & ~0x8) | ((progress[word + 4] & bit) != 0 ? 0x8 : 0));
            turret->flags = static_cast<u8>((turret->flags & ~0x10) | ((progress[word + 6] & bit) != 0 ? 0x10 : 0));
            turret->flags = static_cast<u8>((turret->flags & ~0x20) | ((progress[word + 8] & bit) != 0 ? 0x20 : 0));
            turret->runtime_flags =
                static_cast<u8>((turret->runtime_flags & ~4) | ((progress[word + 10] & bit) != 0 ? 4 : 0));
            if (static_cast<u8 *>(progress_ptr)[index + 0x30] != 0xff) {
                turret->field_0x12e = static_cast<u8 *>(progress_ptr)[index + 0x30];
            }
        }

        if (turret->anim_set != NULL) {
            for (GAMEANIMOBJ_s *object = turret->anim_set->objects; object != NULL; object = object->next) {
                u8 *object_data = static_cast<u8 *>(object->object_data);
                if (object_data[1] == 3) {
                    NuSpecialSetVisibility(&object->special, (turret->flags >> 5) & 1);
                }
            }
        }
    }
}

static void *GizTurrets_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *turret_sys = static_cast<GIZTURRETSYS_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZTURRETSYS_s)));

    turret_sys->capacity = world->current_level->max_turrets;
    turret_sys->turrets = static_cast<GIZTURRET_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, turret_sys->capacity * sizeof(GIZTURRET_s)));
    turret_sys->anim_pool =
        GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->unknown_0108, 4, turret_sys->capacity * 2);

    for (i32 i = 0; i < turret_sys->capacity; ++i) {
        turret_sys->turrets[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->unknown_0108, turret_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_turret_sys = turret_sys;
    return turret_sys;
}

static i32 GizTurrets_Load(void *world_ptr, void *system_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    const unsigned char version = static_cast<unsigned char>(EdFileReadChar());
    system->count = static_cast<u16>(EdFileReadShort());
    if (system->count == 0) {
        return 1;
    }

    GIZTURRET_s *turret = system->turrets;
    for (i32 index = 0; index < system->count; ++index, ++turret) {
        GAMEANIMSET_s *anim_set = turret->anim_set;
        turret->ClearMechObjectInterface();
        memset(turret, 0, sizeof(*turret));
        turret->anim_set = anim_set;
        turret->flags &= static_cast<u8>(~1);
        turret->runtime_flags &= static_cast<u8>(~GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING);
        turret->fire_interval = 1.0f;
        turret->pitch_turn_speed = 8.0f;
        turret->yaw_turn_speed = 8.0f;
        turret->field_0x140 = 0.2f;
        turret->behavior_flags |= 0x802;
        turret->field_0x12f = 1;
        turret->field_0x131 = 1;
        turret->field_0x12a = -1;
        turret->field_0x126 = -1;
        turret->field_0x138 = -1;
        turret->field_0x130 = 1;
        turret->blowup_type = -1;
        turret->bolt_type_id = -1;
        turret->field_0x120 = (WORLD->area != NULL && (WORLD->area->flags & 1) != 0) ? 12.0f : 1.75f;

        EdFileRead(turret->name, sizeof(turret->name));
        GizmoFileReadGameAnimSet(turret->anim_set, world_ptr, GizTurret_ReadAnimSetData, version,
                                 const_cast<char *>("GizTurret"), turret->name);
        EdFileReadNuVec(&turret->position);
        EdFileReadNuVec(&turret->field_0x30);
        EdFileReadNuVec(&turret->field_0x3c);
        EdFileReadNuVec(&turret->field_0x48);
        turret->field_0x58 = EdFileReadInt();
        turret->field_0x5c = EdFileReadInt();
        turret->field_0x64 = EdFileReadInt();
        turret->field_0x68 = EdFileReadInt();
        turret->field_0x6c = EdFileReadInt();
        turret->field_0x70 = EdFileReadInt();
        if (version > 1) {
            turret->behavior_flags = static_cast<u32>(EdFileReadInt());
        }
        turret->field_0x130 = static_cast<u8>(EdFileReadChar());
        for (i32 vector_index = 0; vector_index < turret->field_0x130; ++vector_index) {
            EdFileReadNuVec(&turret->field_0x74[vector_index]);
        }
        turret->field_0xec = EdFileReadFloat();
        turret->field_0xf0 = EdFileReadFloat();
        turret->reflection_alpha = EdFileReadFloat();
        turret->fire_interval = EdFileReadFloat();
        turret->pitch_turn_speed = EdFileReadFloat();
        turret->yaw_turn_speed = EdFileReadFloat();
        turret->field_0x10c = static_cast<i16>(EdFileReadShort());
        turret->field_0x110 = static_cast<i16>(EdFileReadShort());
        turret->field_0x112 = static_cast<i16>(EdFileReadShort());
        EdFileReadNuVec(&turret->field_0x114);

        char name[256];
        if (version > 5) {
            turret->field_0x120 = EdFileReadFloat();
            turret->field_0x12f = static_cast<u8>(EdFileReadChar());
        } else {
            turret->field_0x12f = static_cast<u8>(EdFileReadChar());
        }
        if (version > 3) {
            turret->field_0x131 = static_cast<u8>(EdFileReadChar());
            turret->base_y_rotation = static_cast<i16>(EdFileReadShort());
            if (GizmoFileReadName(name) != 0) {
                turret->bolt_type_id = static_cast<i8>(BoltType_FindIDByName(name, world));
            }
        } else {
            if (GizmoFileReadName(name) != 0) {
                turret->bolt_type_id = static_cast<i8>(BoltType_FindIDByName(name, world));
            }
        }
        if (GizmoFileReadName(name) != 0) {
            turret->field_0x12a = static_cast<i16>(GetSfxId(name));
        }
        if (GizmoFileReadName(name) != 0) {
            turret->field_0x126 = static_cast<i16>(GetSfxId(name));
        }
        if (version > 6) {
            i32 name_result;
            do {
                name_result = GizmoFileReadName(name);
                if (name_result != 0) {
                    turret->field_0x138 = static_cast<i16>(GetSfxId(name));
                }
            } while (name_result != 0);
        }
        if (GizmoFileReadName(name) != 0) {
            turret->blowup_type = static_cast<i16>(GizmoBlowupGetNameTableId(name));
            if (turret->blowup_type != -1) {
                turret->runtime_flags |= GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID;
            }
        }
        turret->field_0x134 = static_cast<i16>(EdFileReadShort());
        if (version <= 4 && turret->anim_set != NULL) {
            i32 object_index = 0;
            for (GAMEANIMOBJ_s *object = turret->anim_set->objects; object != NULL;
                 object = object->next, ++object_index) {
                u8 *object_data = static_cast<u8 *>(object->object_data);
                if (object_index == 0) {
                    object_data[1] = 1;
                } else if (object_index == 1) {
                    object_data[1] = 2;
                } else {
                    object_data[1] = (object_data[0] & 1) != 0 ? 3 : 1;
                }
            }
        }
    }
    return 1;
}

static void GizTurrets_PostLoad(void *world_ptr, void *system_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system != NULL) {
        if (system->count != 0) {
            GIZTURRET_s *turret = system->turrets;
            i32 index = 0;
            do {
                if ((turret->runtime_flags & GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID) != 0) {
                    turret->blowup_type =
                        static_cast<i16>(GizmoBlowupGetTypeFromNameTableId(world, turret->blowup_type));
                    turret->runtime_flags &= ~GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID;
                }
                ++index;
                ++turret;
            } while (system->count > index);
        }
    }
}

static void GizTurrets_AddLevelSfx(void *, void *system_ptr, i32 *sfx_ids, i32 *sfx_count, i32 max_sfx) {
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system == NULL || system->count == 0) {
        return;
    }
    GIZTURRET_s *turret = system->turrets;
    for (i32 index = 0; index < system->count; ++index, ++turret) {
        if (turret->field_0x12a != -1) {
            AddLevelSfxFromId(turret->field_0x12a, sfx_ids, sfx_count, max_sfx);
        }
        if (turret->field_0x126 != -1) {
            AddLevelSfxFromId(turret->field_0x126, sfx_ids, sfx_count, max_sfx);
        }
        if (turret->field_0x138 != -1) {
            AddLevelSfxFromId(turret->field_0x138, sfx_ids, sfx_count, max_sfx);
        }
    }
}

ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizTurret";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x70;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizTurrets_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoTurret_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizTurrets_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = GizTurrets_BoltHitPlat;
    addtype.fns.get_best_bolt_target_fn = GizTurrets_GetBestBoltTarget;
    addtype.fns.late_update_fn = GizTurrets_Update;
    addtype.fns.bolt_hit_fn = GizTurrets_BoltHit;
    addtype.fns.draw_fn = GizTurrets_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoTurret_GetGizmoName;
    addtype.fns.get_output_fn = GizmoTurret_GetOutput;
    addtype.fns.get_output_name_fn = GizmoTurret_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoTurret_GetNumOutputs;
    addtype.fns.activate_fn = GizmoTurret_Activate;
    addtype.fns.activate_rev_fn = GizmoTurret_ActivateRev;
    addtype.fns.set_visibility_fn = GizmoTurret_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizTurrets_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizTurrets_ClearProgress;
    addtype.fns.store_progress_fn = GizTurrets_StoreProgress;
    addtype.fns.reset_fn = GizTurrets_Reset;
    addtype.fns.reserve_buffer_space_fn = GizTurrets_ReserveBufferSpace;
    addtype.fns.load_fn = GizTurrets_Load;
    addtype.fns.post_load_fn = GizTurrets_PostLoad;
    addtype.fns.add_level_sfx_fn = GizTurrets_AddLevelSfx;
    turret_gizmotype_id = type_id;

    return &addtype;
}
