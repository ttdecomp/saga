#pragma once

#include "decomp.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/common.h"

// Forward declarations for the function-pointer table below (full types in
// legoapi/legoapi_types.h).
struct CUTINFO;
struct NUGCUTCHAR_s;
struct BOLT_s;
struct GameObject_s;
struct CHARACTERMODEL_s;
struct nuvec_s;
struct nuhspecial_s;
struct GIZMOBLOWUP_s;
struct GIZOBSTACLE_s;
struct WORLDINFO_s;
struct DOOR_s;
struct AREASAVE_s;
struct GAMECAMERA_s;
struct TEXTCRAWL_s;
struct COLLECTION_s;
class FadeSystem;

struct AREA_GLOBAL_VALUES {
    i32 field_0x00;
    i32 field_0x04;
    i32 field_0x08;
    i32 field_0x0c;
    i32 field_0x10;
    i32 field_0x14;
    i32 field_0x18;
    i32 field_0x1c;
    i32 field_0x20;
    i32 field_0x24;
    i32 field_0x28;
    i32 field_0x2c;
    i32 field_0x30;
};

union AREA_GLOBALS {
    AREA_GLOBAL_VALUES values;
    u8 bytes[0x34];
};

DECOMP_ASSERT(sizeof(AREA_GLOBALS) == 0x34, "AREA_GLOBALS size");

// ----------------------------------------------------------------------
// Placeholder save-game / model-list structures.
// ----------------------------------------------------------------------
typedef struct CHEAT { /* PlaceHolder Structure */
    char *name;
    undefined field1_0x4;
    undefined field2_0x5;
    undefined field3_0x6;
    undefined field4_0x7;
    byte enabled; /* Created by retype action */
    undefined field6_0x9;
    undefined field7_0xa;
    u8 area;
    undefined field9_0xc;
    undefined field10_0xd;
    undefined field11_0xe;
    undefined field12_0xf;
    char *code;
    undefined field14_0x14;
    undefined field15_0x15;
    undefined field16_0x16;
    undefined field17_0x17;
    char *extra_name;
    u32 flag;
} CHEAT;

struct OPTIONSSAVE_s { /* PlaceHolder Structure */
    undefined field0_0x0;
    undefined field1_0x1;
    undefined field2_0x2;
    undefined field3_0x3;
    undefined field4_0x4;
    undefined field5_0x5;
    undefined field6_0x6;
    undefined field7_0x7;
    undefined field8_0x8;
    undefined field9_0x9;
    undefined field10_0xa;
    undefined field11_0xb;
    undefined field12_0xc;
};
typedef struct OPTIONSSAVE_s OPTIONSSAVE;

struct SUPEROPTIONS_s {
    i16 field0_0x0;
    u8 touch_controls;
    u8 field2_0x3;
    f32 left_control_x;
    f32 left_control_y;
    f32 right_control_x;
    f32 right_control_y;
    u8 music_enabled;
    i8 field8_0x15;
    u8 field9_0x16[2];
};
DECOMP_ASSERT(sizeof(SUPEROPTIONS_s) == 0x18, "SUPEROPTIONS size");
extern SUPEROPTIONS_s SuperOptions;

typedef CUSTOMISESAVE_s CUSTOMISESAVE;
DECOMP_ASSERT(sizeof(CUSTOMISESAVE) == 0x6f, "CUSTOMISESAVE size");

struct EPISODESAVE_s {
    f32 superstory_time_limit;
    i32 superstory_score_target;
    u32 flags;
};
DECOMP_ASSERT(sizeof(EPISODESAVE_s) == 0xc, "EPISODESAVE size");

struct GAMESAVE_s {
    u8 field_0x0;
    u8 save_version;
    u8 field_0x2[2];
    struct OPTIONSSAVE_s options_save;
    u8 level_save[0x781b];
    AREASAVE_s area_save[72];
    EPISODESAVE_s episode_save[9];
    u32 field_0x7bf8;
    u32 initial_store_pack_flags;
    u8 field_0x7c00[0x20];
    u32 coins;
    u16 completion;
    u8 field_0x7c26[6];
    u32 field30_0x7c2c;
    CUSTOMISESAVE customizer;
    u8 field_0x7c9f;
    MISSIONSAVE mission_save;
    u8 mission_save_extra[0x58];
    u8 character_save[0x154];
};
DECOMP_ASSERT(sizeof(GAMESAVE_s) == 0x7e58, "GAMESAVE size");
DECOMP_ASSERT(offsetof(GAMESAVE_s, options_save) == 0x4, "GAMESAVE options offset");
DECOMP_ASSERT(offsetof(GAMESAVE_s, area_save) == 0x782c, "GAMESAVE area save offset");
DECOMP_ASSERT(offsetof(GAMESAVE_s, episode_save) == 0x7b8c, "GAMESAVE episode save offset");
DECOMP_ASSERT(offsetof(GAMESAVE_s, initial_store_pack_flags) == 0x7bfc, "GAMESAVE store flags offset");
DECOMP_ASSERT(offsetof(GAMESAVE_s, customizer) == 0x7c30, "GAMESAVE customizer offset");
DECOMP_ASSERT(offsetof(GAMESAVE_s, character_save) == 0x7d04, "GAMESAVE character save offset");

struct CHARCAT_s {
    undefined field0_0x0[4];
    u32 field1_0x4;
    i32 field2_0x8;
};

struct ARCADEITEM_s {
    undefined field0_0x0[0xc];
    char field_c_0xc;
};

struct ARCADE_MODE_s {
    undefined field0_0x0[8];
    i32 field8_0x8;
};

struct GAME_CUSTOMISER_s {
    undefined field0_0x0[0x6c];
    i16 field6c_0x6c;
    i16 field6e_0x6e;
};

// ----------------------------------------------------------------------
// Progress table for the shared animation system.
// ----------------------------------------------------------------------
struct GameAnimSysProgress {
    i32 count;
    i32 entry_size;
    u8 **entries;
};

// ------------------------------------------------------------------------
// Frame rate & timestep
// ------------------------------------------------------------------------
extern i32 PAL;
extern f32 FRAMETIME;
extern f32 DEFAULTFPS;
extern f32 DEFAULTFRAMETIME;
extern void *globalbuffer;
extern i32 MaxAnimJoints;
extern u8 ForcePlayEndFrame;
extern u8 BitCountTable[256];
extern i32 isBitCountTable;
extern f32 MAXFRAMETIME;

// ------------------------------------------------------------------------
// Super buffer / memory arena
// ------------------------------------------------------------------------
extern i32 SUPERBUFFERSIZE;
extern VARIPTR permbuffer_base;
extern VARIPTR original_permbuffer_base;
extern VARIPTR superbuffer_end;

extern "C" {
    extern VARIPTR superbuffer_base;
    extern VARIPTR superbuffer_ptr;
    extern i32 permbuffer_size;
    extern i32 CHARACTERBUFFERSIZE;
    extern i32 EDITBUFFERENDSIZE;
    extern VARIPTR editbuffer_end;
}
extern VARIPTR permbuffer_ptr;
extern VARIPTR permbuffer_end;

// ------------------------------------------------------------------------
// Title & display strings
// ------------------------------------------------------------------------
extern char prodcode[16];
extern char *iconname;
extern char unicodename[64];
extern const char *theEmptyString;

// ------------------------------------------------------------------------
// Episode / area counts
// ------------------------------------------------------------------------
extern i32 EPISODECOUNT;
extern i32 AREACOUNT;
extern struct AREADATA_s *SENATE_ADATA;
extern struct AREADATA_s *UTAPAU_ADATA;
extern struct AREADATA_s *HOTH_ADATA;
extern struct AREADATA_s *BONUSDAGOBAH_ADATA;
extern struct AREADATA_s *BONUSKAMINO_ADATA;
extern struct AREADATA_s *BONUSKASHYYYK_ADATA;
extern struct AREADATA_s *LOSTTEMPLE_ADATA;

// ------------------------------------------------------------------------
// Game save state
// ------------------------------------------------------------------------
extern struct GAMESAVE_s Game;
extern struct GAMESAVE_s BackupGame;
#ifdef __cplusplus
extern "C" {
#endif
    extern OPTIONSSAVE *Game_OptionsSave;
#ifdef __cplusplus
}
#endif
extern u8 *Game_LevelSave;
extern AREASAVE_s *Game_AreaSave;
extern EPISODESAVE_s *Game_EpisodeSave;
extern u8 *Game_CharacterSave;
extern u16 *Game_CompletionSave;
extern MISSIONSAVE *Game_MissionSave;
#ifdef __cplusplus
extern "C" {
#endif
    extern OPTIONSSAVE TempOptions;
#ifdef __cplusplus
}
#endif
extern SUIT_s Suit[10];
extern i16 tBATMANSUIT;
extern i16 tSHADOWSUIT;
extern i16 tGLIDESUIT;
extern i16 tDEMOLITIONSUIT;
extern i16 tSONARSUIT;
extern i16 tROBINSUIT;
extern i16 tWATERSUIT;
extern i16 tTECHNOLOGYSUIT;
extern i16 tMAGNETSUIT;
extern i16 tATTRACTSUIT;

// ------------------------------------------------------------------------
// Character customiser
// ------------------------------------------------------------------------
extern CUSTOMISER *CharacterCustomiser;
extern i32 Customiser_AccessoriesLoaded;

// ------------------------------------------------------------------------
// Completion & bonus points
// ------------------------------------------------------------------------
extern i32 COMPLETIONPOINTS;
extern i32 POINTS_PER_CHARACTER;
extern i32 POINTS_PER_SUPERBONUSCOMPLETE;
extern i32 POINTS_PER_TIMETRIAL;
extern i32 POINTS_PER_STORY;
extern i32 POINTS_PER_CHALLENGE;
extern i32 POINTS_PER_MINIKIT;
extern i32 POINTS_PER_TRUEJEDI;
extern i32 POINTS_PER_REDBRICK;
extern i32 POINTS_PER_MISSION;
extern i32 POINTS_PER_CHEAT;
extern i32 POINTS_PER_GOLDBRICK;
extern i32 BOTHTRUEJEDIGOLDBRICKS;
extern i32 SHOPGOLDBRICKS;
extern i32 GOLDBRICKFORSUPERBONUS;
extern i32 GOLDBRICKFORSUPERSTORY;
extern i32 GOLDBRICKFORCHALLENGE;
extern i32 POINTS_PER_SUPERSTORY;
extern i32 GOLDBRICKPOINTS;
extern i32 CompletionPointInfo[7];
extern i32 OldBonusScore[2];
extern i32 BonusScore[2];
extern i32 BonusCoinTotal;

// ------------------------------------------------------------------------
// Audio & music
// ------------------------------------------------------------------------
struct nusound_filename_info_s;
extern struct nusound_filename_info_s *MusicInfo;
extern struct nusound_filename_info_s *g_music;
extern u32 SFX_MUSIC_COUNT;
extern u8 g_BackgroundUsedFogColour;
extern i32 g_BackgroundColour;
extern i32 NOSOUND;
extern i32 LevMusicAction;
extern i32 LevMusicAmbient;
extern i32 LevMusicOtherAction;
extern i32 LevMusicOtherAmbient;
extern i16 AreaMusic;

// ------------------------------------------------------------------------
// Camera
// ------------------------------------------------------------------------
extern NUCAMERA *pNuCam;

// ------------------------------------------------------------------------
// Platform & device info
// ------------------------------------------------------------------------
struct ANativeWindow;
extern ANativeWindow *g_appWindow;
extern char g_deviceManufacturer[256];
extern char g_deviceModel[256];
extern i32 g_isLowestEndDevice;
extern i32 g_isLowEndDevice;
extern i32 g_isMidRangeDevice;
extern i32 g_lowEndLevelBehaviour;
extern i32 NOAICREATURES;
extern u8 aicreature_sets_alive[16];
extern i32 finishloop_backdroponly;

// ------------------------------------------------------------------------
// Render / compatibility options
// ------------------------------------------------------------------------
extern i32 g_forceSysMemVbs;
extern i32 g_forceETC1;
extern i32 texanimbits;
extern i32 Reflections_On;
extern i32 disable_narrow_socks;
extern i32 script_spline_selected;
extern f32 character_farclip;
extern f32 CutBorderScale;
extern i32 LEGOCAMMODE_DOORCUT;
extern i32 LEGOCAMMODE_OBSTACLE;
extern i32 ObstacleCamBorders;
extern TEXTCRAWL_s TextCrawl_LSW;
extern i32 drawcharactermodel_locatorsupdated;
extern i32 drawcharactermodel_noani;
extern i32 drawcharactermodel_restpose;
extern i32 drawcharactermodel_keepmergeaction;
extern i32 game_keepmergeaction;
extern i32 JointRotation_On;
extern i32 (*MakeLayerList)(CHARACTERMODEL_s *, i16 *, u32);

// ------------------------------------------------------------------------
// Group scenes (NUGSCN)
// ------------------------------------------------------------------------
extern NUGSCN *vehicle_scene;
extern NUGSCN *big_icon_scene;
extern NUGSCN *area_scene;

// ------------------------------------------------------------------------
// Gameplay timers & area state
// ------------------------------------------------------------------------
extern f32 DoubleScoreTime;
extern TIMER GameTimer;
extern TIMER OverallGamePlayTimer;
extern AREA_GLOBALS AreaGlobals;
extern i32 HIGHGAMEOBJECT;
extern GameObject_s *Obj;
extern f32 AreaPickupGravity;
extern f32 HIGHJUMPHEIGHT;
extern TIMER AreaTimer;
extern f32 VehicleAreaRememberSpeed;
extern nugspline_s *ObstacleCamSpl;
extern GAMECAMERA_s *GameCam;
extern u32 ZeroRTL[0x51];
extern char *partdebris_name[64];
enum GAMEPAD_BUTTON_FLAGS {
    GAMEPAD_BUTTON_DPAD_UP = 0x001,
    GAMEPAD_BUTTON_DPAD_DOWN = 0x002,
    GAMEPAD_BUTTON_DPAD_LEFT = 0x004,
    GAMEPAD_BUTTON_DPAD_RIGHT = 0x008,
    GAMEPAD_BUTTON_TAG = 0x010,
    GAMEPAD_BUTTON_SPECIAL = 0x020,
    GAMEPAD_BUTTON_JUMP = 0x040,
    GAMEPAD_BUTTON_ACTION = 0x080,
    GAMEPAD_BUTTON_START = 0x800,
};
extern u32 GAMEPAD_SKIP;
extern i32 MiniCutCam;
extern i32 LEGOCONTEXT_DROPIN;
extern i32 LEGOCONTEXT_DOOMED;
extern i32 Lap;
extern f32 LevTime[5];

// ------------------------------------------------------------------------
// Bonus / arcade / challenge mode
// ------------------------------------------------------------------------
extern i32 BonusWinner;
extern i32 BonusWinFlag;
extern i32 ChallengeMode;
extern struct TIMER_s ChallengeTimer;
extern i32 LSW1;
extern i32 LSW2;
extern i32 Arcade;
extern i32 BuildUpTotal;
extern i32 BuildUpDone;
extern DOOR_s *Door_Last;
extern void (*Door_GoThrough_ExtraCodeFn)(WORLDINFO_s *, DOOR_s *);
extern i32 gone_through_door_to_new_mode;
extern CUTINFO *newmode_cutinfo;
extern DOOR_s *setlastdoor_last;
extern i32 LevelChange;
extern i32 BombGenerator_PlayerBomb[2];

// ------------------------------------------------------------------------
// Player & character objects
// ------------------------------------------------------------------------
extern i16 temp_yrot;
extern i16 temp_xrot;
extern i32 avg_currentspeed_mul;
extern GameObject_s *player2;
extern GameObject_s *player;
extern GameObject_s *Player[8];
extern struct playerprogress_s PlayerProgress[8];
extern i32 DEFAULT_PLAYERHITPOINTS;
extern struct MISSIONSYS_s *MissionSys;

// Feature switches and interaction timing configured by each game variant.
extern i32 LedgeTerrain_On;
extern i32 Grapples_Available;
extern i32 SuperCarry_Bash;
extern i32 SuperCarry_Jump;
extern f32 PUNCHGAP;
extern f32 PUNCHCHARGAP;
extern i32 HINTS_ON;

// ------------------------------------------------------------------------
// Character preview / free-play model lists
// ------------------------------------------------------------------------
extern i32 FreePlay;
extern i32 FreePlayModelCount;
extern i32 FreePlayResidentCount;
extern i32 FreePlayBonusCount;
extern CHARCAT_s *CharCategory;
extern i32 CHARCATEGORYCOUNT;
extern EXTRAMODEL ExtraModelList[];
extern COLLECTION_s CharacterCollection;
extern COLLECTION_s VehicleCollection;
extern COLLECTION_s MiniKitCollection;
extern ARCADEITEM_s ArcadeItem;
extern ARCADE_MODE_s Arcade_Mode[];
extern GAME_CUSTOMISER_s *Game_Customiser;
extern APICHARACTERMODELLIST_s FreePlayModelList[];
extern APICHARACTERMODELLIST_s Hub_ModelList[];
extern APICHARACTERMODELLIST_s *CurrentCList;
extern APICHARACTERMODELLIST_s *CurrentStoryCList;
extern i32 Area_PlayerModelCount;
extern i32 Area_StoryModelCount;
extern i16 Area_PlayerModelList[24];
extern i32 Area_FreePlayModelCount;
extern i16 Area_FreePlayModelList[104];
extern i32 hub_freeplaysource;
extern i32 Area_MissionModelCount;
extern APICHARACTERMODELLIST_s Area_MissionModelList[52];
extern APICHARACTERMODELLIST_s Area_StoryModelList[52];

// ------------------------------------------------------------------------
// Level object tables
// ------------------------------------------------------------------------
extern i32 LevObjRef_FirstObj;
extern i32 LevObjRef_LastObj;
extern i32 LevObjRef_FirstRefObj;
extern LEVELOBJECT *ObjTabList;
extern i32 LEVELOBJECTCOUNT;
extern i32 LEVELOBJECTMAX;
extern i32 EXTRALEVELOBJECTCOUNT;
extern char *ExtraLevelObject_NameTable;
extern i32 ExtraLevelObject_NameTableSize;
extern i32 ExtraLevelObject_NameTableIndex;

extern i16 drawcharicon_hspecial_spin;
extern f32 drawcharicon_hspecial_dz;
extern i32 drawcharicon_find;
extern f32 drawcharicon_hspecial_scale;
extern i32 drawcharicon_i_panel;
extern f32 PANEL3DMULY;
extern f32 PANEL3DMULX;
extern i32 LEGOOBJ_ICON_WEIRDO;
extern i32 LEGOOBJ_ICON_QUESTION;

// ------------------------------------------------------------------------
// Level / area data pointers (LDATA / ADATA)
// ------------------------------------------------------------------------
extern LEVELDATA *ANAKINSFLIGHTB_LDATA;
extern LEVELDATA *ASTEROIDCHASEA_LDATA;
extern LEVELDATA *ASTEROIDCHASEB_LDATA;
extern LEVELDATA *ASTEROIDCHASEC_LDATA;
extern LEVELDATA *ASTEROIDCHASED_LDATA;
extern LEVELDATA *ASTEROIDCHASEMITRO_LDATA;
extern LEVELDATA *BLOCKADERUNNERB_LDATA;
extern LEVELDATA *BLOCKADERUNNERC_LDATA;
extern LEVELDATA *BLOCKADERUNNERD_LDATA;
extern LEVELDATA *BONUS_GUNSHIPA_LDATA;
extern LEVELDATA *BONUS_GUNSHIPB_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITA_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITB_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITC_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITD_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITE_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEA_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEB_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEC_LDATA;
extern LEVELDATA *CLOUDCITYTRAPA_LDATA;
extern LEVELDATA *CLOUDCITYTRAPB_LDATA;
extern LEVELDATA *CLOUDCITYTRAPC_LDATA;
extern LEVELDATA *CLOUDCITYTRAPOUTRO_LDATA;
extern LEVELDATA *CREDITS_LDATA;
extern LEVELDATA *CRUISERA_LDATA;
extern LEVELDATA *CRUISERB_LDATA;
extern LEVELDATA *CRUISERC_LDATA;
extern LEVELDATA *CRUISERD_LDATA;
extern LEVELDATA *CRUISERE_LDATA;
extern LEVELDATA *CRUISERG_LDATA;
extern LEVELDATA *DAGOBAHA_LDATA;
extern LEVELDATA *DAGOBAHB_LDATA;
extern LEVELDATA *DAGOBAHC_LDATA;
extern LEVELDATA *DAGOBAHD_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEA_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEB_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLED_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEE_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEF_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEG_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEMIDTRO_LDATA;
extern LEVELDATA *DEATHSTARBATTLEA_LDATA;
extern LEVELDATA *DEATHSTARBATTLEB_LDATA;
extern LEVELDATA *DEATHSTARBATTLEC_LDATA;
extern LEVELDATA *DEATHSTARBATTLED_LDATA;
extern LEVELDATA *DEATHSTARBATTLEMIDTRO_LDATA;
extern LEVELDATA *DEATHSTARESCAPEA_LDATA;
extern LEVELDATA *DEATHSTARESCAPEB_LDATA;
extern LEVELDATA *DEATHSTARESCAPEC_LDATA;
extern LEVELDATA *DEATHSTARESCAPED_LDATA;
extern LEVELDATA *DEATHSTARRESCUEA_LDATA;
extern LEVELDATA *DEATHSTARRESCUEB_LDATA;
extern LEVELDATA *DEATHSTARRESCUEC_LDATA;
extern LEVELDATA *DEATHSTARRESCUED_LDATA;
extern LEVELDATA *DEATHSTARRESCUEE_LDATA;
extern LEVELDATA *DOGFIGHTA_LDATA;
extern LEVELDATA *DOOKUC_LDATA;
extern LEVELDATA *DOOKUOUTRO_LDATA;
extern LEVELDATA *E1CHARACTERBONUSA_LDATA;
extern LEVELDATA *E2VEHICLEBONUSA_LDATA;
extern LEVELDATA *EMPERORFIGHTA_LDATA;
extern LEVELDATA *ENDORBATTLEA_LDATA;
extern LEVELDATA *ENDORBATTLEB_LDATA;
extern LEVELDATA *ENDORBATTLEC_LDATA;
extern LEVELDATA *ENDORBATTLED_LDATA;
extern LEVELDATA *FACTORYB_LDATA;
extern LEVELDATA *FACTORYD_LDATA;
extern LEVELDATA *FACTORYF_LDATA;
extern LEVELDATA *FACTORYG_LDATA;
extern LEVELDATA *GRIEVOUSA_LDATA;
extern LEVELDATA *GUNGAN_A_LDATA;
extern LEVELDATA *GUNGAN_B_LDATA;
extern LEVELDATA *GUNSHIPA_LDATA;
extern LEVELDATA *GUNSHIPB_LDATA;
extern LEVELDATA *HOTHBATTLEA_LDATA;
extern LEVELDATA *HOTHBATTLEB_LDATA;
extern LEVELDATA *HOTHBATTLEC_LDATA;
extern LEVELDATA *HOTHBATTLED_LDATA;
extern LEVELDATA *HOTHBATTLEE_LDATA;
extern LEVELDATA *HOTHBATTLEOUTRO_LDATA;
extern struct AREADATA_s *HOTHBATTLE_ADATA;
extern LEVELDATA *HOTHESCAPEA_LDATA;
extern LEVELDATA *HOTHESCAPEB_LDATA;
extern LEVELDATA *HOTHESCAPEC_LDATA;
extern LEVELDATA *HOTHESCAPED_LDATA;
extern struct LEVELDATA_s *HUB_LDATA;
extern LEVELDATA *JABBASPALACEA_LDATA;
extern LEVELDATA *JABBASPALACEB_LDATA;
extern LEVELDATA *JABBASPALACED_LDATA;
extern LEVELDATA *JABBASPALACEE_LDATA;
extern LEVELDATA *JABBASPALACE_OUTRO_LDATA;
extern LEVELDATA *JEDI_B_LDATA;
extern LEVELDATA *JEDI_OUTRO_LDATA;
extern LEVELDATA *KAMINOA_LDATA;
extern LEVELDATA *KAMINOC_LDATA;
extern LEVELDATA *KAMINOD_LDATA;
extern LEVELDATA *KAMINOE_LDATA;
extern LEVELDATA *KAMINOF_LDATA;
extern LEVELDATA *KAMINOOUTRO_LDATA;
extern struct AREADATA_s *KAMINO_ADATA;
extern LEVELDATA *KASHYYYKA_LDATA;
extern LEVELDATA *KASHYYYKB_LDATA;
extern LEVELDATA *KASHYYYKC_LDATA;
extern LEVELDATA *KASHYYYKD_LDATA;
extern LEVELDATA *LastLData;
extern i32 last_area;
extern LEVELDATA *LEGOCITY_LDATA;
extern LEVELDATA *MAULA_LDATA;
extern LEVELDATA *MAULB_LDATA;
extern LEVELDATA *MAULD_LDATA;
extern LEVELDATA *MAULE_LDATA;
extern LEVELDATA *MAULF_LDATA;
extern LEVELDATA *MOSEISLEYA_LDATA;
extern LEVELDATA *MOSEISLEYB_LDATA;
extern LEVELDATA *MOSEISLEYC_LDATA;
extern LEVELDATA *MOSEISLEYD_LDATA;
extern LEVELDATA *MOSEISLEYE_LDATA;
extern LEVELDATA *NB_KAMINOALDATA_LDATA;
extern LEVELDATA *NEGOTIATIONSA_LDATA;
extern LEVELDATA *NEGOTIATIONSB_LDATA;
extern LEVELDATA *NEGOTIATIONSC_LDATA;
extern LEVELDATA *NEWTOWN_LDATA;
extern LEVELDATA *PLATFORM_LDATA;
extern LEVELDATA *PODRACEA_LDATA;
extern LEVELDATA *PODRACEB_LDATA;
extern LEVELDATA *PODRACEC_LDATA;
extern LEVELDATA *PODRACEOUTRO1_LDATA;
extern LEVELDATA *PODRACESTATUS_LDATA;
extern LEVELDATA *PODSPRINTA_LDATA;
extern LEVELDATA *RESCUEA_LDATA;
extern LEVELDATA *RESCUEB_LDATA;
extern LEVELDATA *RESCUEC_LDATA;
extern LEVELDATA *RESCUEE_LDATA;
extern LEVELDATA *RETAKEB_LDATA;
extern LEVELDATA *RETAKED_LDATA;
extern LEVELDATA *RETAKEE_LDATA;
extern LEVELDATA *RETAKEG_LDATA;
extern LEVELDATA *RETAKEINTRO1_LDATA;
extern LEVELDATA *RETAKEINTRO2_LDATA;
extern LEVELDATA *RETAKEINTRO3_LDATA;
extern LEVELDATA *SARLACCPITA_LDATA;
extern LEVELDATA *SARLACCPITB_LDATA;
extern LEVELDATA *SARLACCPITC_LDATA;
extern LEVELDATA *SENATEA_LDATA;
extern LEVELDATA *SPEEDERCHASEA_LDATA;
extern LEVELDATA *STATUS_LDATA;
extern LEVELDATA *TATOOINEA_LDATA;
extern LEVELDATA *TATOOINEC_LDATA;
extern LEVELDATA *TATOOINED_LDATA;
extern LEVELDATA *TATOOINEE_LDATA;
extern LEVELDATA *TEMPLEA_LDATA;
extern LEVELDATA *TEMPLEB_LDATA;
extern LEVELDATA *TEMPLEC_LDATA;
extern LEVELDATA *TEMPLESTATUS_LDATA;
extern LEVELDATA *TITLES_LDATA;
extern u32 trenchrun[8];
extern LEVELDATA *VADERA_LDATA;
extern LEVELDATA *VADERB_LDATA;
extern LEVELDATA *VADERC_LDATA;

// ------------------------------------------------------------------------
// Level hack data & progress
// ------------------------------------------------------------------------
extern void *LevelProgressData;
extern GameAnimSysProgress gameanimsysprogress;
extern void *LevelHackData;
extern void *OldLevelHackData;
extern i32 LevelHackSize;
extern i32 LevelHackSendTimer;

// ------------------------------------------------------------------------
// Level load keywords
// ------------------------------------------------------------------------
extern nufpcomjmp_s *Level_ConfigBeforeLoad_GameKeywords;
extern nufpcomjmp_s *Level_ConfigAfterLoad_GameKeywords;

// ------------------------------------------------------------------------
// Level streaming & loading
// ------------------------------------------------------------------------
extern struct LEVELDATA_s *NewLData;
extern i32 grab_screen_image;
extern i32 waiting_for_new_level;
extern i32 new_level_from_menu;
extern i32 BGLOAD;
extern i32 reset_restart;
extern i32 newlevelfrommenu_newmenuid;
extern i32 newlevelfrommenu_newmenuy;
extern i32 NextArea_FreePlay;
extern i32 LOADEROFF;
extern i32 no_more_loads;
extern i32 other_level;
extern i32 other_level_override;
extern i32 CUTSTOPGAME;
extern void *CutStopInfo;
extern f32 WaitingForLevelTime;
extern f32 WaitingForCharacterTime;
extern f32 g_BgLoadDelayHackTimer;
extern i32 Door_UseCutCam;
extern i16 LevelLoad[48];
extern i32 LevelLoadCount;

// Main game loop state (read/written by NuMain; see batman.h for the rest).
extern i32 Level;
extern struct GIZAIMESSAGESYS_s *gizaimessagesys;

// ------------------------------------------------------------------------
// Level script arrays (Lev*)
// ------------------------------------------------------------------------
extern nuhspecial_s LevHSpecial[88];
extern NUMTX LevMtx;
extern f32 LevAlpha;
extern f32 TitlesAlpha;
extern f32 newgamealpha;
extern i32 newgamefade;
extern f32 newgamewait;
extern i32 newgame_menudrawoff;
extern i32 netnewgame;
extern i32 MenuLoadOccurred;
extern i32 MenuSaveOccurred;
extern i32 Tag_DoneFirst;
extern i32 Tag_DoneAny;
extern i32 LevSfxFlag[4];
extern u8 dynamic_antinodes[0x1500]; // AI anti-node spawn data (cleared per level)
extern i32 LevInstAnim[12];
extern i32 LevArea[4];
extern i32 LevPathNodes[8];
extern void *LevPathCnx[16];
extern i32 LevGameObject[8];
extern i32 LevGamePart[8];
extern i32 LevAIMessage[8];
extern GIZBUILDIT_s *LevBuildIt[4];
extern i32 LevelLocator;
extern GIZOBSTACLE_s *LevGizObst[8];
extern i32 LevBlowUp[5];
extern i32 LevSfxId[4];
extern i32 LevelCodeSpline[8];
extern GIZFORCE_s *LevGizForce[4];
extern GIZMO *LevGizmo[12];
extern void *LevAIPathNode[4];
extern i32 LevBoltIgnorePlatIds[2];
extern i32 LevPlatID[2];
extern i32 LevPathCnxDir;
extern i32 LevDeaths;
extern u8 LevLock[5];
extern i32 LevSafePlatID[2];
extern u64 LevHSpecialExists;

// ------------------------------------------------------------------------
// Cutscene & system misc
// ------------------------------------------------------------------------
extern u32 EXBLOWUPFLAGS;
extern i32 BeenAttacked;
enum RESETBIT_FLAGS {
    RESETBIT_REINITIALISE_LEVEL = 1 << 0,
    RESETBIT_DOOR_TRANSITION = 1 << 3,
    RESETBIT_CLEAR_LEVEL_PROGRESS = 1 << 5,
    RESETBIT_USE_CUSTOMISER_SETUP = 1 << 6,
};
extern u32 ResetBits;
extern i32 NetPaused;
extern f32 mtl_animation_speed_scale;
extern u16 script_mask;
extern i32 global_frame_count_paused;
extern i32 (*GizObstacle_CheckExcludeFlagsFn)(GIZOBSTACLE_s *, GameObject_s *);
extern void (*AIPathCnxHelperSysInitFn)(WORLDINFO_s *);

// ------------------------------------------------------------------------
// Loading screen (LoadPerm) globals
// ------------------------------------------------------------------------
extern LEVELOBJECT ObjTab[0x2ee]; // level-object type table (.data @0x618240, 0xff-terminated)
extern struct LEVELSPLINE SplTab[26];
extern u8 LSW_CharCategory[0x78];
extern u8 Cheat[0x5a0];
extern u8 CharVariants_Game[0x5c];
extern MemoryManager theMemoryManager;
extern struct TEXTENTRY LSW_Text[713];

extern void *ActionInfo;
extern char *ExtraActionData;
extern void *theGameThings;
extern void *theThingManager;

extern NUGSCN *saveicon_scene;
extern NUGSCN *button_scene;

extern FadeSystem *pFadeInfo;

// Cut-scene / gameplay hook wiring (original .data function pointers).
extern void (*CutScene_StartFn)(CUTINFO *);
extern void (*CutScene_PreUpdateFn)(CUTINFO *);
extern void (*CutScene_PostUpdateFn)(void);
extern void (*CutScene_StoppedFn)(CUTINFO *);
extern void (*CutScene_ReplaceCharacterModelFn)(CUTINFO *, NUGCUTCHAR_s *);
extern void (*InitBolt_AddMomentumType)(BOLT_s *, GameObject_s *, nuvec_s *);
extern void (*Bolt_HitPlatFn)(BOLT_s *);
extern void (*Bolt_HitCustomFn)(BOLT_s *, nuvec_s *);
extern void (*GameBlowUpBlownUpFn)(GIZMOBLOWUP_s *);
extern void (*GizObstacle_SetDefaultSFXFn)(void *, GIZOBSTACLE_s *);

extern i32 PermDataLoaded;          // original .data init 1
extern i32 LoadPerm_LanguageSelect; // bss
extern i32 LoadPerm_StringsLoaded;  // bss
extern i32 menu_flash;              // bss
extern i32 noscenespecials;         // disables automatic display-scene specials
extern f32 game_pulse;
extern f32 global_pulse;
extern i32 IntroText_TextID; // .data init -1
extern i32 LANGUAGECOUNT;    // .data init 6
extern i32 Text_Language;
extern LANGUAGEDATA Text_LanguageList_Default[6];
extern LANGUAGEDATA *Text_LanguageList;
extern f32 INTROTEXT_Y;
extern f32 INTROTEXT_SCALE;

// IntroText / QFont globals (TTapp BSS @0x127c200 / 0x124f6f0)
extern char **TTab;
struct vufnt_s;
extern vufnt_s *QFont2D;
extern vufnt_s *QFont2DButtons;
extern vufnt_s *QFont3D;
extern vufnt_s *QFont2DZ;
extern vufnt_s *QFont2DLower;
extern vufnt_s *QFont3DZ;
extern vufnt_s *QFont3DTime;
extern vufnt_s *SmartTextFont;
extern i32 create_qfont3d;
extern i32 create_qfont2dz;
extern i32 create_qfont2dlower;
extern i32 create_qfont3dz;
