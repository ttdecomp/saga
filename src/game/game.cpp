#include "game/game.h"

WORLDINFO WorldInfo;
WORLDINFO* WORLD = &WorldInfo;

static WORLDINFO* LWORLD = &WorldInfo;

void WorldInfo_Activate(void) {
    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);
}

void WorldInfo_Init(WORLDINFO *info) {
    
}