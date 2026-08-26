#pragma once

#include "decomp.h"
#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/props/doors/door.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nuvec.h"

// Per-player saved progress.  Stride is 0x10 in the original binary.
typedef struct playerprogress_s {
    u32 coins;     // 0x00
    u16 field_0x4; // 0x04
    u8 field_0x6;  // 0x06
    u8 field_0x7;  // 0x07
    u8 hitpoints;  // 0x08
    u8 field_0x9;  // 0x09
    u8 field_0xa;  // 0x0a
    u8 field_0xb;  // 0x0b
    f32 field_0xc; // 0x0c
} PLAYERPROGRESS;

// Character system (apicharsys).  field7_0x1c is a player model id list,
// field8_0x20 is a CHARACTERDATA array (stride 0x4c).
typedef struct playerdata_s {
    undefined field0_0x0[0x18];
    void *field_0x18;           // 0x18  character data array (stride 0x54)
    i16 *playermodelids;        // field7_0x1c (pointer to player model id list)
    CHARACTERDATA char_data[8]; // field8_0x20
} PLAYERDATA;

// Player start spawn entry.  Stride 0x10 in the original.
typedef struct playerstart_s {
    NUVEC *pos; // 0x00
    f32 u4;     // 0x04
    f32 u8;     // 0x08
    i16 angle;  // 0x0c
} PLAYERSTARTENTRY;

extern PLAYERSTARTENTRY PlayerStart[8];

// ---- Globals used by Players_Init ----

extern GameObject_s *Player[8];
extern i32 PLAYERCOUNT;
extern i32 netclient;
extern i32 UsePlayerList;
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

extern PLAYERDATA *apicharsys;

// ---- Helper functions called by Players_Init ----

GameObject_s *AddCreature(i32 id, i32 param);
GameObject_s *AddGameObject(i32 id);
i32 InitCreature(GameObject_s *obj, i32 id, i32 param);
void PreResetCode(GameObject_s *obj);
void PostResetCode(GameObject_s *obj);
TORPEDOPACKET *GetTorpedoPacket(void);
void SetHitPoints(GameObject_s *obj, i32 hp);
void RememberPlayerIDs(i32 a, i32 b, i32 c);

struct WORLDINFO_s;
void Players_InitPositions(struct WORLDINFO_s *world);
void PlayerItemTypes_Reset(struct WORLDINFO_s *world);

void Players_Init(void);

// ---- Player/gameobject helpers (defined in players.cpp) ----

void DrawOffsetCode(GameObject_s *obj, i32 param);
extern "C" void rtlDynamicEnable(i32 id, i32 param);
float GameObjectNearFloor(GameObject_s *obj, float h, float *out);
float GetHoverPosY(GameObject_s *obj);
i32 Player_HasPurpleForce(GameObject_s *obj);
extern u32 GAMEPAD_ACTION;
extern u32 GAMEPAD_JUMP;
extern u32 GAMEPAD_SPECIAL;
extern u32 GAMEPAD_TAG;
extern u32 GAMEPAD_START;
void ChatterSfx(GameObject_s *g, i32 a, float b);
void Move_VEHICLE(GameObject_s *g);
extern "C" void ComplexSockPosition(void *a, void *b, i32 c, i32 d, SOCKPOSITION *out);
void *CutScenePlayer_Available(void);

void KillPlayer(GameObject_s *player, i32 cause, i32 unknown, struct nuvec_s *);
