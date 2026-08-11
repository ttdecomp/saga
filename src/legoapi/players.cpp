#include "legoapi/players.h"

#include <string.h>

#include "legoapi/cheat.h"
#include "legoapi/gamepads.h"
#include "legoapi/gizmos/gizbuildits.h"
#include "legoapi/qrand.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

struct PlayerStartPosition {
    NUVEC *position;
    u8 unknown_04[0xc];
};

struct BOLT_s {
    u8 unknown_000[0xf0];
    u32 flags;
};

struct PlayerWorld {
    u8 unknown_0000[0x130];
    void *game_object_system;
    u8 unknown_0134[0x2980];
    void *lighting_system;
    u8 unknown_2ab8[0x30];
    AISYS *ai_system;
};

struct MISSIONSYS_s;
struct COINPACKET_s;
struct LEVEL_PROGRESS_s;
struct bgprocinfo_s;
struct WORLDINFO_s;

struct ApiCharacterSystem {
    i32 character_count;
    u8 unknown_04[0x14];
    u8 *character_info;
    i16 *character_indices;
    CHARACTERDATA *character_data;
};

struct PlayerGamePad {
    u8 unknown_00[4];
    u32 movement_state[8];
    u8 unknown_24[2];
    u16 direction;
    f32 magnitude;
    u8 unknown_2c[0x14];
    u32 camera_state[4];
    u8 unknown_50[4];
    void *input_device;
    u8 unknown_58[2];
    u8 flags;
};

struct PlayerCharacterRuntime {
    u8 unknown_000[0x116];
    u8 uses_weapon_action;
};

struct PlayerTerrainEntry {
    u8 unknown_00[4];
    u32 flags;
    u8 unknown_08[4];
};

struct PlayerProgressEntry {
    u32 progress;
    i16 previous_character_id;
    u8 active;
    u8 alternate_state;
    u8 custom_part;
    u8 custom_color;
    u8 custom_detail;
    u8 special_state;
    u32 runtime_value;
};

static_assert(sizeof(PlayerProgressEntry) == 0x10, "PlayerProgressEntry size");

void Whip_Release(GameObject_s *object);
void SuperCarry_Release(GameObject_s *object);
void SpecialMove_ReleaseVictim(GameObject_s *object);
i32 RotDiff(u16 first, u16 second);
extern u8 *GameCam;
void FreeTorpedoPacket(void **packet);
i32 KillGameObject(GameObject_s *object, i32 reason, i32 unused);
void ReleaseTakeOver(GameObject_s *object, i32 immediate);
i32 Mission_Active(MISSIONSYS_s *mission);
GameObject_s *GetNamedGameObject(AISYS *ai_system, char *name);
void InitCreature(GameObject_s *object, i32 character_id, i32 initialize_ai);
void ResetPlayerPacket(PLAYERPACKET *packet, CHARACTERDATA *character_data);
extern "C" void ResetAnimPacket(void *packet, i32 animation);
void SetAnimTimeRandom(GAMECHARACTERDATA *character_data, f32 *frame);
void ResetCoinPacket(COINPACKET_s *packet);
void DrawOffsetCode(GameObject_s *object, i32 reset);
void ResetAICreature(GameObject_s *object, AISYS *ai_system);
void InitSurfaceInfo(GameObject_s *object);
i32 SetObjOnSurface(GameObject_s *object, i32 force);
void LightGameObject(GameObject_s *object, void *lighting_system);
void Player_ResetContexts(PLAYERPACKET *packet);
void CharShadows_Reset(PLAYERPACKET *packet);
void ResetForceGlow(PLAYERPACKET *packet);
void Move_CHARACTER(GameObject_s *object);
void Animate_CHARACTER(GameObject_s *object);
void AutoWeaponOnOff(GameObject_s *object);
i32 Cheat_IsOn(i32 cheat);
void *WorldInfo_CurrentlyActive();
void ResetPlayer(GameObject_s *object, i32 reset_position, NUVEC *position, i32 set_surface);
void InstantKillParts(GameObject_s *object, i32 remove_parts, f32 delay);
i32 LoseCoins(GameObject_s *object, i32 all);
void AddPickups(i32 count, i32 pickup_type, i32 character_id, i32 flags, NUVEC *position, NUVEC *velocity, f32 scatter,
                i32 owner, f32 scale, f32 lifetime, GameObject_s *source, i32 collectable, i32 unused, bool silent);
void SetProtocolDroidDeactivatedAction(GameObject_s *object);
void Arcade_PlayerKilled(i32 player_index, i32 unused = 0);
void Shards_HandleLostObj(WORLDINFO_s *world, GameObject_s *object);
void LoseHelmet(GameObject_s *object, i32 debris = 1, i32 sound = 1);
void DestroySnakeBody(GameObject_s *object);
void *Suit_GetDefault(i32 character_id);
void GizForce_ResetLOS(GameObject_s *object);
i32 StartSlide(GameObject_s *object, i32 force);
void APILoadCharacterModels(i16 *character_list, i32 unused, void **buffer, void *buffer_end, i32 load_icons);
void IconScenes_Load(i16 *character_list, i32 unused, void **buffer, void **buffer_end);
void *IconScene_FindById(i32 character_id);
extern "C" i32 NuSpecialFind(void *scene, void *special, char *name);
void *APICharacterLoaded(i32 character_id);
void Customiser_SaveModelTextureIDs(void *customiser, void *character);
typedef void (*CharacterCallback)(GameObject_s *object);
void SetMoveAndAnimateFunctions(u32 required_flags, u32 excluded_flags, u32 required_game_flags,
                                u32 excluded_game_flags, i32 character_type, CharacterCallback move,
                                CharacterCallback animate, CharacterCallback post_animate);
void Move_JEDI(GameObject_s *object);
void Animate_JEDI(GameObject_s *object);
void Move_DROIDGENERIC(GameObject_s *object);
void Animate_PROTOCOL(GameObject_s *object);
void Animate_ASTROMECH(GameObject_s *object);
void PostAnimate_ASTROMECH(GameObject_s *object);
void Move_CANNON(GameObject_s *object);
void Animate_CANNON(GameObject_s *object);
void Move_VEHICLE(GameObject_s *object);
void Animate_VEHICLE(GameObject_s *object);
void Move_BEAST(GameObject_s *object);
void Animate_BEAST(GameObject_s *object);
void Animate_BATTLEDROID(GameObject_s *object);
void Move_HOVERDROID(GameObject_s *object);
void Animate_HOVERDROID(GameObject_s *object);
void Move_WALKER(GameObject_s *object);
void Animate_WALKER(GameObject_s *object);
void Move_ATAT(GameObject_s *object);
void Animate_ATAT(GameObject_s *object);
void Move_CRITTER(GameObject_s *object);
void Animate_CRITTER(GameObject_s *object);
void Move_POD(GameObject_s *object);
void Animate_POD(GameObject_s *object);
void PostAnimate_FETT(GameObject_s *object);
void Move_WEIRDO(GameObject_s *object);
void Animate_WEIRDO(GameObject_s *object);
void Move_GEONOSIAN(GameObject_s *object);
void Animate_GEONOSIAN(GameObject_s *object);
void Move_DROIDEKA(GameObject_s *object);
void Animate_DROIDEKA(GameObject_s *object);
void Move_SUPERBATTLEDROID(GameObject_s *object);
void Animate_SUPERBATTLEDROID(GameObject_s *object);
void Move_BARMAN(GameObject_s *object);
void Animate_BARMAN(GameObject_s *object);
void Move_JAWA(GameObject_s *object);
void Move_DRAGBOMB(GameObject_s *object);
void Move_REPUBLICGUNSHIP(GameObject_s *object);
void Animate_REPUBLICGUNSHIP(GameObject_s *object);
void Move_SPEEDERBIKE(GameObject_s *object);
void Animate_SPEEDERBIKE(GameObject_s *object);
void Animate_DEFAULT(GameObject_s *object);
void StarWars_AutoSetAICapabilities(GameObject_s *object);
extern i32 FreePlay;
extern ApiCharacterSystem *apicharsys;
extern i32 NOAICREATURES;
extern i32 netclient;
extern f32 TOGGLEHOLDTIME;
extern NUVEC v000;
extern NUVEC v010;
extern NUVEC plr_lastpos;
extern i32 PLAYERCOUNT;
extern GAMECHARACTERDATA GCDATA_DEFAULT;
extern i16 tUNKNOWN;
extern PlayerTerrainEntry TerSurface[];
extern i32 BonusWinner;
extern i32 waiting_for_character;
extern i32 apiloadcharactermodels_append;
extern i32 apiloadcharactermodels_nopakfile;
extern i32 CHARPAK;
extern void *characterbuffer_ptr;
extern void *characterbuffer_end;
extern i32 LEVELOBJECTCOUNT;
extern u8 ObjTab[];
extern char *save_icon[];
extern void *CharacterCustomiser;
extern i32 hub_character_ready;
extern i32 g_loadingCharacterInHub;
extern i32 makeplayerlist_freeplay;
extern i16 FreePlayModelList[];
extern i16 PlayerList[];
extern PlayerProgressEntry PlayerProgress[];
extern void *PlayerSuit[];
extern u8 PlayerTorpedoCount[];
extern void *HUB_LDATA;
extern void *NewLData;
extern f32 DEFAULT_MOVE_RANGE;
extern u32 (*SpecialRouteCharacterTypeIDFn)(i32 character_type);
extern "C" i16 id_DARTHMAUL;
extern "C" i16 id_COUNTDOOKU;
extern "C" i16 id_DROIDEKA;
extern "C" i16 id_MINIDROIDEKA;
extern "C" i16 id_SUPERBATTLEDROID;
extern "C" i16 id_BARMAN;
extern "C" i16 id_JAWA;
extern "C" i16 id_UGNAUGHT;
extern "C" i16 id_DRAGBOMB;
extern "C" i16 id_REPUBLICGUNSHIP;
extern "C" i16 id_REPUBLICGUNSHIP_GREEN;
extern "C" i16 id_SPEEDERBIKE;
extern "C" i16 id_PROBEDROID;
extern "C" i16 id_BODYGUARD;
extern "C" i16 id_IMPERIALGUARD;
extern "C" i16 id_GONKDROID;
extern "C" i16 id_MOSEISLEYCITIZEN;
extern "C" i16 id_CANTINAALIEN;
extern "C" i16 id_CLOUDCITYCITIZEN;
extern "C" i16 id_BOB;
extern "C" i16 id_GEONOSIAN;
extern "C" PlayerWorld *WORLD;
extern "C" f32 AnimDuration(i32 character_id, i32 animation, f32 start, f32 end, i32 flags);
extern "C" AIGROUP *CreateAIGroup(AISYS *ai_system, u8 group_index, f32 x_spacing, f32 z_spacing, i32 character_group);
extern "C" void AddToAIGroup(AIGROUP *group, AIGROUP *leader_group, GameObject_s *object);
extern "C" void AISysGetCharacterPathPos(AISYS *ai_system, GameObject_s *object, AISCRIPTPROCESS *process, i32 mask,
                                         i32 use_closest);
extern "C" void NuVecRotateY(NUVEC *destination, NUVEC *source, i32 angle);
extern "C" void NuVecRotateYValZ(NUVEC *destination, f32 length, i32 angle);
extern "C" void UpdateAnimPacket(GAMECHARACTERDATA *character_data, void *animation_packet, f32 elapsed_frames,
                                 f32 speed, f32 elapsed, f32 blend);
extern "C" f32 NuAnimEndFrame(void *animation);
extern "C" void AIScriptProcess(AISYS *ai_system, GameObject_s *object, AISCRIPTPROCESS *packet,
                                AISCRIPTPROCESS *process, f32 elapsed);
extern "C" void AddGameDebris(void *debris_system, i32 type, NUVEC *position);
extern "C" void ResetMiniAnimPacket(void *packet, i32 animation);
extern f32 animduration_blendouttime;
extern f32 DEFENDTIME;
extern i32 Arcade;
extern i32 VehicleArea;
extern i32 GAMEDEMO;
extern i32 PlayerID[2];
extern u8 *Game_CharacterSave;
u8 Collection_Got(i32 character_id);
extern "C" void ComplexSockPosition(void *path_system, NUVEC *position, i32 previous_sock, u32 sock_mask,
                                    SOCKPOSITION_s *sock_position);

template <typename T> static T &ObjectField(GameObject_s *object, size_t offset) {
    return *(T *)((u8 *)object + offset);
}

template <typename T> static T &PacketField(PLAYERPACKET *packet, size_t offset) {
    return *(T *)((u8 *)packet + offset);
}

static AISCRIPTPROCESS *GetAIProcess(GameObject_s *object) {
    return &object->ai_script_process;
}

PlayerStartPosition PlayerStart[8];
GameObject_s *player2;
GameObject_s *player;
GameObject_s *Player[8];
void (*Player_ClearContextFn)(GameObject_s *object, i32 context);

static i32 TerrainKillPlayer(GameObject_s *object, i32 terrain_type, NUVEC *direction) {
    u32 terrain_flags = TerSurface[terrain_type].flags;
    PlayerCharacterRuntime *runtime = (PlayerCharacterRuntime *)object->character_data->field11_0x24;
    bool kills_player = (terrain_flags & 1) != 0;
    if ((terrain_flags & 0x4000) != 0 && (*(u32 *)((u8 *)object->character_data + 4) & 0x10) == 0 &&
        (*((u8 *)runtime + 0x91) & 0x80) == 0) {
        kills_player = true;
    }
    if ((terrain_flags & 0x8000) != 0 && (object->player_index == -1 || direction->x > 0.0f)) {
        kills_player = true;
    }
    if (!kills_player) {
        return 0;
    }

    InstantKillParts(object, 1, 0.0f);
    if ((i8)object->state_flags < 0 && BonusWinner == -1) {
        i32 coins = LoseCoins(object, 1);
        AddPickups(coins, 0, 0, 0, &object->origin, NULL, 2.0f, -1, 1.0f, 2000000.0f, object, 1, 0, false);
    }
    KillPlayer(object, 2, 1);
    return 1;
}

void DrawWeapon_SetSabreObjects(GameObject_s *object, i32 right_sabre, i32 left_sabre, i32 right_blaster,
                                i32 left_blaster, i32 *objects, i32 *weapon_objects) {
    if (right_sabre != 0 || left_sabre != 0 || right_blaster != 0 || left_blaster != 0) {
        if (object->character_id == id_DARTHMAUL) {
            *objects = 0x12;
            *weapon_objects = 0x12;
        } else if (object->character_id == id_COUNTDOOKU && *(i8 *)(*(u8 **)((u8 *)WORLD + 0x2ac0) + 0x13e) != 0) {
            *objects = 0x13;
            *weapon_objects = 0x13;
        } else {
            *objects = 0x11;
            *weapon_objects = 0x11;
        }

        if (right_sabre != 0) {
            if (object->character_id == id_DARTHMAUL) {
                if (object->death_state != 0) {
                    return;
                }
                objects[1] = 0x6d;
                objects[2] = 0x6e;
                if ((object->player_packet.runtime_flags_4 & 8) != 0) {
                    objects[3] = 0x6e;
                }
                return;
            }
            if (object->death_state != 0) {
                return;
            }
            objects[1] = 0x65;
            objects[2] = 0x66;
            if ((object->player_packet.runtime_flags_4 & 8) != 0) {
                objects[3] = 0x66;
            }
            return;
        }
        if (left_sabre != 0) {
            if (object->death_state != 0) {
                return;
            }
            objects[1] = 0x67;
            objects[2] = 0x68;
            if ((object->player_packet.runtime_flags_4 & 8) != 0) {
                objects[3] = 0x68;
            }
            return;
        }
    }

    if (right_blaster != 0) {
        if (object->death_state == 0) {
            objects[1] = 0x69;
            objects[2] = 0x6a;
            if ((object->player_packet.runtime_flags_4 & 8) != 0) {
                objects[3] = 0x6a;
            }
        }
    } else if (left_blaster != 0 && object->death_state == 0) {
        objects[1] = 0x6b;
        objects[2] = 0x6c;
        if ((object->player_packet.runtime_flags_4 & 8) != 0) {
            objects[3] = 0x6c;
        }
    }
}

void SetLayers_BOB(GameObject_s *object) {
    i32 layers[] = {0xd, 0xe, 0x13, 0x14, 0xb, 0xc, 0xf, 1, 2, 3, 0x15, 0x16, 0x17, 5, 6, 7, 8, 9, 10, 0x10, 0x11};

    object->unknown_efd = object->unknown_efd & ~2 | (qrand() < 0x8000) * 2;
    qrand();
    object->layer_mask = 1;
    object->layer_mask |= 1U << layers[qrand() / 0x2aab + 7];
    object->layer_mask |= 1U << layers[qrand() / 0x2000 + 0xd];
    object->layer_mask |= 1U << layers[qrand() / 0x5556 + 4];
    object->layer_mask |= 1U << layers[qrand() / 0x8000];
    object->layer_mask |= 1U << layers[qrand() / 0x8000 + 2];
}

void SetLayers_MOSEISLEYCITIZEN(GameObject_s *object) {
    static const u8 hat_layer[] = {0, 0, 0, 7, 14};
    static const u8 head_layer[] = {20, 20, 8, 8, 4};
    static const u8 body_layer[] = {3, 9, 19};
    static const u8 arms_layer[] = {1, 12, 15};
    static const u8 hands_layer[] = {2, 13, 16};
    static const u8 waist_layer[] = {6, 10, 17};
    static const u8 legs_layer[] = {5, 11, 18};

    object->layer_mask = 0;
    object->layer_mask |= 1U << hat_layer[qrand() / 0x3334];
    object->layer_mask |= 1U << head_layer[qrand() / 0x3334];
    object->layer_mask |= 1U << body_layer[qrand() / 0x5556];
    object->layer_mask |= 1U << arms_layer[qrand() / 0x5556];
    object->layer_mask |= 1U << hands_layer[qrand() / 0x5556];
    object->layer_mask |= 1U << waist_layer[qrand() / 0x5556];
    object->layer_mask |= 1U << legs_layer[qrand() / 0x5556];
}

void LoadSingleCharacter(bgprocinfo_s *) {
    i16 character_list[8];
    character_list[0] = waiting_for_character;
    character_list[1] = 1;
    character_list[2] = -1;
    character_list[3] = 0;
    apiloadcharactermodels_append = 1;
    apiloadcharactermodels_nopakfile = CHARPAK == 0;
    APILoadCharacterModels(character_list, 0, &characterbuffer_ptr, characterbuffer_end, 1);
    IconScenes_Load(character_list, 0, &characterbuffer_ptr, &characterbuffer_end);

    void *scene = IconScene_FindById(character_list[0]);
    if (scene != NULL) {
        for (i32 index = 0; index < LEVELOBJECTCOUNT; index++) {
            if (ObjTab[index * 8] != 3) {
                continue;
            }
            u8 *special = *(u8 **)((u8 *)WORLD + 0x2ac0) + index * 0x10;
            if (special[0xe] == 0 && NuSpecialFind(scene, special, save_icon[index * 2]) != 0) {
                special[0xe] = 1;
            }
        }
    }

    void *character = APICharacterLoaded(character_list[0]);
    Customiser_SaveModelTextureIDs(CharacterCustomiser, character);
    hub_character_ready = waiting_for_character;
    waiting_for_character = -1;
    g_loadingCharacterInHub = 0;
}

void TerrainPlayer(GameObject_s *object) {
    PlayerGamePad *game_pad = (PlayerGamePad *)object->player_packet.game_pad;

    if (object->death_state == 0 && object->ground_contact_flags != 0 && (u32)(i32)object->ground_surface_type < 0x20) {
        TerrainKillPlayer(object, object->ground_surface_type, &ObjectField<NUVEC>(object, 0xf34));
    } else if (object->death_state == 0 && object->terrain_contact_age != 0 &&
               (u32)(i32)object->edge_surface_type < 0x20) {
        TerrainKillPlayer(object, object->edge_surface_type, &ObjectField<NUVEC>(object, 0xf34));
    }

    game_pad->flags &= 0xe9;
    PacketField<u32>(&object->player_packet, 0x660) = 0;
    *(u32 *)((u8 *)game_pad + 0x54) = 0;
    NuVecRotateYValZ(&ObjectField<NUVEC>(object, 0xf3c), 1.0f, object->yaw);
}

void AnimatePlayer(GameObject_s *object) {
    object->animation_state = object->animation_index;
    PlayerGamePad *game_pad = (PlayerGamePad *)object->player_packet.game_pad;
    GAMECHARACTERDATA *game_character_data = object->game_character_data;
    ObjectField<i16>(object, 0x7e0) = ObjectField<i16>(object, 0x7de);
    (void)WorldInfo_CurrentlyActive();

    if ((ObjectField<u8>(object, 0x1f6) & 4) == 0) {
        ((void (*)(GameObject_s *))object->character_data->field9_0x1c)(object);
    }

    i16 requested_animation = ObjectField<i16>(object, 0x3e6);
    if (requested_animation != -1 && ((requested_animation == 0xe9 && object->player_packet.character_state != 0x1c) ||
                                      requested_animation == object->previous_animation_state)) {
        object->previous_animation_state = ObjectField<i16>(object, 0x3e8);
    }

    if (game_character_data == NULL ||
        (object->death_state != 0 && (object->reflection_plane_y == 0.0f || object->death_state == 1))) {
        return;
    }

    f32 direction = (object->unknown_efd & 0x40) == 0 ? 1.0f : -1.0f;
    void **animation_table = *(void ***)((u8 *)game_character_data + 0xc);
    f32 speed;
    u8 *runtime = (u8 *)object->character_data->field11_0x24;
    if (animation_table[1] == NULL) {
        if ((runtime[0x92] & 0x10) == 0) {
            speed = game_pad->magnitude;
        } else {
            f32 forward_speed = object->velocity.x * ObjectField<NUVEC>(object, 0xf3c).x +
                                object->velocity.z * ObjectField<NUVEC>(object, 0xf3c).z;
            speed = forward_speed < 0.0f ? 0.0f : forward_speed;
        }
    } else {
        speed = *(f32 *)((u8 *)game_pad + 0x34);
        if (speed > 0.0f && object->character_id == id_GONKDROID && Cheat_IsOn(8) == 0) {
            speed = *(f32 *)(runtime + 0x18);
        }
    }

    f32 time_scale = 1.0f;
    if (object->player_packet.character_state == 0x2d && object->player_packet.buildit != NULL) {
        time_scale = GizBuildItMul(object);
    }
    UpdateAnimPacket(game_character_data, (u8 *)object + 8, FRAMETIME * 30.0f * time_scale, speed * direction,
                     time_scale * FRAMETIME, *(f32 *)(runtime + 0x20));

    if ((object->animation_flags & 4) != 0) {
        i16 animation;
        f32 *frame;
        if (object->animation_suppressed == 0) {
            animation = object->animation_index;
            frame = &object->animation_frame;
        } else {
            animation = object->fallback_animation_index;
            frame = (f32 *)((u8 *)object + 0x1c);
        }

        if (animation == 0x5f) {
            f32 duration = AnimDuration(object->character_id, 0x5f, 0.0f, 0.0f, 0);
            i32 frame_count = (i32)(duration / 0.3f);
            if (NuFmod(duration, 3.0f) > 0.15f) {
                frame_count++;
            }
            u32 frame_index = qrand() / ((0xffff / frame_count) + 1);
            void **animation_info = *(void ***)((u8 *)game_character_data + 8);
            f32 selected_frame = frame_index * (*(f32 *)((u8 *)animation_info[0x5f] + 0x18) * 0.3f) + 1.0f;
            if (selected_frame < NuAnimEndFrame(animation_table[0x5f])) {
                *frame = selected_frame;
            }
            ObjectField<u8>(object, 0xe21) = ObjectField<u8>(object, 0xe21) & 0xbf | (frame_index & 1) << 6;
        }
    }
    AutoWeaponOnOff(object);
    AddFootSteps(object);
}

void FixUpCharacters(CHARFIXUP *fixups) {
    for (i32 index = 0; index < CHARCOUNT; index++) {
        GCDataList[index] = GCDATA_DEFAULT;
        CHARACTERDATA *character_data = &CDataList[index];
        if (character_data->field0_0x0 == -1) {
            character_data->field0_0x0 = tUNKNOWN;
        }
        if (character_data->field8_0x18 == 0) {
            character_data->field8_0x18 = (u32)Move_CHARACTER;
        }
        if (character_data->field9_0x1c == 0) {
            character_data->field9_0x1c = (u32)Animate_CHARACTER;
        }
    }

    if (fixups != NULL) {
        while (fixups->name != NULL) {
            if (fixups->id != NULL) {
                *fixups->id = CharIDFromName(fixups->name);
            }
            fixups++;
        }
    }
}

void ExtraCharacterFixUpAfterConfig() {
    SetMoveAndAnimateFunctions(8, 8, 0, 0, -1, Move_JEDI, Animate_JEDI, NULL);
    SetMoveAndAnimateFunctions(0x1000010, 0x10, 0, 0, -1, Move_DROIDGENERIC, NULL, NULL);
    SetMoveAndAnimateFunctions(0x30, 0x30, 0, 0, -1, Move_DROIDGENERIC, Animate_PROTOCOL, NULL);
    SetMoveAndAnimateFunctions(0x50, 0x50, 0, 0, -1, Move_DROIDGENERIC, Animate_ASTROMECH, PostAnimate_ASTROMECH);
    SetMoveAndAnimateFunctions(0, 0, 0x800, 0x800, -1, Move_CANNON, Animate_CANNON, NULL);
    SetMoveAndAnimateFunctions(0x2000, 0x2000, 0, 0, -1, Move_VEHICLE, Animate_VEHICLE, NULL);
    SetMoveAndAnimateFunctions(0x40000000, 0x40000000, 0, 0, -1, Move_BEAST, Animate_BEAST, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 4, Move_DROIDGENERIC, Animate_BATTLEDROID, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0x11, Move_HOVERDROID, Animate_HOVERDROID, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0xf, Move_WALKER, Animate_WALKER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0x10, Move_ATAT, Animate_ATAT, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0x12, Move_CRITTER, Animate_CRITTER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0x12, Move_CRITTER, Animate_CRITTER, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0x14, Move_POD, Animate_POD, NULL);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 2, NULL, NULL, PostAnimate_FETT);
    SetMoveAndAnimateFunctions(0, 0, 0, 0, 0, Move_WEIRDO, Animate_WEIRDO, NULL);

    for (i32 index = 0; index < CHARCOUNT; index++) {
        if ((GCDataList[index].field148_0x97 & 0x10) != 0) {
            CDataList[index].field8_0x18 = (u32)Move_GEONOSIAN;
            CDataList[index].field9_0x1c = (u32)Animate_GEONOSIAN;
        }
    }

    if (id_DROIDEKA != -1) {
        CDataList[id_DROIDEKA].field8_0x18 = (u32)Move_DROIDEKA;
        CDataList[id_DROIDEKA].field9_0x1c = (u32)Animate_DROIDEKA;
    }
    if (id_MINIDROIDEKA != -1) {
        CDataList[id_MINIDROIDEKA].field8_0x18 = (u32)Move_DROIDEKA;
        CDataList[id_MINIDROIDEKA].field9_0x1c = (u32)Animate_DROIDEKA;
    }
    if (id_SUPERBATTLEDROID != -1) {
        CDataList[id_SUPERBATTLEDROID].field8_0x18 = (u32)Move_SUPERBATTLEDROID;
        CDataList[id_SUPERBATTLEDROID].field9_0x1c = (u32)Animate_SUPERBATTLEDROID;
    }
    if (id_BARMAN != -1) {
        CDataList[id_BARMAN].field8_0x18 = (u32)Move_BARMAN;
        CDataList[id_BARMAN].field9_0x1c = (u32)Animate_BARMAN;
    }
    if (id_JAWA != -1) {
        CDataList[id_JAWA].field8_0x18 = (u32)Move_JAWA;
    }
    if (id_UGNAUGHT != -1) {
        CDataList[id_UGNAUGHT].field8_0x18 = (u32)Move_JAWA;
    }
    if (id_DRAGBOMB != -1) {
        CDataList[id_DRAGBOMB].field8_0x18 = (u32)Move_DRAGBOMB;
    }
    if (id_REPUBLICGUNSHIP != -1) {
        CDataList[id_REPUBLICGUNSHIP].field8_0x18 = (u32)Move_REPUBLICGUNSHIP;
        CDataList[id_REPUBLICGUNSHIP].field9_0x1c = (u32)Animate_REPUBLICGUNSHIP;
    }
    if (id_REPUBLICGUNSHIP_GREEN != -1) {
        CDataList[id_REPUBLICGUNSHIP_GREEN].field8_0x18 = (u32)Move_REPUBLICGUNSHIP;
        CDataList[id_REPUBLICGUNSHIP_GREEN].field9_0x1c = (u32)Animate_REPUBLICGUNSHIP;
    }
    if (id_SPEEDERBIKE != -1) {
        CDataList[id_SPEEDERBIKE].field8_0x18 = (u32)Move_SPEEDERBIKE;
        CDataList[id_SPEEDERBIKE].field9_0x1c = (u32)Animate_SPEEDERBIKE;
    }
    if (id_PROBEDROID != -1) {
        CDataList[id_PROBEDROID].field9_0x1c = (u32)Animate_DEFAULT;
    }
    if (id_BODYGUARD != -1) {
        CDataList[id_BODYGUARD].field8_0x18 = (u32)Move_JEDI;
        CDataList[id_BODYGUARD].field9_0x1c = (u32)Animate_JEDI;
    }
    if (id_IMPERIALGUARD != -1) {
        CDataList[id_IMPERIALGUARD].field8_0x18 = (u32)Move_JEDI;
        CDataList[id_IMPERIALGUARD].field9_0x1c = (u32)Animate_JEDI;
    }
}

void ResetPlayerPacket(PLAYERPACKET *packet, CHARACTERDATA *character_data) {
    PacketField<f32>(packet, 0x664) = 0.0f;
    PacketField<f32>(packet, 0x68c) = 0.0f;
    PacketField<u8>(packet, 0x101) &= ~1;
    PacketField<i16>(packet, 0x764) = 0;
    PacketField<i16>(packet, 0x766) = 0;
    PacketField<i16>(packet, 0x768) = 0;
    Player_ResetContexts(packet);

    PlayerGamePad *game_pad = (PlayerGamePad *)packet->game_pad;
    game_pad->camera_state[0] = 0;
    game_pad->flags &= ~2;
    for (i32 index = 0; index < 8; index++) {
        game_pad->movement_state[index] = 0;
    }
    game_pad->camera_state[1] = 0;
    game_pad->camera_state[2] = 0;
    game_pad->camera_state[3] = 0;
    game_pad->flags &= ~4;
    game_pad->flags &= ~0x10;
    PacketField<u8>(packet, 0x76d) &= ~4;
    PacketField<u32>(packet, 0x660) = 0;
    game_pad->input_device = NULL;
    CharShadows_Reset(packet);

    packet->origin_y_offset = 0.0f;
    PacketField<f32>(packet, 0x6e8) = 0.0f;
    for (i32 index = 0; index < 8; index++) {
        PacketField<u32>(packet, 0x614 + index * 4) = 0;
    }
    ResetForceGlow(packet);
    PacketField<f32>(packet, 0x6ec) = 2000000.0f;
    PacketField<f32>(packet, 0x73c) = 0.0f;
    PacketField<f32>(packet, 0x6fc) = 0.0f;
    PacketField<u8>(packet, 0x76c) &= ~1;
    PacketField<u16>(packet, 0x76a) = 0xffff;
    PacketField<i16>(packet, 0x75c) = -1;
    i32 random = qrand();
    PacketField<f32>(packet, 0x718) = 0.0f;
    PacketField<u16>(packet, 0x75e) = random > 0x7fff ? 0x4f : 0x26;
    PacketField<u8>(packet, 0x773) = qrand() >> 8;
    PacketField<u8>(packet, 0x774) = qrand() >> 8;
    PacketField<u8>(packet, 0x775) = qrand() >> 8;
    PacketField<u8>(packet, 0x776) = qrand() >> 8;
    PacketField<u8>(packet, 0x770) &= ~1;
    PacketField<f32>(packet, 0x71c) = 0.0f;
    PacketField<f32>(packet, 0x720) = 0.0f;
    PacketField<f32>(packet, 0x728) = 0.0f;
    PacketField<f32>(packet, 0x69c) = 0.0f;
    PacketField<f32>(packet, 0x690) = 0.0f;
    PacketField<u32>(packet, 0x694) = 0;
    PacketField<u32>(packet, 0x794) = 0;
    PacketField<f32>(packet, 0x688) = 0.0f;
    PacketField<u8>(packet, 0x771) &= 0xf3;
    PacketField<NUVEC>(packet, 0x5d4) = v010;
    PacketField<GameObject_s *>(packet, 0x604) = NULL;
    PacketField<i8>(packet, 0x78e) = -1;
}

void Player_ResetContexts(PLAYERPACKET *packet) {
    packet->tag_cooldown = 0.0f;
    PacketField<i8>(packet, 0x100) = -1;
    f32 sabre_active_scale = (PacketField<u8>(packet, 0x76e) & 1) != 0 ? 1.0f : 0.0f;
    PacketField<f32>(packet, 0x670) = 5.0f;
    packet->damage_scale = 0.0f;
    PacketField<f32>(packet, 0x678) = 0.0f;
    PacketField<f32>(packet, 0x668) = 0.0f;
    PacketField<u8>(packet, 0x77e) = 0;
    PacketField<f32>(packet, 0x67c) = sabre_active_scale;
    packet->character_state = -1;
    PacketField<u8>(packet, 0xf5) = 0;
    PacketField<u8>(packet, 0xf4) = 0;

    PlayerGamePad *game_pad = (PlayerGamePad *)packet->game_pad;
    game_pad->flags &= ~4;
    game_pad->flags &= ~0x10;
    PacketField<u32>(packet, 0x660) = 0;
    PacketField<u8>(packet, 0x76d) &= ~4;
    PacketField<u32>(packet, 0x648) = 0;
    i32 random = qrand();
    PacketField<u32>(packet, 0x6a0) = 0;
    PacketField<f32>(packet, 0x6a4) = 0.0f;
    PacketField<f32>(packet, 0x674) = 0.0f;
    PacketField<f32>(packet, 0x6e0) = 0.0f;
    PacketField<f32>(packet, 0x6f8) = 0.0f;
    PacketField<f32>(packet, 0x710) = 0.0f;
    PacketField<u32>(packet, 0x724) = 0;
    PacketField<u32>(packet, 0x730) = 0;
    PacketField<f32>(packet, 0x734) = 0.0f;
    PacketField<u16>(packet, 0xe8) = 0;
    PacketField<i16>(packet, 0x75c) = -1;
    if (random < 0) {
        random += 0x7fff;
    }
    PacketField<u8>(packet, 0xee) = 0;
    PacketField<i16>(packet, 0x760) = -1;
    PacketField<u8>(packet, 0x78c) = random >> 15;
    PacketField<i16>(packet, 0x762) = -1;
    PacketField<u8>(packet, 0x77d) = 0;
    packet->takeover_object = NULL;
}

void SetFlicker(GameObject_s *object, f32 time) {
    object->damage_cooldown = time;
    object->player_packet.runtime_flags_4 &= ~0x70000;
}

void ResetPlayerAI(GameObject_s *object) {
    ObjectField<u32>(object, 0x109c) = 0;
    ObjectField<u8>(object, 0x4a4) &= 0x9f;
    object->head_movement_flags &= 0xdf;
    ObjectField<u8>(object, 0x4a5) &= 0xfd;
    ObjectField<u32>(object, 0x44c) = 0;
    ObjectField<u32>(object, 0x450) = 0;
    ObjectField<i16>(object, 0x3e6) = -1;
    ObjectField<i16>(object, 0x3e8) = -1;
    ObjectField<u32>(object, 0x440) = 0;
    ObjectField<u8>(object, 0x1092) = 0;
    ObjectField<u8>(object, 0x1093) = 0;
    ObjectField<u32>(object, 0x1098) = 0;
    ObjectField<u8>(object, 0x1094) = 0;
    object->usable_object = NULL;
    *(u32 *)&object->use_action = 0;
    object->use_distance = 0.0f;
    ObjectField<u32>(object, 0x4b4) = 0;
    ObjectField<u32>(object, 0xf14) = 0;
    memset(&object->path_info, 0, sizeof(object->path_info));
    ObjectField<i8>(object, 0x3f8) = -1;
    ObjectField<u8>(object, 0x3f9) = 0;
    ObjectField<i16>(object, 0x3e4) = -1;
    AISysGetCharacterPathPos(WORLD->ai_system, object, &object->ai_script_process, 0xff, 1);
}

void InitPlayerAI(GameObject_s *object) {
    StarWars_AutoSetAICapabilities(object);
    bool can_pull_levers = CanPullLevers(object->character_id);
    ObjectField<u8>(object, 0x4a6) &= 0xfe;
    object->head_movement_flags = object->head_movement_flags & 0x7f | can_pull_levers << 7;
    ObjectField<u32>(object, 0x3ec) = 0;
    ObjectField<u32>(object, 0x3f0) = 0;

    u32 object_flags = object->object_flags;
    if (FreePlay == 0 || (object_flags & 0x400) != 0) {
        if (SpecialRouteCharacterTypeIDFn != NULL) {
            i32 character_type;
            void *suit = ObjectField<void *>(object, 0xcac);
            if (suit == NULL) {
                character_type = *(i32 *)((u8 *)object->game_character_data + 0xc);
            } else {
                character_type = *(i32 *)((u8 *)suit + 4);
            }
            u8 route_type = SpecialRouteCharacterTypeIDFn(character_type);
            if (route_type < 0x40) {
                u32 high_mask = (route_type >> 5) & 1;
                ObjectField<u32>(object, 0x3f0) = high_mask << (route_type & 0x1f);
                ObjectField<u32>(object, 0x3ec) = (high_mask ^ 1) << (route_type & 0x1f);
            } else if (route_type != 0xff) {
                ObjectField<u32>(object, 0x3ec) = 0xffffffff;
                ObjectField<u32>(object, 0x3f0) = 0xffffffff;
            }
        }
    } else {
        ObjectField<u32>(object, 0x3ec) = 0xffffffff;
        ObjectField<u32>(object, 0x3f0) = 0xffffffff;
    }

    ObjectField<u8>(object, 0x370) = 0;
    u8 flags_ef8 = ObjectField<u8>(object, 0xef8);
    u32 character_flags = *(u32 *)((u8 *)object->character_data + 4);
    ObjectField<u32>(object, 0xf20) = 0;
    u8 flag_ef8_bit2 = ((character_flags >> 7) & 1) << 2;
    ObjectField<u8>(object, 0xef8) = flags_ef8 & 0x2b | 2 | flag_ef8_bit2 | 8;
    u8 flags_ef9 = ObjectField<u8>(object, 0xef9);
    u8 flags_eff = ObjectField<u8>(object, 0xeff);
    u8 flags_efb = ObjectField<u8>(object, 0xefb);
    u8 flags_f03 = ObjectField<u8>(object, 0xf03);
    u8 flags_f02 = ObjectField<u8>(object, 0xf02);
    ObjectField<u8>(object, 0xeff) = flags_eff & 0xc7;
    ObjectField<u8>(object, 0xefc) &= 0xc0;
    ObjectField<u8>(object, 0xefd) &= 0xe6;
    ObjectField<u8>(object, 0xef9) = flags_ef9 & 0xfc;
    ObjectField<u8>(object, 0xf01) &= 0xe9;
    ObjectField<u8>(object, 0xefb) = flags_efb & 0x25;
    ObjectField<u8>(object, 0xf00) &= 0x5c;
    ObjectField<u8>(object, 0xf03) = flags_f03 & 0x31;
    ObjectField<f32>(object, 0xee0) = 1.0e9f;
    ObjectField<f32>(object, 0xee4) = 1.0e9f;
    ObjectField<f32>(object, 0x1034) = 1.0e9f;
    ObjectField<f32>(object, 0x1038) = 1.0e9f;
    ObjectField<u8>(object, 0xf04) &= 0xfc;
    ObjectField<u32>(object, 0x1058) = 0;
    u8 route_flags = ((object_flags >> 10 ^ 1) & 1) << 2;
    ObjectField<u8>(object, 0xf02) = flags_f02 & 0xf3 | route_flags;
    ObjectField<f32>(object, 0x4ac) = DEFAULT_MOVE_RANGE;
    ObjectField<u8>(object, 0xf02) = flags_f02 & 0x53 | route_flags;
    ObjectField<u8>(object, 0x4a7) = ObjectField<u8>(object, 0x4a7) & 0xe3 | (DEFAULT_MOVE_RANGE > 0.0f) << 2;
    ObjectField<u8>(object, 0xf03) = flags_f03 & 0x30;
    u8 flags_efa = ObjectField<u8>(object, 0xefa);
    ObjectField<u8>(object, 0xefa) = flags_efa & 0xf3;
    ObjectField<u8>(object, 0x7b5) &= 0xfd;
    ObjectField<void *>(object, 0xecc) = NULL;
    ObjectField<void *>(object, 0x394) = NULL;
    ObjectField<void *>(object, 0x39c) = NULL;
    ObjectField<u8>(object, 0xefa) = flags_efa & 0xc3 | ((character_flags >> 27) & 1) << 4;
    ObjectField<u8>(object, 0xefb) = flags_efb & 0x20;
    ObjectField<u8>(object, 0xeff) = flags_eff & 0xc6;
    ObjectField<f32>(object, 0xed4) = 1.0f;
    ObjectField<f32>(object, 0xed0) = 0.0f;
    ObjectField<f32>(object, 0x3a0) = 1.0e9f;
    ObjectField<void *>(object, 0x3a4) = NULL;
    ObjectField<void *>(object, 0x3ac) = NULL;
    ObjectField<u8>(object, 0x4a5) &= 0xaf;
    ObjectField<u32>(object, 0x2a0) = 0;
    ObjectField<u32>(object, 0x2a4) = 0;
    ObjectField<u8>(object, 0xef9) = flags_ef9 & 0xf4;
    ObjectField<void *>(object, 0xebc) = NULL;
    ObjectField<f32>(object, 0x3b0) = 1.0e9f;
    ObjectField<void *>(object, 0xec0) = NULL;
    ObjectField<u32>(object, 0x10b0) = 0;
    ObjectField<u32>(object, 0x10b4) = 0;
    ObjectField<void *>(object, 0xec4) = NULL;
    ObjectField<void *>(object, 0x448) = NULL;
    ObjectField<void *>(object, 0xec8) = NULL;
    ObjectField<void *>(object, 0xed8) = NULL;
    ObjectField<u8>(object, 0xef8) = flags_ef8 & 0x2a | 2 | flag_ef8_bit2 | 8;
    memset((u8 *)object + 0xf48, 0, 0x68);
    ObjectField<u8>(object, 0x1089) = 0;
    ObjectField<void *>(object, 0xfe4) = NULL;
    ObjectField<void *>(object, 0x3b4) = NULL;
    ObjectField<u32>(object, 0x294) = 0;
    ObjectField<u32>(object, 0x298) = 0;
    ObjectField<u32>(object, 0x29c) = 0;

    u8 *character_data = (u8 *)object->character_data;
    bool state_flag =
        (character_data[5] & 0x40) != 0 && (object->player_index == -1 || ObjectField<void *>(object, 0xcc0) != NULL);
    ObjectField<u8>(object, 0x1f8) = ObjectField<u8>(object, 0x1f8) & 0xfd | state_flag << 1;
    u8 *runtime = *(u8 **)(character_data + 0x24);
    ObjectField<u8>(object, 0x1f9) &= 0xf7;
    ObjectField<u8>(object, 0x1fa) &= 0xe7;
    ObjectField<u8>(object, 0xf00) &= 0xfb;
    object->head_movement_flags &= 0xbf;
    ObjectField<u8>(object, 0x108f) = 0;
    ObjectField<u8>(object, 0xeff) = ObjectField<u8>(object, 0xeff) & 0x7b | ((*(u32 *)(runtime + 0x90) >> 13) << 7);
    ObjectField<u32>(object, 0x10c0) = 0;
    u8 flags_f04 = ObjectField<u8>(object, 0xf04);
    ObjectField<u32>(object, 0x10d8) = 0;
    ObjectField<f32>(object, 0x1040) = 1.0f;
    ObjectField<f32>(object, 0x1044) = 0.0f;
    ObjectField<u8>(object, 0xf04) = flags_f04 & 0x73 | runtime[0x98] & 0x80;
    ResetPlayerAI(object);
}

void SetGameObjectCharacterData(GameObject_s *object) {
    CHARACTERDATA *character_data = object->character_data;
    object->scale = character_data->field17_0x3c;
    object->base_collision_radius = character_data->field14_0x30;
    object->model_bottom = character_data->field15_0x34;
    object->model_top = character_data->field16_0x38;
    ScaleGameObject(object);
}

void SetHitPoints(GameObject_s *object, i32 hit_points) {
    object->hit_points = hit_points;
    if (object->maximum_hit_points < hit_points) {
        object->hit_points = object->maximum_hit_points;
    }
}

u32 MakePlayerList(i32 count) {
    u32 player_count = 0;
    for (i32 index = 0; index < count; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL) {
            if (makeplayerlist_freeplay == 1 && player_count == 1 && WORLD != NULL &&
                *(void **)((u8 *)WORLD + 0x128) == HUB_LDATA && HUB_LDATA != NewLData) {
                player_count = 2;
                PlayerList[1] = FreePlayModelList[index * 2];
                PlayerProgress[1].active = 0;
            }
            continue;
        }

        if (makeplayerlist_freeplay == 1) {
            PlayerList[player_count] = FreePlayModelList[index * 2];
        } else if (makeplayerlist_freeplay != 2) {
            PlayerList[player_count] = object->character_id;
        }

        PlayerProgressEntry &progress = PlayerProgress[player_count];
        progress.active = (i8)object->state_flags < 0;
        bool alternate_state = false;
        if (ObjectField<i8>(object, 0x7a5) != 6 && ObjectField<i8>(object, 0xe32) != 2) {
            alternate_state = true;
            if ((ObjectField<u8>(object, 0xe22) & 1) == 0) {
                alternate_state = ObjectField<i8>(object, 0xe32) != 0;
            }
        }
        progress.alternate_state = alternate_state;

        COINPACKET_s *coin_packet = object->player_packet.coin_packet;
        progress.progress = coin_packet != NULL ? *(u32 *)coin_packet : 0;
        progress.special_state = ObjectField<i8>(object, 0xe31) == 1;
        if (object->death_state == 0) {
            progress.custom_part = ObjectField<u8>(object, 0x108b);
            progress.custom_color = ObjectField<u8>(object, 0xe38);
        } else {
            progress.custom_part = ObjectField<u8>(object, 0x108a);
            progress.custom_color = 4;
        }
        progress.custom_detail = ObjectField<u8>(object, 0x108e);
        progress.previous_character_id = ObjectField<i16>(object, 0x106e);
        progress.runtime_value = ObjectField<u32>(object, 0xdec);
        PlayerSuit[player_count] = ObjectField<void *>(object, 0xcac);

        u8 *torpedo_packet = ObjectField<u8 *>(object, 0xcb4);
        PlayerTorpedoCount[player_count] = torpedo_packet != NULL ? *torpedo_packet : 0;
        player_count++;
    }

    if (player_count <= 7) {
        for (u32 index = player_count; index < 8; index++) {
            PlayerList[index] = -1;
        }
    }
    makeplayerlist_freeplay = 0;
    return player_count;
}

void Player_CopyEssentials(GameObject_s *source, GameObject_s *destination) {
    *(u32 *)((u8 *)destination + 0xc94) = *(u32 *)((u8 *)source + 0xc94);
    *(u32 *)((u8 *)destination + 0x7ec) = *(u32 *)((u8 *)source + 0x7ec);
    *(u32 *)((u8 *)destination + 0x7f0) = *(u32 *)((u8 *)source + 0x7f0);
    *(u32 *)((u8 *)destination + 0xcb0) = *(u32 *)((u8 *)source + 0xcb0);
    *(u32 *)((u8 *)destination + 0xcb4) = *(u32 *)((u8 *)source + 0xcb4);
    *(u32 *)((u8 *)destination + 0xc98) = *(u32 *)((u8 *)source + 0xc98);
    destination->maximum_hit_points = source->maximum_hit_points;
    *(u32 *)((u8 *)destination + 0xeb4) = *(u32 *)((u8 *)source + 0xeb4);
    destination->object_flags = source->object_flags;
}

void ClearAICreatures() {
    i32 index = 0;
    GameObject_s *object = Obj;
    if (HIGHGAMEOBJECT > 0) {
        do {
            while ((object->state_flags & 1) == 0 || (object->object_flags & 0x400) == 0) {
                index++;
                object++;
                if (index >= HIGHGAMEOBJECT) {
                    return;
                }
            }
            FreeTorpedoPacket(&object->player_packet.torpedo_packet);
            RemoveGameObject(object, 1);
            index++;
            object++;
        } while (index < HIGHGAMEOBJECT);
    }
}

void StoreProgressAICharacter(LEVEL_PROGRESS_s *progress) {
    if (progress == NULL) {
        return;
    }

    u32 *destroyed_mask = (u32 *)((u8 *)progress + 0x2804);
    destroyed_mask[0] = 0;
    destroyed_mask[1] = 0;
    u32 object_count = HIGHGAMEOBJECT;
    for (u32 index = 0; index < object_count; index++) {
        GameObject_s *object = &Obj[index];
        if ((object->state_flags & 1) != 0 && (object->object_flags & 0x400) != 0 && object->ai_lifecycle_state == 4) {
            destroyed_mask[index >> 5] |= 1U << (index & 0x1f);
        }
    }
}

void AICreatureResumeScript(GameObject_s *object) {
    AISCRIPTPROCESS *process = GetAIProcess(object);
    AISCRIPT *script = process->base_script;
    if (script != NULL) {
        AIScriptProcessorInit(WORLD->ai_system, (AIPACKET *)process, process, NULL, NULL, NULL, 0, script,
                              script->base_state);
        process->active_ref_count = 0;
    }
}

void DeactivateGameObject(GameObject_s *object) {
    if (object == NULL) {
        return;
    }

    GameObject_s *takeover_object = object->player_packet.takeover_object;
    if (takeover_object != NULL) {
        if ((object->object_flags & 0x4000) == 0) {
            KillGameObject(takeover_object, 4, 0);
        } else {
            ReleaseTakeOver(object, 1);
        }
    }

    object->state_flags &= ~0x1000;
    i8 spawn_index = object->ai_spawn_index;
    if (spawn_index != -1 &&
        AIScriptSetBaseScriptStateByName((AISCRIPTPROCESS *)((u8 *)object + 0x2c0), (char *)"InActive") != 0) {
        object->ai_lifecycle_state = 0;
        *((u8 *)&WORLD->ai_system->creatures[(u8)spawn_index] + 0x87) = 2;
    } else {
        object->ai_lifecycle_state = 4;
    }
}

void DeactivateCharacter(char *name) {
    if (Mission_Active(NULL) == 0 && name != NULL) {
        GameObject_s *object = GetNamedGameObject(WORLD->ai_system, name);
        if (object != NULL && (FreePlay == 0 || (object->object_flags & 0x400) != 0)) {
            DeactivateGameObject(object);
        }
    }
}

i32 GetDefaultIdle(GameObject_s *object) {
    PlayerCharacterRuntime *runtime = (PlayerCharacterRuntime *)object->character_data->field11_0x24;
    u8 *animation_table = *(u8 **)((u8 *)object->game_character_data + 0xc);
    i32 action;
    i32 animation_offset;
    if (runtime->uses_weapon_action == 0) {
        bool has_weapon = (*((u32 *)object->game_character_data + 1) & 0x80) != 0;
        action = has_weapon ? 0x76 : 0x19;
        animation_offset = has_weapon ? 0x1d8 : 0x64;
    } else {
        action = 0x19;
        animation_offset = 0x64;
    }

    void *held_object = *(void **)((u8 *)object + 0xcb0);
    if (held_object != NULL && *((u8 *)held_object + 0x7d) != 0) {
        return 0x97;
    }

    bool has_animation = *(void **)(animation_table + animation_offset) != NULL;
    if (*(void **)(animation_table + 4) == NULL) {
        if (has_animation) {
            return action;
        }
    } else if (!has_animation) {
        return 1;
    }

    u8 *player_packet = (u8 *)&object->player_packet;
    if (((player_packet[0x76e] & 1) != 0 || player_packet[0x77e] == 1) && has_animation) {
        return action;
    }
    return 1;
}

void ResetCharacterIdle(GameObject_s *object, i32 reset, i32 action) {
    if (action == 0x97) {
        return;
    }

    object->idle_action = action;
    object->idle_timer = 0.0f;
    u8 *game_character_data = (u8 *)object->game_character_data;
    void **animation_table = *(void ***)(game_character_data + 0xc);
    if (animation_table[object->idle_action] != NULL) {
        u8 *animation_info = ((u8 **)*(void ***)(game_character_data + 8))[object->idle_action];
        u32 repetitions = animation_info[0xc];
        if (repetitions != 0) {
            u32 maximum_repetitions = animation_info[0xd];
            if (repetitions < maximum_repetitions) {
                repetitions += qrand() / ((0xffff / (maximum_repetitions - repetitions)) + 1);
            }
            object->idle_duration =
                AnimDuration(object->character_id, object->idle_action, 0.0f, 0.0f, 0) * repetitions -
                animduration_blendouttime;
        } else {
            object->idle_duration = QRAND_FLOAT() * 7.0f + 8.0f;
        }
    } else {
        object->idle_duration = QRAND_FLOAT() * 7.0f + 8.0f;
    }

    if (reset > 0) {
        object->idle_elapsed = 0.0f;
        if (reset != 1) {
            object->previous_idle_action = -1;
        }
    }
}

static void NewCharacterIdle(GameObject_s *object, i32 default_action) {
    PlayerCharacterRuntime *runtime = (PlayerCharacterRuntime *)object->character_data->field11_0x24;
    i32 weapon_idle = 0x19;
    if (runtime->uses_weapon_action == 0 && (*((u32 *)object->game_character_data + 1) & 0x80) != 0) {
        weapon_idle = 0x76;
    }

    u8 *game_character_data = (u8 *)object->game_character_data;
    void **animation_table = *(void ***)(game_character_data + 0xc);
    u8 **animation_info = (u8 **)*(void ***)(game_character_data + 8);
    i32 idle_animations[0xe9];
    i32 idle_count = 0;
    for (i32 animation = 0; animation < 0xe9; animation++) {
        if (animation_table[animation] == NULL) {
            continue;
        }
        u32 flags = *(u32 *)(animation_info[animation] + 4);
        if ((flags & 0x10) != 0 && ((flags & 0x800) != 0) == (weapon_idle == default_action)) {
            idle_animations[idle_count++] = animation;
        }
    }

    if (idle_count == 0) {
        ResetCharacterIdle(object, 0, default_action);
        return;
    }
    if (idle_count == 1) {
        object->idle_action = idle_animations[0];
    } else {
        do {
            i32 random_index = qrand() / ((0xffff / idle_count) + 1);
            object->idle_action = idle_animations[random_index];
        } while (object->idle_action == object->previous_idle_action);
    }

    u8 *info = animation_info[object->idle_action];
    u32 repetitions = info[0xc];
    if (repetitions == 0) {
        repetitions = 1;
        object->previous_idle_action = object->idle_action;
    } else {
        u32 maximum_repetitions = info[0xd];
        if (repetitions < maximum_repetitions && maximum_repetitions != 0) {
            repetitions += qrand() / ((0xffff / (maximum_repetitions - repetitions)) + 1);
        }
        object->previous_idle_action = object->idle_action;
        if (repetitions > 1 && (*(u32 *)(info + 4) & 2) == 0) {
            repetitions = 1;
        }
    }

    object->idle_timer = 0.0f;
    object->idle_duration = AnimDuration(object->character_id, object->idle_action, 0.0f, 0.0f, 0) * repetitions -
                            animduration_blendouttime;
}

void UpdateCharacterIdle(GameObject_s *object) {
    if (object->game_character_data == NULL) {
        return;
    }

    PlayerCharacterRuntime *runtime = (PlayerCharacterRuntime *)object->character_data->field11_0x24;
    i32 default_action = 0x19;
    if (runtime->uses_weapon_action == 0 && (*((u32 *)object->game_character_data + 1) & 0x80) != 0) {
        default_action = 0x76;
    }

    i32 current_animation = object->previous_animation_state;
    bool update_timer = false;
    if (current_animation == 1) {
        if (default_action == object->animation_state) {
            ResetCharacterIdle(object, 1, 1);
            return;
        }
        update_timer = true;
    } else if (default_action == current_animation) {
        if (object->animation_state == 1) {
            ResetCharacterIdle(object, 1, default_action);
            return;
        }
        update_timer = true;
    } else {
        i32 idle = GetDefaultIdle(object);
        if (idle != -1) {
            ResetCharacterIdle(object, 1, idle);
            return;
        }
        if (object->previous_animation_state == 1) {
            update_timer = true;
        } else if (default_action != object->previous_animation_state) {
            ResetCharacterIdle(object, 1, GetDefaultIdle(object));
            return;
        }
    }

    if (update_timer) {
        object->idle_elapsed += FRAMETIME;
        object->idle_timer += FRAMETIME;
        if (object->idle_timer >= object->idle_duration) {
            if (default_action == object->idle_action) {
                NewCharacterIdle(object, default_action);
            } else {
                ResetCharacterIdle(object, 1, default_action);
            }
        }
    }
    object->previous_animation_state = object->idle_action;
}

GameObject_s *AddCreature(i32 character_id, i32 initialize_ai) {
    if ((u32)character_id >= 0x154 || apicharsys->character_indices[character_id] == -1) {
        return NULL;
    }

    GameObject_s *object = AddGameObject(character_id);
    if (object != NULL) {
        InitCreature(object, character_id, initialize_ai);
    }
    return object;
}

void InitAICreatures(AISYS *ai_system) {
    if (Mission_Active(NULL) != 0 || netclient != 0 || ai_system == NULL) {
        return;
    }

    for (i32 creature_index = 0; creature_index < ai_system->creature_count; creature_index++) {
        AICREATURE *creature = &ai_system->creatures[creature_index];
        if (((creature->flags & 0x20) != 0 && g_lowEndLevelBehaviour != 0) || creature->group_idx == 0) {
            continue;
        }

        AIGROUP *group = NULL;
        for (u32 member_index = 0; member_index < creature->group_idx; member_index++) {
            if (NOAICREATURES != 0 && (((u8 *)&GCDataList[creature->type])[0x90] & 0x40) == 0) {
                continue;
            }
            if (member_index >= 32 || (creature->unknown_58 & (1U << member_index)) == 0) {
                continue;
            }

            GameObject_s *object = AddCreature(creature->type, 1);
            if (object == NULL) {
                continue;
            }

            object->state_flags &= ~0x1000;
            u32 original_flags = object->object_flags;
            object->object_flags = original_flags | 0x400;
            u32 character_flags = *(u32 *)((u8 *)&apicharsys->character_data[creature->type] + 4);
            if ((character_flags & 0x200) != 0) {
                object->object_flags = original_flags | 0x404;
            } else if ((character_flags & 4) != 0) {
                object->object_flags = original_flags | 0x401;
            }

            object->secondary_flags |= (character_flags & 0x1000) != 0 ? 5 : 1;
            object->ai_spawn_index = creature_index;

            if (member_index == 0 && creature->group_idx > 1 && creature->start_stagger == 0.0f) {
                u8 *runtime = (u8 *)object->character_data->field11_0x24;
                group = CreateAIGroup(ai_system, creature->pos_across, creature->x_spacing, creature->z_spacing,
                                      *(i32 *)(runtime + 0x1c));
            }
            if (group != NULL) {
                AddToAIGroup(group, group, object);
            }

            object->ai_script_process.unknown_a0 = creature->area;
            object->ai_script_process.unknown_a4 = creature->locator;
            ObjectField<void *>(object, 0x46c) = creature->respawn_locator;
            object->ai_script_process.unknown_b0 = creature->set;
        }
    }

    ai_system->has_done_reset = 0;
}

void ResetPlayerMoves(GameObject_s *object) {
    ResetPlayerPacket(&object->player_packet, object->character_data);
    object->interaction_timer = 0.0f;
    object->player_packet.tag_cooldown = 0.0f;
    object->surface_normal = v000;
    object->movement_flags |= 0x80;
    object->player_packet.toggle_hold_timer = TOGGLEHOLDTIME;
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));

    void **animation_table = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animation_table[1] == NULL) {
        ResetAnimPacket((u8 *)object + 8, -1);
    } else {
        ResetAnimPacket((u8 *)object + 8, 1);
        SetAnimTimeRandom(object->game_character_data, &object->animation_frame);
    }

    object->previous_ground_contact_flags = 0;
    object->ground_contact_flags = 0;
    SetGameObjectCharacterData(object);
    object->spawn_protection_timer = 0.0f;
    SetFlicker(object, 0.0f);
    ResetCoinPacket(object->player_packet.coin_packet);
    object->jump_stuck_timer = 0.0f;
    object->state_flags &= ~0x2000;
    DrawOffsetCode(object, 1);
}

void ResetAICreatures(AISYS *ai_system) {
    if (ai_system == NULL || ai_system->has_done_reset != 0) {
        return;
    }
    ai_system->has_done_reset = 1;

    for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
        GameObject_s *object = &Obj[object_index];
        if ((object->state_flags & 1) == 0 || (object->object_flags & 0x400) == 0) {
            continue;
        }

        object->state_flags &= ~0x1000;
        if (WORLD->game_object_system != NULL) {
            u32 *destroyed_mask = (u32 *)((u8 *)WORLD->game_object_system + 0x2804);
            if ((destroyed_mask[(u32)object_index >> 5] & (1U << (object_index & 0x1f))) != 0) {
                object->ai_lifecycle_state = 4;
                object->death_state = 1;
                continue;
            }
        }

        AICREATURE *creature = &ai_system->creatures[(u8)object->ai_spawn_index];
        ObjectField<u32>(object, 0x4b8) = 0;
        object->ai_lifecycle_state = 0;
        AISCRIPTPROCESS *process = GetAIProcess(object);
        AIScriptProcessorInit(WORLD->ai_system, (AIPACKET *)process, process, creature, creature->script_name,
                              (char *)"InActive", 1, NULL, NULL);

        if (process->state != NULL && process->state->name != NULL &&
            NuStrICmp(process->state->name, "InActive") == 0) {
            creature->activate_type = 2;
            continue;
        }
        if (creature->activate_type != 0) {
            continue;
        }
        if (((u8 *)&Game)[1] < creature->activation_difficulty) {
            object->ai_lifecycle_state = 4;
        } else if (creature->group_idx < 2 || creature->start_stagger <= 0.0f || object->ai_group_member_index == 0) {
            ResetAICreature(object, ai_system);
        } else {
            object->ai_lifecycle_state = 1;
            ObjectField<f32>(object, 0x4bc) = object->ai_group_member_index * creature->start_stagger;
        }
    }
}

GameObject_s *ActivateCharacter(char *name, NUVEC *position, i32 heading) {
    if (Mission_Active(NULL) != 0 || name == NULL) {
        return NULL;
    }

    GameObject_s *object = GetNamedGameObject(WORLD->ai_system, name);
    if (object == NULL || (object->state_flags & 0x1000) != 0 ||
        (FreePlay != 0 && (object->object_flags & 0x400) == 0)) {
        return NULL;
    }

    if (object->ai_spawn_index == -1) {
        object->state_flags |= 0x1000;
        AISCRIPTPROCESS *process = GetAIProcess(object);
        AIScriptSetBaseScriptStateByName(process, (char *)"Active");
        if (position != NULL) {
            object->position = *position;
            object->yaw = heading;
            object->heading = heading;
            object->desired_heading = heading;
            AISysGetCharacterPathPos(WORLD->ai_system, object, process, 0xff, 1);

            object->spawn_position = object->position;
            object->origin = object->position;
            object->old_position = object->position;
            object->position_history_1 = object->position;
            object->last_safe_position = object->position;
            object->previous_safe_position = object->position;
            plr_lastpos = object->position;
            object->player_packet.cached_velocity = v000;
            object->velocity = v000;
            InitSurfaceInfo(object);
            SetObjOnSurface(object, 0);
        }
    } else {
        ResetAICreature(object, WORLD->ai_system);
    }
    LightGameObject(object, WORLD->lighting_system);
    return object;
}

NUVEC *Player_StartPos(GameObject_s *object) {
    u32 index = (i8)object->player_index;
    if ((u8)object->player_index > 7) {
        index = object->mask_bit_index;
    }
    NUVEC *position = PlayerStart[index & 7].position;
    return position != NULL ? position : PlayerStart[0].position;
}

void SetPlayer() {
    if (Player[0] == NULL || (i8)Player[0]->state_flags >= 0) {
        if (Player[1] == NULL || (i8)Player[1]->state_flags >= 0) {
            player = NULL;
            return;
        }
        player = Player[1];
    } else {
        player = Player[0];
        if (Player[1] != NULL && (i8)Player[1]->state_flags < 0) {
            player2 = Player[1];
            return;
        }
    }
    player2 = NULL;
}

GameObject_s *Player_FindByID(i32 id) {
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object != NULL && (object->state_flags_low & 0x1001) == 0x1001 && object->character_id == id) {
            return object;
        }
    }
    return NULL;
}

void RememberPlayerIDs(i32 force, i32 first_id, i32 second_id) {
    if (VehicleArea != 0 || GAMEDEMO != 0) {
        return;
    }
    if (force == 0 && (*(u32 *)(*(u8 **)((u8 *)WORLD + 0x128) + 0x64) & 0x4e2) != 2) {
        return;
    }

    if (first_id != -1 && (((u8 *)&CDataList[first_id])[5] & 0x20) == 0 && PlayerID[0] != first_id &&
        Collection_Got(first_id) == 1 && ((u8 *)&GCDataList[first_id])[0x116] != 0 && PlayerID[1] != first_id) {
        PlayerID[0] = first_id;
    }
    if (second_id != -1 && (((u8 *)&CDataList[second_id])[5] & 0x20) == 0 && PlayerID[1] != second_id &&
        Collection_Got(second_id) == 1 && ((u8 *)&GCDataList[second_id])[0x116] != 0 && PlayerID[0] != second_id) {
        PlayerID[1] = second_id;
    }
    if (first_id != second_id && PlayerID[1] == first_id && PlayerID[0] == second_id) {
        PlayerID[0] = first_id;
        PlayerID[1] = second_id;
    }
}

i32 Players_AveragePos(NUVEC *position, SOCKPOSITION_s *sock_position) {
    NUVEC total = v000;
    f32 count = 0.0f;
    for (i32 index = 0; index < 2; index++) {
        GameObject_s *object = Player[index];
        if (object != NULL && (i8)object->state_flags < 0) {
            NuVecAdd(&total, &total, &object->position);
            count += 1.0f;
        }
    }

    if (count <= 0.0f) {
        if (sock_position != NULL) {
            sock_position->sock_index = -1;
        }
        return 0;
    }

    NuVecScale(position, &total, 1.0f / count);
    if (sock_position != NULL) {
        ComplexSockPosition(*(void **)((u8 *)WORLD + 0x2988), position, -1, 0xffffffff, sock_position);
    }
    return 1;
}

GameObject_s *GetOtherActivePlayer(GameObject_s *object) {
    GameObject_s *other = NULL;
    if (object == Player[0]) {
        other = Player[1];
    } else if (object == Player[1]) {
        other = Player[0];
    }
    return other != NULL && (i8)other->state_flags < 0 ? other : NULL;
}

i32 ActivePlayerInRange(NUVEC *position, f32 distance_squared, f32 *actual_distance_squared) {
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL || (i8)object->state_flags >= 0) {
            continue;
        }
        f32 distance = NuVecDistSqr(&object->origin, position, NULL);
        if (distance < distance_squared) {
            if (actual_distance_squared != NULL) {
                *actual_distance_squared = distance;
            }
            return 1;
        }
    }
    return 0;
}

i32 AvailableToPlayer(u32 character_flags, i32 character_type, i32 detail, i32 require_all) {
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL || object->character_data == NULL) {
            continue;
        }

        u32 object_flags = object->character_data->field1_0x4;
        i8 object_type = *((i8 *)object->character_data->field11_0x24 + 0x116);
        u8 object_detail = ObjectField<u8>(object, 0x108e);
        if (require_all == 0) {
            if (detail == 0 || (character_flags != 0 && (object_flags & character_flags) == character_flags) ||
                (character_type != -1 && object_type == character_type) || object_detail == detail) {
                return 1;
            }
        } else if ((character_flags == 0 || (object_flags & character_flags) == character_flags) &&
                   (character_type == -1 || object_type == character_type) &&
                   (detail == 0 || object_detail == detail)) {
            return 1;
        }
    }

    if (FreePlay == 0 || apicharsys->character_count <= 0) {
        return 0;
    }
    for (i32 index = 0; index < apicharsys->character_count; index++) {
        i16 loaded_index = apicharsys->character_indices[index];
        if (loaded_index == -1 || (apicharsys->character_info[loaded_index * 0x54 + 2] & 1) == 0) {
            continue;
        }

        bool available = Game_CharacterSave != NULL && (Game_CharacterSave[index] & 1) != 0;
        if (!available && (GCDataList[index].field144_0x90 >= 0 || Cheats_CheckFlags(0x100) == 0)) {
            continue;
        }

        if (character_flags != 0) {
            bool flags_match = (CDataList[index].field1_0x4 & character_flags) == character_flags;
            if (flags_match) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (character_type == -1 || ((i8 *)&GCDataList[index])[0x116] == character_type) {
            return 1;
        }
    }
    return 0;
}

bool FindNearestPlayerToVec(NUVEC *position, GameObject_s **nearest, f32 &distance_squared, bool check_character_flags,
                            u32 character_flags) {
    *nearest = NULL;
    distance_squared = 0.0f;
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL || (i8)object->state_flags >= 0) {
            continue;
        }

        f32 distance = NuVecDistSqr(&object->position, position, NULL);
        if (distance >= distance_squared && *nearest != NULL) {
            continue;
        }
        if (check_character_flags) {
            u32 flags = *(u32 *)((u8 *)object->character_data->field11_0x24 + 0x90);
            if ((flags & character_flags) == 0) {
                continue;
            }
        }
        distance_squared = distance;
        *nearest = object;
    }
    return *nearest != NULL;
}

bool FindFurthestPlayerFromVec(NUVEC *position, GameObject_s **furthest, f32 &distance_squared,
                               bool check_character_flags, u32 character_flags) {
    *furthest = NULL;
    distance_squared = 0.0f;
    for (i32 index = 0; index < 8; index++) {
        GameObject_s *object = Player[index];
        if (object == NULL || (i8)object->state_flags >= 0) {
            continue;
        }

        f32 distance = NuVecDistSqr(&object->position, position, NULL);
        if (distance <= distance_squared && *furthest != NULL) {
            continue;
        }
        if (check_character_flags) {
            u32 flags = *(u32 *)((u8 *)object->character_data->field11_0x24 + 0x90);
            if ((flags & character_flags) == 0) {
                continue;
            }
        }
        distance_squared = distance;
        *furthest = object;
    }
    return *furthest != NULL;
}

void FindPlayerAndSetWeapon(i32 id, i32 enabled) {
    GameObject_s *object = Player_FindByID(id);
    if (object == NULL) {
        return;
    }

    ObjectField<u8>(object, 0xe22) = ObjectField<u8>(object, 0xe22) & ~1 | enabled & 1;
    PacketField<f32>(&object->player_packet, 0x67c) = enabled == 0 ? 0.0f : 1.0f;
    if (enabled != 0 && (i8)object->state_flags >= 0) {
        ObjectField<f32>(object, 0xed8) = DEFENDTIME;
    }
}

void PlayerTakeHit(GameObject_s *object, GameObject_s *attacker) {
    if (object->player_index != -1) {
        return;
    }

    i8 saved_action_latch = ObjectField<i8>(object, 0xe31);
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    i32 hit_animation;
    i16 hit_action;
    if (saved_action_latch == 1) {
        hit_animation = animations[0x3e] != NULL;
        hit_action = 0x3e;
    } else {
        hit_animation = animations[0x3d] != NULL;
        hit_action = 0x3d;
    }

    if (!hit_animation) {
        if ((ObjectField<u8>(object, 0xefb) & 8) != 0) {
            ResetAnimPacket((u8 *)object + 8, -1);
            Player_ClearContext(object, 1);
            Player_ResetContexts(&object->player_packet);
        }
        return;
    }

    if (object->player_packet.character_state == 0x1b && object->player_packet.opponent != NULL &&
        object->player_packet.opponent->player_packet.character_state == 0x1c) {
        object->player_packet.opponent->player_packet.character_state = -1;
    }
    object->player_packet.action = hit_action;
    ResetAnimPacket((u8 *)object + 8, -1);
    Player_ClearContext(object, 1);
    Player_ResetContexts(&object->player_packet);
    object->player_packet.character_state = 0x15;
    f32 duration = AnimDuration(object->character_id, object->player_packet.action, 0.0f, 0.0f, 1);
    PacketField<f32>(&object->player_packet, 0xb8) = duration > 0.0f ? duration : 1.0f;
    ObjectField<i8>(object, 0xe31) = saved_action_latch;
    SetFlicker(object, 0.4f);

    if (attacker != NULL) {
        u16 angle = NuAtan2D(attacker->origin.x - object->origin.x, attacker->origin.z - object->origin.z);
        object->yaw = angle;
        object->heading = angle;
        object->desired_heading = angle;
    }
}

void ActivatePlayer(GameObject_s *object) {
    object->player_packet.character_state = -1;
    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    if (animations[0x80] != NULL) {
        object->player_packet.character_state = 0x41;
        object->player_packet.action = 0x80;
        PacketField<f32>(&object->player_packet, 0xb8) = AnimDuration(object->character_id, 0x80, 0.0f, 0.0f, 1);
        PacketField<u32>(&object->player_packet, 0xc0) = 0;
    }
}

i32 DeactivatePlayer(GameObject_s *object, f32 duration, GameObject_s *attacker) {
    if (object->player_packet.character_state == 0x17 && duration <= PacketField<f32>(&object->player_packet, 0xb8)) {
        return 0;
    }

    u32 effect_duration = PacketField<u32>(&object->player_packet, 0x670);
    Player_ClearContext(object, 1);
    if (object->player_packet.character_state == 0x3e) {
        return 0;
    }
    Player_ResetContexts(&object->player_packet);
    object->player_packet.character_state = 0x17;
    PacketField<u32>(&object->player_packet, 0x670) = effect_duration;

    AISCRIPTPROCESS *process = &object->ai_script_process;
    if (AIScriptSetBaseScriptStateByName(process, (char *)"BeenDeactivated") != 0) {
        AIScriptProcess(WORLD->ai_system, object, process, process, FRAMETIME);
    }

    void **animations = *(void ***)((u8 *)object->game_character_data + 0xc);
    void **animation_info = *(void ***)((u8 *)object->game_character_data + 8);
    u8 *runtime = (u8 *)object->character_data->field11_0x24;
    bool special_hit = attacker != NULL && (attacker->character_data->field1_0x4 & 0x40) != 0 &&
                       (i8)runtime[0x116] == 1 && animations[0x62] != NULL;
    if (special_hit) {
        duration = 4.0f;
        object->player_packet.action = 0x62;
    } else if (animations[0x81] != NULL && (*(u8 *)((u8 *)animation_info[0x81] + 4) & 2) == 0) {
        object->player_packet.action = 0x81;
    } else {
        object->player_packet.action = animations[0x41] != NULL ? 0x41 : 1;
    }

    if ((object->character_data->field1_0x4 & 0x20) != 0) {
        SetProtocolDroidDeactivatedAction(object);
    }
    PacketField<u8>(&object->player_packet, 0xf4) = 0;
    PacketField<f32>(&object->player_packet, 0xb8) = duration;
    PacketField<u32>(&object->player_packet, 0xc0) = 0;
    PacketField<u32>(&object->player_packet, 0xb4) = 0;
    NewBuzz(*(NUPAD **)object->player_packet.game_pad, 0.1f, 0);
    return 1;
}

i32 KillPlayer(GameObject_s *object, i32 reason, i32 immediate, NUVEC *) {
    if (object->death_state != 0) {
        return 0;
    }
    if (immediate == 0) {
        if (object->spawn_protection_timer > 0.0f) {
            return 0;
        }
    } else {
        object->spawn_protection_timer = 0.0f;
    }

    if (Arcade != 0) {
        if (object->player_index == 0) {
            Arcade_PlayerKilled(1);
        } else if (object->player_index == 1) {
            Arcade_PlayerKilled(0);
        }
    }
    return KillGameObject(object, reason, 0);
}

i32 NewPlayerCharacter(GameObject_s *object, i32 character_id, i32 previous_character_id, i32) {
    if (character_id == previous_character_id || character_id < 0 || character_id >= CHARCOUNT ||
        apicharsys->character_indices[character_id] == -1) {
        return 0;
    }

    Shards_HandleLostObj((WORLDINFO_s *)WORLD, object);
    if (VehicleArea == 0) {
        AddGameDebris(*(void **)((u8 *)WORLD + 0x134), 0x5c, &object->origin);
    }

    i8 old_state = object->player_packet.character_state;
    i16 old_render_pitch = PacketField<i16>(&object->player_packet, 0x760);
    u32 old_character_flags = object->character_data->field1_0x4;
    LoseHelmet(object);
    DestroySnakeBody(object);

    object->character_id = character_id;
    NUVEC old_velocity = object->velocity;
    u8 old_current_hit_priority = ObjectField<u8>(object, 0x108b);
    u8 old_hit_priority = ObjectField<u8>(object, 0x108a);
    u8 old_state_flags_1 = ObjectField<u8>(object, 0x1f9);
    object->game_character_data =
        (GAMECHARACTERDATA *)(apicharsys->character_info + apicharsys->character_indices[character_id] * 0x54);
    object->character_data = &apicharsys->character_data[character_id];
    ObjectField<GameObject_s *>(object, 0xcc4) = NULL;
    ObjectField<void *>(object, 0xcac) = Suit_GetDefault(character_id);

    u32 previous_path_flags = ObjectField<u32>(object, 0xf14);
    i8 previous_ai_action = ObjectField<i8>(object, 0x3f8);
    void *big_jump_target = ObjectField<void *>(object, 0xf20);
    u8 render_flags = ObjectField<u8>(object, 0xefe);
    i8 ai_state = ObjectField<i8>(object, 0x3f9);
    void *path_data = ObjectField<void *>(object, 0x414);
    void *path_connection = ObjectField<void *>(object, 0x418);
    u8 path_endpoint = ObjectField<u8>(object, 0x41c);
    u8 path_extra_41d[3];
    memcpy(path_extra_41d, (u8 *)object + 0x41d, sizeof(path_extra_41d));
    u16 path_extra_420 = ObjectField<u16>(object, 0x420);
    u8 path_control_flags = ObjectField<u8>(object, 0x422);
    u8 path_extra_423 = ObjectField<u8>(object, 0x423);
    f32 path_progress = ObjectField<f32>(object, 0x424);
    u32 path_extra_428 = ObjectField<u32>(object, 0x428);
    u32 path_connection_flags = ObjectField<u32>(object, 0x4b4);

    InitPlayerAI(object);
    ObjectField<u8>(object, 0xefe) = ObjectField<u8>(object, 0xefe) & 0xdf | render_flags & 0x20;
    ObjectField<void *>(object, 0xf20) = big_jump_target;
    ObjectField<i8>(object, 0x3f9) = ai_state;
    ObjectField<void *>(object, 0x414) = path_data;
    ObjectField<void *>(object, 0x418) = path_connection;
    ObjectField<u8>(object, 0x41c) = path_endpoint;
    memcpy((u8 *)object + 0x41d, path_extra_41d, sizeof(path_extra_41d));
    ObjectField<u16>(object, 0x420) = path_extra_420;
    ObjectField<u8>(object, 0x422) = path_control_flags;
    ObjectField<u8>(object, 0x423) = path_extra_423;
    ObjectField<f32>(object, 0x424) = path_progress;
    ObjectField<u32>(object, 0x428) = path_extra_428;
    ObjectField<u32>(object, 0xf14) = previous_path_flags;
    object->height_scale = 1.0f;
    ObjectField<i8>(object, 0x3f8) = previous_ai_action;
    ObjectField<u32>(object, 0x4b4) = path_connection_flags;

    u8 *runtime = (u8 *)object->character_data->field11_0x24;
    object->layer_mask = *(u32 *)(runtime + 0xa0);
    ObjectField<u8>(object, 0xe38) = 4;
    ObjectField<u8>(object, 0xe37) = runtime[0xf5];
    Player_ClearContext(object, 1);
    ResetPlayerMoves(object);
    SetGameObjectCharacterData(object);
    ResetAnimPacket((u8 *)object + 8, -1);
    Player_ResetContexts(&object->player_packet);
    GetTopBot(object);
    GameObjectDimensions(object);
    object->player_packet.tag_cooldown = 2.0f;
    object->movement_flags |= 0x80;
    object->player_packet.toggle_hold_timer = TOGGLEHOLDTIME;
    InitSurfaceInfo(object);
    i32 placed_on_surface = SetObjOnSurface(object, 1);
    GizForce_ResetLOS(object);
    ObjectField<u8>(object, 0xefe) &= 0xf7;
    ObjectField<u8>(object, 0x108b) = old_current_hit_priority;
    ObjectField<u8>(object, 0x108a) = old_hit_priority;
    ObjectField<u32>(object, 0xef0) = 0;
    ObjectField<u32>(object, 0xef4) = 0;
    ObjectField<u8>(object, 0x1f9) = ObjectField<u8>(object, 0x1f9) & 0xdf | old_state_flags_1 & 0x20;
    ResetMiniAnimPacket((u8 *)&object->player_packet + 0x10c, -1);
    PacketField<u8>(&object->player_packet, 0x77e) = 0;
    PacketField<f32>(&object->player_packet, 0x67c) =
        (PacketField<u8>(&object->player_packet, 0x76e) & 1) != 0 ? 1.0f : 0.0f;

    if (placed_on_surface == 0) {
        object->velocity.y = old_velocity.y;
    } else {
        PacketField<f32>(&object->player_packet, 0x664) = 0.2f;
    }
    if (old_state == 0x33 && StartSlide(object, 0) != 0) {
        object->velocity.x = old_velocity.x;
        object->velocity.z = old_velocity.z;
    }
    if (object->velocity.y == 0.0f) {
        object->velocity.y = -0.1f;
    }
    if ((old_character_flags & 0x2000) != 0 && (((u8 *)object->character_data)[5] & 0x20) != 0) {
        PacketField<i16>(&object->player_packet, 0x760) = old_render_pitch;
    }

    memcpy((u8 *)object + 0x22c, runtime + 0x64, 0x10);
    ObjectField<u8>(object, 0xf01) = ObjectField<u8>(object, 0xf01) & 0xf7 | ((*(u32 *)(runtime + 0x90) >> 17) & 1)
                                                                                 << 3;

    static const u8 cantina_head[] = {0, 3, 6};
    static const u8 cantina_body[] = {1, 4, 7};
    static const u8 cantina_legs[] = {2, 5, 8};
    static const u8 cloud_head[] = {1, 5, 7};
    static const u8 cloud_body[] = {2, 3, 6};
    static const u8 cloud_legs[] = {0, 4, 8};

    if (object->character_id == id_MOSEISLEYCITIZEN) {
        SetLayers_MOSEISLEYCITIZEN(object);
    } else if (object->character_id == id_CANTINAALIEN) {
        object->layer_mask = 1U << cantina_head[qrand() / 0x5556];
        object->layer_mask |= 1U << cantina_body[qrand() / 0x5556];
        object->layer_mask |= 1U << cantina_legs[qrand() / 0x5556];
    } else if (object->character_id == id_CLOUDCITYCITIZEN) {
        object->layer_mask = 1U << cloud_head[qrand() / 0x5556];
        object->layer_mask |= 1U << cloud_body[qrand() / 0x5556];
        object->layer_mask |= 1U << cloud_legs[qrand() / 0x5556];
        ObjectField<u8>(object, 0xf01) = ObjectField<u8>(object, 0xf01) & 0xf7 | ((object->layer_mask >> 7) & 1) << 3;
    } else if (object->character_id == id_BOB) {
        SetLayers_BOB(object);
    } else if (object->character_id == id_GEONOSIAN) {
        ObjectField<u8>(object, 0xefd) = ObjectField<u8>(object, 0xefd) & 0xfd | (qrand() < 0x8000 ? 2 : 0);
    }

    if ((runtime[0x91] & 0x80) == 0) {
        object->object_flags &= ~0x20000;
    } else {
        object->object_flags |= 0x20000;
    }
    return 1;
}

void SetPlayerGroupPosition(f32 x, f32 y, f32 z) {
    NUVEC offset = {0.0f, 0.0f, 0.2f};
    for (i32 index = 0; index < PLAYERCOUNT; index++) {
        NUVEC position = {x, y, z};
        NuVecAdd(&position, &position, &offset);
        ResetPlayer(Player[index], 1, &position, 1);
        NuVecRotateY(&offset, &offset, 0x2000);
    }
}

bool CanPullLevers(i32 character_id) {
    u32 flags = *(u32 *)((u8 *)&CDataList[character_id] + 4);
    return (flags & 0x1000010) != 0x1000010 && (flags & 0x40088) != 0;
}

bool CanWearHatsInFreePlay(i32 character_id) {
    return (((u8 *)&GCDataList[character_id])[0x90] & 0x10) == 0 && CanPullLevers(character_id);
}

void Player_ClearContext(GameObject_s *object, i32 context) {
    if (Player_ClearContextFn != NULL) {
        Player_ClearContextFn(object, context);
    }
    Whip_Release(object);
    SuperCarry_Release(object);
    SpecialMove_ReleaseVictim(object);
    object->player_packet.runtime_flags_4 &= ~0xc00;
}

bool MovingBackwards(GameObject_s *object) {
    PlayerGamePad *game_pad = (PlayerGamePad *)object->player_packet.game_pad;
    if (game_pad->input_device != NULL && game_pad->magnitude != 0.0f) {
        i32 difference = RotDiff(game_pad->direction + *(u16 *)(GameCam + 0x1fc), object->yaw);
        return (u32)(difference + 0x4000) > 0x8000;
    }
    return false;
}

bool Players_BothActive() {
    return Player[0] != NULL && (i8)Player[0]->state_flags < 0 && Player[1] != NULL && (i8)Player[1]->state_flags < 0;
}

static i32 PlayerHasCheat(GameObject_s *object, u32 cheat) {
    return Cheats_CheckFlags(cheat) != 0 || (object != NULL && object->player_packet.cheat_powerup_time > 0.0f);
}

i32 Player_HasInvincibility(GameObject_s *object) {
    return PlayerHasCheat(object, 0x80);
}

i32 Player_HasFastBuild(GameObject_s *object) {
    return PlayerHasCheat(object, 0x4000);
}

i32 Player_HasDeflectBolts(GameObject_s *object) {
    return PlayerHasCheat(object, 0x80000);
}

i32 Player_HasDoubleBoltDamage(GameObject_s *object) {
    return PlayerHasCheat(object, 2);
}

i32 Player_HasDoubleBoltDamage_FromBolt(BOLT_s *bolt) {
    if ((bolt->flags & 1) != 0) {
        return Player_HasDoubleBoltDamage(Player[0]);
    }
    if ((bolt->flags & 2) != 0) {
        return Player_HasDoubleBoltDamage(Player[1]);
    }
    return 0;
}

i32 Player_HasDoubleWeaponDamage(GameObject_s *object) {
    return PlayerHasCheat(object, 0x400);
}

bool Player_HasPurpleForce(GameObject_s *object) {
    return Cheat_IsOn(0x1c) != 0 || (object != NULL && object->player_packet.cheat_powerup_time > 0.0f);
}
