#include <stddef.h>

#include "globals.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nusound/nusound.h"

i32 PAL = 0;
f32 FRAMETIME = 0;
f32 DEFAULTFPS = 0;
f32 DEFAULTFRAMETIME = 0;
f32 MAXFRAMETIME = 0;

i32 SUPERBUFFERSIZE = 0x2EB8EEB;
VARIPTR permbuffer_base;
VARIPTR original_permbuffer_base;
VARIPTR superbuffer_end;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;

char prodcode[16] = {0};
char *iconname = "lego.ico";
char unicodename[64] = "LEGO Star Wars Saga";

CHEATSYSTEM CheatSystem = {0};

i32 EPISODECOUNT = 0;
i32 AREACOUNT = 0;

GAMESAVE_s Game = {0};

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

NUSOUND_FILENAME_INFO *MusicInfo = NULL;
NUSOUND_FILENAME_INFO *g_music = NULL;
u8 g_BackgroundUsedFogColour = 0;
u32 SFX_MUSIC_COUNT = 0;

NUCAMERA *pNuCam = NULL;

i32 NOSOUND = 0;

const char *theEmptyString = "";

ANativeWindow *g_appWindow = NULL;

char g_deviceManufacturer[256] = {0};
char g_deviceModel[256] = {0};

i32 g_forceSysMemVbs = 0;

i32 g_forceETC1 = 0;

i32 g_isLowestEndDevice = 0;
i32 g_isLowEndDevice = 0;
i32 g_isMidRangeDevice = 0;
i32 g_lowEndLevelBehaviour = 0;
