#include "legoapi/move.h"

#include <string.h>

#include "legoapi/bolts.h"
#include "legoapi/gamepads.h"
#include "legoapi/level.h"
#include "legoapi/players.h"
#include "legoapi/qrand.h"
#include "legoapi/world.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

struct GAMECAMERA_s;
struct AREADATA_s;
struct HINT_s;
struct nuhspecial_s;

struct MoveTerrainEntry {
    u32 unknown_0;
    u32 flags;
    u32 unknown_8;
};

extern "C" {
    extern i16 id_JEDISTARFIGHTERREDEP3;
    extern i16 id_JEDISTARFIGHTERYELLOWEP3;
    extern i16 id_TIEINTERCEPTOR;
    extern i32 LEGOCONTEXT_GLIDE;
    extern i32 VehicleArea;
    extern f32 CurrentSpeed;
    extern f32 ForceThrowGravity;
    extern f32 FRAMETIME;
    extern u8 *CInfo;
    extern u32 GAMEPAD_ACTION;
    extern u32 GAMEPAD_JUMP;
    extern f32 SLAMJUMPSPEED;
    extern f32 MAGNETOFFSET;
    extern i32 ForcePush_SuperPush;
    extern i32 disable_narrow_socks;
    extern void *HUB_ADATA;
    extern u8 BladeTab[];
    extern u8 Default_ADDPART[200];
    extern void *PODSPRINT_ADATA;
    extern AREADATA_s *PODRACE_ADATA;
    extern LEVELDATA_s *DOGFIGHTA_LDATA;
    extern LEVELDATA_s *BONUS_GUNSHIPA_LDATA;
    extern LEVELDATA_s *BONUS_GUNSHIPB_LDATA;
    extern LEVELDATA_s *SPEEDERCHASEA_LDATA;
    extern LEVELDATA_s *HOTHBATTLEA_LDATA;
    extern LEVELDATA_s *DEATHSTARBATTLED_LDATA;
    extern i32 IDLESPEEDINNARROWSOCKSONLY;
    extern f32 TURNTIME;
    extern i16 id_GAMORREANGUARD;
    extern i16 id_GONKDROID;
    extern i16 id_GRIEVOUS;
    extern i16 id_DARTHMAUL;
    extern i16 id_SPEEDERBIKE;
    extern MoveTerrainEntry TerLayer[];
    extern i16 id_MINISLAVE1;
    f32 AnimDuration(i32 character_id, i32 action, f32 start, f32 end, i32 loop);
    f32 AnimSpeed(GAMECHARACTERDATA *character_data, i32 action);
    void NuVecRotateX(NUVEC *destination, NUVEC *source, i32 angle);
    void NuVecRotateY(NUVEC *destination, NUVEC *source, i32 angle);
    void NuVecInvMtxRotate(NUVEC *destination, NUVEC *source, NUMTX *matrix);
    i32 AnimPlaying(void *animation_packet, i32 action, i32 unknown_2, i32 unknown_3);
    void ResetAnimPacket(void *animation_packet, i32 action);
    void ResetMiniAnimPacket(void *animation_packet, i32 action);
    void AddGameDebris(void *system, i32 debris_id, NUVEC *position);
    i32 NuSpecialExistsFn(nuhspecial_s *special);
    void NuSpecialSetVisibility(nuhspecial_s *special, i32 visible);
    NUMTX *NuSpecialGetDrawMtx(nuhspecial_s *special);
    void NuSpecialSetDrawMtx(nuhspecial_s *special, NUMTX *matrix);
    void NuSpecialUpdate(nuhspecial_s *special);
    void NuSpecialGetRadius(nuhspecial_s *special, NUVEC *center, f32 *radius);
    void *AddPart(void *configuration);
}

extern GAMECAMERA_s *GameCam;
void GameCam_Blend(GAMECAMERA_s *camera, f32 time, f32 delay, i32 mode);
i32 ObjInNarrowSock(GameObject_s *object, void *sockets, i32 count);
void ChatterSfx(GameObject_s *object, i32 sfx, f32 volume);
void Move_VEHICLE(GameObject_s *object);
void PlayJumpSfx(GameObject_s *object, i32 secondary);
void Player_ResetContexts(PLAYERPACKET *packet);
void KillGameObject(GameObject_s *object, i32 reason, i32 immediate);
void LetGoOfBalloon(GameObject_s *object);
void NewBuzzFrames(NUPAD *pad, i32 frames, i32 delay);
void GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 unknown_2, i32 unknown_3);
void TurnCodeCamSafe(GameObject_s *object, NUMTX *matrix);
void StartJump(GameObject_s *object, i32 mode);
i32 Slam_Start(GameObject_s *object, f32 speed);
void StartEndOfJump(GameObject_s *object);
void StartHold(GameObject_s *object);
void FastWeaponIn(GameObject_s *object, i32 immediate);
void FastWeaponOut(GameObject_s *object, i32 immediate);
void PlayLandSfx(GameObject_s *object, i32 secondary);
i32 ObjLandReady(GameObject_s *object);
i32 RotDiff(u16 first, u16 second);
i32 Cheat_IsOn(i32 cheat);
i32 CannotKill(GameObject_s *object);
i32 ObjHitObj_Flags(GameObject_s *object);
bool Player_HasPurpleForce(GameObject_s *object);
void PartCollide_3D(PART_s *part);
bool MosEisleyC_PastBarrier(GameObject_s *object);
extern "C" void AddVariableShotDebrisEffectTimed1(void *debris, NUVEC *position, i32 count, f32 time, i32 unknown_4,
                                                  i32 unknown_5, void *unknown_6);
extern "C" i32 PlaySfx(const char *name, NUVEC *position);
void SeekVec(NUVEC *output, NUVEC *current, NUVEC *target, f32 speed);

template <typename T> static T &ObjectField(GameObject_s *object, size_t offset) {
    return *(T *)((u8 *)object + offset);
}

bool TwistLevel(LEVELDATA_s *level) {
    return DOGFIGHTA_LDATA != NULL && DOGFIGHTA_LDATA == level;
}

bool GunshipInLevel(LEVELDATA_s *level) {
    return BONUS_GUNSHIPA_LDATA != NULL && BONUS_GUNSHIPA_LDATA == level;
}

bool CircleLevel(LEVELDATA_s *level) {
    return BONUS_GUNSHIPB_LDATA != NULL && BONUS_GUNSHIPB_LDATA == level;
}

bool PodLevel(AREADATA_s *area) {
    return PODRACE_ADATA != NULL && PODRACE_ADATA == area;
}

static GameObject_s *ForceBackObj;
static NUVEC *ForceBackPos;
static i32 ForceBackType;
static f32 ForceBackRadius;
static f32 ForceBackRadius2;
static i32 getvehiclehoverheight_hothbattlehack;

i32 ObjZappedBlue(GameObject_s *object) {
    return object->player_packet.character_state == 0x42 ||
           (object->player_packet.character_state == 0x1c && object->player_packet.opponent != NULL &&
            (object->player_packet.opponent->player_packet.runtime_flags_0 & 0x100) != 0);
}

bool FaceOpponent(GameObject_s *object, NUVEC *position) {
    if (position == NULL) {
        GameObject_s *opponent = object->player_packet.opponent;
        if (opponent != NULL) {
            if ((opponent->state_flags & 0x1001) != 0x1001 || opponent->death_state != 0) {
                return false;
            }
            position = &opponent->position;
        } else {
            if (object->player_packet.opponent_target == NULL) {
                return false;
            }
            position = (NUVEC *)((u8 *)object->player_packet.opponent_target + 0x50);
        }
    }
    object->desired_heading = NuAtan2D(position->x - object->position.x, position->z - object->position.z);
    return true;
}

f32 GetVehicleAreaRememberSpeed() {
    if (VehicleArea == 0) {
        return 0.0f;
    }
    f32 speed = 0.0f;
    f32 count = 0.0f;
    if (Player[0] != NULL && (i8)Player[0]->state_flags < 0) {
        speed = *(f32 *)((u8 *)Player[0] + 0xdc8);
        count = 1.0f;
    }
    if (Player[1] != NULL && (i8)Player[1]->state_flags < 0) {
        speed += *(f32 *)((u8 *)Player[1] + 0xdc8);
        count += 1.0f;
    }
    if (count > 1.0f) {
        speed /= count;
    }
    return speed < 0.25f ? 0.25f : speed;
}

f32 avg_currentspeed_mul;

void AveragePlayerCurrentSpeedMul() {
    avg_currentspeed_mul = 0.0f;
    bool first_player = false;
    if (Player[0] != NULL && (i8)Player[0]->state_flags < 0) {
        avg_currentspeed_mul = *(f32 *)((u8 *)Player[0] + 0xc34);
        first_player = true;
    }
    if (Player[1] != NULL && (i8)Player[1]->state_flags < 0) {
        avg_currentspeed_mul += *(f32 *)((u8 *)Player[1] + 0xc34);
        if (first_player) {
            avg_currentspeed_mul *= 0.5f;
        }
    }
}

f32 ForceAlongSock(GameObject_s *object) {
    f32 force = 0.0f;
    if (object->sock_index != -1 && CurrentSpeed != 0.0f) {
        force = (ObjectField<f32>(object, 0xc38) + 1.0f) * CurrentSpeed * avg_currentspeed_mul;
        NUVEC movement = {0.0f, 0.0f, force};
        NuVecRotateX(&movement, &movement, ObjectField<u16>(object, 0x684));
        NuVecRotateY(&movement, &movement, ObjectField<u16>(object, 0x686));
        NuVecAdd(&ObjectField<NUVEC>(object, 0xf24), &ObjectField<NUVEC>(object, 0xf24), &movement);
    }
    return force;
}

f32 ForceTowardsMid(GameObject_s *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    u8 *socket_list = *(u8 **)((u8 *)world + 0x2988);
    if (socket_list == NULL || object->sock_index == -1 || ObjectField<u8>(object, 0x68a) >= 2) {
        return 0.0f;
    }

    u8 *socket = *(u8 **)socket_list + object->sock_index * 0x13c;
    u16 flags = *(u16 *)(socket + 0x68);
    f32 inner_radius = *(f32 *)(socket + 0x74);
    f32 outer_radius = *(f32 *)(socket + 0x78);
    f32 force = 0.0f;
    NUVEC &movement = ObjectField<NUVEC>(object, 0xf24);

    if ((flags & 2) == 0) {
        if (inner_radius > 0.0f && outer_radius > 0.0f) {
            NUVEC direction;
            if ((flags & 4) == 0) {
                NuVecSub(&direction, &ObjectField<NUVEC>(object, 0x668), &object->position);
            } else {
                direction.x = ObjectField<f32>(object, 0x668) - object->position.x;
                direction.y = 0.0f;
                direction.z = ObjectField<f32>(object, 0x670) - object->position.z;
            }
            f32 distance_squared = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
            if (distance_squared >= inner_radius * inner_radius) {
                f32 distance = NuFsqrt(distance_squared);
                force = (distance - inner_radius) / (outer_radius - inner_radius);
                if (force > 3.0f) {
                    force = 3.0f;
                }
                if (force >= 0.0f) {
                    if ((flags & 4) == 0 && *(f32 *)(socket + 0x7c) != 1.0f) {
                        if (object->terrain_contact_age == 4) {
                            NuVecInvMtxRotate(&direction, &direction, &ObjectField<NUMTX>(object, 0xbf4));
                            direction.y /= *(f32 *)(socket + 0x7c);
                            NuVecMtxRotate(&direction, &direction, &ObjectField<NUMTX>(object, 0xbf4));
                        } else {
                            i32 x_rotation = RotDiff(0, ObjectField<u16>(object, 0x684));
                            i32 y_rotation = RotDiff(0, ObjectField<u16>(object, 0x686));
                            NuVecRotateY(&direction, &direction, -y_rotation);
                            NuVecRotateX(&direction, &direction, -x_rotation);
                            direction.y /= *(f32 *)(socket + 0x7c);
                            NuVecRotateX(&direction, &direction, x_rotation);
                            NuVecRotateY(&direction, &direction, y_rotation);
                        }
                    }
                    f32 scale = (*(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x1c) * force) /
                                NuFsqrt(distance_squared);
                    movement.x += direction.x * scale;
                    if ((flags & 4) == 0) {
                        movement.y += direction.y * scale;
                    }
                    movement.z += direction.z * scale;
                }
            }
        }
        if ((flags & 4) == 0) {
            return force;
        }
        if ((flags & 8) != 0) {
            f32 center_y = ObjectField<f32>(object, 0x66c);
            if (object->position.y >= center_y) {
                u8 *character = (u8 *)object->character_data;
                f32 top = center_y + *(f32 *)(character + 0x30) * *(f32 *)(character + 0x3c) * 3.0f;
                f32 height = object->position.y >= top ? 1.0f : (object->position.y - center_y) / (top - center_y);
                movement.y -= height * *(f32 *)(*(u8 **)(character + 0x24) + 0x1c);
            }
        }
    } else if (object->terrain_contact_age != 4 && inner_radius > 0.0f && outer_radius > 0.0f) {
        f32 vertical_distance = ObjectField<f32>(object, 0x66c) - object->position.y;
        if (vertical_distance * vertical_distance >= inner_radius * inner_radius) {
            force = (NuFabs(vertical_distance) - inner_radius) / (outer_radius - inner_radius);
            movement.y += vertical_distance * *(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x1c) * force;
        }
    }
    return force;
}

void CurrentStart(GameObject_s *object, i32 always, i32 use_socket_rotation) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ObjectField<f32>(object, 0xc3c) = 0.0f;
    if (object->sock_index == -1) {
        return;
    }

    u8 *socket_list = *(u8 **)((u8 *)world + 0x2988);
    if (socket_list == NULL) {
        return;
    }

    u8 *socket = *(u8 **)socket_list + object->sock_index * 0x13c;
    f32 speed = *(f32 *)(socket + 0x70);
    if (speed == 0.0f || (!always && !TwistLevel(world->current_level))) {
        return;
    }

    NUVEC socket_movement = {0.0f, 0.0f, speed};
    ObjectField<f32>(object, 0xc3c) = speed;
    if ((ObjectField<u8>(object, 0xf02) & 0x20) == 0) {
        socket_movement.z *= ObjectField<f32>(object, 0xc34);
    } else {
        socket_movement.z *= ObjectField<f32>(object, 0xed4);
    }

    if (use_socket_rotation == 0) {
        NuVecRotateY(&object->velocity, &object->velocity, object->yaw);
    } else {
        NuVecRotateX(&object->velocity, &socket_movement, ObjectField<u16>(object, 0x684));
        NuVecRotateY(&object->velocity, &object->velocity, ObjectField<u16>(object, 0x686));
    }
    ObjectField<u8>(object, 0x4a6) &= ~4;
    ObjectField<NUVEC>(object, 0xf24) = object->velocity;
}

f32 VehicleTurnOrLoopOffset(GameObject_s *object) {
    i8 state = object->player_packet.character_state;
    f32 elapsed = *(f32 *)((u8 *)&object->player_packet + 0xb8);
    f32 duration = *(f32 *)((u8 *)&object->player_packet + 0xc0);
    f32 progress = 1.0f - elapsed / duration;
    if (state == 0x2a || state == 0x36) {
        f32 wave = (1.0f - NU_SIN_LUT(progress * 65536.0f + 16384.0f)) * 0.5f;
        f32 height = *(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x88);
        return state == 0x2a ? wave * height * 0.5f : wave * height;
    }
    if (state == 0x3a && (i8)object->state_flags < 0 && PODSPRINT_ADATA != NULL &&
        PODSPRINT_ADATA == *(void **)((u8 *)WORLD + 0x12c)) {
        return NU_SIN_LUT(progress * 32768.0f) * 0.25f;
    }
    return 0.0f;
}

f32 GetHoverPosY(GameObject_s *object) {
    u8 *level = (u8 *)WORLD->current_level;
    if ((((u8 *)object->character_data)[5] & 0x20) == 0 || (level[0x66] & 4) != 0) {
        return 2000000.0f;
    }
    f32 turn_offset = VehicleTurnOrLoopOffset(object);
    if (object->edge_surface_type == 0x10 || object->edge_surface_type == 7) {
        return object->edge_shadow_height + turn_offset;
    }
    if (object->floor_height == 2000000.0f) {
        return 2000000.0f;
    }
    f32 surface_height = object->floor_height;
    if (object->edge_shadow_height != 2000000.0f && object->edge_shadow_height > surface_height) {
        surface_height = object->edge_shadow_height;
    }
    f32 level_hover_height = *(f32 *)(level + 0xb4);
    f32 hover_height = level_hover_height != 0.0f ? level_hover_height
                                                  : *(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x28);
    return surface_height + hover_height + turn_offset;
}

f32 GetVehicleHoverHeight(GameObject_s *object, f32 *vertical_offset) {
    LEVELDATA_s *level = WORLD->current_level;
    f32 hover_height = ObjectField<f32>(object, 0x1034);
    if (hover_height == 1000000000.0f) {
        hover_height = *(f32 *)((u8 *)level + 0xb4);
        if (hover_height == 0.0f) {
            u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
            hover_height = *(f32 *)(runtime + 0x28);
            if (object->character_id == id_SPEEDERBIKE && level == SPEEDERCHASEA_LDATA) {
                if (disable_narrow_socks != 0) {
                    hover_height *= 0.5f;
                }
            }
        }
    }

    bool stack_players = false;
    if (level == SPEEDERCHASEA_LDATA) {
        stack_players = disable_narrow_socks == 0 && object->player_index != -1 &&
                        ObjectField<void *>(object, 0xe70) == NULL && Player[0] != NULL &&
                        ObjectField<void *>(Player[0], 0xcc0) != NULL && Player[0]->character_id == id_SPEEDERBIKE &&
                        Player[1] != NULL && ObjectField<void *>(Player[1], 0xcc0) != NULL &&
                        Player[0]->character_id == Player[1]->character_id;
    } else {
        stack_players = (i8)object->state_flags < 0 && Player[0] != NULL &&
                        (*((u8 *)Player[0]->character_data + 5) & 0x20) != 0 &&
                        ((i8)Player[0]->state_flags < 0 || Player[0]->player_packet.character_state == 0x24) &&
                        Player[1] != NULL && (*((u8 *)Player[1]->character_data + 5) & 0x20) != 0 &&
                        ((i8)Player[1]->state_flags < 0 || Player[1]->player_packet.character_state == 0x24);
    }

    if (vertical_offset != NULL) {
        *vertical_offset = 0.0f;
    }
    if (stack_players) {
        f32 combined_radius = Player[0]->bounds_radius + Player[1]->bounds_radius;
        f32 inner_distance = combined_radius * combined_radius;
        f32 outer_radius = combined_radius * 3.0f;
        f32 outer_distance = outer_radius * outer_radius;
        f32 distance = NuVecXZDistSqr(&Player[0]->position, &Player[1]->position, NULL);
        f32 weight;
        if (distance < inner_distance) {
            weight = 1.0f;
        } else if (distance < outer_distance) {
            weight = 1.0f - (distance - inner_distance) / (outer_distance - inner_distance);
        } else {
            weight = 0.0f;
        }
        f32 offset = weight * (Player[0]->bounds_half_height + Player[1]->bounds_half_height) * 0.6f;
        f32 result = hover_height - offset;
        f32 overlap_correction = result < offset ? offset - result : 0.0f;
        if (object == Player[0]) {
            result = hover_height + offset;
            if (vertical_offset != NULL) {
                *vertical_offset = offset;
            }
        } else if (vertical_offset != NULL) {
            *vertical_offset = -offset;
        }
        hover_height = result + overlap_correction;
    }

    getvehiclehoverheight_hothbattlehack = 0;
    if (level == HOTHBATTLEA_LDATA && (TerLayer[object->ground_surface_type].flags & 1) != 0) {
        hover_height = 0.0f;
        getvehiclehoverheight_hothbattlehack = 1;
    }
    return hover_height;
}

f32 GetVehicleSpeedMul(GameObject_s *object, f32 speed) {
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    f32 minimum_speed = *(f32 *)(runtime + 0x10);
    f32 maximum_speed = *(f32 *)(runtime + 0x1c);
    i8 state = object->player_packet.character_state;
    f32 adjusted_speed;
    if (state == 0x36 || state == 0x2a || state == 0x3a) {
        adjusted_speed = maximum_speed;
    } else if ((i8)object->state_flags < 0) {
        bool narrow_sock = false;
        if (WORLD->current_level == DEATHSTARBATTLED_LDATA) {
            narrow_sock = ObjInNarrowSock(object, *(void **)((u8 *)WORLD + 0x2988), *(i32 *)((u8 *)WORLD + 0x11c)) != 0;
        }
        if (narrow_sock) {
            f32 ratio = (speed - minimum_speed) / (maximum_speed - minimum_speed);
            ratio = ratio < 0.0f ? 0.5f : ratio * 0.5f + 0.5f;
            adjusted_speed = ratio * maximum_speed;
        } else if ((IDLESPEEDINNARROWSOCKSONLY == 0 || ((u8 *)object)[0xe3b] != 0) &&
                   (((u8 *)object)[0xf03] & 2) == 0) {
            adjusted_speed = speed < minimum_speed ? minimum_speed : speed;
        } else {
            adjusted_speed = ((speed - minimum_speed) / (maximum_speed - minimum_speed)) * maximum_speed;
        }
    } else {
        adjusted_speed = ((speed - minimum_speed) / (maximum_speed - minimum_speed)) * maximum_speed;
    }
    if (adjusted_speed < 0.0f) {
        adjusted_speed = 0.0f;
    }
    return adjusted_speed / maximum_speed;
}

void PostResetCode(GameObject_s *object) {
    ((u8 *)object)[0x1091] = 0;
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    i16 chatter = *(i16 *)(runtime + 0xe8);
    if (chatter != -1) {
        ChatterSfx(object, chatter, (f32)runtime[0x11e]);
    }
    void *move_function = *(void **)((u8 *)object->character_data + 0x18);
    i16 *state = (i16 *)((u8 *)object + 0xe1c);
    if (move_function != (void *)Move_VEHICLE && *state != 0 && (object->player_packet.runtime_flags_4 & 0x2000) == 0) {
        *state = 0;
    }
}

void DrawOffsetCode(GameObject_s *object, i32 immediate) {
    NUVEC target = v000;
    if (object->player_packet.equipment != NULL) {
        u16 flags = *(u16 *)((u8 *)object->player_packet.equipment + 0xc);
        if ((flags & 0x10) != 0) {
            target.y = 0.0076f;
        } else if ((flags & 0x40) != 0) {
            target.y = MAGNETOFFSET;
        }
    }

    bool use_cached = false;
    if (object->player_packet.character_state == 0x44 && (u16)(object->player_packet.action - 5) >= 2) {
        use_cached = true;
    }
    if ((ObjectField<u32>(object, 0x7a4) & 0xffff00) == 0x54300) {
        use_cached = true;
    }
    if (use_cached) {
        target = ObjectField<NUVEC>(object, 0x750);
    }

    NUVEC &current = ObjectField<NUVEC>(object, 0xea0);
    if (immediate == 0) {
        if (current.x == target.x && current.y == target.y && current.z == target.z) {
            return;
        }
        NUVEC direction;
        f32 distance = NuVecDist(&target, &current, &direction);
        f32 step = FRAMETIME * 0.4f;
        if (step < distance) {
            NuVecNorm(&direction, &direction);
            NuVecScale(&direction, &direction, step);
            NuVecAdd(&current, &current, &direction);
            return;
        }
    }
    current = target;
}

void NewSeekHalfLife(i32 &current, i32 target, f32 amount) {
    current = (i16)((i16)current + (i16)(i32)((f32)(i16)(target - (i16)current) * amount));
}

void StartJetPackFall(GameObject_s *object, i32 immediate) {
    u8 *packet = (u8 *)&object->player_packet;
    *(f32 *)(packet + 0xb8) = 0.0f;
    u8 combat_flags = object->player_packet.combat_flags_0;
    packet[0xfa] &= 0xfe;
    packet[0x77d] = 0;
    object->player_packet.character_state = 0;
    object->player_packet.combat_flags_0 = combat_flags & 0xaf;
    packet[0xf4] = 0;
    object->player_packet.action = 6;
    packet[0xf5] = 2;
    *(u32 *)(packet + 0x65c) = 0;

    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    if (immediate == 0 && animations[0x49] != NULL) {
        object->player_packet.combat_flags_0 = combat_flags & 0x2f;
        object->player_packet.action = 0x49;
        object->velocity.y = *(f32 *)(runtime + 0x2c);
        PlayJumpSfx(object, 0);
    } else {
        object->player_packet.character_state = -1;
    }
    if (runtime[0x116] == 2) {
        PlayJumpSfx(object, 1);
    }
}

void JetPackCode(GameObject_s *object, i32 land, i32 fall, i32 immediate) {
    i8 state = object->player_packet.character_state;
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    if (state == 0x19 || state == 2 || (runtime[0x116] == 2 && state == 1)) {
        ObjectField<f32>(object, 0x76c) -= FRAMETIME;
        if (ObjectField<f32>(object, 0x76c) <= 0.0f) {
            object->player_packet.character_state = -1;
        }
        return;
    }

    if (state != 0) {
        if (ObjectField<i8>(object, 0xe31) == 1) {
            if ((fall != 0 || land != 0) && state == -1) {
                StartJetPackFall(object, fall);
            }
            return;
        }

        if (state == 0x13) {
            if (AnimPlaying((u8 *)object + 8, object->player_packet.action, 1, 0) == 0) {
                return;
            }
            ObjectField<f32>(object, 0x76c) -= FRAMETIME;
            if (ObjectField<f32>(object, 0x76c) > 0.0f) {
                return;
            }
            ObjectField<i8>(object, 0xe31) = 1;
            PlayJumpSfx(object, 0);
            object->velocity.y = *(f32 *)(runtime + 0x2c);
            if (runtime[0x116] == 2 && object->player_packet.character_state != 7) {
                FastWeaponOut(object, 1);
            }
            ObjectField<u32>(object, 0xd64) = 0;
            object->player_packet.character_state = -1;
            return;
        }

        if (land == 0 || (object->ground_contact_flags == 0 && ObjectField<f32>(object, 0xd18) <= 0.0f) ||
            ObjLandReady(object) == 0) {
            return;
        }
        if (state == 6) {
            FastWeaponIn(object, 0);
        } else if (state == 7) {
            FastWeaponOut(object, 0);
        }
        object->player_packet.action = 0x48;
        f32 duration = AnimDuration(object->character_id, 0x48, 0.0f, 0.0f, 1);
        ObjectField<f32>(object, 0x76c) = duration;
        if (duration <= 0.0f) {
            ObjectField<i8>(object, 0xe31) = 1;
            PlayJumpSfx(object, 0);
            object->velocity.y = *(f32 *)(runtime + 0x2c);
            if (runtime[0x116] == 2 && state != 7) {
                FastWeaponOut(object, 1);
            }
            ObjectField<u32>(object, 0xd64) = 0;
        } else {
            object->player_packet.character_state = 0x13;
        }
        ObjectField<f32>(object, 0xdb4) = 2000000.0f;
        return;
    }

    if (object->ground_contact_flags == 0) {
        return;
    }
    ObjectField<f32>(object, 0x76c) = 0.0f;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    i32 action;
    i8 next_state;
    if (object->player_packet.action == 0x49 && animations[0x4a] != NULL) {
        action = 0x4a;
        next_state = 0x19;
    } else {
        action = 10;
        next_state = 2;
    }
    object->player_packet.action = action;
    f32 duration = AnimDuration(object->character_id, action, 0.0f, 0.0f, 1);
    ObjectField<f32>(object, 0x76c) = duration;
    if (duration > 0.0f) {
        object->player_packet.character_state = next_state;
        ResetAnimPacket((u8 *)object + 8, -1);
    } else {
        object->player_packet.character_state = -1;
    }
    PlayLandSfx(object, 0);
}

void ComboRotateCode(GameObject_s *object, i32 hold) {
    if (object->player_packet.character_state != 9) {
        return;
    }
    f32 elapsed = ObjectField<f32>(object, 0x76c) + FRAMETIME;
    ObjectField<f32>(object, 0x76c) = elapsed;
    i16 rotation;
    if (elapsed != elapsed) {
        rotation = (i32)(elapsed / ObjectField<f32>(object, 0x774) * 32768.0f);
    } else {
        if (hold == 0) {
            object->player_packet.character_state = -1;
        } else {
            StartHold(object);
        }
        rotation = -0x8000;
    }
    i16 start_heading = ObjectField<i16>(object, 0x796);
    i16 heading = ObjectField<i8>(object, 0x79f) == 1 ? start_heading + rotation : start_heading - rotation;
    object->yaw = heading;
    object->desired_heading = heading;
    object->heading = heading;
}

i32 Dodge_UpdateHint(HINT_s *hint) {
    void *area = *(void **)((u8 *)WORLD + 0x12c);
    if (area != NULL && area == HUB_ADATA) {
        return 0;
    }

    i16 hint_id = *(i16 *)hint;
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL) {
            continue;
        }
        u32 character_flags = *(u32 *)((u8 *)object->character_data + 4);
        if ((character_flags & 0x2000) != 0) {
            return 0;
        }
        if ((i8)object->state_flags >= 0 || ObjectField<void *>(object, 0xcf4) == NULL) {
            continue;
        }
        if (hint_id == 0x265) {
            if ((character_flags & 8) == 0) {
                void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
                if (animations[0x4f] != NULL || animations[0x26] != NULL) {
                    return 1;
                }
            }
        } else if (hint_id == 0x5dd && (character_flags & 8) != 0) {
            return 1;
        }
    }
    return 0;
}

void MakeThrowVector(NUVEC *output, NUVEC *source, NUVEC *target, NUVEC *target_velocity, f32 speed, f32 gravity) {
    NUVEC flat_source = {source->x, 0.0f, source->z};
    NUVEC predicted_target = *target;
    NUVEC direction;
    for (i32 iteration = 0; iteration < 3; iteration++) {
        NuVecSub(&direction, &predicted_target, &flat_source);
        f32 distance = NuVecMag(&direction);
        NuVecAddScale(&predicted_target, target, target_velocity, distance / speed);
    }

    f32 distance = NuVecXZDist(&predicted_target, source, NULL);
    f32 flight_time = FRAMETIME + distance / speed;
    u16 heading = NuAtan2D(predicted_target.x - source->x, predicted_target.z - source->z);
    output->y = (predicted_target.y - source->y) / flight_time - gravity * 0.5f * (distance / speed);
    output->x = speed * NuTrigTable[heading >> 1];
    output->z = speed * NuTrigTable[(heading + 0x4000) >> 1 & 0x7fff];
}

void LightSabreDebris(GameObject_s *object) {
    i32 blade_debris = -1;
    i8 blade_index = ObjectField<i8>(object, 0xe42);
    if (blade_index != -1) {
        blade_debris = *(i16 *)(BladeTab + blade_index * 0x10 + 4);
    }

    i32 blade_count;
    if (object->character_id == id_GRIEVOUS) {
        blade_count = 4;
    } else {
        blade_count = object->character_id == id_DARTHMAUL ? 2 : 1;
    }

    for (i32 blade = 0; blade < blade_count; blade++) {
        i32 debris;
        bool grievous_colours =
            object->character_id == id_GRIEVOUS &&
            (object->player_index == -1 || (Cheat_IsOn(0x19) == 0 && !Player_HasPurpleForce(object)));
        if (grievous_colours) {
            debris = blade == 0 || blade == 3 ? 3 : 2;
        } else {
            if (blade_debris == -1) {
                continue;
            }
            debris = blade_debris;
        }

        if (object->drawn_this_frame == 0 || (ObjectField<u8>(object, 0xe23) & 8) == 0) {
            continue;
        }
        u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
        i32 first_joint = (i8)runtime[0x100 + blade * 2];
        i32 second_joint = (i8)runtime[0x101 + blade * 2];
        if (first_joint == -1 || second_joint == -1 ||
            *(void **)((u8 *)object->game_character_data + 0x14 + first_joint * 4) == NULL ||
            *(void **)((u8 *)object->game_character_data + 0x14 + second_joint * 4) == NULL) {
            continue;
        }

        NUVEC first = ObjectField<NUVEC>(object, 0x824 + first_joint * 0x40);
        NUVEC second = ObjectField<NUVEC>(object, 0x824 + second_joint * 0x40);
        NUVEC middle;
        NuVecAdd(&middle, &first, &second);
        NuVecScale(&middle, &middle, 0.5f);
        void *debris_system = *(void **)((u8 *)WORLD + 0x134);
        AddGameDebris(debris_system, debris, &first);
        AddGameDebris(debris_system, debris, &middle);
        AddGameDebris(debris_system, debris, &second);
    }
}

void StartLaunch(GameObject_s *object) {
    if (object->player_packet.character_state == 0x2c) {
        return;
    }
    Player_ClearContext(object, 1);
    Player_ResetContexts(&object->player_packet);
    object->player_packet.character_state = 0x2c;
    object->player_packet.balloon_type = 0;
    *(NUVEC *)((u8 *)&object->player_packet + 0x90) = object->position;
    object->previous_safe_position = object->position;
}

void StartFlatten(GameObject_s *attacker, GameObject_s *victim) {
    i8 state = victim->player_packet.character_state;
    u8 *runtime = *(u8 **)((u8 *)victim->character_data + 0x24);
    if (state == 0x33 || state == 0x3d || (runtime[0x90] & 0x40) != 0 ||
        (*(f32 *)(runtime + 0x28) != 0.0f && ObjectField<i8>(victim, 0xe31) == 1) || (attacker->state_flags & 4) != 0 ||
        victim->ground_contact_flags == 0) {
        return;
    }

    Player_ClearContext(victim, 0);
    Player_ResetContexts(&victim->player_packet);
    victim->player_packet.character_state = 0x3d;
    ObjectField<f32>(victim, 0x76c) = 0.0f;
    victim->velocity.x *= 0.5f;
    victim->velocity.z *= 0.5f;

    void **animations = *(void ***)((u8 *)victim->game_character_data + 0xc);
    victim->player_packet.action = animations[0x85] != NULL ? 0x85 : 5;
    ObjectField<f32>(victim, 0x774) = (f32)qrand() * (1.0f / 65535.0f) + 1.0f;

    NUVEC direction;
    NuVecSub(&direction, &victim->origin, &attacker->origin);
    i16 heading = NuAtan2D(direction.x, direction.z);
    bool backwards =
        direction.x * ObjectField<f32>(victim, 0xf3c) + direction.z * ObjectField<f32>(victim, 0xf44) < 0.0f;
    if (backwards) {
        heading += 0x8000;
    }
    victim->player_packet.balloon_type = backwards;
    if (victim->player_packet.action != 0x85) {
        victim->yaw = heading;
        victim->heading = heading;
        victim->desired_heading = heading;
    }

    ObjectField<u32>(victim, 0xd40) = 0;
    NewBuzzFrames(ObjectField<GAMEPAD_s *>(attacker, 0xc94)->pad, 1, 0);
    GameAudio_PlaySfxById(*(i16 *)(runtime + 0xde), &victim->origin, 0, 0);
    NewBuzz(ObjectField<GAMEPAD_s *>(victim, 0xc94)->pad, 0.1f, 0);
}

void FlattenCode(GameObject_s *object) {
    if (object->player_packet.character_state != 0x3d) {
        return;
    }

    GameObject_s *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    for (i32 index = 0; index < HIGHGAMEOBJECT; index++) {
        GameObject_s *candidate = &Obj[index];
        u8 *runtime = *(u8 **)((u8 *)candidate->character_data + 0x24);
        if ((candidate->state_flags & 0x1001) != 0x1001 || candidate->death_state != 0 || (runtime[0x91] & 0x10) == 0 ||
            (CInfo[candidate->player_packet.character_state * 0x10 + 9] & 0x80) != 0) {
            continue;
        }

        f32 range = object->bounds_radius + candidate->bounds_radius + 0.1f;
        f32 distance = NuVecDistSqr(&object->origin, &candidate->origin, NULL);
        if (distance < range * range && (i8)object->state_flags >= 0) {
            ObjectField<f32>(object, 0x76c) = 0.0f;
            return;
        }
        if (distance < nearest_distance && object != candidate) {
            nearest_distance = distance;
            nearest = candidate;
        }
    }

    f32 elapsed = ObjectField<f32>(object, 0x76c) + FRAMETIME;
    ObjectField<f32>(object, 0x76c) = elapsed;
    if (elapsed < ObjectField<f32>(object, 0x774)) {
        NewRumble(ObjectField<GAMEPAD_s *>(object, 0xc94)->pad, (f32)qrand() * (1.0f / 65535.0f) * 0.4f, 0);
        return;
    }

    object->player_packet.character_state = -1;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animations[6] == NULL) {
        return;
    }
    StartJump(object, 0);
    if (nearest != NULL && (i8)object->state_flags < 0) {
        NUVEC direction;
        NuVecSub(&direction, &object->position, &nearest->position);
        direction.y = 0.0f;
        NuVecNorm(&direction, &direction);
        NuVecScale(&direction, &direction, 12.0f);
        NuVecAdd(&object->velocity, &object->velocity, &direction);
    }
}

void ReleaseEat(GameObject_s *object) {
    if (object->player_packet.opponent != NULL && (object->player_packet.runtime_flags_4 & 1) != 0) {
        KillGameObject(object->player_packet.opponent, 2, 0);
        object->player_packet.opponent = NULL;
    }
    if ((u8)(object->player_packet.character_state - 0x38) < 2) {
        object->player_packet.opponent = NULL;
        object->player_packet.character_state = -1;
    }
    object->player_packet.runtime_flags_4 &= ~1;
}

void GrabVictim(GameObject_s *object, GameObject_s *victim) {
    if (object->player_packet.character_state != -1) {
        return;
    }

    object->player_packet.character_state = 0x38;
    i16 action = 100;
    if (victim->character_id == id_GAMORREANGUARD && victim->player_index == -1) {
        action = 0x1f;
    }
    object->player_packet.action = action;

    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animations[action] == NULL) {
        ObjectField<f32>(object, 0x774) = 2.0f;
    } else {
        ObjectField<f32>(object, 0x774) = AnimDuration(object->character_id, action, 0.0f, 0.0f, 1);
    }
    object->player_packet.buildit = NULL;
    ObjectField<f32>(object, 0x76c) = 0.0f;
    object->player_packet.opponent = victim;
    object->player_packet.opponent_target = NULL;
    object->player_packet.runtime_flags_4 &= ~1;
    ObjectField<u8>(object, 0x7ac) &= ~0x40;
}

void EatVictim(GameObject_s *object) {
    object->player_packet.character_state = -1;
    if (object->player_packet.opponent == NULL || (object->player_packet.runtime_flags_4 & 1) == 0) {
        return;
    }
    object->player_packet.action = 0x4e;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animations[0x4e] != NULL) {
        object->player_packet.character_state = 0x3f;
        *(f32 *)((u8 *)&object->player_packet + 0xb8) = AnimDuration(object->character_id, 0x4e, 0.0f, 0.0f, 1);
        ((u8 *)&object->player_packet)[0xf8] &= 0xbf;
    }
}

void StartTurn(GameObject_s *object) {
    u8 *packet = (u8 *)&object->player_packet;
    *(f32 *)(packet + 0xb8) = TURNTIME;
    *(f32 *)(packet + 0xbc) = TURNTIME;
    *(f32 *)(packet + 0xc0) = TURNTIME;
    object->player_packet.character_state = 0x2a;
    object->player_packet.action = 1;
    PlaySfx("XWing_LoopDeLoop", &object->origin);
}

void ApplyExtraRotation(GameObject_s *object, NUMTX *matrix) {
    if (object->player_packet.character_state == 0x2a) {
        TurnCodeCamSafe(object, matrix);
    } else if (object->player_packet.character_state == 0x3a) {
        f32 progress = 1.0f - ObjectField<f32>(object, 0x76c) / ObjectField<f32>(object, 0x774);
        f32 wave = (NuTrigTable[(i32)(progress * 32768.0f + 16384.0f) >> 1 & 0x7fff] + 1.0f) * 0.5f;
        i32 rotation = (i32)((1.0f - wave) * 65536.0f);
        if (object->player_packet.balloon_type == 0) {
            NuMtxPreRotateZ(matrix, rotation);
        } else {
            NuMtxPreRotateZ(matrix, -rotation);
        }
    }
    if (object->character_id == id_MINISLAVE1) {
        NuMtxPreRotateX(matrix, -0x4000);
        NuMtxPreRotateY(matrix, 0x8000);
    }
}

void MoveInactiveVehicle(GameObject_s *object, i32, GameObject_s **target_player) {
    NUVEC *movement = (NUVEC *)((u8 *)object + 0xf24);
    *movement = v000;
    GameObject_s *target = NULL;
    if (object == Player[0] || object == Player[1]) {
        target = object == Player[0] ? Player[1] : Player[0];
        NUVEC target_position = target->position;
        SeekVec(&object->position, &object->position, &target_position, 10.0f);
        object->velocity = *movement;
        object->yaw = target->yaw;
        object->desired_heading = target->yaw;
        object->heading = target->yaw;
    } else {
        NUVEC target_position = v000;
        f32 count = 0.0f;
        if (Player[0] != NULL) {
            target_position.x += Player[0]->position.x;
            target_position.y += Player[0]->position.y;
            target_position.z += Player[0]->position.z;
            count = 1.0f;
        }
        if (Player[1] != NULL) {
            target_position.x += Player[1]->position.x;
            target_position.y += Player[1]->position.y;
            target_position.z += Player[1]->position.z;
            count += 1.0f;
        }
        if (count > 0.0f) {
            target_position.x /= count;
            target_position.y /= count;
            target_position.z /= count;
        }
        SeekVec(&object->position, &object->position, &target_position, 10.0f);
        object->velocity = *movement;
    }
    if (target_player != NULL) {
        *target_player = target;
    }
    *(u32 *)((u8 *)object + 0xd40) = 0;
    *(u32 *)((u8 *)object + 0xddc) = 0;
}

bool NeedsPretendAnim(GameObject_s *object) {
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animations[object->animation_state] != NULL && object->character_id != id_JEDISTARFIGHTERREDEP3 &&
        object->character_id != id_JEDISTARFIGHTERYELLOWEP3) {
        return object->character_id == id_TIEINTERCEPTOR;
    }
    return true;
}

void SetBallooningHeight(GameObject_s *object, f32 height) {
    if (object->player_packet.character_state == 0x5d) {
        object->player_packet.balloon_height = height;
    }
}

void StartBallooning(GameObject_s *object, i32 type) {
    object->player_packet.balloon_type = type;
    object->player_packet.character_state = 0x5d;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    object->player_packet.action = animations[0xb1] != NULL ? 0xb1 : 1;
    object->player_packet.balloon_height = 1000000000.0f;
}

void LetGoOfBalloon(GameObject_s *object) {
    if (object->player_packet.character_state != 0x5d) {
        return;
    }
    object->player_packet.character_state = -1;

    u8 *world_data = *(u8 **)((u8 *)WORLD + 0x2ac0);
    if (world_data[0xf7e] == 0) {
        return;
    }
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    i32 joint = (i8)runtime[0x108];
    if (joint == -1 || *(void **)((u8 *)object->game_character_data + 0x14 + joint * 4) == NULL) {
        return;
    }

    NUMTX matrix;
    NuMtxSetTranslation(&matrix, &ObjectField<NUVEC>(object, 0x824 + joint * 0x40));
    NUVEC momentum = {0.0f, 0.0f, 0.2f};
    NuVecMtxRotate(&momentum, &momentum, &matrix);

    u8 configuration[200];
    memcpy(configuration, Default_ADDPART, sizeof(configuration));
    *(NUMTX **)(configuration + 0x00) = &matrix;
    *(NUVEC **)(configuration + 0x08) = &momentum;
    *(GameObject_s **)(configuration + 0x0c) = object;

    nuhspecial_s *special = (nuhspecial_s *)(world_data + 0xf70);
    NUVEC center;
    f32 radius;
    NuSpecialGetRadius(special, &center, &radius);
    *(f32 *)(configuration + 0x14) = radius;
    *(f32 *)(configuration + 0x18) = radius;
    *(f32 *)(configuration + 0x1c) = 0.5f;
    *(nuhspecial_s **)(configuration + 0x24) = special;
    *(i32 *)(configuration + 0x28) = 0xf7;
    *(u32 *)(configuration + 0x2c) = 0x8000200;
    *(void (**)(PART_s *))(configuration + 0x40) = PartCollide_3D;
    *(f32 *)(configuration + 0x8c) = FRAMETIME;
    *(u32 *)(configuration + 0x90) = object->mask_bit_index;

    PART_s *part = (PART_s *)AddPart(configuration);
    if (part != NULL) {
        *(u32 *)((u8 *)part + 0x218) = ObjHitObj_Flags(object) & 0xffff;
    }
}

void PopBalloon(GameObject_s *object) {
    LetGoOfBalloon(object);
}

i32 Glide_Start(GameObject_s *object) {
    if (LEGOCONTEXT_GLIDE == -1) {
        return 0;
    }
    object->player_packet.character_state = 0x4f;
    object->player_packet.action = 0x93;
    *(f32 *)((u8 *)&object->player_packet + 0xb8) = 0.0f;
    *(f32 *)((u8 *)&object->player_packet + 0xc0) = 0.4f;
    object->player_packet.balloon_type = 0;
    object->player_packet.buildit = NULL;
    return 1;
}

void Glide_MoveCode(GameObject_s *object) {
    i8 state = object->player_packet.character_state;
    if (state != 0x4f) {
        u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
        bool can_glide_without_equipment = (runtime[0x94] & 0x40) != 0;
        if ((*(u8 *)((u8 *)object->character_data + 4) & 0x40) != 0) {
            return;
        }
        if (!can_glide_without_equipment) {
            if (object->player_packet.equipment == NULL || (*((u8 *)object->player_packet.equipment + 0xc) & 2) == 0) {
                return;
            }
        }

        u8 *game_pad = (u8 *)object->player_packet.game_pad;
        if ((*(u32 *)(game_pad + 8) & GAMEPAD_JUMP) == 0 || object->ground_contact_flags != 0) {
            return;
        }
        if (!can_glide_without_equipment && AnimPlaying((u8 *)object + 8, 5, 1, 0) == 0) {
            return;
        }

        state = object->player_packet.character_state;
        if (state != 0x11 && state != -1) {
            if (state != 0 || (!can_glide_without_equipment && ObjectField<i8>(object, 0x7ad) >= 0)) {
                return;
            }
        }
        Glide_Start(object);
        return;
    }

    if (object->ground_contact_flags == 0 && object->floor_height != 2000000.0f &&
        object->predicted_bottom_center.y - object->floor_height > object->bounds_half_height) {
        u8 *game_pad = (u8 *)object->player_packet.game_pad;
        void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
        if ((*(u32 *)(game_pad + 4) & GAMEPAD_ACTION) != 0 && animations[0x21] != NULL &&
            Slam_Start(object, SLAMJUMPSPEED) != 0) {
            ResetAnimPacket((u8 *)object + 8, -1);
            ResetMiniAnimPacket((u8 *)object + 0x7c0, -1);
            return;
        }
    }

    ObjectField<u8>(object, 0xe25) |= 0x10;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    i32 action = object->player_packet.action;
    if (animations[action] != NULL && AnimPlaying((u8 *)object + 8, action, 1, 0) == 0) {
        return;
    }

    ObjectField<f32>(object, 0x76c) += FRAMETIME;
    if (object->ground_contact_flags != 0) {
        object->player_packet.character_state = -1;
        return;
    }

    u8 *game_pad = (u8 *)object->player_packet.game_pad;
    if ((*(u32 *)(game_pad + 4) & GAMEPAD_JUMP) != 0) {
        if (ObjectField<f32>(object, 0x774) < 0.1f) {
            ObjectField<f32>(object, 0x774) = 0.1f;
        }
    } else {
        ObjectField<f32>(object, 0x774) -= FRAMETIME;
        if (ObjectField<f32>(object, 0x774) <= 0.0f) {
            StartEndOfJump(object);
        }
    }
}

i32 Glide_SetTargetMom(GameObject_s *object) {
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    i32 action = object->player_packet.action;
    f32 speed;
    if (animations[action] == NULL) {
        speed = *(f32 *)(*(u8 **)((u8 *)object->character_data + 0x24) + 0x1c);
    } else {
        speed = AnimSpeed(object->game_character_data, action);
    }

    NUVEC &movement = ObjectField<NUVEC>(object, 0xf24);
    if (object->player_packet.buildit == NULL) {
        movement.x = NuTrigTable[object->desired_heading >> 1] * speed;
        movement.z = speed * NuTrigTable[(object->desired_heading + 0x4000) >> 1 & 0x7fff];
        movement.y = (i8)object->state_flags < 0 ? -0.5f : -0.375f;
        f32 elapsed = ObjectField<f32>(object, 0x76c);
        if (elapsed < 0.5f) {
            movement.y *= elapsed + elapsed;
        }
    } else {
        movement.x = NuTrigTable[object->desired_heading >> 1] * speed * 0.25f;
        movement.z = speed * 0.25f * NuTrigTable[(object->desired_heading + 0x4000) >> 1 & 0x7fff];
    }
    return 1;
}

i32 ForcePushed_YRotation(GameObject_s *object) {
    GameObject_s *source = object->player_packet.opponent;
    if (source != NULL && source->player_packet.character_state == 0x1b &&
        (source->player_packet.runtime_flags_4 & 0x380) == 0 &&
        (object->velocity.x != 0.0f || object->velocity.z != 0.0f)) {
        object->desired_heading = NuAtan2D(object->velocity.x, object->velocity.z) - 0x8000;
        if (ObjectField<i8>(object, 0x7a8) == 4 && Cheat_IsOn(0x13) != 0) {
            object->desired_heading -= 0x8000;
        }
    } else {
        FaceOpponent(object, NULL);
    }
    return 0;
}

i32 ForcePushed_SetTargetMom(GameObject_s *object, f32 *speed) {
    GameObject_s *source = object->player_packet.opponent;
    NUVEC &movement = ObjectField<NUVEC>(object, 0xf24);
    if (source == NULL) {
        movement.x = 0.0f;
        movement.z = 0.0f;
        return 0;
    }

    if ((source->player_packet.runtime_flags_4 & 0x180) != 0) {
        movement.x = 0.0f;
        movement.z = 0.0f;
        if (source->player_packet.character_state == 0x1b && source->player_packet.balloon_type == 1) {
            NUVEC target = ObjectField<NUVEC>(source, 0x738);
            target.y -= object->model_bottom * object->scale;
            NUVEC direction;
            f32 distance = NuVecDistSqr(&target, &object->position, &direction);
            if (distance > 0.01f) {
                NuVecNorm(&direction, &direction);
                NuVecScale(&movement, &direction, 1.0f);
            }
            *speed = 20.0f;
            return 1;
        }
        return 0;
    }

    if (ObjectField<i8>(object, 0x7a8) == 4 && Cheat_IsOn(0x13) != 0) {
        u16 angle = NuAtan2D(object->origin.x - source->origin.x, object->origin.z - source->origin.z);
        movement.x = -NuTrigTable[angle >> 1] * 2.5f;
        movement.z = -NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] * 2.5f;
        return 0;
    }

    f32 direction_x;
    f32 direction_z;
    if (ForcePush_SuperPush != 0 && (i8)object->state_flags >= 0 &&
        (source->player_packet.runtime_flags_4 & 0x380) == 0) {
        GameObject_s *nearest = NULL;
        f32 nearest_distance = 2.25f;
        f32 push_x = object->position.x - source->position.x;
        f32 push_z = object->position.z - source->position.z;
        for (i32 index = 0; index < HIGHGAMEOBJECT; index++) {
            GameObject_s *candidate = &Obj[index];
            if ((candidate->state_flags & 0x1001) != 0x1001 || candidate->death_state != 0 || candidate == source ||
                candidate == object || candidate->player_index != -1 || (ObjectField<u8>(candidate, 0xefb) & 8) != 0 ||
                CannotKill(candidate) != 0) {
                continue;
            }
            u8 *character = (u8 *)candidate->character_data;
            if ((*(u32 *)(character + 4) & 0x4002010) != 0x10) {
                continue;
            }
            u8 *runtime = *(u8 **)(character + 0x24);
            if ((runtime[0x90] & 0x40) != 0 || (runtime[0x95] & 2) != 0 || candidate->character_id == id_GONKDROID ||
                candidate->bounds_min.y > object->bounds_max.y || object->bounds_min.y > candidate->bounds_max.y) {
                continue;
            }
            f32 candidate_x = candidate->position.x - object->position.x;
            f32 candidate_z = candidate->position.z - object->position.z;
            f32 candidate_distance = candidate_x * candidate_x + candidate_z * candidate_z;
            if (push_x * candidate_x + push_z * candidate_z >= 0.0f && candidate_distance < nearest_distance) {
                nearest_distance = candidate_distance;
                nearest = candidate;
            }
        }
        if (nearest != NULL) {
            direction_x = nearest->origin.x - object->origin.x;
            direction_z = nearest->origin.z - object->origin.z;
        } else {
            direction_x = object->origin.x - source->origin.x;
            direction_z = object->origin.z - source->origin.z;
        }
    } else {
        direction_x = object->origin.x - source->origin.x;
        direction_z = object->origin.z - source->origin.z;
    }

    u16 angle = NuAtan2D(direction_x, direction_z);
    movement.x = NuTrigTable[angle >> 1] * 2.5f;
    movement.z = NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] * 2.5f;
    return 0;
}

i32 ForcePushed_SuperPush_Occurring(GameObject_s *first, GameObject_s *second) {
    GameObject_s *pushed;
    GameObject_s *candidate;
    if (first->player_packet.character_state == 0x1c) {
        pushed = first;
        candidate = second;
    } else {
        if (second->player_packet.character_state != 0x1c) {
            return 0;
        }
        pushed = second;
        candidate = first;
    }

    GameObject_s *source = pushed->player_packet.opponent;
    if (ObjectField<i8>(pushed, 0x7a8) != 0 || source == NULL || source->player_packet.character_state != 0x1b ||
        (source->player_packet.runtime_flags_4 & 0x380) != 0 || candidate->character_id == id_GONKDROID ||
        (candidate->state_flags & 0x1001) != 0x1001 || candidate->death_state != 0 || candidate == source ||
        candidate == pushed || candidate->player_index != -1 || (ObjectField<u8>(candidate, 0xefb) & 8) != 0 ||
        CannotKill(candidate) != 0) {
        return 0;
    }

    u8 *character = (u8 *)candidate->character_data;
    if ((*(u32 *)(character + 4) & 0x4002010) != 0x10) {
        return 0;
    }
    u8 *runtime = *(u8 **)(character + 0x24);
    return (runtime[0x90] & 0x40) == 0 && (runtime[0x95] & 2) == 0;
}

void ForcedBackCode(GameObject_s *object) {
    f32 distance = 1000000.0f;
    if (ForceBackPos != NULL && (ObjectField<u8>(object, 0xeff) & 0x10) == 0) {
        distance = NuVecDistSqr(&object->origin, ForceBackPos, NULL);
    }

    if (object->player_packet.character_state != 0x22) {
        bool matching_type = ForceBackType != 2 || (*((u8 *)object->character_data + 4) & 0x10) != 0;
        bool matching_source =
            ForceBackType != 3 || ForceBackObj == NULL || ForceBackObj->player_packet.opponent == object;
        f32 inner_radius = ForceBackRadius - 0.2f;
        if (ForceBackPos != NULL && object != ForceBackObj && matching_type && !MosEisleyC_PastBarrier(object) &&
            matching_source && distance < inner_radius * inner_radius) {
            Player_ClearContext(object, 1);
            Player_ResetContexts(&object->player_packet);
            ObjectField<f32>(object, 0x76c) = 0.5f;
            ObjectField<f32>(object, 0x768) = 0.0f;
            object->player_packet.character_state = 0x22;
            object->player_packet.action = 5;
            PlaySfx("JForcePush", &object->origin);
        }
        return;
    }

    if (MosEisleyC_PastBarrier(object)) {
        object->position.z = 5.85f;
    }
    ObjectField<f32>(object, 0x768) += FRAMETIME;
    f32 remaining = ObjectField<f32>(object, 0x76c);
    if (remaining > 0.0f) {
        remaining -= FRAMETIME;
        if (remaining < 0.0f) {
            remaining = 0.0f;
        }
        ObjectField<f32>(object, 0x76c) = remaining;
    }

    f32 outer_radius = ForceBackRadius + 0.2f;
    if (ForceBackPos == NULL || outer_radius * outer_radius < distance) {
        if (remaining <= 0.0f) {
            object->player_packet.character_state = -1;
            ObjectField<u8>(object, 0xeff) &= ~2;
            ObjectField<u32>(object, 0xd18) = 0;
            ObjectField<u8>(object, 0xe25) |= 4;
        }
    } else if (ForceBackType == 4 && remaining < 0.2f) {
        ObjectField<f32>(object, 0x76c) = 0.2f;
    }

    if (ForceBackType == 2) {
        u8 *scene = *(u8 **)((u8 *)WORLD + 0x134);
        void *debris = *(void **)(*(u8 **)(scene + 8) + 0x8ac);
        AddVariableShotDebrisEffectTimed1(debris, &object->origin, 0x32, FRAMETIME, 0, 0, NULL);
    }
    NewRumble(ObjectField<GAMEPAD_s *>(object, 0xc94)->pad, (f32)qrand() * (1.0f / 65535.0f) * 0.6f, 0);
}

static bool HasJoint(GameObject_s *object, i32 joint) {
    return joint != -1 && *(void **)((u8 *)object->game_character_data + 0x14 + joint * sizeof(void *)) != NULL;
}

static NUVEC JointPosition(GameObject_s *object, i32 joint) {
    NUMTX *matrix = &object->player_packet.joint_matrices[joint];
    NUVEC position = {matrix->m30, matrix->m31, matrix->m32};
    return position;
}

void ForceLightning_Origin(GameObject_s *object, NUVEC *origin, NUVEC *target) {
    *origin = object->origin;
    if (target != NULL) {
        target->y = 1000000000.0f;
    }
    if (object->drawn_this_frame == 0) {
        return;
    }
    u8 *runtime = *(u8 **)((u8 *)object->character_data + 0x24);
    i32 origin_joint = (i8)runtime[0x108];
    if (!HasJoint(object, origin_joint)) {
        return;
    }
    *origin = JointPosition(object, origin_joint);
    if (target != NULL &&
        (*(f32 *)((u8 *)&object->player_packet + 0x67c) == 0.0f || ((u8 *)&object->player_packet)[0x77e] == 2)) {
        i32 target_joint = (i8)runtime[0x109];
        if (HasJoint(object, target_joint)) {
            *target = JointPosition(object, target_joint);
        }
    }
}

i32 LightSabre_ColourFromObj(i32 colour, i32 *object_id) {
    i32 result;
    i32 mapped_object;
    switch (colour) {
        case 0x65:
            result = 0;
            mapped_object = 0x66;
            break;
        case 0x67:
            result = 1;
            mapped_object = 0x68;
            break;
        case 0x69:
            result = 2;
            mapped_object = 0x6a;
            break;
        case 0x6b:
            result = 3;
            mapped_object = 0x6c;
            break;
        default:
            result = -1;
            mapped_object = -1;
            break;
    }
    if (object_id != NULL) {
        *object_id = mapped_object;
    }
    return result;
}

void EndForce(GameObject_s *object, i32) {
    if (object != NULL && (object->object_flags & 0x40000) == 0) {
        object->player_packet.force_glow_target = NULL;
        object->player_packet.force_glow_candidate = NULL;
        object->player_packet.runtime_flags_0 &= ~0x20000;
        object->player_packet.character_state = -1;
    }
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
}

void ReleaseForce(GameObject_s *object, i32 immediate) {
    i8 state = object->player_packet.character_state;
    if (state == 0x1b) {
        GameObject_s *target = object->player_packet.opponent;
        if (target != NULL) {
            if (target->player_packet.character_state == 0x1c) {
                target->player_packet.character_state = -1;
                target->player_packet.opponent = NULL;
            }
            object->player_packet.opponent = NULL;
        }
        EndForce(object, immediate);
        return;
    }
    if (state == 0x1d) {
        GameObject_s *target = ObjectField<GameObject_s *>(object, 0xca8);
        if (target != NULL) {
            ObjectField<u8>(target, 0x206) &= ~(1 << object->player_index);
            target = ObjectField<GameObject_s *>(object, 0xca8);
            if (ObjectField<i8>(target, 0x206) == 0) {
                ObjectField<u8>(target, 0x206) = 0xff;
                target = ObjectField<GameObject_s *>(object, 0xca8);
            }
            ObjectField<f32>(target, 0xe8) = ForceThrowGravity;
            ObjectField<GameObject_s *>(object, 0xca8) = NULL;
        }
        object->player_packet.character_state = -1;
        EndForce(object, immediate);
        return;
    }
    if (state == 8) {
        if ((i8)object->state_flags < 0) {
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        }
        EndForce(object, immediate);
    }
}

void ResetForceBack() {
    ForceBackObj = NULL;
    ForceBackPos = NULL;
}

void SetForceBack(GameObject_s *object, NUVEC *position, f32 radius, i32 type) {
    ForceBackRadius = radius;
    ForceBackObj = object;
    ForceBackPos = object != NULL ? &object->origin : position;
    ForceBackRadius2 = radius * radius;
    ForceBackType = type;
}

void DrawForceBackEffect(nuhspecial_s *special) {
    if (special == NULL || NuSpecialExistsFn(special) == 0) {
        return;
    }
    if (ForceBackObj == NULL || ForceBackType == 3) {
        NuSpecialSetVisibility(special, 0);
        return;
    }

    NuSpecialSetVisibility(special, 1);
    NUMTX matrix = *NuSpecialGetDrawMtx(special);
    NUVEC position = {ObjectField<f32>(ForceBackObj, 0x19c), ForceBackObj->floor_height + 0.005f,
                      ObjectField<f32>(ForceBackObj, 0x1a4)};
    NUVEC scale = {ForceBackRadius, ForceBackRadius, ForceBackRadius};
    NuMtxSetTranslation(&matrix, &position);
    NuMtxPreScale(&matrix, &scale);
    NuSpecialSetDrawMtx(special, &matrix);
    NuSpecialUpdate(special);
}

void ResetForceGlow(PLAYERPACKET *player_packet) {
    player_packet->force_glow_current = 0.0f;
    player_packet->force_glow_target_value = 0.0f;
    player_packet->force_glow_hold_time = 0.0f;
    player_packet->force_glow_target = NULL;
    player_packet->force_glow_candidate = NULL;
    player_packet->force_glow_step = 0.2f;
}

static NUVEC *TargetPosition(GameObject_s *object) {
    return (NUVEC *)((u8 *)&object->player_packet + 0x5bc);
}

static NUVEC *TargetVelocity(GameObject_s *object) {
    return (NUVEC *)((u8 *)&object->player_packet + 0x5c8);
}

static void FinishTarget(GameObject_s *object, void *target) {
    object->player_packet.runtime_flags_0 |= 0x800;
    object->player_packet.attack_target = VehicleArea != 0 ? NULL : target;
}

i32 SetObjTarget(GameObject_s *object, GameObject_s *target) {
    *TargetPosition(object) = target->origin;
    if ((((u8 *)object->character_data)[5] & 0x20) == 0 && (object->object_flags & 1) != 0) {
        TargetPosition(object)->y += Bolt_ObjTargetPosYAdjust(target);
    }
    *TargetVelocity(object) = v000;
    FinishTarget(object, target);
    return 1;
}

i32 SetGizmoBlowUpTarget(GameObject_s *object, GIZMOBLOWUP_s *target) {
    *TargetPosition(object) = *(NUVEC *)((u8 *)target + 0x50);
    *TargetVelocity(object) = v000;
    FinishTarget(object, target);
    return 1;
}

i32 SetPartTarget(GameObject_s *object, PART_s *target) {
    *TargetPosition(object) = *(NUVEC *)((u8 *)target + 0x30);
    *TargetVelocity(object) = *(NUVEC *)((u8 *)target + 0x80);
    FinishTarget(object, target);
    return 1;
}
