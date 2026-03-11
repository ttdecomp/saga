#include "legoapi/cheat.h"

#include "nu2api/nucore/nustring.h"

CHEATSYSTEM CheatSystem = {0};

void Cheat_SetArea(i32 cheat, i32 areaId) {
    if (cheat >= 0 && cheat < CheatSystem.cheats_count && areaId >= 0 && areaId < AREACOUNT) {
        CheatSystem.cheats[cheat].area = areaId;
    }
}

int Cheat_FindByName(char *name) {
    int iVar1;
    int i;

    i = 0;
    if (0 < CheatSystem.cheats_count) {
        do {
            iVar1 = NuStrICmp(CheatSystem.cheats[i].name, name);
            if (iVar1 == 0) {
                return i;
            }
            i = i + 1;
        } while (i < CheatSystem.cheats_count);
    }
    return -1;
}

u32 Cheats_CheckFlags(u32 flag) {
    return CheatSystem.flags & flag;
}

u32 Cheat_CheckFlags(int cheat_index, u32 flag_mask) {
    if ((cheat_index > -1) && (cheat_index < CheatSystem.cheats_count)) {
        CHEAT target_cheat = CheatSystem.cheats[cheat_index];
        return flag_mask & target_cheat.flag;
    }
    return 0;
}