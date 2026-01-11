#ifndef GAME_GAME_H
#define GAME_GAME_H

struct WORLDINFO_s {
    char data[0xa360];
};

typedef struct WORLDINFO_s WORLDINFO;

extern WORLDINFO WorldInfo;
extern WORLDINFO* WORLD;

#ifdef __cplusplus
void WorldInfo_Activate(void);
void WorldInfo_Init(WORLDINFO *info);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // GAME_GAME_H