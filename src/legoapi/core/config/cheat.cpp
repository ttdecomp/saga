#include "legoapi/core/config/cheat.h"

#include "globals.h"
#include "nu2api/nucore/nustring.h"

i32 ONEPLAYERPOWERUPS = 1;
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
    if (cheat < 0 || cheat >= CheatSystem.cheats_count)
        return 0;

    CHEAT *target = &CheatSystem.cheats[cheat];
    if (target->enabled)
        return 1;

    // A one-shot power-up can keep a cheat "on" for a short window (until
    // Cheat_PowerUpTime runs out in Cheats_Update). The power-up only counts
    // when the player is in the matching venue.
    if (ONEPLAYERPOWERUPS != 0)
        return 0;
    if (Cheat_PowerUpTime <= 0.0f)
        return 0;

    if (VehicleArea != 0) {
        if (target->flag & CHEAT_FLAG_POWERUP_VEHICLE)
            return 1;
    } else if (target->flag & CHEAT_FLAG_POWERUP_ONFOOT) {
        return 1;
    }
    return 0;
}
