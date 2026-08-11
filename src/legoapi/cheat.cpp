#include "legoapi/cheat.h"

#include "nu2api/nucore/nustring.h"

CHEATSYSTEM CheatSystem = {0};

i32 Cheat_PowerUpActive(i32) {
    return 0;
}

void Cheat_SetArea(i32 cheat, i32 areaId) {
    if (cheat >= 0 && cheat < CheatSystem.cheats_count && areaId >= 0 && areaId < AREACOUNT) {
        CheatSystem.cheats[cheat].area = areaId;
    }
}

i32 Cheat_FindByName(char *name) {
    for (i32 i = 0; i < CheatSystem.cheats_count; i++) {
        if (NuStrICmp(CheatSystem.cheats[i].name, name) == 0) {
            return i;
        }
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

i32 Cheat_IsOn(i32 cheat) {
    if (cheat < 0 || cheat >= CheatSystem.cheats_count || CheatSystem.cheats == NULL) {
        return 0;
    }
    return CheatSystem.cheats[cheat].enabled != 0;
}
