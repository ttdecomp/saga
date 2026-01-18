#pragma once

#include "game/world.h"
#include "globals.h"

struct leveldatadisplay_s { /* PlaceHolder Structure */
    undefined field0_0x0;
    undefined field1_0x1;
    undefined field2_0x2;
    undefined field3_0x3;
    undefined field4_0x4;
    undefined field5_0x5;
    undefined field6_0x6;
    undefined field7_0x7;
    int64_t field8_0x8;
    float field9_0x10;
    short field10_0x14;
    undefined field11_0x16;
    undefined field12_0x17;
    undefined field13_0x18;
    undefined field14_0x19;
    undefined field15_0x1a;
    undefined field16_0x1b;
};
typedef struct leveldatadisplay_s LEVELDATADISPLAY;

enum levelflags_e {
    LEVEL_INTRO = 0x20,
    LEVEL_MIDTRO = 0x40,
    LEVEL_OUTRO = 0x80,
    LEVEL_TEST = 0x200,
    LEVEL_STATUS = 0x400,
    LEVEL_NEWGAME = 0x10000,
    LEVEL_LOADGAME = 0x20000,
};
typedef enum levelflags_e LEVELFLAGS;

struct leveldata_s { /* PlaceHolder Structure */
    char dir[64];
    char name[32]; /* Created by retype action */
    undefined2 field2_0x60;
    undefined2 field3_0x62;
    LEVELFLAGS flags;
    void *loadFn;
    void *initFn;
    void *resetFn; /* Created by retype action */
    void *updateFn;
    void *alwaysUpdateFn;
    void *drawFn;
    void *drawStatusFn;
    LEVELDATADISPLAY field12_0x84;
    short musicIndex;
    undefined2 field14_0xa2;
    undefined2 field15_0xa4;
    undefined2 field16_0xa6;
    undefined2 field17_0xa8;
    undefined2 field18_0xaa;
    undefined1 field19_0xac;
    undefined1 field20_0xad;
    undefined1 field21_0xae;
    undefined1 field22_0xaf;
    undefined4 field23_0xb0;
    undefined4 field24_0xb4;
    undefined1 field25_0xb8;
    undefined1 field26_0xb9;
    undefined1 field27_0xba;
    undefined1 field28_0xbb;
    undefined1 field29_0xbc;
    undefined1 field30_0xbd;
    undefined1 field31_0xbe;
    undefined1 field32_0xbf;
    float field33_0xc0;
    undefined4 field34_0xc4;
    undefined4 field35_0xc8;
    undefined4 field36_0xcc;
    undefined field37_0xd0;
    undefined field38_0xd1;
    undefined field39_0xd2;
    undefined field40_0xd3;
    undefined1 field41_0xd4;
    byte field42_0xd5;
    byte field43_0xd6;
    undefined1 field44_0xd7;
    undefined1 field45_0xd8;
    undefined1 field46_0xd9;
    undefined1 field47_0xda;
    undefined1 field48_0xdb;
    float field49_0xdc;
    float field50_0xe0;
    int field51_0xe4;
    undefined4 field52_0xe8;
    undefined4 field53_0xec;
    undefined4 field54_0xf0;
    undefined1 field55_0xf4;
    undefined1 field56_0xf5;
    undefined1 field57_0xf6;
    undefined1 field58_0xf7;
    undefined1 field59_0xf8;
    undefined1 field60_0xf9;
    undefined1 field61_0xfa;
    byte max_obstacles;
    undefined1 field63_0xfc;
    undefined1 field64_0xfd;
    undefined1 field65_0xfe;
    undefined1 field66_0xff;
    undefined1 field67_0x100;
    undefined1 field68_0x101;
    undefined1 field69_0x102;
    undefined1 field70_0x103;
    undefined1 field71_0x104;
    undefined1 field72_0x105;
    undefined1 field73_0x106;
    undefined1 field74_0x107;
    undefined1 field75_0x108;
    undefined1 field76_0x109;
    undefined1 field77_0x10a;
    undefined1 field78_0x10b;
    undefined1 field79_0x10c;
    undefined1 field80_0x10d;
    undefined1 field81_0x10e;
    undefined1 field82_0x10f;
    undefined1 field83_0x110;
    undefined1 field84_0x111;
    undefined1 field85_0x112;
    byte max_bombgens;
    undefined1 field87_0x114;
    undefined1 field88_0x115;
    undefined field89_0x116;
    undefined field90_0x117;
    undefined2 field91_0x118;
    undefined field92_0x11a;
    undefined field93_0x11b;
    float field94_0x11c;
    float field95_0x120;
    undefined4 field96_0x124;
    undefined4 field97_0x128;
    int musicTracks[6];
};
typedef struct leveldata_s LEVELDATA;

C_API_START

extern LEVELDATA *LDataList;
extern LEVELDATA *NEWGAME_LDATA;
extern LEVELDATA *LOADGAME_LDATA;

C_API_END

#ifdef __cplusplus

LEVELDATA *Levels_ConfigureList(char *file, VARIPTR *buf, VARIPTR *buf_end, int32_t maxLevelCount, int32_t *levelcount,
                                void *levelSetDefaults);

void Level_SetDefaults(LEVELDATA *level);
LEVELDATA *Level_FindByName(char *name, int *indexDest);
void Level_Draw(WORLDINFO *world);

static inline LEVELFLAGS operator|(LEVELFLAGS a, LEVELFLAGS b) {
    return static_cast<LEVELFLAGS>(static_cast<int>(a) | static_cast<int>(b));
}
static inline LEVELFLAGS operator|=(LEVELFLAGS &a, LEVELFLAGS b) {
    a = a | b;
    return a;
}

#endif
