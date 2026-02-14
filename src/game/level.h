#pragma once

#include "game/world.h"
#include "nu2api.saga/nucore/common.h"

typedef struct leveldatadisplay_s {
    f32 unknown_00;
    f32 unknown_04;

    f32 level_width;
    f32 level_depth;

    f32 unknown_10;
    i16 unknown_14;

    char unknown_16;
    char unknown_17;
    char unknown_18;
    char unknown_19;
    char unknown_1a;
    char unknown_1b;
} LEVELDATADISPLAY;

enum {
    LEVEL_UNKNOWN_FLAG_2 = 1 << 1,
    LEVEL_UNKNOWN_FLAG_8 = 1 << 3,
    LEVEL_INTRO = 1 << 5,
    LEVEL_MIDTRO = 1 << 6,
    LEVEL_OUTRO = 1 << 7,
    LEVEL_TEST = 1 << 9,
    LEVEL_STATUS = 1 << 10,
    LEVEL_NEWGAME = 1 << 16,
    LEVEL_LOADGAME = 1 << 17,
};

typedef struct LEVELDATA_s {
    char dir[0x40];
    char name[0x20];

    i16 unknown_060;

    i16 idx;

    u32 flags;

    void *load_fn;
    void *init_fn;
    void *reset_fn;
    void *update_fn;
    void *always_update_fn;
    void *draw_fn;
    void *draw_status_fn;

    LEVELDATADISPLAY data_display;

    i16 music_index;

    i16 unknown_0a2;
    i16 unknown_0a4;
    i16 unknown_0a6;
    i16 unknown_0a8;
    i16 unknown_0aa;

    char unknown_0ac;
    char unknown_0ad;
    char unknown_0ae;
    char unknown_0af;

    i32 unknown_0b0;
    i32 unknown_0b4;

    char unknown_0b8;
    char unknown_0b9;
    char unknown_0ba;
    char unknown_0bb;
    char unknown_0bc;
    char unknown_0bd;
    char unknown_0be;
    char unknown_0bf;

    f32 unknown_0c0;
    f32 unknown_0c4;
    f32 unknown_0c8;
    f32 unknown_0cc;

    char unknown_0d0;
    char unknown_0d1;
    char unknown_0d2;
    char unknown_0d3;

    char unknown_0d4;
    char unknown_0d5;
    char unknown_0d6;
    char unknown_0d7;
    char unknown_0d8;
    char unknown_0d9;
    char unknown_0da;
    char unknown_0db;

    f32 unknown_0dc;
    f32 unknown_0e0;

    u16 max_antinodes;
    u16 max_gizmo_blowups;
    u16 max_gizmo_blowup_types;

    u16 unknown_0ea;
    u16 unknown_0ec;
    u16 unknown_0ee;
    u16 unknown_0f0;
    u16 unknown_0f2;

    u8 field55_0xf4;

    u8 max_giz_timers;

    u8 field57_0xf6;
    u8 field58_0xf7;
    u8 field59_0xf8;
    u8 field60_0xf9;
    u8 field61_0xfa;

    u8 max_obstacles;

    u8 field63_0xfc;
    u8 field64_0xfd;
    u8 field65_0xfe;
    u8 field66_0xff;
    u8 field67_0x100;
    u8 field68_0x101;
    u8 field69_0x102;
    u8 field70_0x103;
    u8 field71_0x104;
    u8 field72_0x105;
    u8 field73_0x106;
    u8 field74_0x107;
    u8 field75_0x108;
    u8 field76_0x109;
    u8 field77_0x10a;
    u8 field78_0x10b;
    u8 field79_0x10c;
    u8 field80_0x10d;
    u8 field81_0x10e;
    u8 field82_0x10f;
    u8 field83_0x110;
    u8 field84_0x111;
    u8 field85_0x112;
    u8 max_bombgens;

    u8 field87_0x114;
    u8 field88_0x115;
    u8 field89_0x116;
    u8 field90_0x117;
    i16 field91_0x118;
    u8 field92_0x11a;
    u8 field93_0x11b;

    f32 unknown_11c;
    f32 unknown_120;

    i32 field96_0x124;
    i32 field97_0x128;

    i32 music_tracks[3][2];
} LEVELDATA;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA *LDataList;
    extern LEVELDATA *NEWGAME_LDATA;
    extern LEVELDATA *LOADGAME_LDATA;
#ifdef __cplusplus
}

typedef void LEVELSETDEFAULTSFN(LEVELDATA *);

LEVELDATA *Levels_ConfigureList(char *file, VARIPTR *buf, VARIPTR *buf_end, i32 max_level_count, i32 *level_count_out,
                                LEVELSETDEFAULTSFN *set_defaults_fn);

void Level_SetDefaults(LEVELDATA *level);
LEVELDATA *Level_FindByName(char *name, int *idx_out);
void Level_Draw(WORLDINFO *world);

#endif
