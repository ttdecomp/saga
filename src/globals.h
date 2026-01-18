#pragma once

#include <stdint.h>

#include "game/mission.h"

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

C_API_START

typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned char byte;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

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
    struct OPTIONSSAVE_s optionsSave;
    undefined field5_0x10;
    undefined1 levelSave; /* Created by retype action */
    undefined field7_0x12[30746];
    undefined1 areaSave; /* Created by retype action */
    undefined field9_0x782d;
    undefined field10_0x782e;
    undefined field11_0x782f;
    undefined field12_0x7830;
    undefined field13_0x7831;
    undefined field14_0x7832;
    undefined field15_0x7833;
    float field16_0x7834;
    char field17_0x7838[852];
    undefined1 episodeSave; /* Created by retype action */
    char field19_0x7b8d[107];
    uint field20_0x7bf8[1];
    char field21_0x7bfc[36];
    uint coins;
    uint16_t completion; /* Created by retype action */
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
    MISSIONSAVE missionSave;
    char field37_0x7ca4[88];
    void *characterSave;
    char field39_0x7d08[336];
};

extern CHEATSYSTEM CheatSystem;

extern int32_t PAL;
extern float FRAMETIME;
extern float DEFAULTFPS;
extern float DEFAULTFRAMETIME;
extern float MAXFRAMETIME;

extern int32_t SUPERBUFFERSIZE;
extern VARIPTR permbuffer_base;
extern VARIPTR original_permbuffer_base;
extern VARIPTR superbuffer_end;
extern VARIPTR permbuffer_ptr;
extern VARIPTR permbuffer_end;

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
