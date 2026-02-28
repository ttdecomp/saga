#pragma once

#include "legoapi/gizmo.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"


#define INVALID_WORLD_PTR (WORLDINFO *)0xffffd510

typedef struct WORLDINFO_s {
    char filler0[0x104];

    VARIPTR giz_buffer;

    VARIPTR unknown_108;
    i32 unknown_10c;

    i32 unknown_110;

    char filler1[0x8];

    i32 unknown_11c;
    i32 unknown_120;
    i32 unknown_124;

    struct LEVELDATA_s *current_level;
    char filler2[0x10];
    NUGSCN *current_gscn;

    char filler3[0x29b0 - 4];
    int gizmo_count;
    GIZMO gizmos[44];

    struct GIZTIMER_s *giz_timers;
    i32 giz_timers_count;

    char filler5[172];
} WORLDINFO;

#ifdef __cplusplus
extern "C" {
#endif
    extern WORLDINFO WorldInfo[];

    /// @brief Global pointer to the currently active world info
    extern WORLDINFO *WORLD;

    extern f32 g_BgLoadDelayHackTimer;

    extern i32 waiting_for_level;
    extern i32 level_already_loaded;
    extern i32 next_level;
#ifdef __cplusplus
}

void WorldInfo_InitOnce(void);

void WorldInfo_Init(WORLDINFO *world);

void WorldInfo_Load(WORLDINFO *world);
i32 WorldInfo_Reset(WORLDINFO *world, i32 level_idx);

void WorldInfo_Activate(void);
void WorldInfo_StreamLevel(BGPROCINFO *bg_info);

WORLDINFO *WorldInfo_CurrentlyActive(void);
WORLDINFO *WorldInfo_CurrentlyLoading(void);

#endif
