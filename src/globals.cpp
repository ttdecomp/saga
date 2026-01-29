#include "globals.h"
#include "nu2api.saga/nucore/common.h"

int32_t PAL = 0;
float FRAMETIME = 0;
float DEFAULTFPS = 0;
float DEFAULTFRAMETIME = 0;
float MAXFRAMETIME = 0;

int32_t SUPERBUFFERSIZE = 0x2EB8EEB;
VARIPTR permbuffer_base;
VARIPTR original_permbuffer_base;
VARIPTR superbuffer_end;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;

char prodcode[16] = {0};
char *iconname = "lego.ico";
char unicodename[64] = "LEGO Star Wars Saga";

CHEATSYSTEM CheatSystem = {0};

int32_t EPISODECOUNT = 0;
int32_t AREACOUNT = 0;
int32_t LEVELCOUNT = 0;

GAMESAVE_s Game = {0};

int32_t COMPLETIONPOINTS = 0;
int32_t POINTS_PER_CHARACTER = 0;
int32_t POINTS_PER_SUPERBONUSCOMPLETE = 0;
int32_t POINTS_PER_TIMETRIAL = 0;
int32_t POINTS_PER_STORY = 0;
int32_t POINTS_PER_CHALLENGE = 0;
int32_t POINTS_PER_MINIKIT = 0;
int32_t POINTS_PER_TRUEJEDI = 0;
int32_t POINTS_PER_REDBRICK = 0;
int32_t POINTS_PER_MISSION = 0;
int32_t POINTS_PER_CHEAT = 0;
int32_t POINTS_PER_GOLDBRICK = 0;
int32_t BOTHTRUEJEDIGOLDBRICKS = 0;
int32_t SHOPGOLDBRICKS = 0;
int32_t GOLDBRICKFORSUPERBONUS = 0;
int32_t GOLDBRICKFORSUPERSTORY = 0;
int32_t GOLDBRICKFORCHALLENGE = 0;
int32_t POINTS_PER_SUPERSTORY = 0;
int32_t GOLDBRICKPOINTS = 0;

int32_t CompletionPointInfo[7] = {0};

nusound_filename_info_s *MusicInfo = NULL;

nucamera_s *pNuCam = NULL;