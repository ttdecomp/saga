#pragma once

#include "globals.h"

#ifdef __cplusplus

void Cheat_SetArea(i32 cheat, i32 areaId);

i32 Cheat_FindByName(char *name);
u32 Cheats_CheckFlags(u32 flag);
u32 Cheat_CheckFlags(i32 cheat_index, u32 flag_mask);
i32 Cheat_IsOn(i32 cheat);
i32 Cheat_PowerUpActive(i32 index);
struct cheatsystem_s {
    CHEAT *cheats;
    i32 cheats_count;
    i32 flags;
    undefined field3_0xc;
    undefined field4_0xd;
    undefined field5_0xe;
    undefined field6_0xf;
};
typedef struct cheatsystem_s CHEATSYSTEM;

#endif
