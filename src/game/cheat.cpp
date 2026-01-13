#include "game/cheat.h"

#include "nu2api.saga/nucore/nustring.h"

void Cheat_SetArea(int32_t cheat, int32_t areaId) {
    if (cheat >= 0 && cheat < CheatSystem.cheatsCount && areaId >= 0 && areaId < AREACOUNT) {
        CheatSystem.cheats[cheat].area = areaId;
    }
}

int Cheat_FindByName(char *name) {
    int iVar1;
    int i;

    i = 0;
    if (0 < CheatSystem.cheatsCount) {
        do {
            iVar1 = NuStrICmp(CheatSystem.cheats[i].name, name);
            if (iVar1 == 0) {
                return i;
            }
            i = i + 1;
        } while (i < CheatSystem.cheatsCount);
    }
    return -1;
}
