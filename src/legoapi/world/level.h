#pragma once

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"

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

#endif
