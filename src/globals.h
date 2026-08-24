#pragma once

#include "decomp.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/common.h"

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
};
typedef struct OPTIONSSAVE_s OPTIONSSAVE;

struct customisesave_s { /* PlaceHolder Structure */
    undefined2 field0_0x0;
    undefined2 field1_0x2;
    undefined2 field2_0x4;
    undefined2 field3_0x6;
    undefined2 field4_0x8;
    undefined2 field5_0xa;
    undefined2 field6_0xc;
    undefined2 field7_0xe;
    undefined2 field8_0x10;
    undefined field9_0x12;
    undefined field10_0x13;
    char field11_0x14[1];
    undefined field12_0x15;
    undefined field13_0x16;
    undefined field14_0x17;
    undefined field15_0x18;
    undefined field16_0x19;
    undefined field17_0x1a;
    undefined field18_0x1b;
    undefined field19_0x1c;
    undefined field20_0x1d;
    undefined field21_0x1e;
    undefined field22_0x1f;
    undefined field23_0x20;
    undefined field24_0x21;
    undefined field25_0x22;
    undefined field26_0x23;
    undefined field27_0x24;
    undefined field28_0x25;
    undefined field29_0x26;
    undefined field30_0x27;
    undefined field31_0x28;
    undefined field32_0x29;
    undefined field33_0x2a;
    undefined field34_0x2b;
    undefined field35_0x2c;
    undefined field36_0x2d;
    undefined field37_0x2e;
    undefined field38_0x2f;
    undefined field39_0x30;
    undefined field40_0x31;
    undefined field41_0x32;
    undefined field42_0x33;
    undefined field43_0x34;
    undefined field44_0x35;
    undefined field45_0x36;
    undefined field46_0x37;
    undefined2 field47_0x38;
    undefined2 field48_0x3a;
    undefined2 field49_0x3c;
    undefined2 field50_0x3e;
    undefined2 field51_0x40;
    undefined2 field52_0x42;
    undefined2 field53_0x44;
    undefined2 field54_0x46;
    undefined2 field55_0x48;
    undefined field56_0x4a;
    undefined field57_0x4b;
    undefined1 name; /* Created by retype action */
    undefined field59_0x4d;
    undefined field60_0x4e;
    undefined field61_0x4f;
    undefined field62_0x50;
    undefined field63_0x51;
    undefined field64_0x52;
    undefined field65_0x53;
    undefined field66_0x54;
    undefined field67_0x55;
    undefined field68_0x56;
    undefined field69_0x57;
    undefined field70_0x58;
    undefined field71_0x59;
    undefined field72_0x5a;
    undefined field73_0x5b;
    undefined field74_0x5c;
    undefined field75_0x5d;
    undefined field76_0x5e;
    undefined field77_0x5f;
    undefined field78_0x60;
    undefined field79_0x61;
    undefined field80_0x62;
    undefined field81_0x63;
    undefined field82_0x64;
    undefined field83_0x65;
    undefined field84_0x66;
    undefined field85_0x67;
    undefined field86_0x68;
    undefined field87_0x69;
    undefined field88_0x6a;
    undefined field89_0x6b;
    undefined field90_0x6c;
    undefined field91_0x6d;
    undefined field92_0x6e;
};
typedef struct customisesave_s CUSTOMISESAVE;

struct GAMESAVE_s {
    undefined field0_0x0;
    undefined field1_0x1;
    undefined field2_0x2;
    undefined field3_0x3;
    struct OPTIONSSAVE_s options_save;
    undefined field5_0x10;
    undefined1 level_save; /* Created by retype action */
    undefined field7_0x12[30746];
    undefined1 area_save; /* Created by retype action */
    undefined field9_0x782d;
    undefined field10_0x782e;
    undefined field11_0x782f;
    undefined field12_0x7830;
    undefined field13_0x7831;
    undefined field14_0x7832;
    undefined field15_0x7833;
    f32 field16_0x7834;
    char field17_0x7838[852];
    undefined1 episode_save; /* Created by retype action */
    char field19_0x7b8d[107];
    u32 field20_0x7bf8[1];
    char field21_0x7bfc[36];
    u32 coins;
    u16 completion; /* Created by retype action */
    undefined field24_0x7c26;
    undefined field25_0x7c27;
    undefined field26_0x7c28;
    undefined field27_0x7c29;
    undefined field28_0x7c2a;
    undefined field29_0x7c2b;
    undefined field30_0x7c2c;
    undefined field31_0x7c2d;
    undefined field32_0x7c2e;
    undefined field33_0x7c2f;
    CUSTOMISESAVE customizer;
    undefined field35_0x7c9f;
    MISSIONSAVE mission_save;
    char field37_0x7ca4[88];
    void *character_save;
    char field39_0x7d08[336];
};

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

struct VEHICLECOLLECTION_s {
    void *field0_0x0;
    undefined field4_0x4[2];
    u16 count_0x6;
};

struct EXTRAMODELLISTENTRY_s {
    i16 *model_list;
    void *field4_0x4;
};

extern i32 PAL;
extern f32 FRAMETIME;
extern f32 DEFAULTFPS;
extern f32 DEFAULTFRAMETIME;
extern f32 MAXFRAMETIME;

extern i32 SUPERBUFFERSIZE;
extern VARIPTR permbuffer_base;
extern VARIPTR original_permbuffer_base;
extern VARIPTR superbuffer_end;
extern VARIPTR permbuffer_ptr;
extern VARIPTR permbuffer_end;

extern char prodcode[16];
extern char *iconname;
extern char unicodename[64];

extern i32 EPISODECOUNT;
extern i32 AREACOUNT;

extern struct GAMESAVE_s Game;

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

struct nusound_filename_info_s;
extern struct nusound_filename_info_s *MusicInfo;
extern struct nusound_filename_info_s *g_music;
extern u32 SFX_MUSIC_COUNT;
extern u8 g_BackgroundUsedFogColour;
extern NUCAMERA *pNuCam;

extern i32 NOSOUND;

extern const char *theEmptyString;

struct ANativeWindow;
extern ANativeWindow *g_appWindow;

extern char g_deviceManufacturer[256];
extern char g_deviceModel[256];

extern i32 g_forceSysMemVbs;

extern i32 g_forceETC1;

extern i32 g_isLowestEndDevice;
extern i32 g_isLowEndDevice;
extern i32 g_isMidRangeDevice;
extern i32 g_lowEndLevelBehaviour;

extern f32 DoubleScoreTime;
extern f32 GameTimer[2];
extern i32 AreaGlobals;

extern i32 BonusWinner;
extern i32 BonusWinFlag;
extern i32 ChallengeMode;
extern struct TIMER_s ChallengeTimer;
extern i32 HIGHGAMEOBJECT;
extern void *Obj;
extern f32 AreaPickupGravity;

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

extern struct LEVELDATA_s *NewLData;
extern struct LEVELDATA_s *HUB_LDATA;
extern i32 grab_screen_image;
extern i32 FreePlay;
extern i32 waiting_for_new_level;

extern void *LevelProgressData;

struct GameAnimSysProgress {
    i32 count;
    i32 entry_size;
    u8 **entries;
};
extern GameAnimSysProgress gameanimsysprogress;

extern nufpcomjmp_s *Level_ConfigBeforeLoad_GameKeywords;
extern nufpcomjmp_s *Level_ConfigAfterLoad_GameKeywords;

extern u32 EXBLOWUPFLAGS;

extern i32 BeenAttacked;

extern i32 last_area;
extern LEVELDATA *LastLData;
extern LEVELDATA *TITLES_LDATA;
extern LEVELDATA *STATUS_LDATA;
extern LEVELDATA *ASTEROIDCHASEMITRO_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITE_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEB_LDATA;
extern LEVELDATA *CLOUDCITYTRAPOUTRO_LDATA;
extern LEVELDATA *CRUISERB_LDATA;
extern LEVELDATA *CRUISERE_LDATA;
extern LEVELDATA *CRUISERG_LDATA;
extern LEVELDATA *DAGOBAHD_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEB_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEMIDTRO_LDATA;
extern LEVELDATA *DEATHSTARBATTLEA_LDATA;
extern LEVELDATA *DEATHSTARBATTLEB_LDATA;
extern LEVELDATA *DEATHSTARRESCUEA_LDATA;
extern LEVELDATA *DEATHSTARRESCUED_LDATA;
extern LEVELDATA *DEATHSTARRESCUEE_LDATA;
extern LEVELDATA *DOGFIGHTA_LDATA;
extern LEVELDATA *DOOKUOUTRO_LDATA;
extern LEVELDATA *E1CHARACTERBONUSA_LDATA;
extern LEVELDATA *E2VEHICLEBONUSA_LDATA;
extern LEVELDATA *ENDORBATTLEB_LDATA;
extern LEVELDATA *ENDORBATTLEC_LDATA;
extern LEVELDATA *ENDORBATTLED_LDATA;
extern LEVELDATA *FACTORYD_LDATA;
extern LEVELDATA *FACTORYF_LDATA;
extern LEVELDATA *GUNGAN_B_LDATA;
extern LEVELDATA *HOTHBATTLEB_LDATA;
extern LEVELDATA *HOTHBATTLED_LDATA;
extern LEVELDATA *HOTHBATTLEOUTRO_LDATA;
extern LEVELDATA *JABBASPALACE_OUTRO_LDATA;
extern LEVELDATA *JEDI_OUTRO_LDATA;
extern LEVELDATA *KAMINOC_LDATA;
extern LEVELDATA *KAMINOA_LDATA;
extern struct AREADATA_s *KAMINO_ADATA;
extern LEVELDATA *MOSEISLEYC_LDATA;
extern LEVELDATA *NEGOTIATIONSC_LDATA;
extern LEVELDATA *PODRACEOUTRO1_LDATA;
extern LEVELDATA *PODRACESTATUS_LDATA;
extern LEVELDATA *RETAKEB_LDATA;
extern LEVELDATA *RETAKEINTRO1_LDATA;
extern LEVELDATA *RETAKEINTRO2_LDATA;
extern LEVELDATA *RETAKEINTRO3_LDATA;
extern LEVELDATA *SENATEA_LDATA;
extern LEVELDATA *TATOOINEC_LDATA;
extern LEVELDATA *TATOOINEE_LDATA;
extern LEVELDATA *TEMPLEB_LDATA;
extern LEVELDATA *TEMPLESTATUS_LDATA;
extern LEVELDATA *SPEEDERCHASEA_LDATA;
extern LEVELDATA *NEGOTIATIONSA_LDATA;
extern LEVELDATA *NEGOTIATIONSB_LDATA;
extern LEVELDATA *GUNGAN_A_LDATA;
extern LEVELDATA *RESCUEA_LDATA;
extern LEVELDATA *RESCUEB_LDATA;
extern LEVELDATA *RESCUEC_LDATA;
extern LEVELDATA *RESCUEE_LDATA;
extern LEVELDATA *PODRACEB_LDATA;
extern LEVELDATA *PODRACEA_LDATA;
extern LEVELDATA *PODRACEC_LDATA;
extern LEVELDATA *PODSPRINTA_LDATA;
extern LEVELDATA *ANAKINSFLIGHTB_LDATA;
extern LEVELDATA *RETAKED_LDATA;
extern LEVELDATA *RETAKEE_LDATA;
extern LEVELDATA *RETAKEG_LDATA;
extern LEVELDATA *MAULA_LDATA;
extern LEVELDATA *MAULB_LDATA;
extern LEVELDATA *MAULD_LDATA;
extern LEVELDATA *MAULE_LDATA;
extern LEVELDATA *MAULF_LDATA;
extern LEVELDATA *JEDI_B_LDATA;
extern LEVELDATA *GUNSHIPA_LDATA;
extern LEVELDATA *GUNSHIPB_LDATA;
extern LEVELDATA *BONUS_GUNSHIPA_LDATA;
extern LEVELDATA *BONUS_GUNSHIPB_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITA_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITB_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITC_LDATA;
extern LEVELDATA *BOUNTYHUNTERPURSUITD_LDATA;
extern LEVELDATA *FACTORYB_LDATA;
extern LEVELDATA *FACTORYG_LDATA;
extern LEVELDATA *DOOKUC_LDATA;
extern LEVELDATA *KAMINOD_LDATA;
extern LEVELDATA *KAMINOOUTRO_LDATA;
extern LEVELDATA *KAMINOE_LDATA;
extern LEVELDATA *KAMINOF_LDATA;
extern LEVELDATA *NB_KAMINOALDATA_LDATA;
extern LEVELDATA *CRUISERA_LDATA;
extern LEVELDATA *CRUISERC_LDATA;
extern LEVELDATA *CRUISERD_LDATA;
extern LEVELDATA *GRIEVOUSA_LDATA;
extern LEVELDATA *TEMPLEA_LDATA;
extern LEVELDATA *TEMPLEC_LDATA;
extern LEVELDATA *KASHYYYKA_LDATA;
extern LEVELDATA *KASHYYYKB_LDATA;
extern LEVELDATA *KASHYYYKC_LDATA;
extern LEVELDATA *KASHYYYKD_LDATA;
extern LEVELDATA *VADERA_LDATA;
extern LEVELDATA *VADERB_LDATA;
extern LEVELDATA *VADERC_LDATA;
extern LEVELDATA *BLOCKADERUNNERB_LDATA;
extern LEVELDATA *BLOCKADERUNNERC_LDATA;
extern LEVELDATA *BLOCKADERUNNERD_LDATA;
extern LEVELDATA *MOSEISLEYA_LDATA;
extern LEVELDATA *MOSEISLEYB_LDATA;
extern LEVELDATA *MOSEISLEYD_LDATA;
extern LEVELDATA *MOSEISLEYE_LDATA;
extern LEVELDATA *DEATHSTARBATTLEMIDTRO_LDATA;
extern i32 texanimbits;
extern u32 trenchrun[8];
extern LEVELDATA *TATOOINEA_LDATA;
extern LEVELDATA *TATOOINED_LDATA;
extern LEVELDATA *DEATHSTARRESCUEB_LDATA;
extern LEVELDATA *DEATHSTARRESCUEC_LDATA;
extern LEVELDATA *DEATHSTARESCAPEA_LDATA;
extern LEVELDATA *DEATHSTARESCAPEB_LDATA;
extern LEVELDATA *DEATHSTARESCAPEC_LDATA;
extern LEVELDATA *DEATHSTARESCAPED_LDATA;
extern LEVELDATA *DEATHSTARBATTLEC_LDATA;
extern LEVELDATA *DEATHSTARBATTLED_LDATA;
extern LEVELDATA *HOTHBATTLEA_LDATA;
extern LEVELDATA *HOTHBATTLEC_LDATA;
extern LEVELDATA *HOTHBATTLEE_LDATA;
extern LEVELDATA *HOTHESCAPEA_LDATA;
extern LEVELDATA *HOTHESCAPEB_LDATA;
extern LEVELDATA *HOTHESCAPEC_LDATA;
extern LEVELDATA *HOTHESCAPED_LDATA;
extern LEVELDATA *ASTEROIDCHASEA_LDATA;
extern LEVELDATA *ASTEROIDCHASEB_LDATA;
extern LEVELDATA *ASTEROIDCHASEC_LDATA;
extern LEVELDATA *ASTEROIDCHASED_LDATA;
extern LEVELDATA *DAGOBAHA_LDATA;
extern LEVELDATA *DAGOBAHB_LDATA;
extern LEVELDATA *DAGOBAHC_LDATA;
extern LEVELDATA *CLOUDCITYTRAPA_LDATA;
extern LEVELDATA *CLOUDCITYTRAPB_LDATA;
extern LEVELDATA *CLOUDCITYTRAPC_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEA_LDATA;
extern LEVELDATA *CLOUDCITYESCAPEC_LDATA;
extern LEVELDATA *JABBASPALACEA_LDATA;
extern LEVELDATA *JABBASPALACEB_LDATA;
extern LEVELDATA *JABBASPALACED_LDATA;
extern LEVELDATA *JABBASPALACEE_LDATA;
extern LEVELDATA *ENDORBATTLEA_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEA_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLED_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEE_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEF_LDATA;
extern LEVELDATA *DEATHSTAR2BATTLEG_LDATA;
extern LEVELDATA *EMPERORFIGHTA_LDATA;
extern LEVELDATA *SARLACCPITA_LDATA;
extern LEVELDATA *SARLACCPITB_LDATA;
extern LEVELDATA *SARLACCPITC_LDATA;
extern LEVELDATA *LEGOCITY_LDATA;
extern LEVELDATA *NEWTOWN_LDATA;
extern LEVELDATA *PLATFORM_LDATA;
extern LEVELDATA *CREDITS_LDATA;
extern i32 new_level_from_menu;
extern i32 BGLOAD;

extern i32 Reflections_On;
extern i32 disable_narrow_socks;
extern i32 script_spline_selected;
extern f32 character_farclip;
extern i32 LevMusicAction;
extern i32 LevMusicAmbient;
extern i32 LevMusicOtherAction;
extern i32 LevMusicOtherAmbient;
extern u32 ResetBits;

extern i32 reset_restart;
extern i32 newlevelfrommenu_newmenuid;
extern i32 newlevelfrommenu_newmenuy;
extern i32 NextArea_FreePlay;

extern i32 FreePlayModelCount;
extern i32 FreePlayResidentCount;
extern i32 FreePlayBonusCount;
extern CHARCAT_s *CharCategory;
extern i32 CHARCATEGORYCOUNT;
extern EXTRAMODELLISTENTRY_s ExtraModelList[];
extern VEHICLECOLLECTION_s VehicleCollection;
extern ARCADEITEM_s ArcadeItem;
extern ARCADE_MODE_s Arcade_Mode[];
extern GAME_CUSTOMISER_s *Game_Customiser;
extern APICHARACTERMODELLIST_s FreePlayModelList[];
extern APICHARACTERMODELLIST_s Hub_ModelList[];

extern i16 temp_yrot;
extern i16 temp_xrot;
extern i32 avg_currentspeed_mul;
extern GameObject_s *player2;
extern GameObject_s *player;
