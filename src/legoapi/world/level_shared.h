#ifndef LEGOAPI_WORLD_LEVEL_SHARED_H
#define LEGOAPI_WORLD_LEVEL_SHARED_H

// Shared externs for the level-loading subsystem files split out of
// levels.cpp (leveldata, areas, levelstreaming, levelobjects, boss).
// Includes mirror what the original levels.cpp used.

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/world.h"

// --- Level data (leveldata.cpp) ---
extern void *LevelHackData;
extern void *OldLevelHackData;
extern i32 LevelHackSize;
extern i32 LevelHackSendTimer;

extern i32 LevFlag[4];
extern i32 LevHSpecial[264];
extern i32 LevSfxFlag[4];
extern void *dynamic_antinodes;
extern i32 LevInstAnim[12];
extern i32 LevArea[4];
extern i32 LevPathNodes[8];
extern void *LevPathCnx[16];
extern i32 LevGameObject[8];
extern i32 LevGamePart[8];
extern i32 LevAIMessage[8];
extern i32 LevelLocator;
extern i32 LevGizObst[8];
extern i32 LevBlowUp[5];
extern i32 LevGizmo[12];
extern i32 LevSfxId[4];
extern i32 LevelCodeSpline[8];
extern GIZFORCE_s *LevGizForce[4];
extern i32 LevAIPathNode[4];
extern i32 LevBoltIgnorePlatIds[2];
extern i32 LevPlatID[2];
extern i32 LevPathCnxDir;
extern i32 LevDeaths;
extern i32 LevLock[4];
extern i32 LevSafePlatID[2];

extern i32 LEVELOBJECTCOUNT;
extern i32 AREACOUNT;
extern void *ADataList;
extern void *LevelProgressData;
extern void CompleteLevel(WORLDINFO_s *);
void StoreLevelProgress(WORLDINFO_s *);
extern WORLDINFO_s *WORLD;
i32 KillBoss(i32, i32, float);
extern void GoToNewLevel(i32);
extern i32 NewCutScene(CUTINFO *, CUTSYS *, char *, i32);
extern void ClearLevelProgress(i32, struct WORLDINFO_s *);

// --- Areas (areas.cpp) ---
extern struct GAMESAVE_s Game;
extern struct GAMESAVE_s BackupGame;
extern i32 Customiser_AccessoriesLoaded;
extern NUGSCN *vehicle_scene;
extern CUSTOMISER *CharacterCustomiser;
extern i32 HIGHGAMEOBJECT;
extern void *Obj;
extern i32 Area;
extern i32 last_area;
extern NUGSCN *big_icon_scene;
extern NUGSCN *area_scene;
extern struct LEVELDATA_s *HUB_LDATA;
extern i32 LSW1;
extern i32 LSW2;
extern i32 Arcade;
extern f32 HIGHJUMPHEIGHT;
extern i32 BuildUpTotal;
extern i32 BuildUpDone;
extern struct AREADATA_s *HOTHBATTLE_ADATA;
extern TIMER AreaTimer;
extern f32 VehicleAreaRememberSpeed;
extern i32 OldBonusScore[2];
extern i32 BonusScore[2];
extern i32 BonusCoinTotal;
extern void *Door_Last;
extern i32 LevelChange;
extern i32 BombGenerator_PlayerBomb[2];
extern i32 Lap;
extern f32 LevTime;
extern i32 AreaGlobals;
extern i32 Door_UseCutCam;
extern GameObject_s *Player[8];
extern struct playerprogress_s PlayerProgress[8];
extern i32 DEFAULT_PLAYERHITPOINTS;
extern i32 Area_PlayerModelCount;
extern i32 Area_StoryModelCount;
extern i16 Area_PlayerModelList[24];
extern i32 Area_FreePlayModelCount;
extern i16 Area_FreePlayModelList[104];
extern i32 Area_MissionModelCount;
extern APICHARACTERMODELLIST_s Area_MissionModelList[52];
extern i16 AreaMusic;
extern struct MISSIONSYS_s *MissionSys;
extern APICHARACTERMODELLIST_s Area_StoryModelList[52];
extern void *LevelLoad;
extern i32 LevelLoadCount;

// --- Level streaming (levelstreaming.cpp) ---
extern i32 LOADEROFF;
extern i32 BGLOAD;
extern struct LEVELDATA_s *NewLData;
extern i32 no_more_loads;
extern i32 other_level;
extern i32 other_level_override;
extern i32 CUTSTOPGAME;
extern void *CutStopInfo;
extern i32 WaitingForLevelTime;
extern f32 g_BgLoadDelayHackTimer;

// --- Level objects (levelobjects.cpp) ---
extern struct LEVELOBJECT *ObjTabList;

// --- Function prototypes ---
extern i32 Store_IsPackUnlocked(i32);
extern void ReCalculateCompletionPoints(void);
extern void Hub_LockUnlockDoors(struct WORLDINFO_s *);
extern void FreeTorpedoPacket(struct TORPEDOPACKET_s **);
extern void RemoveGameObject(struct GameObject_s *, i32);
extern void IconScenes_Dump(void);
extern void CharScenes_AreaDump(void);
extern void Particles_DumpAreaPage(void);
extern void Customiser_RestoreModelTextureIDs(struct CUSTOMISER *);
extern void Customiser_DumpAccessories(struct CUSTOMISER *);
extern "C" void APIDumpCharacterModels(i32);
extern "C" void NuGScnRemove(void *);
extern "C" i32 NuSpecialExistsFn(void *);
extern "C" void *NuSpecialGetInstanceix(void *);
extern "C" i16 FindPlatInst(void *);
extern void SuperCounters_Reset(i32);
extern void NewAreaMusicChanges(void);
extern void ClearTakeOverObjectSys(void);
extern void Door_Reset(void);
extern void ResetMinikitCounter(void);
extern i32 Mission_Active(struct MISSIONSYS_s *);
extern i32 Players_AveragePos(struct nuvec_s *, struct SOCKPOSITION_s *);
extern "C" void *NuFParCreate(const char *);
extern "C" i32 NuFParDestroy(void *);
extern "C" i32 NuFParGetLine(void *);
extern "C" i32 NuFParGetWord(void *);
extern "C" void NuStrCpy(char *, const char *);
extern "C" void NuStrCat(char *, const char *);
extern "C" i32 NuStrICmp(const char *, const char *);

// --- Forward struct tags ---
struct TORPEDOPACKET_s;
struct GameObject_s;
struct CUSTOMISER;
struct MISSIONSYS_s;
struct nuvec_s;
struct SOCKPOSITION_s;
struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;
struct numtx_s;

// Level-subsystem entry points used by the episode level files.
void *SetLevelHack(i32);
void ResetLevel(WORLDINFO_s *, char *, i32);
i16 BoltType_FindIDByName(char *, WORLDINFO_s *);
void *BoltType_FindByID(i32, WORLDINFO_s *);
void Bolt_Add(GameObject_s *, nuvec_s *, numtx_s *, i32, i32);
void TBOPENFN(char *, i32);
void TBCLOSEFN(char *, i32);

#endif // LEGOAPI_WORLD_LEVEL_SHARED_H
