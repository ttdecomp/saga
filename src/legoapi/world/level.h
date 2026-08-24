#pragma once

#include "legoapi/world/world.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/timer.h"
#include "nu2api/nucore/common.h"

// Forward declarations for tags referenced by the level-loading externs and
// entry-point prototypes declared at the end of this header.  Full definitions
// live in the headers that own them.
struct GameObject_s;
struct GIZFORCE_s;
struct nuvec_s;
struct numtx_s;
struct CUTINFO;
struct CUTSYS;
struct MISSIONSYS_s;
struct CUSTOMISER;
struct TORPEDOPACKET_s;
struct SOCKPOSITION_s;
struct AREADATA_s;

struct LEVEL_PROGRESS_s {
    char data[0x2800];
    i32 flags;
};

typedef struct LEVELDATADISPLAY {
    f32 unknown_00;
    f32 unknown_04;

    f32 far_clip;
    f32 fog_start;
    f32 particle_thin;

    i16 unknown_14;

    char bg_red_top;
    char bg_red_bottom;

    char bg_green_top;
    char bg_green_bottom;

    char bg_blue_top;
    char bg_blue_bottom;
} LEVELDATADISPLAY;

enum {
    LEVEL_CONFIG_LOADED = 1 << 0,  // 0x1  set once level config has been (re)loaded
    LEVEL_GAMEPLAY = 1 << 1,       // 0x2  playable level (has world: SockSys/AI loaded)
    LEVEL_UNKNOWN_FLAG_4 = 1 << 2, // 0x4  set by default on level_start
    LEVEL_TERRAIN = 1 << 3,        // 0x8  level has a terrain file
    LEVEL_INTRO = 1 << 5,          // 0x20
    LEVEL_MIDTRO = 1 << 6,         // 0x40
    LEVEL_OUTRO = 1 << 7,          // 0x80
    LEVEL_TEST = 1 << 9,           // 0x200
    LEVEL_STATUS = 1 << 10,        // 0x400
    LEVEL_NEWGAME = 1 << 16,       // 0x10000
    LEVEL_LOADGAME = 1 << 17,      // 0x20000
};

typedef struct LEVELDATA_s {
    char dir[0x40];
    char name[0x20];

    i16 unknown_060;

    i16 idx;

    u32 flags;

    void (*load_fn)(WORLDINFO *, VARIPTR *, VARIPTR *);
    void (*init_fn)(WORLDINFO *);
    void (*reset_fn)(WORLDINFO *);
    void (*update_fn)(WORLDINFO *);
    void (*always_update_fn)(WORLDINFO *);
    void (*draw_fn)(WORLDINFO *);
    void (*draw_status_fn)(WORLDINFO *);

    LEVELDATADISPLAY data_display;

    i16 music_index;

    i16 unknown_0a2;
    i16 max_ter_platforms;
    u16 max_ter_groups;
    i16 unknown_0a8;
    i16 unknown_0aa;

    char mipmap_mode;

    char blob_shadow_alpha;
    char unknown_0ae;
    char area_index;

    f32 cam_tilt;

    f32 hover_height;

    char unknown_0b8;
    char unknown_0b9;
    char unknown_0ba;
    char unknown_0bb;
    char unknown_0bc;
    char unknown_0bd;
    char unknown_0be;
    char unknown_0bf;

    f32 unknown_0c0;
    f32 cam_pullback_dist;
    f32 cam_lateral_dist;
    f32 unknown_0cc;

    char unknown_0d0;
    char unknown_0d1;
    char unknown_0d2;
    char unknown_0d3;

    char area_level_index;

    char blob_shadow_fade_near;
    char blob_shadow_fade_far;

    char cam_pos_seek;
    char cam_angle_seek;

    char unknown_0d9;
    char unknown_0da;
    char unknown_0db;

    f32 conveyor_x_speed;
    f32 conveyor_z_speed;

    u16 max_antinodes;
    u16 max_gizmo_blowups;
    u16 max_gizmo_blowup_types;
    u16 max_pickups;
    u16 max_obstacle_objs;
    u16 max_buildit_objs;
    u16 max_force_objs;
    u16 max_bombgen_objs;

    u8 max_tightropes;
    u8 max_giz_timers;
    u8 max_signals;
    u8 max_levers;
    u8 max_technos;
    u8 max_zipups;
    u8 max_grapples;
    u8 max_obstacles;
    u8 max_buildits;
    u8 max_shards;
    u8 max_spinners;
    u8 max_minicuts;
    u8 max_minicut_parts;
    u8 max_giz_specials;
    u8 max_attractos;
    u8 max_climb_objs;
    u8 max_guidelines;
    u8 max_ledges;
    u8 max_security_doors;
    u8 max_tubes;
    u8 max_giz_panels;
    u8 max_hat_machines;
    u8 max_force;
    u8 max_push_blocks;
    u8 max_push_block_end_pos;
    u8 max_doors;
    u8 max_teleports;
    u8 max_giz_randoms;
    u8 max_torp_machines;
    u8 max_spinner_anim_objs;
    u8 max_turrets;
    u8 max_bombgens;
    u8 max_bridges;
    u8 max_plugs;

    u8 field89_0x116;
    u8 field90_0x117;
    i16 field91_0x118;
    u8 field92_0x11a;
    u8 field93_0x11b;

    f32 unknown_11c;
    f32 unknown_120;

    f32 wind_speed;
    f32 wind_size;

    i32 music_tracks[3][2];
} LEVELDATA;

#ifndef HOST_BUILD
static_assert(sizeof(void *) != 4 || sizeof(LEVELDATA_s) == 0x144, "LEVELDATA_s size mismatch");
#endif

typedef struct LEVELOBJECT {
    u8 kind;
    u8 pad_01;
    u8 pad_02;
    u8 pad_03;
    char *name;
} LEVELOBJECT;

typedef struct LEVELFIXUP {
    char *name;
    LEVELDATA **level;
    void (*load_fn)(WORLDINFO *, VARIPTR *, VARIPTR *);
    void (*init_fn)(WORLDINFO *);
    void (*reset_fn)(WORLDINFO *);
    void (*update_fn)(WORLDINFO *);
    void (*always_update_fn)(WORLDINFO *);
    void (*draw_fn)(WORLDINFO *);
    void (*draw_status_fn)(WORLDINFO *);
} LEVELFIXUP;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA *LDataList;
    extern LEVELDATA *levelconfig_ldata;

    extern LEVELDATA *NEWGAME_LDATA;
    extern LEVELDATA *LOADGAME_LDATA;

    extern LEVELFIXUP LevFixUp;

    extern i32 LEVELCOUNT;
#ifdef __cplusplus
}

typedef void LEVELSETDEFAULTSFN(LEVELDATA *);

LEVELDATA *Levels_ConfigureList(char *file, VARIPTR *buf, VARIPTR *buf_end, i32 max_level_count, i32 *level_count_out,
                                LEVELSETDEFAULTSFN *set_defaults_fn);

void Level_SetDefaults(LEVELDATA *level);

LEVELDATA *Level_FindByName(char *name, i32 *idx_out);

void Level_Draw(WORLDINFO *world);

void Level_Update(WORLDINFO *world);

i32 LevelObject_GetReflection(i32 objId);

char *LevelObject_FindNameFromIndex(i32 index);
i32 LevelObject_FindIndexFromName(char *name);
i32 LevelObject_FindIndexFromName_RefOnly(char *name);

i32 LevelObject_AddExtra(char *name, i32 kind);

void ClearLevelProgress(i32 index, WORLDINFO *world);

void FixUpLevels(LEVELFIXUP *fixup);

void Levels_FixUp(LEVELFIXUP *fixup);

typedef struct nufpcomjmp_s nufpcomjmp_s;

void Level_RegisterGameConfigKeywords(nufpcomjmp_s *beforeLoadKeywords, nufpcomjmp_s *afterLoadKeywords);

void SetLevelExBlowupFlags(u32 flags);
u32 GetLevelExBlowupFlags(void);
void GoToNewLevel(i32 levelIdx);

void LevelConfig_BeforeLoad(LEVELDATA *level, char *buffer, nufpcomjmp_s *keywords);
void LevelConfig_AfterLoad(LEVELDATA *level, char *buffer, nufpcomjmp_s *keywords);

void Level_LoadConfigFile(WORLDINFO *world);

// ---------------------------------------------------------------------------
// Level-loading shared state & cross-TU entry points (previously level_shared.h).
// The definitions live in globals.cpp / the owning subsystem .cpp files; they
// are declared here as the level-subsystem header so the level files share them.
// ---------------------------------------------------------------------------

// --- Level data (leveldata.cpp) ---
extern void *LevelHackData;
extern void *OldLevelHackData;
extern i32 LevelHackSize;
extern i32 LevelHackSendTimer;
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
extern void *LevGizObst[8];
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

// --- Level streaming (levelstreaming.cpp) ---
extern i32 LOADEROFF;
extern i32 BGLOAD;
extern struct LEVELDATA_s *NewLData;
extern i32 no_more_loads;
extern i32 other_level;
extern i32 other_level_override;
void LevelStreaming_DoorOverride(WORLDINFO_s *, struct LEVELDATA_s *, float, float *);
extern i32 CUTSTOPGAME;
extern void *CutStopInfo;
extern i32 WaitingForLevelTime;
extern f32 g_BgLoadDelayHackTimer;

// --- Areas extra state (areas.cpp), defined in globals.cpp ---
extern struct GAMESAVE_s BackupGame;
extern CUSTOMISER *CharacterCustomiser;
extern i32 Customiser_AccessoriesLoaded;
extern NUGSCN *vehicle_scene;
extern NUGSCN *big_icon_scene;
extern NUGSCN *area_scene;
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
extern i32 Door_UseCutCam;
extern i32 Area_PlayerModelCount;
extern i32 Area_StoryModelCount;
extern i16 Area_PlayerModelList[24];
extern i32 Area_FreePlayModelCount;
extern i16 Area_FreePlayModelList[104];
extern i32 Area_MissionModelCount;
extern APICHARACTERMODELLIST_s Area_MissionModelList[52];
extern i16 AreaMusic;
extern APICHARACTERMODELLIST_s Area_StoryModelList[52];
extern void *LevelLoad;
extern i32 LevelLoadCount;

// --- Level objects / progress data ---
extern struct LEVELOBJECT *ObjTabList;
extern i32 LEVELOBJECTCOUNT;
extern void *LevelProgressData;

// --- Cross-TU function prototypes (level-loading entry points) ---
extern void CompleteLevel(WORLDINFO_s *);
void StoreLevelProgress(WORLDINFO_s *);
i32 KillBoss(i32, i32, float);
void KillBossNewLevel(i32, i32, float, i32);
void KillBossCompleteLevel(i32, i32, float);
i32 KillBossPlayCutScene(i32, i32, float, char *name);
extern i32 NewCutScene(CUTINFO *, CUTSYS *, char *, i32);
void *SetLevelHack(i32);
void ResetLevel(WORLDINFO_s *, char *, i32);
extern i8 BoltType_FindIDByName(char *, WORLDINFO_s *);
extern void *BoltType_FindByID(i32, WORLDINFO_s *);
extern void Bolt_Add(GameObject_s *, nuvec_s *, numtx_s *, i32, i32);
void TBOPENFN(char *, i32);
void TBCLOSEFN(char *, i32);
void InitMiniSnowTroopers(WORLDINFO_s *, i32, i32, i32);
void UpdateMiniSnowTroopers(WORLDINFO_s *);

// --- Shared game-object / player state ---
extern GameObject_s *Player[8];
extern struct playerprogress_s PlayerProgress[8];
extern i32 DEFAULT_PLAYERHITPOINTS;
extern struct MISSIONSYS_s *MissionSys;

#endif
