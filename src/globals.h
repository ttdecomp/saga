#pragma once

#include <stdint.h>

#include "decomp.h"
#include "export.h"

C_API_START

struct episodedata_s {
    int32_t _0;
    int16_t areaIds[10];
    int8_t areaCount;
};
typedef struct episodedata_s EPISODEDATA;

struct cheat_s { /* PlaceHolder Structure */
    char *name;
    undefined field1_0x4;
    undefined field2_0x5;
    undefined field3_0x6;
    undefined field4_0x7;
    byte enabled; /* Created by retype action */
    undefined field6_0x9;
    undefined field7_0xa;
    uint8_t area;
    undefined field9_0xc;
    undefined field10_0xd;
    undefined field11_0xe;
    undefined field12_0xf;
    char *code;
    undefined field14_0x14;
    undefined field15_0x15;
    undefined field16_0x16;
    undefined field17_0x17;
    char *extraName;
    uint flag;
};
typedef struct cheat_s CHEAT;

struct cheatsystem_s {
    CHEAT *cheats;
    int cheatsCount;
    int flags;
    undefined field3_0xc;
    undefined field4_0xd;
    undefined field5_0xe;
    undefined field6_0xf;
};
typedef struct cheatsystem_s CHEATSYSTEM;

extern CHEATSYSTEM CheatSystem;

extern EPISODEDATA *EDataList;

extern int32_t PAL;
extern float FRAMETIME;
extern float DEFAULTFPS;
extern float DEFAULTFRAMETIME;
extern float MAXFRAMETIME;

extern int32_t SUPERBUFFERSIZE;
extern void *permbuffer_base;
extern void *original_permbuffer_base;
extern void *superbuffer_end;
extern void *permbuffer_ptr;
extern void *permbuffer_end;

extern char prodcode[16];
extern char *iconname;
extern char unicodename[64];

extern int32_t EPISODECOUNT;
extern int32_t AREACOUNT;
extern int32_t LEVELCOUNT;

extern GAMESAVE_s Game;

extern int32_t COMPLETIONPOINTS;
extern int32_t POINTS_PER_CHARACTER;
extern int32_t POINTS_PER_SUPERBONUSCOMPLETE;
extern int32_t POINTS_PER_TIMETRIAL;
extern int32_t POINTS_PER_STORY;
extern int32_t POINTS_PER_CHALLENGE;
extern int32_t POINTS_PER_MINIKIT;
extern int32_t POINTS_PER_TRUEJEDI;
extern int32_t POINTS_PER_REDBRICK;
extern int32_t POINTS_PER_MISSION;
extern int32_t POINTS_PER_CHEAT;
extern int32_t POINTS_PER_GOLDBRICK;
extern int32_t BOTHTRUEJEDIGOLDBRICKS;
extern int32_t SHOPGOLDBRICKS;
extern int32_t GOLDBRICKFORSUPERBONUS;
extern int32_t GOLDBRICKFORSUPERSTORY;
extern int32_t GOLDBRICKFORCHALLENGE;
extern int32_t POINTS_PER_SUPERSTORY;
extern int32_t GOLDBRICKPOINTS;

extern int32_t CompletionPointInfo[7];

C_API_END
