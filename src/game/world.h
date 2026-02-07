#pragma once

#include "globals.h"

struct LEVELDATA_s;
struct GIZTIMER_s;

struct WORLDINFO_s {
    char filler0[0x104];
    VARIPTR giz_buffer;
    char filler1[0x20];
    struct LEVELDATA_s *current_level;
    char filler2[0x4fd0];
    struct GIZTIMER_s* giz_timers;
    i32 giz_timers_count;
    char filler3[21084];
};

typedef struct WORLDINFO_s WORLDINFO;

extern WORLDINFO WorldInfo;

/// @brief Global pointer to the currently active world info
extern WORLDINFO *WORLD;

#ifdef __cplusplus

void WorldInfo_Activate(void);
void WorldInfo_Init(WORLDINFO *info);
WORLDINFO *WorldInfo_CurrentlyActive(void);
WORLDINFO *WorldInfo_CurrentlyLoading(void);
void WorldInfo_InitOnce(void);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
