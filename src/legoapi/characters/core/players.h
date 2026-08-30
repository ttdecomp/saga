#pragma once

#include "decomp.h"
#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/props/doors/door.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nuvec.h"

// Per-player saved progress.  Stride is 0x10 in the original binary.
typedef struct playerprogress_s {
    u32 coins;     // 0x00
    u16 field_0x4; // 0x04
    u8 active;     // 0x06
    u8 field_0x7;  // 0x07
    u8 hitpoints;  // 0x08
    u8 field_0x9;  // 0x09
    u8 field_0xa;  // 0x0a
    u8 field_0xb;  // 0x0b
    f32 field_0xc; // 0x0c
} PLAYERPROGRESS;

// Runtime model slot owned by the character system. Each of the three model
// tables contains model_id_capacity pointers.
typedef CHARACTERMODEL_s APICHARACTERMODEL;

typedef void (*APICHARACTERLIGHTFN)(APIOBJECT_s *object);
void SetCreatureLights(APIOBJECT_s *object);

// Character-system arena header. The pointer fields deliberately describe
// the original allocations instead of baking their offsets into users.
typedef struct apicharactersys_s {
    i32 character_count;                     // 0x00
    i32 model_capacity;                      // 0x04
    i32 model_id_capacity;                   // 0x08
    i32 permanent_model_count;               // 0x0c
    i32 loaded_model_count;                  // 0x10
    i32 animation_capacity;                  // 0x14
    APICHARACTERMODEL *models;               // 0x18
    i16 *playermodelids;                     // 0x1c
    CHARACTERDATA *char_data;                // 0x20
    ANIMLIST_s *animations;                  // 0x24
    i32 area_animation_count;                // 0x28
    i32 loaded_animation_count;              // 0x2c
    i32 animation_load_attempts;             // 0x30
    APICHARACTERLIGHTFN set_creature_lights; // 0x34
} APICHARACTERSYS;

DECOMP_ASSERT(sizeof(APICHARACTERMODEL) == 0x54, "APICHARACTERMODEL size");
DECOMP_ASSERT(sizeof(APICHARACTERSYS) == 0x38, "APICHARACTERSYS size");

// Player start spawn entry.  Stride 0x10 in the original.
typedef struct playerstart_s {
    NUVEC *pos;                 // 0x00
    SOCKLOCATION sock_location; // 0x04
    f32 sock_ratio;             // 0x08
    i16 angle;                  // 0x0c
} PLAYERSTARTENTRY;

extern PLAYERSTARTENTRY PlayerStart[8];

// ---- Globals used by Players_Init ----

extern GameObject_s *Player[8];
extern i32 PLAYERCOUNT;
extern i32 netclient;
extern i32 UsePlayerList;
extern i32 makeplayerlist_freeplay;
extern i16 PlayerList[8];
extern i32 PlayerID[2];
extern i32 GAMEDEMO;
extern i16 Area_PlayerIDList[9];
extern APICHARACTERMODELLIST_s Area_StoryModelList[52];
extern PLAYERPROGRESS PlayerProgress[8];
extern i32 Hub_UsePlayerList;
extern i32 LevelChangesInArea;
extern i32 bonusmodearcade;
extern i32 VehicleArea;
extern SOCKPOSITION OldPlrSPos[8];
extern char Batarang[8 * 0xb4];
extern void *PlayerSuit[8];
extern u8 PlayerTorpedoCount[8];
extern COINPACKET CoinPacket[2];
extern u32 BackUpPlayers[];
extern char GizForceLOSInfo[0xc60];
extern i32 DEFAULT_PLAYERHITPOINTS;
extern u32 LEGOOBJ_DEFAULTLASTCOIN;

extern APICHARACTERSYS *apicharsys;

i32 PlayersDropInOut();

extern "C" APICHARACTERMODEL *APICharacterLoaded(i32 character_id);
extern "C" void APICharacterModelReset(APICHARACTERMODEL *model);
extern "C" void APILoadCharacterModels(APICHARACTERMODELLIST_s *list, i32 append, VARIPTR *buf, VARIPTR buf_end,
                                       i32 area_models);
extern "C" void APICharacterSysInit(VARIPTR *buf, VARIPTR buf_end, i32 char_count, i32 model_capacity,
                                    i32 model_id_capacity, i32 extra_capacity, CHARACTERDATA *cdata_list,
                                    APICHARACTERLIGHTFN set_creature_lights);

// ---- Helper functions called by Players_Init ----

GameObject_s *AddCreature(i32 id, i32 param);
GameObject_s *AddGameObject(i32 id);
i32 InitCreature(GameObject_s *obj, i32 id, i32 param);
void PreResetCode(GameObject_s *obj);
void PostResetCode(GameObject_s *obj);
NUVEC *Player_StartPos(GameObject_s *obj);
i32 Players_BothActive(void);
TORPEDOPACKET *GetTorpedoPacket(void);
void SetHitPoints(GameObject_s *obj, i32 hp);
void RememberPlayerIDs(i32 a, i32 b, i32 c);

struct WORLDINFO_s;
void Players_InitPositions(struct WORLDINFO_s *world);
void PlayerItemTypes_Reset(struct WORLDINFO_s *world);

void Players_Init(void);
i32 MakePlayerList(i32 count);

// ---- Player/gameobject helpers (defined in players.cpp) ----

void DrawOffsetCode(GameObject_s *obj, i32 param);
extern "C" void rtlDynamicEnable(i32 id, i32 param);
float GameObjectNearFloor(GameObject_s *obj, float h, float *out);
float GetHoverPosY(GameObject_s *obj);
i32 Player_HasPurpleForce(GameObject_s *obj);
extern u32 GAMEPAD_ACTION;
extern u32 GAMEPAD_DDOWN;
extern u32 GAMEPAD_DLEFT;
extern u32 GAMEPAD_DRIGHT;
extern u32 GAMEPAD_DUP;
extern u32 GAMEPAD_JUMP;
extern u32 GAMEPAD_SPECIAL;
extern u32 GAMEPAD_TAG;
extern u32 GAMEPAD_START;
void ChatterSfx(GameObject_s *g, i32 a, float b);
void Move_VEHICLE(GameObject_s *g);
extern "C" void ComplexSockPosition(SOCKSYS *sock_sys, NUVEC *position, i32 prior_sock, i32 prior_segment,
                                    SOCKPOSITION *result);
void *CutScenePlayer_Available(void);

void KillPlayer(GameObject_s *player, i32 cause, i32 unknown, struct nuvec_s *);
