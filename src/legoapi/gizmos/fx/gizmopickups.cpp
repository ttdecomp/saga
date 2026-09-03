#include "legoapi/gizmos/fx/gizmopickups.h"

#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

extern "C" {
    i32 NuPortalWhichRoom(NUGSCN *scene, NUVEC *position);
    void NewTerrPlatformsOff(void);
}

extern FadeSystem FadeSys;
extern i32 BonusArea;
extern i32 TimingBarSet;

f32 GameShadow(GameObject_s *object, NUVEC *position, f32 probe_height, i32 terrain_mask);
void FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation);
void SetAreaPickupGravity(i32 area, i32 level);
void SuperCounter_ActivateGizmoPickup(GIZMO_s *gizmo, GIZMOPICKUP_s *pickup);
void SuperCounters_ResetProcessed(WORLDINFO_s *world);
void MiniKitDetector(NUVEC *position);

namespace {

    enum : i32 {
        GIZMOPICKUP_PROGRESS_CAPACITY = 512,
        GIZMOPICKUP_PROGRESS_WORDS = GIZMOPICKUP_PROGRESS_CAPACITY / 32,
        GIZMOPICKUP_TEMPORARY_CAPACITY = 64,
    };

    struct GIZMOPICKUPPROGRESS_s {
        u32 collected[GIZMOPICKUP_PROGRESS_WORDS];
        u32 enabled[GIZMOPICKUP_PROGRESS_WORDS];
        u32 visible[GIZMOPICKUP_PROGRESS_WORDS];
        u32 activated[GIZMOPICKUP_PROGRESS_WORDS];
    };
    DECOMP_ASSERT(sizeof(GIZMOPICKUPPROGRESS_s) == 0x100, "GIZMOPICKUP progress ABI");

    GIZMOPICKUPSYS_s *GizmoPickupSys = &GizmoPickupSys_Game;

    GIZMO_PICKUP_TYPE *GetPickupType(const GIZMOPICKUP_s &pickup) {
        i32 type_index = pickup.type_index;
        if ((pickup.state_flags & GIZMOPICKUP_STATE_ALTERNATE_TYPE) != 0 && GizmoPickupSys->alternate_type >= 0) {
            type_index = GizmoPickupSys->alternate_type;
        }
        if (type_index < 0 || type_index >= GizmoPickupSys->type_count) {
            type_index = 0;
        }
        return &GizmoPickupSys->types[type_index];
    }

    i32 FindPickupTypeIndex(char type_code) {
        for (i32 index = 0; index < GizmoPickupSys->type_count; ++index) {
            if (GizmoPickupSys->types[index].type_code == type_code) {
                return index;
            }
        }
        return 0;
    }

    f32 GetAreaPickupScale(const WORLDINFO *world) {
        if (world == NULL || world->area == NULL) {
            return 1.0f;
        }
        if ((world->area->flags & AREAFLAG_NOPICKUPGRAVITY) != 0) {
            return 6.0f;
        }
        if ((world->area->flags & AREAFLAG_VEHICLE_AREA) != 0) {
            return BonusArea == 0 ? 5.0f : 3.0f;
        }
        return 1.0f;
    }

    void ClearPickupProgress(GIZMOPICKUPPROGRESS_s *progress) {
        if (progress == NULL) {
            return;
        }
        for (i32 word = 0; word < GIZMOPICKUP_PROGRESS_WORDS; ++word) {
            progress->collected[word] = 0;
            progress->enabled[word] = 0xffffffff;
            progress->visible[word] = 0xffffffff;
            progress->activated[word] = 0;
        }
    }

    void DrawPickupList(WORLDINFO *world, GIZMOPICKUP_s *pickups, i32 count) {
        if (world == NULL || pickups == NULL || world->lev_objs == NULL || GameCam == NULL) {
            return;
        }

        const f32 maximum_distance = world->gizmo_pickup_sys->draw_distance;
        const f32 maximum_distance_squared = maximum_distance * maximum_distance;
        for (i32 index = 0; index < count; ++index) {
            GIZMOPICKUP_s &pickup = pickups[index];
            pickup.state_flags &= static_cast<u8>(~GIZMOPICKUP_STATE_DRAWN);
            if ((pickup.state_flags & GIZMOPICKUP_STATE_ACTIVE) == 0 ||
                (pickup.state_flags & (GIZMOPICKUP_STATE_VISIBLE | GIZMOPICKUP_STATE_COLLECTED)) !=
                    GIZMOPICKUP_STATE_VISIBLE ||
                (pickup.state_flags & GIZMOPICKUP_STATE_DRAW_VISIBLE) == 0) {
                continue;
            }
            if (pickup.room_index >= 0 && world->rooms_visible_ptr != NULL &&
                world->rooms_visible_ptr[pickup.room_index] == 0) {
                continue;
            }

            const f32 camera_x = pickup.position.x - GameCam->pos.x;
            const f32 camera_z = pickup.position.z - GameCam->pos.z;
            if (camera_x * camera_x + camera_z * camera_z > maximum_distance_squared) {
                continue;
            }

            GIZMO_PICKUP_TYPE *type = GetPickupType(pickup);
            const i32 model_index = type->first_model_id + pickup.model_variant;
            LEVEL_OBJECT_RUNTIME_s &model = world->lev_objs[model_index];
            if (model.active == 0) {
                continue;
            }

            NUMTX matrix;
            if ((type->flags & GIZMOPICKUP_TYPE_DRAW_BOBBING) != 0) {
                NuMtxSetRotationY(&matrix, pickup.draw_rotation);
                NUVEC draw_position = pickup.position;
                draw_position.y += type->bob_height * NU_SIN_LUT(pickup.draw_rotation);
                NuMtxTranslate(&matrix, &draw_position);
            } else if ((type->flags & GIZMOPICKUP_TYPE_DRAW_Y_ROTATION) != 0) {
                NuMtxSetRotationY(&matrix, pickup.draw_rotation);
                NuMtxTranslate(&matrix, &pickup.position);
            } else {
                NuMtxSetTranslation(&matrix, &pickup.position);
            }
            if (AreaPickupScale != 1.0f) {
                NuMtxPreScaleU(&matrix, AreaPickupScale);
            }

            if ((pickup.config_flags & GIZMOPICKUP_CONFIG_DISABLE_SHADOW_MAP) != 0) {
                ResetShadowMapRendering();
            }
            const i32 drawn = NuSpecialDrawAt(&model.special, &matrix);
            pickup.state_flags = static_cast<u8>((pickup.state_flags & ~GIZMOPICKUP_STATE_DRAWN) |
                                                 (drawn != 0 ? GIZMOPICKUP_STATE_DRAWN : 0));

            if (drawn != 0 && type->overlay_model_id != -1) {
                LEVEL_OBJECT_RUNTIME_s &overlay = world->lev_objs[type->overlay_model_id];
                if (overlay.active != 0) {
                    ResetShadowMapRendering();
                    NuSpecialDrawAt(&overlay.special, &matrix);
                    EnableShadowMapRendering(0);
                }
            }
            if ((pickup.config_flags & GIZMOPICKUP_CONFIG_DISABLE_SHADOW_MAP) != 0) {
                EnableShadowMapRendering(0);
            }
        }
    }

    void UpdatePickupList(WORLDINFO *world, GIZMOPICKUP_s *pickups, i32 count, bool play_nearby_sfx) {
        if (world == NULL || pickups == NULL) {
            return;
        }

        const bool detector_phase =
            play_nearby_sfx && FadeSys.fade == 0.0f && NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f;
        const bool minikit_detector = detector_phase && Cheats_CheckFlags(0x200) != 0;
        const bool red_brick_detector = detector_phase && Cheats_CheckFlags(0x40000) != 0;
        NUVEC *nearby_pickup = NULL;
        f32 nearest_distance_squared = 1.0f;

        SuperCounters_ResetProcessed(world);
        for (i32 index = 0; index < count; ++index) {
            GIZMOPICKUP_s &pickup = pickups[index];
            if ((pickup.state_flags & GIZMOPICKUP_STATE_ACTIVE) == 0) {
                continue;
            }

            pickup.draw_rotation = static_cast<u16>(pickup.draw_rotation + 8192.0f * FRAMETIME);
            GIZMO_PICKUP_TYPE *type = GetPickupType(pickup);

            if ((pickup.state_flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
                if ((minikit_detector && (type->flags & GIZMOPICKUP_TYPE_MINIKIT_DETECTOR) != 0) ||
                    (red_brick_detector && (type->flags & GIZMOPICKUP_TYPE_RED_BRICK_DETECTOR) != 0)) {
                    NUVEC detector_position = pickup.position;
                    detector_position.y += 0.5f;
                    MiniKitDetector(&detector_position);
                }

                if (play_nearby_sfx && GameCam != NULL) {
                    const f32 distance_squared = NuVecDistSqr(&pickup.position, &GameCam->pos, NULL);
                    if (distance_squared < nearest_distance_squared) {
                        nearest_distance_squared = distance_squared;
                        nearby_pickup = &pickup.position;
                    }
                }
            }

            if (pickup.remaining_visible_time > 0.0f) {
                if (pickup.remaining_visible_time <= 0.5f && PickUpFlickerFrames > 0 &&
                    PickUpFlickerTest <= PickupFlickerFrame % PickUpFlickerFrames) {
                    pickup.state_flags &= static_cast<u8>(~GIZMOPICKUP_STATE_DRAW_VISIBLE);
                }
                if (MiniCutCam != 2) {
                    pickup.remaining_visible_time -= FRAMETIME;
                    if (pickup.remaining_visible_time < 0.0f) {
                        pickup.state_flags &= static_cast<u8>(~GIZMOPICKUP_STATE_ACTIVE);
                    }
                }
            }

            if ((pickup.state_flags &
                 (GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE | GIZMOPICKUP_STATE_COLLECTED)) ==
                    (GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE) &&
                type->field_0x0f == 0 && type->update_fn != NULL) {
                type->update_fn(world, &pickup);
            }
        }

        if (nearby_pickup != NULL) {
            GameAudio_PlaySfx(0x25, nearby_pickup, 0, 0);
        }
    }

} // namespace

i32 gizmopickup_typeid = -1;
f32 AreaPickupScale;
i32 PickUpFlickerTest = 3;
i32 PickUpFlickerFrames = 6;
i32 PickupFlickerFrame;

static i32 GizmoPickups_GetMaxGizmos(void *pickup) {
    WORLDINFO *world = static_cast<WORLDINFO *>(pickup);
    return world != NULL ? world->current_level->max_pickups : 0;
}

static void GizmoPickups_AddGizmos(GIZMOSYS *gizmo_sys, i32 type_id, void *, void *data) {
    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = static_cast<GIZMOPICKUPRUNTIMESYS_s *>(data);
    if (pickup_sys == NULL || pickup_sys->pickups == NULL) {
        return;
    }
    for (i32 index = 0; index < pickup_sys->pickup_count; ++index) {
        GIZMOPICKUP_s &pickup = pickup_sys->pickups[index];
        if ((pickup.config_flags & GIZMOPICKUP_CONFIG_REGISTER_GIZMO) != 0) {
            AddGizmo(gizmo_sys, type_id, NULL, &pickup);
        }
    }
}

static void GizmoPickups_Update(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->gizmo_pickup_sys == NULL) {
        return;
    }

    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = world->gizmo_pickup_sys;
    if (pickup_sys->pickups != NULL && Missions_PickupsOff(MissionSys) == 0) {
        UpdatePickupList(world, pickup_sys->pickups, pickup_sys->pickup_count, true);
    }
    if (pickup_sys->temporary_pickups != NULL && Missions_PickupsOff(MissionSys) == 0) {
        UpdatePickupList(world, pickup_sys->temporary_pickups, GIZMOPICKUP_TEMPORARY_CAPACITY, false);
    }
}

static void GizmoPickups_Draw(void *world_ptr, void *, float) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->gizmo_pickup_sys == NULL) {
        return;
    }

    EnableShadowMapRendering(0);
    if (TimingBarSet == 5) {
        TBOPENFN(const_cast<char *>("Coins"), 5);
    }

    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = world->gizmo_pickup_sys;
    if (pickup_sys->pickups != NULL && Missions_PickupsOff(MissionSys) == 0) {
        DrawPickupList(world, pickup_sys->pickups, pickup_sys->pickup_count);
    }
    if (pickup_sys->temporary_pickups != NULL && Missions_PickupsOff(MissionSys) == 0) {
        DrawPickupList(world, pickup_sys->temporary_pickups, GIZMOPICKUP_TEMPORARY_CAPACITY);
    }

    if (TimingBarSet == 5) {
        TBCLOSEFN(const_cast<char *>("Coins"), 5);
    }
    ResetShadowMapRendering();
}

static char *GizmoPickup_GetGizmoName(GIZMO *gizmo) {
    GIZMOPICKUP_s *pickup = gizmo != NULL ? static_cast<GIZMOPICKUP_s *>(gizmo->object) : NULL;
    return pickup != NULL ? pickup->name : NULL;
}

static i32 GizmoPickup_GetOutput(GIZMO *gizmo, i32, i32) {
    GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
    return (pickup->state_flags & GIZMOPICKUP_STATE_COLLECTED) != 0;
}

static char *GizmoPickup_GetOutputName(GIZMO *, i32) {
    return const_cast<char *>("Collected");
}

static i32 GizmoPickup_GetNumOutputs(GIZMO *) {
    return 1;
}

static void GizmoPickup_Activate(GIZMO *gizmo, i32 activate) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
    if (activate == 0) {
        pickup->state_flags &= static_cast<u8>(~GIZMOPICKUP_STATE_ENABLED);
    } else {
        pickup->state_flags |= GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_ACTIVATED;
        SuperCounter_ActivateGizmoPickup(gizmo, pickup);

        GIZMO_PICKUP_TYPE *type = GetPickupType(*pickup);
        const bool challenge_type = (type->flags & GIZMOPICKUP_TYPE_CHALLENGE_MODE_FILTER) != 0;
        if ((ChallengeMode != 0) == challenge_type && Mission_Active(NULL) == NULL) {
            if (type->activation_sfx_name != NULL) {
                PlaySfx(type->activation_sfx_name, &pickup->position);
            }
            if (type->debris_id != -1) {
                WORLDINFO *world = WorldInfo_CurrentlyActive();
                if (world != NULL) {
                    AddGameDebris(world->debris_sys, type->debris_id, &pickup->position);
                }
            }
        }
    }

    if (pickup->activation_group == 0) {
        return;
    }
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if (world == NULL || world->gizmo_pickup_sys == NULL || world->gizmo_pickup_sys->temporary_pickups == NULL) {
        return;
    }
    const u8 enabled_and_visible = activate != 0 ? GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE : 0;
    for (i32 index = 0; index < GIZMOPICKUP_TEMPORARY_CAPACITY; ++index) {
        GIZMOPICKUP_s &group_pickup = world->gizmo_pickup_sys->temporary_pickups[index];
        if (group_pickup.activation_group == pickup->activation_group) {
            group_pickup.state_flags =
                static_cast<u8>((group_pickup.state_flags & ~(GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE)) |
                                enabled_and_visible);
        }
    }
}

static void GizmoPickup_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo == NULL) {
        return;
    }
    GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
    pickup->state_flags = static_cast<u8>((pickup->state_flags & ~GIZMOPICKUP_STATE_VISIBLE) | ((visible != 0) << 2));
}

static NUVEC *GizmoPickup_GetPos(GIZMO *gizmo) {
    if (gizmo != NULL) {
        GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
        return &pickup->position;
    }
    return NULL;
}

static void *GizmoPickups_AllocateProgressData(VARIPTR *buffer, VARIPTR *buffer_end) {
    return GizmoBufferAlloc(buffer, buffer_end, sizeof(GIZMOPICKUPPROGRESS_s));
}

static void GizmoPickups_ClearProgress(void *, void *progress_ptr) {
    ClearPickupProgress(static_cast<GIZMOPICKUPPROGRESS_s *>(progress_ptr));
}

static void GizmoPickups_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    GIZMOPICKUPPROGRESS_s *progress = static_cast<GIZMOPICKUPPROGRESS_s *>(progress_ptr);
    ClearPickupProgress(progress);
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (progress == NULL || world == NULL || world->gizmo_pickup_sys == NULL ||
        world->gizmo_pickup_sys->pickups == NULL) {
        return;
    }

    const i32 count = world->gizmo_pickup_sys->pickup_count < GIZMOPICKUP_PROGRESS_CAPACITY
                          ? world->gizmo_pickup_sys->pickup_count
                          : GIZMOPICKUP_PROGRESS_CAPACITY;
    for (i32 index = 0; index < count; ++index) {
        const GIZMOPICKUP_s &pickup = world->gizmo_pickup_sys->pickups[index];
        const i32 word = index >> 5;
        const u32 bit = 1u << (index & 31);
        if ((pickup.state_flags & GIZMOPICKUP_STATE_VISIBLE) == 0) {
            progress->visible[word] &= ~bit;
        }
        if ((pickup.state_flags & GIZMOPICKUP_STATE_ENABLED) == 0) {
            progress->enabled[word] &= ~bit;
        }
        if ((pickup.state_flags & GIZMOPICKUP_STATE_COLLECTED) != 0) {
            progress->collected[word] |= bit;
        }
    }
}

static void GizmoPickups_Reset(void *world_ptr, void *, void *progress_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZMOPICKUPPROGRESS_s *progress = static_cast<GIZMOPICKUPPROGRESS_s *>(progress_ptr);

    for (i32 index = 0; index < GizmoPickupSys->type_count; ++index) {
        GIZMO_PICKUP_TYPE &type = GizmoPickupSys->types[index];
        type.shadow_extent_x = type.shadow_radius_x * type.draw_distance;
        type.shadow_extent_z = type.shadow_radius_z * type.draw_distance;
    }
    if (world == NULL || world->gizmo_pickup_sys == NULL) {
        return;
    }

    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = world->gizmo_pickup_sys;
    for (i32 index = 0; index < pickup_sys->pickup_count; ++index) {
        GIZMOPICKUP_s &pickup = pickup_sys->pickups[index];
        pickup.type_index = static_cast<u8>(FindPickupTypeIndex(pickup.type_code));

        NewTerrPlatformsOff();
        pickup.floor_height = GameShadow(NULL, &pickup.position, 5.0f, -1);
        if (pickup.floor_height != -1.0f) {
            if (pickup.floor_height < pickup.position.y) {
                FindAnglesZX(&ShadNorm, &pickup.shadow_x_rotation, &pickup.shadow_z_rotation);
            } else {
                pickup.floor_height = 2000000.0f;
            }
        }

        pickup.state_flags = GIZMOPICKUP_STATE_ACTIVE | GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE |
                             GIZMOPICKUP_STATE_DRAW_VISIBLE;
        GIZMO_PICKUP_TYPE &type = GizmoPickupSys->types[pickup.type_index];
        pickup.model_variant = 0;
        if (type.random_model_count != 0) {
            pickup.model_variant = static_cast<u8>(static_cast<u16>(qrand()) / (0xffff / type.random_model_count + 1));
        }
        pickup.room_index = world->current_gscn != NULL
                                ? static_cast<i8>(NuPortalWhichRoom(world->current_gscn, &pickup.position))
                                : -1;
        pickup.draw_rotation = static_cast<u16>(qrand());
        pickup.remaining_visible_time = 0.0f;

        if (progress != NULL && index < GIZMOPICKUP_PROGRESS_CAPACITY) {
            const i32 word = index >> 5;
            const u32 bit = 1u << (index & 31);
            pickup.state_flags =
                static_cast<u8>((pickup.state_flags & ~(GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE |
                                                        GIZMOPICKUP_STATE_COLLECTED | GIZMOPICKUP_STATE_ACTIVATED)) |
                                ((progress->enabled[word] & bit) != 0 ? GIZMOPICKUP_STATE_ENABLED : 0) |
                                ((progress->visible[word] & bit) != 0 ? GIZMOPICKUP_STATE_VISIBLE : 0) |
                                ((progress->collected[word] & bit) != 0 ? GIZMOPICKUP_STATE_COLLECTED : 0) |
                                ((progress->activated[word] & bit) != 0 ? GIZMOPICKUP_STATE_ACTIVATED : 0));
        }
        if ((pickup.config_flags & GIZMOPICKUP_CONFIG_REQUIRES_ACTIVATION) != 0 &&
            (pickup.state_flags & GIZMOPICKUP_STATE_ACTIVATED) == 0) {
            pickup.state_flags &= static_cast<u8>(~(GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE));
        }
    }

    if (pickup_sys->temporary_pickups != NULL) {
        memset(pickup_sys->temporary_pickups, 0, sizeof(GIZMOPICKUP_s) * GIZMOPICKUP_TEMPORARY_CAPACITY);
    }
}

static void *GizmoPickups_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->current_level == NULL) {
        return NULL;
    }

    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = static_cast<GIZMOPICKUPRUNTIMESYS_s *>(
        GameBufferAlloc(&world->giz_buffer, &world->unknown_0108, sizeof(GIZMOPICKUPRUNTIMESYS_s)));
    if (pickup_sys == NULL) {
        return NULL;
    }
    pickup_sys->draw_distance = 10.0f;
    pickup_sys->pickup_scale = GetAreaPickupScale(world);
    AreaPickupScale = pickup_sys->pickup_scale;
    SetAreaPickupGravity(world->level_sub_id, world->level_idx);

    if (world->current_level->max_pickups != 0) {
        pickup_sys->pickups = static_cast<GIZMOPICKUP_s *>(GameBufferAlloc(
            &world->giz_buffer, &world->unknown_0108, world->current_level->max_pickups * sizeof(GIZMOPICKUP_s)));
    }
    pickup_sys->temporary_pickups = static_cast<GIZMOPICKUP_s *>(GameBufferAlloc(
        &world->giz_buffer, &world->unknown_0108, GIZMOPICKUP_TEMPORARY_CAPACITY * sizeof(GIZMOPICKUP_s)));
    world->gizmo_pickup_sys = pickup_sys;
    return pickup_sys;
}

static i32 GizmoPickups_Load(void *world_ptr, void *data) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    GIZMOPICKUPRUNTIMESYS_s *pickup_sys = static_cast<GIZMOPICKUPRUNTIMESYS_s *>(data);
    if (world == NULL || pickup_sys == NULL || pickup_sys->pickup_count != 0) {
        return 0;
    }

    const i32 version = EdFileReadInt();
    pickup_sys->pickup_count = EdFileReadInt();
    if (version >= 3) {
        pickup_sys->total_score = EdFileReadInt();
    }
    if (version >= 5) {
        pickup_sys->draw_distance = EdFileReadFloat();
        pickup_sys->pickup_scale = EdFileReadFloat();
        if (version == 5) {
            pickup_sys->pickup_scale = GetAreaPickupScale(world);
        }
        AreaPickupScale = pickup_sys->pickup_scale;
    } else {
        pickup_sys->pickup_scale = 1.0f;
        AreaPickupScale = 1.0f;
    }

    if (pickup_sys->draw_distance < 10.0f) {
        pickup_sys->draw_distance = 10.0f;
    }
    if (version == 6 && world->area != NULL && (world->area->flags & AREAFLAG_VEHICLE_AREA) != 0 &&
        pickup_sys->draw_distance < 100.0f) {
        pickup_sys->draw_distance = 100.0f;
    }
    SetAreaPickupGravity(world->level_sub_id, world->level_idx);

    for (i32 index = 0; index < pickup_sys->pickup_count; ++index) {
        GIZMOPICKUP_s &pickup = pickup_sys->pickups[index];
        EdFileRead(pickup.name, sizeof(pickup.name));
        EdFileReadNuVec(&pickup.position);
        pickup.type_code = static_cast<char>(EdFileReadChar());
        if (version >= 2) {
            pickup.config_flags = static_cast<u8>(EdFileReadChar());
        }
        if (version >= 4) {
            pickup.activation_group = static_cast<u8>(EdFileReadChar());
        }
    }
    return 1;
}

void GizmoPickups_PostLoad(void *, void *) {
}

ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE addtype;

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "GizmoPickup";
    addtype.prefix = "";
    addtype.fns.unknown1 = 0x100;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_max_gizmos_fn = GizmoPickups_GetMaxGizmos;
    addtype.fns.get_pos_fn = GizmoPickup_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.add_gizmos_fn = GizmoPickups_AddGizmos;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.late_update_fn = GizmoPickups_Update;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.draw_fn = GizmoPickups_Draw;
    addtype.fns.get_gizmo_name_fn = GizmoPickup_GetGizmoName;
    addtype.fns.get_output_fn = GizmoPickup_GetOutput;
    addtype.fns.get_output_name_fn = GizmoPickup_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizmoPickup_GetNumOutputs;
    addtype.fns.activate_fn = GizmoPickup_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = GizmoPickup_SetVisibility;
    addtype.fns.allocate_progress_data_fn = GizmoPickups_AllocateProgressData;
    addtype.fns.clear_progress_fn = GizmoPickups_ClearProgress;
    addtype.fns.store_progress_fn = GizmoPickups_StoreProgress;
    addtype.fns.reset_fn = GizmoPickups_Reset;
    addtype.fns.reserve_buffer_space_fn = GizmoPickups_ReserveBufferSpace;
    addtype.fns.load_fn = GizmoPickups_Load;
    addtype.fns.post_load_fn = GizmoPickups_PostLoad;
    addtype.fns.add_level_sfx_fn = NULL;
    gizmopickup_typeid = type_id;

    return &addtype;
}
