#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"

extern i32 VehicleArea;

f32 COINMAGNETSCALE = 3.0f;
f32 COINMSGTIME = 1.0f;
f32 (*GizmoPickups_Collide2DFn)(GameObject_s *) = NULL;

i32 SphereSphereOverlapScaleY(nuvec_s *, f32, f32, nuvec_s *, f32, f32);

void GizmoPickup_InBox(WORLDINFO_s *, i32, nuvec_s *, nuvec_s *) {
}

static __used__ GIZMOPICKUP_s *GizmoPickups_CollideList(GameObject_s *, GIZMOPICKUP_s *, i32);

GIZMOPICKUP_s *GizmoPickups_Collide(WORLDINFO_s *world, GameObject_s *object, i32) {
    if (world == NULL || (object->apiobj.field_0x1f4 & 0x00040000) != 0) {
        return NULL;
    }

    GIZMOPICKUP_s *pickup = GizmoPickups_CollideList(object, world->gizmo_pickup_sys->temporary_pickups, 64);
    if (pickup != NULL) {
        pickup->state_flags =
            static_cast<u8>((pickup->state_flags | GIZMOPICKUP_STATE_COLLECTED) & ~GIZMOPICKUP_STATE_ACTIVE);
    } else {
        pickup =
            GizmoPickups_CollideList(object, world->gizmo_pickup_sys->pickups, world->gizmo_pickup_sys->pickup_count);
        if (pickup == NULL) {
            return NULL;
        }
        pickup->state_flags |= GIZMOPICKUP_STATE_COLLECTED;
    }

    i32 type_index = pickup->type_index;
    if ((pickup->state_flags & GIZMOPICKUP_STATE_ALTERNATE_TYPE) != 0 && GizmoPickupSys_Game.alternate_type != -1) {
        type_index = GizmoPickupSys_Game.alternate_type;
    }
    GIZMO_PICKUP_TYPE *type = &GizmoPickupSys_Game.types[type_index];
    if (type->collection_sfx_name != NULL) {
        PlaySfx(type->collection_sfx_name, &pickup->position);
    }
    if (type->collect_fn != NULL) {
        type->collect_fn(world, pickup, type_index, object, 0);
    }
    if ((type->flags & GIZMOPICKUP_TYPE_FLAG_40) != 0) {
        ++AreaGlobals.values.field_0x18;
    }
    return pickup;
}

void GizmoPickups_SetOnOff() {
}

void GizmoPickup_FindByName(WORLDINFO_s *, char *) {
}

extern ADDGAMEMSG AddGameMsg_Default;
extern i32 DoubleScore;
extern TIMER BonusTimer;

GAMEMESSAGE_s *AddGameMsg(ADDGAMEMSG *message);
i32 CoinsGoToMainTotal();
void EndScoreMessage(GAMEMESSAGE_s *message);
void GameMsg_DrawAdjustNewPos_CoinToTotal(GAMEMESSAGE_s *message);
void NewBuzz(nupad_s *pad, f32 amount, i32 flags);
void NewBuzzFrames(nupad_s *pad, i32 frames, i32 flags);

void GizmoPickup_CollectCoin(WORLDINFO_s *world, nuvec_s *position, i32 type_index, i32 model_variant,
                             GameObject_s *object, i32) {
    i32 player_index;
    if (object != NULL) {
        player_index = object->apiobj.field_0x27c;
        if (player_index > 1) {
            return;
        }
    } else {
        i32 player0_active = Player[0] == NULL ? 0 : static_cast<i8>(Player[0]->apiobj.flags_low) < 0;
        if (Player[1] == NULL || static_cast<i8>(Player[1]->apiobj.flags_low) >= 0) {
            if (player0_active == 0) {
                return;
            }
            player_index = 0;
        } else if (player0_active == 0) {
            player_index = 1;
        } else {
            player_index = qrand() / 0x8000;
        }
    }

    if (static_cast<u32>(type_index - 2) < 2 && object != NULL) {
        if (world->area == NULL || (world->area->flags & AREAFLAG_SUPER_BONUS_AREA) != AREAFLAG_BONUS_AREA) {
            NewBuzz(object->pad_gamepad->pad, 0.1f, 0);
        } else {
            NewBuzzFrames(object->pad_gamepad->pad, 1, 0);
        }
    }

    PlaySfx(BonusTimer.time_elapsed > 0.0f || static_cast<u32>(type_index - 2) < 2 ? const_cast<char *>("PickupCoinB")
                                                                                   : const_cast<char *>("PickupCoin"),
            position);

    const bool main_total = CoinsGoToMainTotal() != 0;
    NUVEC target_position;
    ADDGAMEMSG message = AddGameMsg_Default;
    if (main_total) {
        target_position.x = cointotal_x[player_index];
        target_position.y = STATSPOSY;
        DrawCoinTotalTime = COINMSGTIME + FRAMETIME;
        message.target_scale = COINTOTAL_COINSIZE;
    } else {
        target_position.x = player_index == 1 ? PANEL_COINX : -PANEL_COINX;
        target_position.y = STATSPOSY + PANEL_COINY;
        DrawBuildUpTime = COINMSGTIME + FRAMETIME;
        message.target_scale = PANEL_COINSCALE_END;
    }
    target_position.z = 1.0f;

    GIZMO_PICKUP_TYPE *type = &GizmoPickupSys_Game.types[type_index];
    const i32 model_id = type->first_model_id + model_variant;
    if (model_id == -1 || world->lev_objs[model_id].active == 0) {
        return;
    }

    u32 score = type->score;
    if (object != NULL && object->coinpacket != NULL) {
        if (((DoubleScore >> player_index) & 1) != 0) {
            score *= 2;
        }
        if (object->field_0xdec > 0.0f) {
            score *= 2;
        }
    }

    message.position = position;
    message.target_position = &target_position;
    message.scale = PANEL_COINSCALE_START * AreaPickupScale;
    message.flags = 0x2012d;
    message.duration = COINMSGTIME;
    message.icon = static_cast<i16>(model_id);
    message.extra_position = reinterpret_cast<nuvec_s *>(&world->lev_objs[model_id]);
    message.score = score;
    if (main_total) {
        message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
    }
    message.end_fn = EndScoreMessage;
    message.player_index = static_cast<i8>(player_index);
    AddGameMsg(&message);
}

void GizmoPickup_FindNearest(WORLDINFO_s *, nuvec_s *, float *) {
}

void GizmoPickups_TotalScore(void *) {
}

void GizmoPickup_BeenTurnedOn(GIZMOPICKUP_s *) {
}

void GizmoPickup_NumberOfType(WORLDINFO_s *, i32, char) {
}

void GizmoPickup_TurnOnPickup(GIZMOPICKUP_s *) {
}

// Static pickup collision helpers. Moved from gizmisc_stubs.cpp.

static __used__ float GizmoPickups_Collide2D(GameObject_s *) {
    return 0;
}

static __used__ GIZMOPICKUP_s *GizmoPickups_CollideList(GameObject_s *object, GIZMOPICKUP_s *pickups, i32 count) {
    if (pickups == NULL) {
        return NULL;
    }

    bool collide_2d = false;
    if ((object->apiobj.character_data->model_flags & 0x2000) != 0) {
        collide_2d = VehicleArea != 0;
    }

    f32 collide_scale = 0.0f;
    if (GizmoPickups_Collide2DFn != NULL) {
        collide_scale = GizmoPickups_Collide2DFn(object);
        if (collide_scale != 0.0f) {
            collide_2d = true;
        }
    }

    const bool coin_magnet = Cheats_CheckFlags(0x8000) != 0 || object->field_0xdec > 0.0f;
    f32 scaled_pickup = coin_magnet ? COINMAGNETSCALE * AreaPickupScale : AreaPickupScale;
    const f32 normal_pickup = AreaPickupScale;
    if (collide_scale != 0.0f) {
        scaled_pickup *= collide_scale;
    }
    if (collide_2d && VehicleArea != 0) {
        scaled_pickup += scaled_pickup;
    }

    for (i32 index = 0; index < count; ++index) {
        GIZMOPICKUP_s *pickup = &pickups[index];
        GIZMO_PICKUP_TYPE *type = &GizmoPickupSys_Game.types[pickup->type_index];
        u8 flags = pickup->state_flags;
        if ((type->flags & GIZMOPICKUP_TYPE_CHALLENGE_MODE_FILTER) != 0 ||
            (flags & (GIZMOPICKUP_STATE_ACTIVE | GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_COLLECTED)) !=
                (GIZMOPICKUP_STATE_ACTIVE | GIZMOPICKUP_STATE_ENABLED) ||
            ((flags & GIZMOPICKUP_STATE_DRAWN) == 0 && (pickup->config_flags & 4) == 0)) {
            continue;
        }

        if ((flags & GIZMOPICKUP_STATE_ALTERNATE_TYPE) != 0 && GizmoPickupSys_Game.alternate_type != -1) {
            type = &GizmoPickupSys_Game.types[GizmoPickupSys_Game.alternate_type];
        }
        if (type->field_0x0f != 0 && pickups != WorldInfo_CurrentlyActive()->gizmo_pickup_sys->temporary_pickups) {
            continue;
        }
        if ((pickup->config_flags & 4) != 0) {
            return pickup;
        }

        f32 scale = type->score == 0 ? normal_pickup : scaled_pickup;
        f32 radius_x = type->shadow_extent_x * scale;
        f32 radius_y = scale * (coin_magnet ? type->shadow_extent_z : type->shadow_radius_z);
        if (object->apiobj.collision_min.x > pickup->position.x + radius_x ||
            pickup->position.x - radius_x > object->apiobj.collision_max.x ||
            object->apiobj.collision_min.z > pickup->position.z + radius_x ||
            pickup->position.z - radius_x > object->apiobj.collision_max.z) {
            continue;
        }

        if (collide_2d) {
            f32 dx = pickup->position.x - object->apiobj.collision_position.x;
            f32 dz = pickup->position.z - object->apiobj.collision_position.z;
            f32 radius = radius_x + object->apiobj.field_0x1dc;
            if (dx * dx + dz * dz < radius * radius) {
                return pickup;
            }
        } else if (object->apiobj.collision_min.y <= pickup->position.y + radius_y &&
                   pickup->position.y - radius_y <= object->apiobj.collision_max.y) {
            if (object->field_0xcc0 != NULL) {
                f32 dx = pickup->position.x - object->apiobj.collision_position.x;
                f32 dz = pickup->position.z - object->apiobj.collision_position.z;
                f32 radius = radius_x + object->apiobj.field_0x1dc;
                if (dx * dx + dz * dz < radius * radius) {
                    return pickup;
                }
            } else if (SphereSphereOverlapScaleY(&pickup->position, radius_x, radius_y,
                                                 &object->apiobj.collision_position, object->apiobj.collision_radius,
                                                 object->apiobj.field_0x1e0)) {
                return pickup;
            }
        }
    }
    return NULL;
}

static __used__ void Pup_CollectHeart(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectCharKit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectMinikit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectPowerUp(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectRedBrick(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_UpdatePowerUp(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdateBlueCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdatePurpleCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}
