#include <stddef.h>

#include "globals.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nusound/nusound.h"

struct CUSTOMISER;
struct GIZAIMESSAGESYS_s;

// ----------------------------------------------------------------------
// Shared game globals, grouped by subsystem. See src/globals.h.
// ----------------------------------------------------------------------

// ------------------------------------------------------------------------
// Frame rate & timestep
// ------------------------------------------------------------------------
i32 PAL = 0;
f32 FRAMETIME = 0;
f32 DEFAULTFPS = 0;
f32 DEFAULTFRAMETIME = 0;
f32 MAXFRAMETIME = 0;

// ------------------------------------------------------------------------
// Super buffer / memory arena
// ------------------------------------------------------------------------
i32 SUPERBUFFERSIZE = 0x2EB8EEB;
VARIPTR permbuffer_base;
VARIPTR original_permbuffer_base;
VARIPTR superbuffer_end;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;

// ------------------------------------------------------------------------
// Title & display strings
// ------------------------------------------------------------------------
char prodcode[16] = {0};
char *iconname = "lego.ico";
char unicodename[64] = "LEGO Star Wars Saga";
const char *theEmptyString = "";

// ------------------------------------------------------------------------
// Episode / area counts
// ------------------------------------------------------------------------
i32 EPISODECOUNT = 0;
i32 AREACOUNT = 0;

// ------------------------------------------------------------------------
// Game save state
// ------------------------------------------------------------------------
GAMESAVE_s Game = {0};
GAMESAVE_s BackupGame = {0};

// ------------------------------------------------------------------------
// Character customiser
// ------------------------------------------------------------------------
i32 Customiser_AccessoriesLoaded = 0;
CUSTOMISER *CharacterCustomiser = NULL;

// ------------------------------------------------------------------------
// Completion & bonus points
// ------------------------------------------------------------------------
i32 COMPLETIONPOINTS = 0;
i32 POINTS_PER_CHARACTER = 0;
i32 POINTS_PER_SUPERBONUSCOMPLETE = 0;
i32 POINTS_PER_TIMETRIAL = 0;
i32 POINTS_PER_STORY = 0;
i32 POINTS_PER_CHALLENGE = 0;
i32 POINTS_PER_MINIKIT = 0;
i32 POINTS_PER_TRUEJEDI = 0;
i32 POINTS_PER_REDBRICK = 0;
i32 POINTS_PER_MISSION = 0;
i32 POINTS_PER_CHEAT = 0;
i32 POINTS_PER_GOLDBRICK = 0;
i32 BOTHTRUEJEDIGOLDBRICKS = 0;
i32 SHOPGOLDBRICKS = 0;
i32 GOLDBRICKFORSUPERBONUS = 0;
i32 GOLDBRICKFORSUPERSTORY = 0;
i32 GOLDBRICKFORCHALLENGE = 0;
i32 POINTS_PER_SUPERSTORY = 0;
i32 GOLDBRICKPOINTS = 0;
i32 CompletionPointInfo[7] = {0};
i32 OldBonusScore[2] = {0};
i32 BonusScore[2] = {0};
i32 BonusCoinTotal = 0;

// ------------------------------------------------------------------------
// Audio & music
// ------------------------------------------------------------------------
NUSOUND_FILENAME_INFO *MusicInfo = NULL;
NUSOUND_FILENAME_INFO *g_music = NULL;
u8 g_BackgroundUsedFogColour = 0;
i32 g_BackgroundColour = 0;
u32 SFX_MUSIC_COUNT = 0;
i32 NOSOUND = 0;
i16 AreaMusic = 0;
i32 LevMusicAction = 0;
i32 LevMusicAmbient = 0;
i32 LevMusicOtherAction = 0;
i32 LevMusicOtherAmbient = 0;

// ------------------------------------------------------------------------
// Camera
// ------------------------------------------------------------------------
NUCAMERA *pNuCam = NULL;
GAMECAMERA_s *GameCam = NULL;

// ------------------------------------------------------------------------
// Platform & device info
// ------------------------------------------------------------------------
ANativeWindow *g_appWindow = NULL;
char g_deviceManufacturer[256] = {0};
char g_deviceModel[256] = {0};
i32 g_isLowestEndDevice = 0;
i32 g_isLowEndDevice = 0;
i32 g_isMidRangeDevice = 0;
i32 g_lowEndLevelBehaviour = 0;

// ------------------------------------------------------------------------
// Render / compatibility options
// ------------------------------------------------------------------------
i32 g_forceSysMemVbs = 0;
i32 g_forceETC1 = 0;
i32 Reflections_On = 1;
i32 disable_narrow_socks = 0;
i32 script_spline_selected = 0;
f32 character_farclip = 0.0f;
i32 texanimbits = 0;

// ------------------------------------------------------------------------
// Group scenes (NUGSCN)
// ------------------------------------------------------------------------
NUGSCN *vehicle_scene = NULL;

// ------------------------------------------------------------------------
// Gameplay timers & area state
// ------------------------------------------------------------------------
f32 DoubleScoreTime = 0.0f;
f32 GameTimer[2] = {0.0f, 0.0f};
void *AreaGlobals = NULL;
i32 HIGHGAMEOBJECT = 0;
void *Obj = NULL;
f32 AreaPickupGravity = 0.0f;
f32 HIGHJUMPHEIGHT = 0.0f;
TIMER AreaTimer;
f32 VehicleAreaRememberSpeed = 0;
f32 LevTime = 0.0f;
i32 Lap = 0;

// ------------------------------------------------------------------------
// Bonus / arcade / challenge mode
// ------------------------------------------------------------------------
i32 BonusWinner = 0;
i32 BonusWinFlag = 0;
i32 ChallengeMode = 0;
TIMER ChallengeTimer;
i32 LSW1 = 0;
i32 LSW2 = 0;
i32 Arcade = 0;
i32 BuildUpTotal = 0;
i32 BuildUpDone = 0;
void *Door_Last = NULL;
i32 LevelChange = 0;
i32 BombGenerator_PlayerBomb[2] = {0};
i32 BonusArea = 0;

// ------------------------------------------------------------------------
// Player & character objects
// ------------------------------------------------------------------------
i16 temp_yrot = 0;
i16 temp_xrot = 0;
GameObject_s *player = NULL;
GameObject_s *player2 = NULL;
i32 avg_currentspeed_mul = 0;
i32 pause_rndr_on = 0;
u8 object_switches[0x80] = {0};
GIZFORCE_s *force_array[4] = {0};
GameObject_s *ObiWan = NULL;
GameObject_s **game_objects = NULL;

// ------------------------------------------------------------------------
// Character preview / free-play model lists
// ------------------------------------------------------------------------
i32 FreePlay = 0;
i32 Area_PlayerModelCount = 0;
i32 Area_StoryModelCount = 0;
i16 Area_PlayerModelList[24] = {0};
i32 Area_FreePlayModelCount = 0;
i16 Area_FreePlayModelList[104] = {0};
i32 Area_MissionModelCount = 0;
APICHARACTERMODELLIST_s Area_MissionModelList[52] = {0};
VARIPTR characterbuffer_ptr = {0};
VARIPTR characterbuffer_end = {0};
APICHARACTERMODELLIST_s FreePlayModelList[52] = {0};
APICHARACTERMODELLIST_s Hub_ModelList[8] = {0};
i32 FreePlayModelCount = 0;
i32 FreePlayResidentCount = 0;
i32 FreePlayBonusCount = 0;
CHARCAT_s *CharCategory = NULL;
i32 CHARCATEGORYCOUNT = 0;
EXTRAMODELLISTENTRY_s ExtraModelList[1] = {{0}};
VEHICLECOLLECTION_s VehicleCollection = {0};
ARCADEITEM_s ArcadeItem = {0};
ARCADE_MODE_s Arcade_Mode[1] = {{0}};
GAME_CUSTOMISER_s *Game_Customiser = NULL;
void *CurrentCList = NULL;
void *CurrentStoryCList = NULL;
i32 CharacterDataLoad = 0;
i32 makefreeplaymodellist = 0;
i16 id_DEFAULTCHARACTER[2] = {-1, -1};
i32 CHARPAK = 0;
i32 apiloadcharactermodels_nopakfile = 0;

// ------------------------------------------------------------------------
// Level object tables
// ------------------------------------------------------------------------
i32 LevObjRef_FirstObj = 0;
i32 LevObjRef_LastObj = 0;
i32 LevObjRef_FirstRefObj = 0;
LEVELOBJECT *ObjTabList = NULL;
i32 LEVELOBJECTCOUNT = 0;
i32 LEVELOBJECTMAX = 0;
i32 EXTRALEVELOBJECTCOUNT = 0;
char *ExtraLevelObject_NameTable = NULL;
i32 ExtraLevelObject_NameTableSize = 0;
i32 ExtraLevelObject_NameTableIndex = 0;

// ------------------------------------------------------------------------
// Level / area data pointers (LDATA / ADATA)
// ------------------------------------------------------------------------
LEVELDATA *ANAKINSFLIGHTB_LDATA = NULL;
AREADATA *ANEWHOPE_ADATA = NULL;
LEVELDATA *ASTEROIDCHASEA_LDATA = NULL;
LEVELDATA *ASTEROIDCHASEB_LDATA = NULL;
LEVELDATA *ASTEROIDCHASEC_LDATA = NULL;
LEVELDATA *ASTEROIDCHASED_LDATA = NULL;
LEVELDATA *ASTEROIDCHASEMITRO_LDATA = NULL;
LEVELDATA *BLOCKADERUNNERB_LDATA = NULL;
LEVELDATA *BLOCKADERUNNERC_LDATA = NULL;
LEVELDATA *BLOCKADERUNNERD_LDATA = NULL;
LEVELDATA *BONUS_GUNSHIPA_LDATA = NULL;
LEVELDATA *BONUS_GUNSHIPB_LDATA = NULL;
AREADATA *BONUS_GUNSHIP_ADATA = NULL;
LEVELDATA *BOUNTYHUNTERPURSUITA_LDATA = NULL;
LEVELDATA *BOUNTYHUNTERPURSUITB_LDATA = NULL;
LEVELDATA *BOUNTYHUNTERPURSUITC_LDATA = NULL;
LEVELDATA *BOUNTYHUNTERPURSUITD_LDATA = NULL;
LEVELDATA *BOUNTYHUNTERPURSUITE_LDATA = NULL;
LEVELDATA *CLOUDCITYESCAPEA_LDATA = NULL;
LEVELDATA *CLOUDCITYESCAPEB_LDATA = NULL;
LEVELDATA *CLOUDCITYESCAPEC_LDATA = NULL;
LEVELDATA *CLOUDCITYTRAPA_LDATA = NULL;
LEVELDATA *CLOUDCITYTRAPB_LDATA = NULL;
LEVELDATA *CLOUDCITYTRAPC_LDATA = NULL;
LEVELDATA *CLOUDCITYTRAPOUTRO_LDATA = NULL;
LEVELDATA *CRUISERA_LDATA = NULL;
LEVELDATA *CRUISERB_LDATA = NULL;
LEVELDATA *CRUISERC_LDATA = NULL;
LEVELDATA *CRUISERD_LDATA = NULL;
LEVELDATA *CRUISERE_LDATA = NULL;
LEVELDATA *CRUISERG_LDATA = NULL;
LEVELDATA *DAGOBAHA_LDATA = NULL;
LEVELDATA *DAGOBAHB_LDATA = NULL;
LEVELDATA *DAGOBAHC_LDATA = NULL;
LEVELDATA *DAGOBAHD_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEA_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEB_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLED_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEE_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEF_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEG_LDATA = NULL;
LEVELDATA *DEATHSTAR2BATTLEMIDTRO_LDATA = NULL;
LEVELDATA *DEATHSTARBATTLEA_LDATA = NULL;
LEVELDATA *DEATHSTARBATTLEB_LDATA = NULL;
LEVELDATA *DEATHSTARBATTLEC_LDATA = NULL;
LEVELDATA *DEATHSTARBATTLED_LDATA = NULL;
LEVELDATA *DEATHSTARBATTLEMIDTRO_LDATA = NULL;
LEVELDATA *DEATHSTARESCAPEA_LDATA = NULL;
LEVELDATA *DEATHSTARESCAPEB_LDATA = NULL;
LEVELDATA *DEATHSTARESCAPEC_LDATA = NULL;
LEVELDATA *DEATHSTARESCAPED_LDATA = NULL;
LEVELDATA *DEATHSTARRESCUEA_LDATA = NULL;
LEVELDATA *DEATHSTARRESCUEB_LDATA = NULL;
LEVELDATA *DEATHSTARRESCUEC_LDATA = NULL;
LEVELDATA *DEATHSTARRESCUED_LDATA = NULL;
LEVELDATA *DEATHSTARRESCUEE_LDATA = NULL;
LEVELDATA *DOGFIGHTA_LDATA = NULL;
LEVELDATA *DOOKUC_LDATA = NULL;
LEVELDATA *DOOKUOUTRO_LDATA = NULL;
AREADATA *DOOKU_ADATA = NULL;
LEVELDATA *E1CHARACTERBONUSA_LDATA = NULL;
LEVELDATA *E2VEHICLEBONUSA_LDATA = NULL;
LEVELDATA *EMPERORFIGHTA_LDATA = NULL;
LEVELDATA *ENDORBATTLEA_LDATA = NULL;
LEVELDATA *ENDORBATTLEB_LDATA = NULL;
LEVELDATA *ENDORBATTLEC_LDATA = NULL;
LEVELDATA *ENDORBATTLED_LDATA = NULL;
LEVELDATA *FACTORYB_LDATA = NULL;
LEVELDATA *FACTORYD_LDATA = NULL;
LEVELDATA *FACTORYF_LDATA = NULL;
LEVELDATA *FACTORYG_LDATA = NULL;
LEVELDATA *GRIEVOUSA_LDATA = NULL;
LEVELDATA *GUNGAN_A_LDATA = NULL;
LEVELDATA *GUNGAN_B_LDATA = NULL;
LEVELDATA *GUNSHIPA_LDATA = NULL;
LEVELDATA *GUNSHIPB_LDATA = NULL;
LEVELDATA *HOTHBATTLEA_LDATA = NULL;
LEVELDATA *HOTHBATTLEB_LDATA = NULL;
LEVELDATA *HOTHBATTLEC_LDATA = NULL;
LEVELDATA *HOTHBATTLED_LDATA = NULL;
LEVELDATA *HOTHBATTLEE_LDATA = NULL;
LEVELDATA *HOTHBATTLEOUTRO_LDATA = NULL;
AREADATA *HOTHBATTLE_ADATA = NULL;
LEVELDATA *HOTHESCAPEA_LDATA = NULL;
LEVELDATA *HOTHESCAPEB_LDATA = NULL;
LEVELDATA *HOTHESCAPEC_LDATA = NULL;
LEVELDATA *HOTHESCAPED_LDATA = NULL;
LEVELDATA *HUB_LDATA = NULL;
LEVELDATA *JABBASPALACEA_LDATA = NULL;
LEVELDATA *JABBASPALACEB_LDATA = NULL;
LEVELDATA *JABBASPALACED_LDATA = NULL;
LEVELDATA *JABBASPALACEE_LDATA = NULL;
LEVELDATA *JABBASPALACE_OUTRO_LDATA = NULL;
AREADATA *JEDI_ADATA = NULL;
LEVELDATA *JEDI_B_LDATA = NULL;
LEVELDATA *JEDI_OUTRO_LDATA = NULL;
LEVELDATA *KAMINOA_LDATA = NULL;
LEVELDATA *KAMINOC_LDATA = NULL;
LEVELDATA *KAMINOD_LDATA = NULL;
LEVELDATA *KAMINOE_LDATA = NULL;
LEVELDATA *KAMINOF_LDATA = NULL;
LEVELDATA *KAMINOOUTRO_LDATA = NULL;
struct AREADATA_s *KAMINO_ADATA = NULL;
LEVELDATA *KASHYYYKA_LDATA = NULL;
LEVELDATA *KASHYYYKB_LDATA = NULL;
LEVELDATA *KASHYYYKC_LDATA = NULL;
LEVELDATA *KASHYYYKD_LDATA = NULL;
LEVELDATA *LastLData = NULL;
i32 last_area = -1;
LEVELDATA *LEGOCITY_LDATA = NULL;
LEVELDATA *MAULA_LDATA = NULL;
LEVELDATA *MAULB_LDATA = NULL;
LEVELDATA *MAULD_LDATA = NULL;
LEVELDATA *MAULE_LDATA = NULL;
LEVELDATA *MAULF_LDATA = NULL;
LEVELDATA *MOSEISLEYA_LDATA = NULL;
LEVELDATA *MOSEISLEYB_LDATA = NULL;
LEVELDATA *MOSEISLEYC_LDATA = NULL;
LEVELDATA *MOSEISLEYD_LDATA = NULL;
LEVELDATA *MOSEISLEYE_LDATA = NULL;
LEVELDATA *NB_KAMINOALDATA_LDATA = NULL;
LEVELDATA *NEGOTIATIONSA_LDATA = NULL;
LEVELDATA *NEGOTIATIONSB_LDATA = NULL;
LEVELDATA *NEGOTIATIONSC_LDATA = NULL;
LEVELDATA *NEWTOWN_LDATA = NULL;
LEVELDATA *PODRACEA_LDATA = NULL;
LEVELDATA *PODRACEB_LDATA = NULL;
LEVELDATA *PODRACEC_LDATA = NULL;
LEVELDATA *PODRACEOUTRO1_LDATA = NULL;
LEVELDATA *PODRACESTATUS_LDATA = NULL;
AREADATA *PODRACE_ADATA = NULL;
LEVELDATA *PODSPRINTA_LDATA = NULL;
AREADATA *PODSPRINT_ADATA = NULL;
LEVELDATA *RESCUEA_LDATA = NULL;
LEVELDATA *RESCUEB_LDATA = NULL;
LEVELDATA *RESCUEC_LDATA = NULL;
LEVELDATA *RESCUEE_LDATA = NULL;
LEVELDATA *RETAKEB_LDATA = NULL;
LEVELDATA *RETAKEE_LDATA = NULL;
LEVELDATA *RETAKEG_LDATA = NULL;
LEVELDATA *RETAKEINTRO1_LDATA = NULL;
LEVELDATA *RETAKEINTRO2_LDATA = NULL;
LEVELDATA *RETAKEINTRO3_LDATA = NULL;
LEVELDATA *SARLACCPITA_LDATA = NULL;
LEVELDATA *SARLACCPITB_LDATA = NULL;
LEVELDATA *SARLACCPITC_LDATA = NULL;
LEVELDATA *SENATEA_LDATA = NULL;
LEVELDATA *SPEEDERCHASEA_LDATA = NULL;
LEVELDATA *STATUS_LDATA = NULL;
LEVELDATA *TATOOINEA_LDATA = NULL;
LEVELDATA *TATOOINEC_LDATA = NULL;
LEVELDATA *TATOOINED_LDATA = NULL;
LEVELDATA *TATOOINEE_LDATA = NULL;
LEVELDATA *TEMPLEA_LDATA = NULL;
LEVELDATA *TEMPLEB_LDATA = NULL;
LEVELDATA *TEMPLEC_LDATA = NULL;
LEVELDATA *TEMPLESTATUS_LDATA = NULL;
LEVELDATA *TITLES_LDATA = NULL;
u32 trenchrun[8] = {0};
LEVELDATA *VADERA_LDATA = NULL;
LEVELDATA *VADERB_LDATA = NULL;
LEVELDATA *VADERC_LDATA = NULL;

// ------------------------------------------------------------------------
// Level hack data & progress
// ------------------------------------------------------------------------
void *LevelProgressData = NULL;
GameAnimSysProgress gameanimsysprogress = {0, 0, NULL};
void *LevelHackData = NULL;
void *OldLevelHackData = NULL;
i32 LevelHackSize = 0;
i32 LevelHackSendTimer = 0;

// ------------------------------------------------------------------------
// Level load keywords
// ------------------------------------------------------------------------
nufpcomjmp_s *Level_ConfigBeforeLoad_GameKeywords = NULL;
nufpcomjmp_s *Level_ConfigAfterLoad_GameKeywords = NULL;

// ------------------------------------------------------------------------
// Level streaming & loading
// ------------------------------------------------------------------------
LEVELDATA *NewLData = NULL;
i32 grab_screen_image = 0;
i32 waiting_for_new_level = 0;
i32 LOADEROFF = 0;
i32 no_more_loads = 0;
i32 other_level = 0;
i32 other_level_override = 0;
i32 CUTSTOPGAME = 0;
void *CutStopInfo = NULL;
f32 WaitingForLevelTime = 0;
f32 WaitingForCharacterTime = 0;
i32 LevelLoadCount = 0;
void *LevelLoad = NULL;
i32 new_level_from_menu = 0;
// The original .data initialises BGLOAD to 1 (background loading enabled).
i32 BGLOAD = 1;
i32 reset_restart = 0;
i32 newlevelfrommenu_newmenuid = -1;
i32 newlevelfrommenu_newmenuy = -1;
i32 NextArea_FreePlay = 0;

// ------------------------------------------------------------------------
// Level script arrays (Lev*)
// ------------------------------------------------------------------------
i32 LevFlag[4] = {0};
i32 LevHSpecial[264] = {0};
i32 LevSfxFlag[4] = {0};
void *dynamic_antinodes = NULL;
i32 LevInstAnim[12] = {0};
i32 LevArea[4] = {0};
i32 LevPathNodes[8] = {0};
void *LevPathCnx[16] = {0};
i32 LevGameObject[8] = {0};
i32 LevGamePart[8] = {0};
i32 LevAIMessage[8] = {0};
i32 LevelLocator = 0;
void *LevGizObst[8] = {0};
i32 LevBlowUp[5] = {0};
GIZMO *LevGizmo[12] = {0};
i32 LevForce = 0;
i32 LevSfxId[4] = {0};
i32 LevelCodeSpline[8] = {0};
GIZFORCE_s *LevGizForce[4] = {0};
i32 LevAIPathNode[4] = {0};
i32 LevBoltIgnorePlatIds[2] = {0};
i32 LevPlatID[2] = {0};
i32 LevPathCnxDir = 0;
i32 LevDeaths = 0;
i32 LevLock[4] = {0};
i32 LevSafePlatID[2] = {0};

// ------------------------------------------------------------------------
// Network / multiplayer (podrace, gunship, mines)
// ------------------------------------------------------------------------
RETAKEGNETPACKET_s *retakeg_netpacket = NULL;
i16 trooper_boltid = 0;
i8 trooper_side[3] = {0};
nuhspecial_s *hothtroopers = NULL;
i32 TimingBarSet = 0;
u32 client_mines[0x200] = {0};
MINESYS_s minesys;
i32 nethost = 0;
i32 clients_mines_bitfield[2] = {0};
i32 pod_mines_bitfield[2] = {0};
i32 mine_count = 0;
PODSPRINT_s podsprint;
PODRACENETPACKET_s *podrace_netpacket = NULL;
float gungan_a_time_Normal = 0.5f;
float gungan_a_time_LowEnd = 0.75f;
i32 active_neutral_count = 0;
i32 active_baddy_count = 0;
i16 id_STAP = -1;
// Unmangled globals from the original binary (C linkage). Initial values
// match the .data image of res/libTTapp.so.
extern "C" {
    i32 podrace_section = 0;
    i32 max_nsnipers = 0;
    i32 PodRace_nsnipers = 0;
    SNIPER_s PodRace_snipers[5];
    float PodRace_sniper_fire_time = 0.25f;
    float PodRace_sniper_start_fire_radius = 90.0f;
    float PodRace_sniper_fire_radius = 2.5f;
    float PodRace_sniper_fire_range_time = 2.0f;
    float pod_roll[2] = {0.0f, 0.0f};
    float pod_roll_target[2] = {0.0f, 0.0f};
    float pod_animtime[2] = {1.0f, 1.0f};
}
PODSPRINTNETPACKET_s *podsprint_netpacket = NULL;
i32 gunship_player_dead = 0;
void *kaminoe_netpacket = NULL;
void *factoryb_netpacket = NULL;
struct BONUSGUNSHIP_NETPACKET_s *bonusgunshipb_netpacket = NULL;
u8 dookuC_nodesNeedUpdating = 0;
struct vader_c_s vader_c = {0};
CUTINFO *factoryb_cut = NULL;
void *factoryb_conveyor_stopped_msg = NULL;
i32 bonus_gunship_store_progress_flag = 0;
float podanimendframe = 0.0f;
float pacemaker_alpha_table[0x8000] = {0};
GIZAIMESSAGESYS_s *gizaimessagesys = NULL;
i32 loadareadata_loadlevel = 0;

// ------------------------------------------------------------------------
// Loading screen (LoadPerm) globals
// ------------------------------------------------------------------------
// Original .data @0x622ef0: PermDataLoaded starts at 1.
i32 PermDataLoaded = 1;

// --- LoadPermData working set ---
// Original bss objects carved out of the startup TU; sizes from nm -S.
u8 ObjTab[0x1770];          // level-object table backing storage
u8 SplTab[0x1a0];           // spline table backing storage
u8 LSW_CharCategory[0x78];  // LSW character-category table
u8 Cheat[0x5a0];            // cheat table
u8 CharVariants_Game[0x5c]; // in-game character-variant table
u8 theMemoryManager[0x248]; // inline memory-manager block
u8 LSW_Text[0x1648];        // LSW string table (original .data)

void *ActionInfo = NULL;      // bound to &self+0x38 table at runtime
char *ExtraActionData = NULL; // "run1" pool pointer at runtime
void *theGameThings = NULL;

NUGSCN *saveicon_scene = NULL;
NUGSCN *button_scene = NULL;

FadeSystem *pFadeInfo = NULL;

// Cut-scene / gameplay hook wiring (original .data function pointers).
void (*CutScene_StartFn)(CUTINFO *) = NULL;
void (*CutScene_PreUpdateFn)(CUTINFO *) = NULL;
void (*CutScene_PostUpdateFn)(void) = NULL;
void (*CutScene_StoppedFn)(CUTINFO *) = NULL;
void (*CutScene_ReplaceCharacterModelFn)(CUTINFO *, NUGCUTCHAR_s *) = NULL;
void (*InitBolt_AddMomentumType)(BOLT_s *, GameObject_s *, nuvec_s *) = NULL;
void (*Bolt_HitPlatFn)(BOLT_s *) = NULL;
void (*Bolt_HitCustomFn)(BOLT_s *, nuvec_s *) = NULL;
void (*GameBlowUpBlownUpFn)(GIZMOBLOWUP_s *) = NULL;
void (*GizObstacle_SetDefaultSFXFn)(void *, GIZOBSTACLE_s *) = NULL;
// Original bss @0x6a3f54 / @0x6a3f50.
i32 LoadPerm_LanguageSelect = 0;
i32 LoadPerm_StringsLoaded = 0;
// Original bss @0x124f9c0.
i32 menu_flash = 0;
// Original .data @0x667cb0 / @0x667cc0.
i32 IntroText_TextID = -1;
i32 LANGUAGECOUNT = 6;
// Original .data @0x667ce0: default language list, entries of {language, 0}
// (8 bytes per entry); Text_LanguageList points at it (@0x667d30).
LANGLISTENTRY Text_LanguageList_Default[6] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}};
LANGLISTENTRY *Text_LanguageList = Text_LanguageList_Default;
// Original .data @0x667ca0/@0x667ca4.
f32 INTROTEXT_Y = 0.175f;
f32 INTROTEXT_SCALE = 0.79f;

// BSS 0x127c200 / 0x124f6f0 — QFont & string table pointers
char **TTab = nullptr;
char **TTab_Original = nullptr;
void *QFont2D = nullptr;
void *QFont2DButtons = nullptr;
void *QFont3D = nullptr;
void *QFont3DZ = nullptr;
void *QFont3DTime = nullptr;

// ------------------------------------------------------------------------
// Cutscene & system misc
// ------------------------------------------------------------------------
void *PlayerItemType = 0;
i32 PLAYERITEMTYPECOUNT = 0;
u32 EXBLOWUPFLAGS = 0;
i32 BeenAttacked = 0;
FadeSystem FadeSys;
i32 Paused = 0;
i32 MiniCutCam = 0;
i32 LEGOSPL_SPLIT = 0;
GAMECUTSCENES_s game_cutscenes;
float MiscTime = 0.0f;
u32 ResetBits = 0;
i32 AreaDataLoaded = 1;
i32 Level = 0;

// ------------------------------------------------------------------------
// Main game loop state (shared via batman.h; read/written by NuMain).
// Sizes/types match the original binary's .data/.bss layout.
// ------------------------------------------------------------------------
i32 AddCoinDelay[2] = {0};
i32 adaptivedifficulty[3] = {0};
i32 back_rgba[2] = {0};
TIMER BonusTimer = {0};
f32 brickimpactwait = 0.0f;
i32 BURNOUTON = 1;
VARIPTR characterbuffer_base = {0};
f32 chattersfxwait = 0.0f;
i32 clear_screen_onstill = 1;
f32 coinimpactwait = 0.0f;
i32 COMPLEXSHADOWS = 1;
i32 CUTDRAWWORLD = 0;
i32 CutSceneWaiting = 0;
i32 dagobah_training = 0;
i32 DoubleScore = 0;
i32 drawcharactermodel_nobsa = 0;
i32 DRAWCMODELCALLS = 0;
i32 editor_active = 0;
i32 enable_zero_frametime = 0;
i32 FinishLoop_On = 1;
LANGUAGEDATA Game_LanguageList[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
OPTIONSSAVE *Game_OptionsSave = NULL;
i32 (*GamePads_IgnoreInputFn)(void) = NULL;
i32 g_introState = 1;
i32 gone_through_door_to_new_level = 0;
f32 g_val = 0.0f;
i32 highallocaddr = 0;
i32 HubMainRenderTimeHack = 0;
AREADATA *LastAData = NULL;
i32 loadareacharacters_loadedlevel = 0;
f32 MainRenderTargetTime = 1.0f;
f32 MainRenderTime = 1.0f;
i32 memcard_autosavedisabled = 0;
i32 memcard_autosaveenabled = 0;
i32 menu_i_pack = -1;
i32 newlevel_resumecutaudio = 1;
i32 NewMode = 0;
i32 nurndr_tritot_this_frame = 0;
void (*NuSoundAppTerminateCallback)(void) = NULL;
i32 nuvideo_global_vbcnt = 0;
i32 PANELOFF = 0;
i32 party_cant_be_under_cover = 0;
i32 peak_poly_count = 0;
i32 PlayTrailer = -1;
NUVEC plr_lastpos = {0};
i32 poly_count = 0;
i32 RAYCASTCALLS = 0;
i32 reset_area = 0;
i32 reset_load = 1;
i32 ResetOldFStop = 1;
ripple_set_s *ripples = NULL;
f32 sabrerubwait = 0.0f;
i32 save_paused = 0;
i32 screendump = 0;
i32 SHADOWCALLS = 0;
void *ShadowMat = NULL;
STATUSPACKET_s StatusPacket = {0};
u8 status_plr_active[8] = {0};
i32 SuperStory = 0;
u8 TempOptions[13] = {0};
i32 TERRAINCALLS = 0;
f32 tieoffsfxwait = 0.0f;
f32 tieonsfxwait = 0.0f;
i32 waiting_for_character = -1;
