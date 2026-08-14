#include "legoapi/cheat.h"

#include "globals.h"
#include "nu2api/nucore/nustring.h"

CHEATSYSTEM CheatSystem = {0};

f32 Cheat_PowerUpTime = 0.0f;
i32 ONEPLAYERPOWERUPS = 0;
extern i32 VehicleArea;

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

u32 Cheat_CheckFlags(i32 cheat_index, u32 flag_mask) {
    if ((cheat_index > -1) && (cheat_index < CheatSystem.cheats_count)) {
        CHEAT target_cheat = CheatSystem.cheats[cheat_index];
        return flag_mask & target_cheat.flag;
    }

    return 0;
}

i32 Cheat_IsOn(i32 cheat) {
    if (cheat >= 0 && cheat < CheatSystem.cheats_count) {
        if (CheatSystem.cheats[cheat].enabled != 0) {
            return 1;
        }
        if (ONEPLAYERPOWERUPS == 0 && 0.0f < Cheat_PowerUpTime) {
            u8 bVar = ((u8 *)&CheatSystem.cheats[cheat].flag)[2];
            if (VehicleArea == 0) {
                bVar &= 1U;
            } else {
                bVar &= 2U;
            }
            if (bVar != 0) {
                return 1;
            }
        }
    }
    return 0;
}
