#include "legogame/game.h"

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/area.h"
#include "legoapi/cheat.h"
#include "legoapi/character.h"
#include "legoapi/collection.h"
#include "legoapi/episode.h"
#include "legoapi/gameobject.h"
#include "legoapi/gamepads.h"
#include "legoapi/gizmos/attractos.h"
#include "legoapi/gizmos/door.h"
#include "legoapi/gizmos/gizbuildits.h"
#include "legoapi/level.h"
#include "legoapi/move.h"
#include "legoapi/players.h"
#include "legoapi/qrand.h"
#include "nu2api/nucore/nustring.h"

struct GameCharacterRuntime {
    u8 unknown_000[0x90];
    u32 ability_flags;
    u8 unknown_094[0x82];
    u8 uses_weapon_action;
};

struct CharacterEquipment {
    u8 unknown_00[0xc];
    u16 flags;
};

struct CUTSCENEPLAYERCLIP;
struct CUSTOMISER;
struct GAMEMESSAGE_s;
struct GIZMOBLOWUP_s;
struct PARTDEBSYS_s;

struct ADDGAMEMSG_s {
    u8 unknown_00[0x38];
    void (*end_delay)(GAMEMESSAGE_s *message);
};

extern "C" {
    extern i16 id_GRIEVOUS;
    extern i16 id_ATAT;
    extern i16 id_CHEWBACCA;
    extern i16 id_GEONOSIAN;
    extern i16 id_GAMORREANGUARD;
    extern i16 id_IMPERIALGUARD;
    extern i16 id_TAUNTAUN;
    extern i16 id_DEWBACK;
    extern i16 id_BANTHA;
    extern i16 id_ATST;
    extern i16 id_SNOWMOB;
    extern i16 id_MOONCAR;
    extern i16 id_MAPCAR;
    extern i16 id_PROBEDROID;
    extern i16 id_XWING;
    extern i16 id_YWING;
    extern i16 id_SNOWSPEEDER;
    extern i16 id_MILLENNIUMFALCON;
    extern i16 id_TIEFIGHTER;
    extern i16 id_TIEINTERCEPTOR;
    extern i16 id_TIEFIGHTERDARTH;
    extern i16 id_TIEBOMBER;
    extern i16 id_IMPERIALSHUTTLE;
    extern i16 id_SLAVE1;
    extern i16 id_WOOKIEFLYER;
    extern i16 id_CLONEARC;
    extern i16 id_VULTUREDROID;
    extern i16 id_DROIDTRIFIGHTER;
    extern i16 id_ANAKINSNEWPOD;
    extern i16 id_ANAKINSNEWPODGREEN;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_NEW_REPUBLIC_GUNSHIP_GREEN;
    extern i16 id_NABOOSTARFIGHTER;
    extern i16 id_NABOOSTARFIGHTERLIME;
    extern i16 id_ANAKINSSPEEDER;
    extern i16 id_ANAKINSSPEEDER_GREEN;
    extern i16 id_SEBULBASPOD;
    extern i16 id_SNOWTROOPER;
    extern i16 id_DEATHSTARTROOPER;
    extern i16 id_GONKDROID;
    extern i16 id_IMPERIALSPY;
    extern i16 id_DARTHVADER;
    extern i16 id_GREEDO;
    extern i16 id_BOSSK;
    extern i16 id_CANTINABAND;
    extern i16 id_UGNAUGHT;
    extern i16 id_SERVICECAR;
    extern i16 id_SPEEDERBIKE;
    extern i16 id_THEEMPEROR;
    extern LEVELDATA *TATOOINED_LDATA;
    extern LEVELDATA *DAGOBAHD_LDATA;
    extern LEVELDATA *DEATHSTARESCAPEB_LDATA;
    extern LEVELDATA *DEATHSTARESCAPEC_LDATA;
    extern LEVELDATA *CRUISERE_LDATA;
    extern LEVELDATA *DEATHSTARRESCUED_LDATA;
    extern LEVELDATA *DEATHSTARRESCUEA_LDATA;
    extern LEVELDATA *DEATHSTARESCAPEA_LDATA;
    extern LEVELDATA *KASHYYYKA_LDATA;
    extern LEVELDATA *SPEEDERCHASEA_LDATA;
    extern LEVELDATA *KAMINOE_LDATA;
    extern LEVELDATA *DEATHSTARRESCUEE_LDATA;
    extern LEVELDATA *SARLACCPITA_LDATA;
    extern LEVELDATA *SARLACCPITC_LDATA;
    extern LEVELDATA *ENDORBATTLEA_LDATA;
    extern LEVELDATA *ENDORBATTLEB_LDATA;
    extern LEVELDATA *ENDORBATTLEC_LDATA;
    extern LEVELDATA *ENDORBATTLED_LDATA;
    extern LEVELDATA *VADERA_LDATA;
    extern LEVELDATA *PODRACEA_LDATA;
    extern LEVELDATA *PODRACEOUTRO1_LDATA;
    extern LEVELDATA *HUB_LDATA;
    extern LEVELDATA *KAMINOA_LDATA;
    extern LEVELDATA *KAMINOC_LDATA;
    extern LEVELDATA *MOSEISLEYD_LDATA;
    extern LEVELDATA *DEATHSTARBATTLED_LDATA;
    extern LEVELDATA *ASTEROIDCHASEB_LDATA;
    extern LEVELDATA *SARLACCPITB_LDATA;
    extern LEVELDATA *CRUISERA_LDATA;
    extern LEVELDATA *CRUISERD_LDATA;
    extern LEVELDATA *CLOUDCITYESCAPEA_LDATA;
    extern LEVELDATA *PODSPRINTA_LDATA;
    extern LEVELDATA *PODRACEB_LDATA;
    extern AREADATA *GUNSHIP_ADATA;
    extern AREADATA *BONUS_GUNSHIP_ADATA;
    extern AREADATA *DOGFIGHT_ADATA;
    extern AREADATA *DEATHSTARESCAPE_ADATA;
    extern AREADATA *ASTEROIDCHASE_ADATA;
    extern AREADATA *JABBASPALACE_ADATA;
    extern AREADATA *SPEEDERCHASE_ADATA;
    extern AREADATA *DEATHSTARBATTLE2_ADATA;
    extern AREADATA *E1CHARACTER_ADATA;
    extern AREADATA *E2CHARACTER_ADATA;
    extern AREADATA *E3CHARACTER_ADATA;
    extern AREADATA *BONUSKAMINO_ADATA;
    extern AREADATA *BONUSDAGOBAH_ADATA;
    extern AREADATA *PODSPRINT_ADATA;
    extern i32 (*CanMagnetClimbFn)(GameObject_s *object);
    extern i32 (*CanPushObstaclesFn)(GameObject_s *object);
    extern i32 (*CanSuperCarryFn)(GameObject_s *object);
    extern i32 (*CanPushBlocksFn)(GameObject_s *object);
    extern i32 (*CanGlideFn)(GameObject_s *object);
    extern i32 (*UsingExtraActionsFn)(GameObject_s *object);
    extern i32 (*CanStartHoldFn)(GameObject_s *object);
    extern i32 (*IsWearingBackPackFn)(GameObject_s *object);
    extern i32 (*Fighting_WeaponInActionFn)(GameObject_s *object);
    extern i32 (*Fighting_WeaponOutActionFn)(GameObject_s *object);
    extern i32 (*FindSlamOrigin_UseCPosFn)(GameObject_s *object);
    extern i32 (*Tag_NoHiddenIconFn)(GameObject_s *object);
    extern i32 (*BoltInitSfxFn)(GameObject_s *object);
    extern void (*ExtraDieSfxFn)(GameObject_s *object);
    extern void (*ExtraHurtSfxFn)(GameObject_s *object);
    extern void (*BuckStartExtraFn)(GameObject_s *object);
    extern void (*SurfaceInfo_ExtraReflectFn)(GameObject_s *object);
    extern void (*BigJump_EndOfLandFn)(GameObject_s *object);
    extern void (*Jump_EndOfLandContextFn)(GameObject_s *object);
    extern i32 (*Jump_PreventJumpFn)(GameObject_s *object);
    extern i32 (*Slam_GetDebrisFn)(GameObject_s *object, i32 debris);
    extern void (*Punch_HitHoldFn)(GameObject_s *object, GameObject_s *target);
    extern void (*Punch_HitExtraCodeFn)(GameObject_s *object, NUVEC *hit_position);
    extern i32 (*Punch_GetDamageFn)(GameObject_s *object, GameObject_s *target);
    extern void (*DisguiseAdjustFn)(i32 character_id, i32 disguise_part, NUVEC *scale, NUVEC *offset);
    extern i32 (*SetSoundFadeDistCallBackFn)(WORLDINFO_s *world);
    extern void (*Door_GoThrough_ExtraCodeFn)(WORLDINFO_s *world, DOOR_s *door);
    extern void (*PauseGame_ExtraCodeFn)();
    extern void (*Game_AllGoldBricksFn)();
    extern void (*Game_100PercentFn)();
    extern f32 (*Hint_AlphaTargetFn)();
    extern i32 (*PauseRenderOffFn)();
    extern void (*CutScene_OverrideConfigFileNameFn)(char *name, i32 unknown_1, i32 unknown_2);
    extern bool (*CheckMusicOtherFn)();
    extern bool (*GameAudio_ActionMusicFn)();
    extern f32 (*GizmoPickups_Collide2DFn)(GameObject_s *object);
    extern i32 (*CutScenePlayer_AcceptFn)(CUTSCENEPLAYERCLIP *clip);
    extern GameObject_s *(*GameMsg_GetExtraObjFn)(GAMEMESSAGE_s *message);
    extern bool (*GamePads_IgnoreInputFn)();
    extern bool (*GizmoBlowUp_NoTargetFn)(WORLDINFO_s *world, GameObject_s *object);
    extern bool (*MatrixReflection_CanOverrideFn)();
    extern i32 (*GizBuildit_AutoBuildPosFn)(void *world, NUVEC *position, NUVEC *build_position, u16 *id);
    extern void (*GizmoBlowUp_SfxFn)(GIZMOBLOWUP_s *blow_up, NUVEC *position);
    extern bool (*GizBuildIt_CanStartBuildingFn)(GIZBUILDIT_s *build_it, GameObject_s *object);
    extern void (*GizBuildIt_FinishFn)(GIZBUILDIT_s *build_it);
    extern GIZBUILDIT_s *LevBuildIt;
    extern ADDGAMEMSG_s AddGameMsg_Default;
    extern u8 Cheat[];
    extern i16 LEGOACT_COMBOLAND;
    extern i32 LEGOCONTEXT_LAND_COMBOJUMP;
    extern i32 newgamecam;
    extern i32 VehicleArea;
    extern f32 COINMSGTIME;
    extern f32 DrawBuildUpTime;
    extern char **TTab;
    extern i32 tALLEXTRASUNLOCKED;
    extern f32 nusound_fade_start;
    extern f32 nusound_fade_end;
    extern f32 VehicleAreaRememberSpeed;
    extern i32 Lap;
    extern i32 FreePlay;
    extern LEVELDATA *NewLData;
    extern i32 waiting_for_new_level;
    extern i32 grab_screen_image;
    extern f32 minikittime;
    extern i32 ChallengeMode;
    extern i32 Arcade;
    extern i32 DoubleScore;
    extern i32 MiniCutCam;
    extern i32 CruiserD_LiftChase;
    extern i32 ai_fighting;
    extern CUSTOMISER *CharacterCustomiser;
}

static f32 Jump_PreventJumpHeight;

extern void (*Player_ClearContextFn)(GameObject_s *object, i32 context);
extern u8 *GameCam;
void ReleaseForce(GameObject_s *object, i32 immediate);
void ReleaseEat(GameObject_s *object);
void ReleaseBuildIt(GameObject_s *object, i32 complete);
void ReleasePush(GameObject_s *object);
void ReleaseTakeOver(GameObject_s *object, i32 immediate);
i32 GameAudio_GetSfxId(i32 index);
extern "C" i32 PlaySfx(const char *name, NUVEC *position);
void DoBuckStart(GameObject_s *object);
extern "C" void ResetAnimPacket(void *packet, i32 animation);
i32 Cheat_IsOn(i32 cheat);
bool Player_HasPurpleForce(GameObject_s *object);
void GameAudio_PlaySfx(i32 sfx_id, NUVEC *position, i32 unknown_2, i32 unknown_3);
struct GAMECAMERA_s;
void GameCam_NewShake(GAMECAMERA_s *camera, f32 x, f32 y, f32 time);
void GameCam_Judder(GAMECAMERA_s *camera, f32 strength, i32 unknown, NUVEC *position);
ATTRACTO *Attracto_FindNearest(WORLDINFO_s *world, NUVEC *position, GameObject_s *object, f32 *distance);
void Attracto_GetPos_Top(ATTRACTO *attracto, NUVEC *position);
extern "C" void AddGameDebris(void *system, i32 debris_id, NUVEC *position);
extern "C" u16 AnimMiscFlags(CHARACTERDATA *character_data, i32 action);
extern i8 HubBuildItsCompleted;
void AddFancyMessageRGB(char *text, f32 x, f32 y, f32 time, f32 scale, i32 center, u8 red, u8 green, u8 blue);
void BuyAllShopExtras();
void ReCalculateCompletionPoints();
void AddPartDebris(PARTDEBSYS_s *system, i32 debris_id, NUVEC *position);
void VaderA_GoneThroughDoor(WORLDINFO_s *world, DOOR_s *door);
void PodRace_IncreaseLap();
void CompleteLevel(WORLDINFO_s *world);
void Hub_ResetPanel();
i32 GetMenuID();
i32 Cheat_PowerUpActive(i32 cheat);
bool Hub_Outside();
i32 KaminoInside();
bool KaminoDiscoOn();
u8 DeathStarShieldDown();
bool SarlaccPitDiscoActive(WORLDINFO_s *world);
void Customiser_Set100PercentPieces(CUSTOMISER *customiser);
i32 Customiser_FindPieceByName(CUSTOMISER *customiser, char *name, i32 *category, i32 *piece);

static GameCharacterRuntime *GetGameCharacterRuntime(GameObject_s *object) {
    return (GameCharacterRuntime *)object->character_data->field11_0x24;
}

static i32 CanMagnetClimb_Game(GameObject_s *object) {
    CharacterEquipment *equipment = (CharacterEquipment *)object->player_packet.equipment;
    return equipment != NULL && ((equipment->flags >> 6) & 1) != 0;
}

static i32 CanPushObstacles_Game(GameObject_s *object) {
    return (GetGameCharacterRuntime(object)->ability_flags >> 23) & 1;
}

static i32 CanSuperCarry_Game(GameObject_s *object) {
    return (GetGameCharacterRuntime(object)->ability_flags >> 23) & 1;
}

static i32 CanPushBlocks_Game(GameObject_s *) {
    return 1;
}

static i32 CanGlide_Game(GameObject_s *object) {
    CharacterEquipment *equipment = (CharacterEquipment *)object->player_packet.equipment;
    return equipment != NULL && ((equipment->flags >> 1) & 1) != 0;
}

static i32 UsingExtraActions_Game(GameObject_s *object) {
    u8 *player_packet = (u8 *)&object->player_packet;
    return object->character_id == id_GRIEVOUS && ((player_packet[0x76e] & 1) == 0 || player_packet[0x77e] == 2);
}

static i32 CanStartHold_Game(GameObject_s *) {
    return 1;
}

static i32 IsWearingBackPack_Game(GameObject_s *object) {
    CharacterEquipment *equipment = (CharacterEquipment *)object->player_packet.equipment;
    return equipment != NULL && ((equipment->flags >> 8) & 1) != 0;
}

static i32 Fighting_WeaponInAction_Game(GameObject_s *object) {
    if (GetGameCharacterRuntime(object)->uses_weapon_action != 0 ||
        (*((u32 *)object->game_character_data + 1) & 0x80) == 0) {
        return 0x10;
    }
    return 0x7e;
}

static i32 Fighting_WeaponOutAction_Game(GameObject_s *object) {
    if (GetGameCharacterRuntime(object)->uses_weapon_action != 0 ||
        (*((u32 *)object->game_character_data + 1) & 0x80) == 0) {
        return 0x11;
    }
    return 0x7f;
}

static i32 LastSafePosExtra(GameObject_s *object) {
    if (WORLD->current_level == TATOOINED_LDATA && object->ground_surface_type == 0xe) {
        return 1;
    }
    if (WORLD->current_level == DAGOBAHD_LDATA && object->ground_surface_type == 0x18 && (i8)object->state_flags < 0 &&
        object->edge_surface_type == -1) {
        return 1;
    }
    return WORLD->current_level == DEATHSTARESCAPEC_LDATA && object->character_id == id_SERVICECAR;
}

void GameObjectDimensionsExtra_LSW(GameObject_s *object) {
    i16 action = *(i16 *)((u8 *)&object->player_packet + 0xe6);
    if (object->character_id == id_ATAT && object->player_packet.character_state == 0x17 && action == 0x41) {
        object->bounds_radius += object->bounds_radius;
    }
}

static i32 FindSlamOrigin_UseCPos(GameObject_s *object) {
    return object->character_id == id_GRIEVOUS;
}

static void Player_ClearContext_Game(GameObject_s *object, i32 release_takeover) {
    ReleaseForce(object, 0);
    ReleaseEat(object);
    ReleaseBuildIt(object, 0);
    ReleasePush(object);
    if (release_takeover != 0) {
        ReleaseTakeOver(object, 1);
    }
}

static i32 BoltInitSfx_LSW(GameObject_s *object) {
    if (object != NULL && object->character_id == id_GEONOSIAN && (((u8 *)object)[0xefd] & 2) == 0) {
        return GameAudio_GetSfxId(0x49);
    }
    return -1;
}

static void ExtraDieSfx_LSW(GameObject_s *object) {
    if (object->character_id == id_CHEWBACCA && Cheat[0x88] != 0) {
        PlaySfx("C3_Death", &object->origin);
    }
}

static void ExtraHurtSfx_LSW(GameObject_s *object) {
    if (object->character_id == id_CHEWBACCA && Cheat[0x88] != 0) {
        PlaySfx("C3_Death", &object->origin);
    }
}

void BuckStartExtra_LSW(GameObject_s *object) {
    DoBuckStart(object);
}

static i32 Tag_NoHiddenIcon(GameObject_s *) {
    return WorldInfo_CurrentlyActive()->current_level == DEATHSTARESCAPEB_LDATA && GameCam[1] == 5;
}

static void SurfaceInfo_ExtraReflect(GameObject_s *object) {
    LEVELDATA *level = WORLD->current_level;
    if (level == CRUISERE_LDATA && object->reflection_plane_y == 2000000.0f && object->position.x < 11.0f) {
        *((u8 *)object + 0x1087) = 3;
        object->reflection_plane_y = -39.2f;
        level = WORLD->current_level;
    }
    if (level == DEATHSTARRESCUED_LDATA) {
        if (object->position.z > 20.75f) {
            object->reflection_plane_y = 22.4f;
            *((u8 *)object + 0x1087) = 3;
        }
        if (object->position.x < -20.75f) {
            *((u8 *)object + 0x1087) = 1;
            object->reflection_plane_y = -22.4f;
        }
    }
}

static bool CanResetComboLand(GameObject_s *object) {
    i16 action = *(i16 *)((u8 *)&object->player_packet + 0xe6);
    u8 *player_packet = (u8 *)&object->player_packet;
    return object->character_id == id_THEEMPEROR && LEGOACT_COMBOLAND != -1 && action == LEGOACT_COMBOLAND &&
           ((player_packet[0x76e] & 1) == 0 || player_packet[0x77e] == 2);
}

static void BigJump_EndOfLand(GameObject_s *object) {
    if (CanResetComboLand(object)) {
        ResetAnimPacket((u8 *)object + 8, 0xffff);
    }
}

static void Jump_EndOfLandContext(GameObject_s *object) {
    if (object->player_packet.character_state == LEGOCONTEXT_LAND_COMBOJUMP && CanResetComboLand(object)) {
        ResetAnimPacket((u8 *)object + 8, 0xffff);
    }
}

static i32 Jump_PreventJump(GameObject_s *object) {
    f32 up_y = *(f32 *)((u8 *)object + 0xf34);
    return WORLD->current_level == KASHYYYKA_LDATA && object->floor_height != 2000000.0f &&
           up_y <= Jump_PreventJumpHeight && up_y != Jump_PreventJumpHeight;
}

static i32 Slam_GetDebris_Game(GameObject_s *object, i32 debris) {
    if (object->player_index != -1) {
        if (Cheat_IsOn(0x19) != 0) {
            return 1;
        }
        if (Player_HasPurpleForce(object)) {
            return 4;
        }
    }
    if (object->character_id == id_GRIEVOUS) {
        return (qrand() > 0x7fff) + 2;
    }
    return debris == -1 ? 2 : debris;
}

static void Punch_HitHold(GameObject_s *object, GameObject_s *target) {
    if (object != NULL && target != NULL &&
        (target->character_id == id_IMPERIALGUARD || target->character_id == id_GAMORREANGUARD)) {
        GameAudio_PlaySfx(0x4a, &target->origin, 0, 0);
    }
}

static void Punch_HitExtraCode_LSW(GameObject_s *object, NUVEC *hit_position) {
    if (object->character_id == id_GAMORREANGUARD &&
        (object->player_packet.character_state == 0xd ||
         (object->player_packet.character_state == 0x26 && object->player_packet.action == 0x56))) {
        NewRumbleAllPlayers(0.4f, 0.0f, 1, 0);
        NewRumble(((GAMEPAD_s *)object->player_packet.game_pad)->pad, 0.7f, 0);
        GameCam_Judder((GAMECAMERA_s *)GameCam, 0.25f, 0, &object->origin);
        PlaySfx("fs_gamorr_land", &object->bounds_bottom_center);
        return;
    }
    if ((AnimMiscFlags(object->character_data, object->player_packet.action) & 4) != 0) {
        PlaySfx(hit_position != NULL ? "WhipHit" : "WhipNowt", hit_position != NULL ? hit_position : &object->origin);
    }
}

static i32 Punch_GetDamage_LSW(GameObject_s *object, GameObject_s *target) {
    if (GetGameCharacterRuntime(object)->uses_weapon_action == 7 && Cheat_IsOn(0xc) != 0) {
        GameObject_s *sound_object = target != NULL ? target : object;
        GameAudio_PlaySfx(0x4a, &sound_object->origin, 0, 0);
        GameCam_NewShake(NULL, 0.75f, 0.75f, 1.0f);
        return -1;
    }
    if (target != NULL && (target->character_id == id_IMPERIALGUARD || target->character_id == id_GAMORREANGUARD) &&
        (target->player_packet.character_state == 0x18 || target->player_packet.character_state == 0xc)) {
        GameAudio_PlaySfx(0x4a, &target->origin, 0, 0);
    }
    return 1;
}

static f32 GizmoPickups_Collide2D(GameObject_s *object) {
    return WORLD->current_level == SPEEDERCHASEA_LDATA && object->character_id == id_SPEEDERBIKE ? 2.0f : 0.0f;
}

static bool MatrixReflection_CanOverride() {
    return WORLD->current_level != DEATHSTARESCAPEA_LDATA || GameCam[1] == 0 || GameCam[1] == 3;
}

static i32 CutScenePlayer_Accept(CUTSCENEPLAYERCLIP *) {
    return 1;
}

static bool GizmoBlowUp_NoTarget(WORLDINFO_s *world, GameObject_s *) {
    return world->current_level == DEATHSTARRESCUEA_LDATA && GameCam[1] == 2;
}

static GameObject_s *GameMsg_GetExtraObj(GAMEMESSAGE_s *message) {
    if (*(i16 *)((u8 *)message + 0xe6) == 0xd0) {
        return (GameObject_s *)(*(u8 **)((u8 *)WORLD + 0x2ac0) + 0xd10);
    }
    return NULL;
}

bool Game_IgnoreInput() {
    return newgamecam != 0;
}

static i32 GizBuildit_AutoBuildPos_Game(void *world, NUVEC *position, NUVEC *build_position, u16 *id) {
    WORLDINFO *world_info = world != NULL ? (WORLDINFO *)world : WorldInfo_CurrentlyActive();
    ATTRACTO *attracto = Attracto_FindNearest(world_info, position, NULL, NULL);
    if (attracto == NULL) {
        return 0;
    }
    Attracto_GetPos_Top(attracto, build_position);
    if (id != NULL) {
        *id = *(u16 *)((u8 *)attracto + 0x5c);
    }
    return 1;
}

static bool GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT_s *build_it, GameObject_s *) {
    if (WORLD->current_area != NULL && (WORLD->current_area->flags & AREAFLAG_HUB_AREA) != 0 && LevBuildIt == build_it) {
        return (u32)((u8 *)build_it)[0x74] * 2 <= Game.gold_bricks;
    }
    return true;
}

static void DisguiseAdjust_LSW(i32 character_id, i32 disguise_part, NUVEC *scale, NUVEC *offset) {
    *scale = v111;
    *offset = v000;

    f32 uniform_scale = 1.0f;
    if (character_id == id_TAUNTAUN) {
        uniform_scale = 2.2f;
    } else if (character_id == id_DEWBACK) {
        uniform_scale = 4.6f;
    } else if (character_id == id_BANTHA) {
        uniform_scale = 3.8f;
    } else if (character_id == id_ATST) {
        uniform_scale = 5.1f;
    } else if (character_id == id_SNOWMOB) {
        uniform_scale = 2.25f;
    } else if (character_id == id_MOONCAR || character_id == id_MAPCAR) {
        uniform_scale = 2.3f;
    } else if (character_id == id_ATAT || character_id == id_NABOOSTARFIGHTER ||
               character_id == id_NABOOSTARFIGHTERLIME) {
        uniform_scale = 7.0f;
    } else if (character_id == id_PROBEDROID) {
        uniform_scale = 3.0f;
    } else if (character_id == id_XWING || character_id == id_MILLENNIUMFALCON) {
        uniform_scale = 10.0f;
    } else if (character_id == id_YWING) {
        uniform_scale = 6.5f;
    } else if (character_id == id_SNOWSPEEDER) {
        uniform_scale = 4.0f;
    } else if (character_id == id_TIEFIGHTER || character_id == id_TIEINTERCEPTOR ||
               character_id == id_TIEFIGHTERDARTH || character_id == id_TIEBOMBER ||
               character_id == id_WOOKIEFLYER || character_id == id_VULTUREDROID ||
               character_id == id_DROIDTRIFIGHTER || character_id == id_ANAKINSNEWPOD ||
               character_id == id_ANAKINSNEWPODGREEN || character_id == id_NEW_REPUBLIC_GUNSHIP ||
               character_id == id_NEW_REPUBLIC_GUNSHIP_GREEN) {
        uniform_scale = 6.0f;
    } else if (character_id == id_IMPERIALSHUTTLE || character_id == id_SLAVE1 ||
               character_id == id_SEBULBASPOD) {
        uniform_scale = 8.0f;
    } else if (character_id == id_CLONEARC || character_id == id_ANAKINSSPEEDER ||
               character_id == id_ANAKINSSPEEDER_GREEN) {
        uniform_scale = 5.0f;
    }

    if ((u32)(disguise_part - 5) <= 1) {
        offset->z = -0.03f;
    } else if (character_id == id_SNOWTROOPER || character_id == id_DEATHSTARTROOPER) {
        offset->z = -0.01f;
    } else if (((u8 *)&GCDataList[character_id])[0x116] == 1) {
        offset->z = -0.03f;
    } else if (character_id == id_GONKDROID) {
        offset->z = -0.0075f;
    } else if (character_id == id_IMPERIALSPY || character_id == id_DARTHVADER || character_id == id_GREEDO ||
               character_id == id_BOSSK || character_id == id_CANTINABAND) {
        offset->z = -0.03f;
    } else if (character_id == id_IMPERIALGUARD) {
        offset->z = 0.015f;
    } else if (character_id == id_UGNAUGHT) {
        offset->z = -0.0225f;
    }

    if (uniform_scale != 1.0f) {
        scale->x = uniform_scale;
        scale->y = uniform_scale;
        scale->z = uniform_scale;
    }
}

static void GizBuildIt_FinishFn_Game(GIZBUILDIT_s *build_it) {
    if (WORLD->current_area == NULL || (WORLD->current_area->flags & AREAFLAG_HUB_AREA) == 0 || LevBuildIt != build_it ||
        HubBuildItsCompleted < 0) {
        return;
    }

    AddFancyMessageRGB(TTab[tALLEXTRASUNLOCKED], 0.0f, 0.0f, 0.6f, 1.25f, 1, 0xff, 0, 0);
    BuyAllShopExtras();
    ReCalculateCompletionPoints();
    AddPartDebris(*(PARTDEBSYS_s **)((u8 *)WORLD + 0x138), 0xf, &build_it->position);
}

static i32 SetSoundFadeDistCallBackFn_LSW(WORLDINFO_s *) {
    LEVELDATA *level = WORLD->current_level;
    AREADATA *area = WORLD->current_area;

    if (level == KAMINOE_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 22.5f;
    } else if (area != NULL && (area == GUNSHIP_ADATA || area == BONUS_GUNSHIP_ADATA)) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 125.0f;
    } else if (area != NULL && area == DOGFIGHT_ADATA) {
        nusound_fade_start = 100.0f;
        nusound_fade_end = 250.0f;
    } else if (level == DEATHSTARRESCUEE_LDATA) {
        nusound_fade_start = 5.0f;
        nusound_fade_end = 50.0f;
    } else if (area != NULL && area == DEATHSTARESCAPE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 15.0f;
    } else if (area != NULL && area == ASTEROIDCHASE_ADATA) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 150.0f;
    } else if (area != NULL && area == JABBASPALACE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITA_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 20.0f;
    } else if (level == SARLACCPITC_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 55.0f;
    } else if (area != NULL && area == SPEEDERCHASE_ADATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 50.0f;
    } else if (level == ENDORBATTLEA_LDATA) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 12.0f;
    } else if (level == ENDORBATTLEB_LDATA || level == ENDORBATTLEC_LDATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 45.0f;
    } else if (level == ENDORBATTLED_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 12.0f;
    } else if (area != NULL && area == DEATHSTARBATTLE2_ADATA) {
        nusound_fade_start = 50.0f;
        nusound_fade_end = 200.0f;
    } else if (area != NULL && (area == E1CHARACTER_ADATA || area == E2CHARACTER_ADATA ||
                                area == E3CHARACTER_ADATA || area == BONUSKAMINO_ADATA ||
                                area == BONUSDAGOBAH_ADATA)) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 40.0f;
    } else {
        return 0;
    }
    return 1;
}

static void GoThroughDoor_ExtraCode(WORLDINFO_s *world, DOOR_s *door) {
    VehicleAreaRememberSpeed = GetVehicleAreaRememberSpeed();

    if (world->current_level == VADERA_LDATA) {
        VaderA_GoneThroughDoor(world, door);
    }

    if (PODRACEB_LDATA == NULL || door->destination_level_index != PODRACEB_LDATA->idx ||
        world->current_level != PODRACEA_LDATA) {
        return;
    }

    if (Lap != 3) {
        PodRace_IncreaseLap();
    } else if (FreePlay != 0) {
        CompleteLevel(world);
    } else {
        NewLData = PODRACEOUTRO1_LDATA;
        grab_screen_image = 1;
        if (waiting_for_level != -1) {
            waiting_for_new_level = 1;
        }
    }
}

static void PauseGame_ExtraCode() {
    Hub_ResetPanel();
}

static void LEGO_AllGoldBricksFn() {}

static void LEGO_100PercentFn() {
    if (CharacterCustomiser == NULL) {
        return;
    }

    Customiser_Set100PercentPieces(CharacterCustomiser);
    i32 category[4];
    i32 piece[5];
    if (Customiser_FindPieceByName(CharacterCustomiser, "hat_hair_00", category, piece) != 0 && category[0] != -1 &&
        piece[0] != -1) {
        (*(i16 **)((u8 *)CharacterCustomiser + 0x174))[category[0]] = (i16)piece[0];
    }
}

static f32 Hint_AlphaTarget() {
    return minikittime > 0.0f && ChallengeMode == 0 ? 0.0f : 1.0f;
}

static i32 PauseRenderOff() {
    GetMenuID();
    return 0;
}

static void CutScene_OverrideConfigFileName_LSW(char *name, i32, i32) {
    static const char prefix[] = "episodei\\ep1_podrace_";
    if (PODSPRINT_ADATA == NULL || WorldInfo_CurrentlyActive()->current_area != PODSPRINT_ADATA ||
        NuStrIStr(name, (char *)prefix) != name) {
        return;
    }

    char *suffix = name + NuStrLen(prefix);
    if (NuStrICmp(suffix, "arrival1") == 0 || NuStrICmp(suffix, "arrival2") == 0 ||
        NuStrICmp(suffix, "arrival3") == 0 || NuStrICmp(suffix, "arrival4") == 0 ||
        NuStrICmp(suffix, "intro") == 0 || NuStrICmp(suffix, "tuskenraiders") == 0 ||
        NuStrICmp(suffix, "outro1") == 0 || NuStrICmp(suffix, "outro2") == 0) {
        NuStrCat(name, "_sprint");
    }
}

static bool CheckMusicOther() {
    NUVEC position;
    SOCKPOSITION_s sock_position;
    if (Players_AveragePos(&position, &sock_position) == 0) {
        return false;
    }

    LEVELDATA *level = WORLD->current_level;
    if (level == HUB_LDATA) {
        return Hub_Outside();
    }
    if (level == KAMINOA_LDATA) {
        return KaminoInside() != 0;
    }
    if (level == KAMINOC_LDATA) {
        return KaminoDiscoOn();
    }
    if (level == KAMINOE_LDATA) {
        return KaminoInside() == 0;
    }
    if (level == MOSEISLEYD_LDATA) {
        return sock_position.sock_index == 3;
    }
    if (level == DEATHSTARBATTLED_LDATA) {
        static i32 DEATHSTAR_HOLDCOUNT;
        if (DeathStarShieldDown() != 0) {
            DEATHSTAR_HOLDCOUNT = 30;
            return true;
        }
        if (DEATHSTAR_HOLDCOUNT > 0) {
            DEATHSTAR_HOLDCOUNT--;
            return true;
        }
    } else if (level == ASTEROIDCHASEB_LDATA) {
        return GameCam[1] == 4;
    } else if (level == SARLACCPITB_LDATA) {
        return SarlaccPitDiscoActive(WORLD);
    }
    return false;
}

static bool PlayerNeedsActionMusic(GameObject_s *object) {
    if (object == NULL) {
        return false;
    }
    if (*(void **)((u8 *)object + 0x3a4) != NULL) {
        return true;
    }
    GameObject_s *target = *(GameObject_s **)((u8 *)object + 0x394);
    return target != NULL && target->death_state == 0 && *(f32 *)((u8 *)object + 0x398) < 3.0f;
}

static bool ActionMusicFn() {
    LEVELDATA *level = WORLD->current_level;
    if (Arcade != 0 || DoubleScore != 0 || Cheat_PowerUpActive(-1) != 0 ||
        (level == CRUISERA_LDATA && MiniCutCam != 0) ||
        (level == CRUISERD_LDATA && CruiserD_LiftChase != 0) || level == DEATHSTARRESCUEE_LDATA) {
        return true;
    }

    if (level == MOSEISLEYD_LDATA) {
        if (CheckMusicOther()) {
            return true;
        }
        level = WORLD->current_level;
    }
    if (level == CLOUDCITYESCAPEA_LDATA) {
        return true;
    }
    if (level == SPEEDERCHASEA_LDATA) {
        NUVEC position;
        SOCKPOSITION_s sock_position;
        if (Players_AveragePos(&position, &sock_position) != 0) {
            if (sock_position.sock_index == 7 || sock_position.sock_index == 8) {
                return true;
            }
            if (sock_position.sock_index == 2 || sock_position.sock_index == 3 || sock_position.sock_index == 4 ||
                sock_position.sock_index == 6 || sock_position.sock_index == 9) {
                return false;
            }
        }
        level = WORLD->current_level;
    }
    if (level == HUB_LDATA) {
        return ai_fighting != 0;
    }
    return PlayerNeedsActionMusic(Player[0]) || PlayerNeedsActionMusic(Player[1]);
}

void GizmoBlowUp_Sfx(GIZMOBLOWUP_s *, NUVEC *position) {
    bool pod_sprint = WORLD->current_level == PODSPRINTA_LDATA && (u8)(GameCam[1] - 10) < 3 &&
                      *(f32 *)(GameCam + 0x2c) > 145.0f && *(f32 *)(GameCam + 0x2c) < 195.0f;
    bool pod_race = WORLD->current_level == PODRACEB_LDATA && GameCam[1] == 0 && *(f32 *)(GameCam + 0x2c) > 145.0f;
    if (pod_sprint || pod_race) {
        PlaySfx("PodX_PurpCrysHit", position);
    } else {
        GameAudio_PlaySfx(0x33, position, 0, 0);
    }
}

static void GameMsg_EndDelay_Game(GAMEMESSAGE_s *message) {
    u8 *raw = (u8 *)message;
    if (*(i32 *)(raw + 0xdc) == 0) {
        return;
    }
    if (VehicleArea != 0) {
        DrawBuildUpTime = COINMSGTIME + 1.0f;
        return;
    }
    if (raw[0xfe] != 1) {
        return;
    }
    AddGameDebris(*(void **)((u8 *)WORLD + 0x134), 0x38, (NUVEC *)(raw + 0x7c));
    u8 *player = *(u8 **)((u8 *)WORLD + 0x12c);
    if (player != NULL && (player[0x7b] & 1) != 0) {
        *(f32 *)(raw + 0x94) += (f32)qrand() * 1.5259022e-5f * 0.2f - 0.1f;
        *(f32 *)(raw + 0x98) += (f32)qrand() * 1.5259022e-5f * 0.2f - 0.1f;
        i32 random = qrand();
        raw[0xf9] = 0;
        *(f32 *)(raw + 0x9c) += (f32)random * 1.5259022e-5f * 0.2f - 0.1f;
    }
}

u16 MakeSaveHash(void) {
    return Game.completion;
}

i32 drawautosaveicon = 0;

void DrawAutoSaveIcon(void) {
    drawautosaveicon = 1;
    return;
}

char SuperOptions[24] = {0};

void InitGameBeforeConfig(void) {
    if (PAL == 0) {
        NuStrCpy(prodcode, "BASLUS-21409");
        FRAMETIME = 0.016666668;
    } else {
        NuStrCpy(prodcode, "BESLES-54221");
        FRAMETIME = 0.02;
    }
    DEFAULTFPS = 1.0 / FRAMETIME;
    DEFAULTFRAMETIME = 0.016666668;
    MAXFRAMETIME = 0.1;
    permbuffer_ptr = permbuffer_base;
    permbuffer_end = superbuffer_end;

    saveloadInit(&permbuffer_base, superbuffer_end, 0x7e58, prodcode, iconname, unicodename, 4);
    original_permbuffer_base = permbuffer_base;
    SaveSystemInitialise(3, (void *)MakeSaveHash, &Game, sizeof(GAMESAVE_s), 1, (void *)DrawAutoSaveIcon, SuperOptions,
                         sizeof(SuperOptions));
}

void InitGameAfterConfig(void) {
    AREADATA *pAVar5 = ADataList;

    POINTS_PER_CHARACTER = 1;
    POINTS_PER_SUPERBONUSCOMPLETE = 3;
    POINTS_PER_TIMETRIAL = 2;
    POINTS_PER_STORY = 6;
    POINTS_PER_CHALLENGE = 2;
    POINTS_PER_MINIKIT = 2;
    // TopShelf[6].field0_0x0[0] = '\0';
    // TopShelf[6].field0_0x0[1] = '\0';
    // TopShelf[6].field0_0x0[2] = '\0';
    // TopShelf[6].field0_0x0[3] = '\0';

    POINTS_PER_TRUEJEDI = 2;
    POINTS_PER_REDBRICK = 1;
    POINTS_PER_MISSION = 2;
    POINTS_PER_CHEAT = 1;
    POINTS_PER_GOLDBRICK = 1;
    BOTHTRUEJEDIGOLDBRICKS = 0;
    SHOPGOLDBRICKS = 8;
    GOLDBRICKFORSUPERBONUS = 1;
    GOLDBRICKFORSUPERSTORY = 0;
    GOLDBRICKFORCHALLENGE = 1;

    if (EPISODECOUNT > 0) {
        i32 p = (EPISODECOUNT - 1) * POINTS_PER_SUPERSTORY;
        CompletionPointInfo[0] = POINTS_PER_SUPERSTORY + CompletionPointInfo[0] + p;
        COMPLETIONPOINTS = p + POINTS_PER_SUPERSTORY + COMPLETIONPOINTS;
    }

    AREADATA *pAVar10 = ADataList;

    if (0 < AREACOUNT) {
        i32 areaIndex;
        i32 areaId = 0;
        AREADATA *area = ADataList;
        u16 areaFlags;
        do {
            i32 episode = Episode_ContainsArea(areaId, &areaIndex);
            area->episode_index = episode;
            area->area_index = (byte)areaIndex;
            if ((area != HUB_ADATA) && (areaFlags = area->flags, (areaFlags & 0x2022) == 0)) {
                if ((areaFlags & 0x100) == 0) {
                    if ((areaFlags & 4) != 0) {
                        COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_TIMETRIAL;
                        CompletionPointInfo[1] = CompletionPointInfo[1] + POINTS_PER_TIMETRIAL;
                        goto LAB_0012ca5f;
                    }
                    COMPLETIONPOINTS = POINTS_PER_STORY + COMPLETIONPOINTS;
                    CompletionPointInfo[1] = POINTS_PER_STORY + CompletionPointInfo[1];
                    if ((areaFlags & AREAFLAG_NO_GOLDBRICK) == 0) {
                        GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                    }

                    if ((areaFlags & 0x10) == 0) {
                        if ((areaFlags & 0x4000) != 0) {
                            COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_TRUEJEDI;
                            CompletionPointInfo[1] = POINTS_PER_TRUEJEDI + CompletionPointInfo[1];
                            GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                        }
                    } else {
                        i32 local_24 = COMPLETIONPOINTS + POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                        i32 local_28 = POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI + CompletionPointInfo[1];
                        i32 iVar4 = GOLDBRICKPOINTS + 2;
                        if (BOTHTRUEJEDIGOLDBRICKS != 0) {
                            local_24 = local_24 + POINTS_PER_TRUEJEDI;
                            local_28 = local_28 + POINTS_PER_TRUEJEDI;
                            iVar4 = GOLDBRICKPOINTS + 3;
                        }
                        GOLDBRICKPOINTS = iVar4;
                        COMPLETIONPOINTS = POINTS_PER_REDBRICK + local_24 + POINTS_PER_CHALLENGE;
                        CompletionPointInfo[1] = POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE + local_28;
                        if (GOLDBRICKFORCHALLENGE != 0) {
                            GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                        }
                    }
                } else {
                    COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_SUPERBONUSCOMPLETE;
                    CompletionPointInfo[1] = CompletionPointInfo[1] + POINTS_PER_SUPERBONUSCOMPLETE;
                    if (GOLDBRICKFORSUPERBONUS == 0)
                        goto LAB_0012ca68;
                LAB_0012ca5f:
                    GOLDBRICKPOINTS = GOLDBRICKPOINTS + 1;
                }
            LAB_0012ca68:
                if (area->cheat != 0xff) {
                    Cheat_SetArea((int)(char)area->cheat, areaId);
                }
            }
            areaId = areaId + 1;
            area = area + 1;
        } while (areaId < AREACOUNT);
    }

    // char cVar1;
    // char cVar2;
    // ushort uVar3;
    // int iVar8;

    // int local_28;
    // int local_24;
    // int local_20[3];

    // i16 uVar4 = tUNKNOWN;

    AREADATA *pAVar2 = ADataList;
    i32 iVar3 = AREACOUNT;
    i32 areaId = LEVELCOUNT;

    // i16 tab = tUNKNOWN;
    AREADATA *area;
    u8 episode;
    u8 bVar1;

    i32 areaIndex;

    if (0 < LEVELCOUNT) {
        i32 i = 0;
        LEVELDATA *level = LDataList;
        do {
            if (799 < level->unknown_060) {
                // level->field2_0x60 = tab;
            }
            level->unknown_0af = 0xff;
            level->unknown_0d4 = 0xff;
            if (iVar3 < 1) {
                areaIndex = 0;
            } else {
                areaIndex = 0;
                area = pAVar2;
                do {
                    if (area->field28_0x7d != 0) {
                        i32 iVar4 = 0;
                        do {
                            if (area->field2_0x60[iVar4] == i) {
                                level->unknown_0af = (byte)areaIndex;
                                level->unknown_0d4 = (byte)iVar4;
                                episode = (byte)areaIndex;
                            } else {
                                episode = level->unknown_0af;
                            }
                            if (episode != 0xff) {
                                areaIndex = areaIndex + 1;
                                bVar1 = pAVar2[(char)episode].episode_index;
                                level->unknown_0ae = bVar1;
                                if ((pAVar2[(char)episode].flags & 1) != 0) {
                                    level->unknown_0d9 = 0x32;
                                    level->unknown_0da = 0x14;
                                }
                                if (((bVar1 != 0xff) && ((pAVar2[(char)episode].flags & 0x106) == 0)) &&
                                    ((level->flags & (LEVEL_STATUS | LEVEL_OUTRO | LEVEL_MIDTRO | LEVEL_INTRO)) == 0)) {
                                    level->flags |= 0x2000;
                                }
                                goto LAB_0012cb40;
                            }
                            iVar4 = iVar4 + 1;
                        } while (iVar4 < (i32)(u32)area->field28_0x7d);
                    }
                    areaIndex = areaIndex + 1;
                    area = area + 1;
                } while (areaIndex != iVar3);
            }
        LAB_0012cb40:
            i = i + 1;
            level = level + 1;
        } while (i < areaId);
    }

    //  Level_RegisterGameConfigKeywords((nufpcomjmp_s *)LevelConfigKeywords_BeforeLoad,
    //  &LevelConfigKeywords_AfterLoad);
    //  Suits_Init();

    Collection_Configure("chars\\collection.txt", &permbuffer_ptr, &permbuffer_end);
    LOG_INFO("Loaded %d collection items", COLLECTION_COMPLETIONCOUNT);

    CompletionPointInfo[2] = CompletionPointInfo[2] + POINTS_PER_CHARACTER * COLLECTION_COMPLETIONCOUNT;
    COMPLETIONPOINTS = COMPLETIONPOINTS + POINTS_PER_CHARACTER * COLLECTION_COMPLETIONCOUNT;

    MissionSys = Missions_Configure("levels\\missions.txt", &permbuffer_ptr, &permbuffer_end, &Game.mission_save);
    if (MissionSys != NULL) {
        areaId = (u32)MissionSys->count * POINTS_PER_MISSION;
        COMPLETIONPOINTS = COMPLETIONPOINTS + areaId;
        CompletionPointInfo[3] = CompletionPointInfo[3] + areaId;
        GOLDBRICKPOINTS = GOLDBRICKPOINTS + (u32)MissionSys->count;
    }

    // Tag_DrawIconFn = Tag_DrawIcon_LSW;
    //_DAT_006312e8 = 0x5d;
    //_DAT_006312ea = 0x5e;
    //_DAT_0063138c = 0x5f;
    //_DAT_0063138e = 0x60;
    //_DAT_00631430 = 0x61;
    //_DAT_00631432 = 0x62;
    //  DEFAULT_PLAYERHITPOINTS = 8;
    //  PLAYERHITPOINTS_2HEARTSIN1 = 1;
    //  CutScenes_InitSystem((CUTSCENESYS *)&CutSceneSys_LSW);
    //  NuGCutDebFixUp_SearchAllPages = 1;
    //  NarrowSockExceptions_Init((NARROWSOCKEXCEPTION *)NarrowSockException_LSW);
    //  APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_LSW, "chars\\commonanims");
    //  SurfaceMaskOff(&TERRAINMASK_NONWEAPON);
    //  SurfaceMaskOn(&TERRAINMASK_NONDROID);
    //  Hub_UsePlayerList = 1;
    //  BoltSys_Init((BOLTSYS *)BoltSys_LSW);
    //  GameAudio_Init((GAMEAUDIO *)GameAudio_LSW);
    //  KITPOSX = 0;
    //  KITPOS2X = 0;
    //  CONVERTOLDPICKUPS = 1;
    //  GrabScreenWhenFading = 1;
    //  troopers_gdeb._0_4_ = 0x83;
    //  troopers_gdeb._4_4_ = 0x49;
    //  troopers_gdeb._8_4_ = 0x4a;
    //  troopers_gdeb._12_4_ = 0x4b;
    //  LEGOHINT_SHOOTCAMERAS = 0x266;
    //  LEGOHINT_PUSHBLOCKS = 0x267;
    //  LEGOHINT_BUILD = 0x25c;
    //  LEGOHINT_FREEPLAYTOGGLE = 600;
    //  PUNCHGAP = 0.3;
    //  PUNCHCHARGAP = 0.3;
    //  f64Jump_AlwaysReachJump2Height = 1;
    //  f64Jump_JediSlam = 1;
    //  CanPunchGirls = 0;
    ExtraHurtSfxFn = ExtraHurtSfx_LSW;
    ExtraDieSfxFn = ExtraDieSfx_LSW;
    BuckStartExtraFn = BuckStartExtra_LSW;
    BoltInitSfxFn = BoltInitSfx_LSW;
    //  REDBRICKPOSX = 0.0;
    //  REDBRICKPOSY = -0.5;
    //  REDBRICKPOS2X = 1.25;
    //  REDBRICKPOS2Y = 0.0;
    GameObjectDimensionsExtraFn = GameObjectDimensionsExtra_LSW;
    Punch_GetDamageFn = Punch_GetDamage_LSW;
    Punch_HitHoldFn = Punch_HitHold;
    Punch_HitExtraCodeFn = Punch_HitExtraCode_LSW;
    SetSoundFadeDistCallBackFn = SetSoundFadeDistCallBackFn_LSW;
    //  PlayerItemTypes_Init((PLAYERITEMTYPE_s *)PlayerItemType_INDY);
    DisguiseAdjustFn = DisguiseAdjust_LSW;
    //  SUPERCARRY_THROWSPEED_XZ = 0x40000000;
    //  SUPERCARRY_THROWSPEED_Y = 0x3fc00000;
    //  SUPERCARRY_RELEASESPEED_XZ = 0x3f800000;
    //  SuperCarry_UseActionButton = 1;
    CutScene_OverrideConfigFileNameFn = CutScene_OverrideConfigFileName_LSW;
    //  CharPivot_Init((CHARPIVOT *)CharPivot_LSW);
    //  DIEAIRSPEED = 0x40600000;
    //  DIEAIRJUMPSPEED = 0x40200000;
    //  ForcePush_Waft = 1;
    //  ForcePush_SuperPush = 1;
    //  ForcePush_SuperMindTrick = 1;
    //  Lighting_HighlightFlash = 1;
    //  Lighting_BlueFlickerFn = ObjZappedBlue;
    AddGameMsg_Default.end_delay = GameMsg_EndDelay_Game;
    //  WorldInfo_InitMenuFn = Game_WorldInfo_InitMenu;
    //  WorldInfo_InitLastFn = Game_WorldInfo_InitLast;
    //  TerSurface._152_2_ = 0x15;
    //  TerSurface._164_2_ = 0x16;
    //  LEGOACT_IDLE = 1;
    //  LEGOACT_WALK = 0;
    //  LEGOACT_JUMP = 6;
    //  LEGOACT_LAND = 7;
    //  LEGOACT_JUMP2 = 9;
    //  LEGOACT_JUMP3 = 0xe;
    //  LEGOACT_LAND2 = 10;
    //  LEGOACT_LAND3 = 0x15;
    //  LEGOACT_COMBOJUMP = 0x12;
    //  LEGOACT_COMBOLAND = 0x13;
    //  LEGOACT_FALL = 5;
    //  LEGOACT_FALLLAND = 0x59;
    //  LEGOACT_BACKPACKFALLLAND = 0xb3;
    //  LEGOACT_DEACTIVATED = 0x41;
    //  LEGOACT_PUNCH_BEHIND = 0x94;
    //  LEGOACT_SHOOTRIGHT = 0x5b;
    //  LEGOACT_SHOOTLEFT = 0x5a;
    //  LEGOACT_SHOOTBACK = 0x5c;
    //  LEGOACT_LEDGE_GRAB = 0xda;
    //  LEGOACT_LEDGE_IDLE = 0x9d;
    //  LEGOACT_LEDGE_LEFT = 0x9e;
    //  LEGOACT_LEDGE_RIGHT = 0x9f;
    //  LEGOACT_HANG_IDLE = 0x91;
    //  LEGOACT_HANG_MOVE = 0x92;
    //  LEGOACT_CLIMB_IDLE = 0x87;
    //  LEGOACT_CLIMB_UP = 0xcd;
    //  LEGOACT_CLIMB_DOWN = 0xce;
    //  LEGOACT_CLIMB_LEFT = 0xcf;
    //  LEGOACT_CLIMB_RIGHT = 0xd0;
    //  LEGOACT_WALLSHUFFLE_IDLE = 0x8a;
    //  LEGOACT_WALLSHUFFLE_LEFT = 0x8b;
    //  LEGOACT_WALLSHUFFLE_RIGHT = 0x8c;
    //  LEGOACT_PUSH = 0x52;
    //  LEGOACT_SUPERPUSH_IDLE = 0xae;
    //  LEGOACT_SUPERPUSH_PUSH = 0xaf;
    //  LEGOACT_SUPERPUSH_PULL = 0xb0;
    //  LEGOACT_BUILD = 0x5f;
    //  LEGOACT_FLIP = 0xc;
    //  LEGOACT_FLIPLAND = 0xd;
    //  LEGOACT_BACKFLIP = 0x77;
    //  LEGOACT_WALLJUMP_WAIT = 0xa1;
    //  LEGOACT_GRAPPLE_IDLE = 0xba;
    //  LEGOACT_GRAPPLE_UP = 0xbb;
    //  LEGOACT_GRAPPLE_DOWN = 0xbc;
    //  LEGOACT_GRAPPLE_HANG = 0xbd;
    //  LEGOACT_GLIDE = 0x93;
    //  LEGOACT_SUPERCARRY_PICKUP = 0xa3;
    //  LEGOACT_SUPERCARRY_IDLE = 0xa4;
    //  LEGOACT_SUPERCARRY_WALK = 0xa5;
    //  LEGOACT_SUPERCARRY_THROW = 0xa6;
    //  LEGOACT_SUPERCARRY_PUTDOWN = 0xd5;
    //  LEGOACT_SUPERCARRY_BASH = 0xd6;
    //  LEGOACT_SUPERCARRY_JUMP = 0xd7;
    //  LEGOACT_SUPERCARRY_LAND = 0xd8;
    //  LEGOACT_SUPERCARRY_FALLLAND = 0xd9;
    //  LEGOACT_MAGNET_WALK_METAL = 0xc5;
    //  LEGOACT_MAGNET_TIPTOE = 0xc6;
    //  LEGOACT_MAGNET_WALK = 199;
    //  LEGOACT_MAGNET_RUN = 200;
    //  LEGOACT_MAGNET_JUMP = 0xc9;
    //  LEGOACT_MAGNET_LAND = 0xca;
    //  LEGOACT_LUNGE = 0x1f;
    //  LEGOACT_LUNGELAND = 0x20;
    //  LEGOACT_SLAM = 0x21;
    //  LEGOACT_SLAMLAND = 0x22;
    //  LEGOACT_COMBATROLL_JUMP = 0xb4;
    //  LEGOACT_COMBATROLL_FALL = 0xb5;
    //  LEGOACT_COMBATROLL_LAND = 0xb6;
    //  LEGOACT_COMBATROLL_FIRE = 0x57;
    //  LEGOACT_EXTRA_JUMP = 0x78;
    //  LEGOACT_EXTRA_JUMP2 = 0x79;
    //  LEGOACT_EXTRA_LAND = 0x7a;
    //  LEGOACT_EXTRA_LAND2 = 0x7b;
    //  LEGOACT_BUCK = 0x2a;
    //  LEGOACT_TEETER = 0xdb;
    //  LEGOACT_WHIP_START = 0xdc;
    //  LEGOACT_WHIP_CRACK = 0xdd;
    //  LEGOACT_WHIP_GRAB = 0xdd;
    //  LEGOACT_WHIP_BREAK = 0xdd;
    //  LEGOACT_WHIP_SWING_START = 0xe0;
    //  LEGOACT_WHIP_SWING_SWING = 0xe1;
    //  LEGOACT_WHIP_SWING_JUMP = 0xe2;
    //  LEGOCONTEXT_TUBE = 0x11;
    //  LEGOCONTEXT_GLIDE = 0x4f;
    //  LEGOCONTEXT_BLOCK = 0xc;
    //  LEGOCONTEXT_HOLD = 0x18;
    //  LEGOCONTEXT_DROPIN = 0x23;
    //  LEGOCONTEXT_DOOMED = 0x2b;
    //  LEGOCONTEXT_LEDGETERRAIN = 0x55;
    //  LEGOCONTEXT_CLIMB = 0x43;
    //  LEGOCONTEXT_HANG = 0x4e;
    //  LEGOCONTEXT_JUMP = 0;
    //  LEGOCONTEXT_BIGJUMP = 0x1f;
    //  LEGOCONTEXT_WALLSHUFFLE = 0x45;
    //  LEGOCONTEXT_COMBO = 5;
    //  LEGOCONTEXT_PUNCH = 0x26;
    //  LEGOCONTEXT_PUSH = 0x27;
    //  LEGOCONTEXT_PUSHSPINNER = 0x28;
    //  LEGOCONTEXT_PUSHOBSTACLE = 0x59;
    //  LEGOCONTEXT_BEENTAKENOVER = 0x3b;
    //  LEGOCONTEXT_GETIN = 0x3c;
    //  LEGOCONTEXT_BUILDIT = 0x2d;
    //  LEGOCONTEXT_WEAPONIN = 6;
    //  LEGOCONTEXT_WEAPONOUT = 7;
    //  LEGOCONTEXT_BACKFLIP = 0x20;
    //  LEGOCONTEXT_WALLJUMPWAIT = 0x57;
    //  LEGOCONTEXT_GRAPPLE = 0x46;
    //  LEGOCONTEXT_SUPERCARRY = 0x58;
    //  LEGOCONTEXT_LAND_JUMP = 1;
    //  LEGOCONTEXT_LAND_JUMP2 = 2;
    //  LEGOCONTEXT_LAND_FLIP = 3;
    //  LEGOCONTEXT_LAND_COMBOJUMP = 4;
    //  LEGOCONTEXT_LAND_LUNGE = 0xd;
    //  LEGOCONTEXT_LAND_SLAM = 0xe;
    //  LEGOCONTEXT_LAND_SPECIAL = 0x19;
    //  LEGOCONTEXT_LAND_COMBATROLL = 0x29;
    //  LEGOCONTEXT_EATEN = 0x39;
    //  LEGOCONTEXT_SPECIALMOVE_ATTACKER = 0x26;
    //  LEGOCONTEXT_SPECIALMOVE_VICTIM = 0x30;
    //  LEGOCONTEXT_BUCK = 0x3e;
    //  LEGOCONTEXT_WHIP = 0x62;
    //  LEGOCONTEXT_NETWAIT = 99;
    //  LEGOMENU_TITLES = 0;
    //  LEGOMENU_NEWGAME = 1;
    //  LEGOMENU_PAUSEMAIN = 2;
    //  LEGOMENU_PAUSECUT = 0x1b;
    //  LEGOMENU_CREDITS = 0x1e;
    //  LEGOMENU_STORE_RESTORING = 0x16;
    //  LEGOMENU_STORE_PURCHASE = 0x17;
    //  LEGOMENU_RESTORE_NEWGAME = 0x1d;
    //  LEGOCAMMODE_DOORCUT = 4;
    //  LEGOCAMMODE_OBSTACLE = 2;
    //  LEGOOBJ_ICON_FRAME_NEUTRAL = 0xa7;
    //  LEGOOBJ_ICON_FRAME_GREEN = 0xa5;
    //  LEGOOBJ_ICON_FRAME_BLUE = 0xa6;
    //  LEGOOBJ_ICON_QUESTION = 0xa1;
    //  LEGOOBJ_ICON_WEIRDO = 0xa3;
    //  LEGOOBJ_CHARKIT = 0xcf;
    //  LEGOOBJ_MINIKIT = 0xce;
    //  LEGOSPL_SPLIT = 5;
    //  LEGOSPL_START = 0;
    //  LEGOGDEB_SPLASH = 0xd;
    //  GizBuilditGDeb._0_2_ = 0x4c;
    //  GizBuilditGDeb._2_2_ = 0x4d;
    //  GizBuilditGDeb._4_2_ = 0x4e;
    //  GizBuilditGDeb._6_2_ = 0x4f;
    //  GizBuilditGDeb._8_2_ = 0x50;
    //  GizBuilditGDeb._10_2_ = 0x51;
    //  GizSpinnerGDeb_Fail._0_2_ = 0x57;
    //  GizSpinnerGDeb_Fail._2_2_ = 1;
    //  GizSpinnerGDeb_Fail._4_2_ = 0x58;
    GamePads_IgnoreInputFn = Game_IgnoreInput;
    Door_GoThrough_ExtraCodeFn = GoThroughDoor_ExtraCode;
    //  GizmoBlowup_TransformDrawFn = GizmoBlowup_TransformDraw_Game;
    //  LEGOASCII_UP = ASCII_UP;
    //  LEGOASCII_DOWN = ASCII_DOWN;
    //  LEGOASCII_LEFT = ASCII_LEFT;
    //  LEGOASCII_RIGHT = ASCII_RIGHT;
    //  LEGOASCII_BIGARROW = ASCII_BIGARROW;
    //  KEEPONSCREEN_SIDESONLY = 1;
    CutScenePlayer_AcceptFn = CutScenePlayer_Accept;
    //  CutScenePlayer_Configure("cut\\clips.txt", &permbuffer_ptr, &permbuffer_end, &tCLIPi, &tINTRO, &tMIDTRO,
    //  &tOUTRO,
    //                           &tENDING);
    CanMagnetClimbFn = CanMagnetClimb_Game;
    CanPushObstaclesFn = CanPushObstacles_Game;
    CanSuperCarryFn = CanSuperCarry_Game;
    CanPushBlocksFn = CanPushBlocks_Game;
    CanGlideFn = CanGlide_Game;
    UsingExtraActionsFn = UsingExtraActions_Game;
    CanStartHoldFn = CanStartHold_Game;
    Player_ClearContextFn = Player_ClearContext_Game;
    //  LEGOTHINGSSCENE_TER_SPINBASE = 0;
    //  LEGOTHINGSSCENE_TER_SPINARM = 1;
    GizBuildit_AutoBuildPosFn = GizBuildit_AutoBuildPos_Game;
    Fighting_WeaponInActionFn = Fighting_WeaponInAction_Game;
    Fighting_WeaponOutActionFn = Fighting_WeaponOutAction_Game;
    //  LEGOOBJ_FLOORTARGET = 0x55;
    //  LEGOOBJ_GRAPPLE_HOOK = 0x3d;
    Slam_GetDebrisFn = Slam_GetDebris_Game;
    IsWearingBackPackFn = IsWearingBackPack_Game;
    //  Grass_Available = 1;
    PauseGame_ExtraCodeFn = PauseGame_ExtraCode;
    //  Hub_PanelBusyFn = Hub_PanelBusy;
    CheckMusicOtherFn = CheckMusicOther;
    GizBuildIt_CanStartBuildingFn = GizBuildIt_CanStartBuildingFn_Game;
    GizBuildIt_FinishFn = GizBuildIt_FinishFn_Game;
    //  POWERUP_TEXTID = (int)tPOWERUP;
    //  BackDrop_AlphaFn = BackDrop_Alpha;
    Game_100PercentFn = LEGO_100PercentFn;
    Game_AllGoldBricksFn = LEGO_AllGoldBricksFn;
    LastSafePosExtraFn = LastSafePosExtra;
    GameAudio_ActionMusicFn = ActionMusicFn;
    PauseRenderOffFn = PauseRenderOff;
    FindSlamOrigin_UseCPosFn = FindSlamOrigin_UseCPos;
    GizmoBlowUp_NoTargetFn = GizmoBlowUp_NoTarget;
    MatrixReflection_CanOverrideFn = MatrixReflection_CanOverride;
    Jump_PreventJumpFn = Jump_PreventJump;
    SurfaceInfo_ExtraReflectFn = SurfaceInfo_ExtraReflect;
    Hint_AlphaTargetFn = Hint_AlphaTarget;
    //  Arcade_TextCrawlID = 0x1f1;
    //  Arcade_TextCrawlParagraphs = 2;
    GizmoPickups_Collide2DFn = GizmoPickups_Collide2D;
    //  LEGOOBJ_DEFAULTLASTCOIN = 0xb7;
    Tag_NoHiddenIconFn = Tag_NoHiddenIcon;
    //  Collection_GetSelectingPlayerIDsFn = Collection_GetSelectingPlayerIDs;
    GizmoBlowUp_SfxFn = GizmoBlowUp_Sfx;
    //  APIObjResetShadowMapRenderingFn = ResetShadowMapRenderingFn;
    //  APIObjEnableShadowMapRenderingFn = EnableShadowMapRenderingFn;
    GameMsg_GetExtraObjFn = GameMsg_GetExtraObj;
    Jump_EndOfLandContextFn = Jump_EndOfLandContext;
    BigJump_EndOfLandFn = BigJump_EndOfLand;

    COMPLETIONPOINTS = POINTS_PER_CHEAT * 0x2c + COMPLETIONPOINTS;
    CompletionPointInfo[4] = POINTS_PER_CHEAT * 0x2c + CompletionPointInfo[4];
    if (0 < SHOPGOLDBRICKS) {
        areaId = (SHOPGOLDBRICKS + -1) * POINTS_PER_GOLDBRICK;
        CompletionPointInfo[6] = CompletionPointInfo[6] + POINTS_PER_GOLDBRICK + areaId;
        COMPLETIONPOINTS = POINTS_PER_GOLDBRICK + COMPLETIONPOINTS + areaId;
        GOLDBRICKPOINTS = SHOPGOLDBRICKS + -1 + GOLDBRICKPOINTS + 1;
    }

    //  iVar9 = CutScenePlayer_Available();
    //  CutScenePlayCount = 0;
    //  if (iVar9 != 0) {
    //      CutScenePlayCount = (u32) * (ushort *)(iVar9 + 8);
    //  }
    //  if (g_lowEndLevelBehaviour != 0) {
    //      Reflections_On = 0;
    //      CharClipToBlobShadows = 1;
    //  }
}

void CompleteLevel(WORLDINFO *world) {
    grab_screen_image = 1;

    if (NewLData == NULL || ((NewLData->flags & 0xe0) != 0 && FreePlay != 0)) {
        LEVELDATA *statusLevel = Area_FindStatusLevel(world->area, NULL);
        NewLData = statusLevel;
        if (statusLevel == NULL) {
            NewLData = HUB_LDATA;
        }
    }

    if (waiting_for_level != -1) {
        waiting_for_new_level = 1;
    }
}

void StoreLevelProgress(WORLDINFO *world) {
    if (world != NULL) {
        // StoreLevelProgressFn(world, world->field277_0x130, 0);
    }
}

void NewAreaMusicChanges(void) {
    BeenAttacked = 0;
}

void Game_WorldInfo_InitLast(WORLDINFO *world) {
    // GizSpinners_InitTerrain(world);
    // Signals_InitTerrain(world);
    // Attractos_InitTerrain(world);
    // SecurityDoors_InitTerrain(world);
    // Levers_InitTerrain(world);
    // GizPanel_InitTerrain(world);
    // HatMachines_InitTerrain(world);
    // GizmoBlowupsFinalSetup(world);
    // InitClimbObjectSys(world);
    // GizmoPushBlockInitAndReset(world, NULL);
}
