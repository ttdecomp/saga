#pragma once

#include "globals.h"

#ifdef __cplusplus

// Effect bits for CHEAT::flag, as consulted by the cheat system and gameplay
// code. The low five bits encode the stud-score multipliers; the two power-up
// bits mark a one-shot cheat that is only active while the player is on foot
// or in a vehicle; the turn-off bit marks cheats that may be disabled again.
enum CheatFlag {
    // Stud-score multiplier applied by Cheat_MultiplyScore.
    CHEAT_FLAG_SCORE_X2 = 0x00000004,
    CHEAT_FLAG_SCORE_X4 = 0x00000008,
    CHEAT_FLAG_SCORE_X6 = 0x00000010,
    CHEAT_FLAG_SCORE_X8 = 0x00000020,
    CHEAT_FLAG_SCORE_X10 = 0x00000040,
    // One-shot cheat that only counts while the player is on foot / in a vehicle.
    CHEAT_FLAG_POWERUP_ONFOOT = 0x00010000,
    CHEAT_FLAG_POWERUP_VEHICLE = 0x00020000,
    // The cheat may be switched off again once bought.
    CHEAT_FLAG_CAN_TURNOFF = 0x00200000,
    // Acts as a power-up in both on-foot and vehicle play.
    CHEAT_FLAG_POWERUP = CHEAT_FLAG_POWERUP_ONFOOT | CHEAT_FLAG_POWERUP_VEHICLE,
    // Flag bits shared by every score-multiplier cheat.
    CHEAT_FLAG_SCORE_COMMON = CHEAT_FLAG_CAN_TURNOFF | CHEAT_FLAG_POWERUP,
};
// Gameplay-effect bits for CHEAT::flag. Their consumers in the shop/effect
// code are not reconstructed yet, so each is provisionally named after the
// cheat(s) in Cheat[] that carry it.
enum CheatEffectFlag {
    CHEAT_FLAG_SILHOUETTES = 0x00000001,
    CHEAT_FLAG_SUPERBLASTERS = 0x00000002,
    CHEAT_FLAG_INVINCIBILITY = 0x00000080,
    CHEAT_FLAG_EXTRATOGGLE = 0x00000100,
    CHEAT_FLAG_MINIKIT_DETECTOR = 0x00000200,
    CHEAT_FLAG_SUPERSABRES = 0x00000400,
    CHEAT_FLAG_SUPER_WEAPON = 0x00002000, // superzapper, rockets, superblasters, ...
    CHEAT_FLAG_REGENERATE = 0x00003000,
    CHEAT_FLAG_FAST_BUILD = 0x00004000,
    CHEAT_FLAG_COIN_MAGNET = 0x00008000,
    CHEAT_FLAG_POWERBRICK_DETECTOR = 0x00040000,
    CHEAT_FLAG_DEFLECT_BOLTS = 0x00080000,
    CHEAT_FLAG_PERFECT_DEFLECT = 0x00100000,
    CHEAT_FLAG_FAST_FORCE = 0x00400000,
};

// Global cheat state: the current cheat table and the combined flag mask of
// every cheat that is switched on (cached in `flags`).
struct cheatsystem_s {
    CHEAT *cheats;    /* 0x00  cheat table, name==0 terminated */
    i32 cheats_count; /* 0x04  number of entries in cheats[] */
    u32 flags;        /* 0x08  OR of all currently-active cheat flags */
    u8 pad_0xc[4];    /* 0x0c  trailing alignment */
};
typedef struct cheatsystem_s CHEATSYSTEM;

// CheatSystem and the shared power-up timer are only reachable from this
// module, so they are hidden (non-preemptible) to let the compiler address
// them directly instead of through the GOT. ONEPLAYERPOWERUPS stays a plain
// exported global, matching the original linkage.
extern CHEATSYSTEM CheatSystem __attribute__((visibility("hidden")));
extern f32 Cheat_PowerUpTime __attribute__((visibility("hidden")));
extern i32 ONEPLAYERPOWERUPS;

// Master cheat definitions, NAME==0 terminated (defined in cheats.cpp). This
// is the table the cheat menu types into and the save system loads.
extern CHEAT Cheat[];

struct nuvec_s;
struct GameObject_s;

void Cheats_Init(CHEAT *cheats);
void Cheats_Reset();
void Cheats_Update();
void Cheats_TurnOff(i32 cheat);
void Cheats_SetFlags();

void Cheat_SetOn(i32 cheat, i32 on, i32);
void Cheat_SetArea(i32 cheat, i32 areaId);
i32 Cheat_FindByName(char *name);
u32 Cheats_CheckFlags(u32 flag);
u32 Cheat_CheckFlags(i32 cheat_index, u32 flag_mask);
i32 Cheat_IsOn(i32 cheat);
void Cheat_StartPowerUp(nuvec_s *nuvec, GameObject_s *gameObject);
u32 Cheat_MultiplyScore(u32 score);
i32 Cheat_PowerUpActive(i32 power_up_index);
void Cheat_GetOnOffBitfield(i32 *onoffs, i32 count);
void Cheat_SetOnOffBitfield(i32 *onoffs, i32 count);
i32 FreePlayUnlocked();

#endif
