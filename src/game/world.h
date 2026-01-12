#ifndef GAME_WORLD_H
#define GAME_WORLD_H


struct LEVELDATA_s;
typedef struct LEVELDATA_s LEVELDATA;

struct WORLDINFO_s {
    char filler0[0x128];
    LEVELDATA* current_level;
    char filler1[0xa234];
};

typedef struct WORLDINFO_s WORLDINFO;

extern WORLDINFO WorldInfo;

/// @brief Global pointer to the currently active world info
extern WORLDINFO* WORLD;

#ifdef __cplusplus
void WorldInfo_Activate(void);
void WorldInfo_Init(WORLDINFO *info);
WORLDINFO* WorldInfo_CurrentlyActive(void);
WORLDINFO* WorldInfo_CurrentlyLoading(void);
void WorldInfo_InitOnce(void);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // GAME_WORLD_H