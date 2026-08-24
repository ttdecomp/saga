#include "legoapi/core/config/cheat.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"

#include <string.h>

struct nuvec_s;
struct nupad_s;

CHEATSYSTEM CheatSystem = {0};
f32 Cheat_PowerUpTime = 0.0f;

// Routines elsewhere in the engine set these to reflect which cheats the
// player currently has switched on; `Cheat` points each entry at its byte.
u8 tCHEAT_BEEPBEEP = 0;
u8 tCHEAT_C3POBITS = 0;
u8 tCHEAT_CHARACTERSTUDS = 0;
u8 tCHEAT_COINMAGNET = 0;
u8 tCHEAT_DAISYCHAINS = 0;
u8 tCHEAT_DARKSIDE = 0;
u8 tCHEAT_DEFLECTBOLTS = 0;
u8 tCHEAT_DISARMTROOPERS = 0;
u8 tCHEAT_DISGUISE = 0;
u8 tCHEAT_EXPLODINGBLASTERBOLTS = 0;
u8 tCHEAT_EXTRATOGGLE = 0;
u8 tCHEAT_FASTBUILD = 0;
u8 tCHEAT_FASTFORCE = 0;
u8 tCHEAT_FORCEPULL = 0;
u8 tCHEAT_FORCEZIPUP = 0;
u8 tCHEAT_INFINITETORPEDOS = 0;
u8 tCHEAT_INVINCIBILITY = 0;
u8 tCHEAT_MINIKITDETECTOR = 0;
u8 tCHEAT_PERFECTDEFLECT = 0;
u8 tCHEAT_POO = 0;
u8 tCHEAT_POOMONEY = 0;
u8 tCHEAT_POWERBRICKDETECTOR = 0;
u8 tCHEAT_PURPLEFORCE = 0;
u8 tCHEAT_REGENERATE = 0;
u8 tCHEAT_ROCKETS = 0;
u8 tCHEAT_SELFDESTRUCT = 0;
u8 tCHEAT_SILHOUETTES = 0;
u8 tCHEAT_SUPERASTROMECH = 0;
u8 tCHEAT_SUPERBLASTERS = 0;
u8 tCHEAT_SUPEREWOKCATAPULT = 0;
u8 tCHEAT_SUPERGONK = 0;
u8 tCHEAT_SUPERJEDISLAM = 0;
u8 tCHEAT_SUPERSLAP = 0;
u8 tCHEAT_SUPERTHERMALDETONATOR = 0;
u8 tCHEAT_SUPERZAPPER = 0;
u8 tCHEAT_TOWDEATHSTAR = 0;
u8 tCHEAT_TRACTORBEAM = 0;
u8 tCHEAT_VEHICLESMARTBOMB = 0;
u8 tCHEAT_WALKIETALKIEDISABLE = 0;
u8 tCHEAT_X10 = 0;
u8 tCHEAT_X2 = 0;
u8 tCHEAT_X4 = 0;
u8 tCHEAT_X6 = 0;
u8 tCHEAT_X8 = 0;

// The master cheat list, terminated by a NAME == NULL entry. `cost` is the
// stud price in the extras/shop menu, `code` is the string the player types
// in the cheat-menu code box, and `flag` is the effect bitmask (CheatFlag).
// Passed to Cheats_Init() by LoadPermData() at startup.
CHEAT Cheat[] = {
    {"extratoggle", &tCHEAT_EXTRATOGGLE, 0, {0}, -1, -1, "VX8FZ6", 25000, "shop_tool1", CHEAT_FLAG_EXTRATOGGLE},
    {"poo", &tCHEAT_POO, 0, {0}, -1, -1, "F4035S", 8000, "shop_tool2", 0},
    {"disguises", &tCHEAT_DISGUISE, 0, {0}, -1, -1, "4HBIHK", 10000, "shop_tool3", 0},
    {"daisychains", &tCHEAT_DAISYCHAINS, 0, {0}, -1, -1, "2J0VGC", 5000, "shop_tool4", 0},
    {"c3pobits", &tCHEAT_C3POBITS, 0, {0}, -1, -1, "RTI8PX", 10000, "shop_tool5", 0},
    {"towdeathstar", &tCHEAT_TOWDEATHSTAR, 0, {0}, -1, -1, "YDQYJR", 5000, "shop_tool6", 0},
    {"silhouettes", &tCHEAT_SILHOUETTES, 0, {0}, -1, -1, "J2ZFT7", 10000, "shop_silhouette", CHEAT_FLAG_SILHOUETTES},
    {"beepbeep", &tCHEAT_BEEPBEEP, 0, {0}, -1, -1, "V1VV95", 7500, "shop_beepbeep", 0},
    {"supergonk", &tCHEAT_SUPERGONK, 0, {0}, -1, -1, "K0HOMF", 50000, "shop_supergonk", 0},
    {"poomoney", &tCHEAT_POOMONEY, 0, {0}, -1, -1, "1J2G9Z", 50000, "shop_poomoney", CHEAT_FLAG_CAN_TURNOFF},
    {"walkietalkiedisable", &tCHEAT_WALKIETALKIEDISABLE, 0, {0}, -1, -1, "9DIHAR", 5000, "shop_walkietalkie", 0},
    {"powerbrickdetector",
     &tCHEAT_POWERBRICKDETECTOR,
     0,
     {0},
     -1,
     -1,
     "0SY75X",
     62500,
     "shop_powerbrick",
     CHEAT_FLAG_POWERBRICK_DETECTOR},
    {"superslap", &tCHEAT_SUPERSLAP, 0, {0}, -1, -1, "YHXD63", 5000, "shop_superslap", 0},
    {"forcezipup", &tCHEAT_FORCEZIPUP, 0, {0}, -1, -1, "GPHF03", 15000, "shop_forcegrapple", 0},
    {"coinmagnet",
     &tCHEAT_COINMAGNET,
     0,
     {0},
     -1,
     -1,
     "FE0YXY",
     100000,
     "shop_coinmagnet",
     CHEAT_FLAG_SCORE_COMMON | CHEAT_FLAG_COIN_MAGNET},
    {"disarmtroopers", &tCHEAT_DISARMTROOPERS, 0, {0}, -1, -1, "D0FSCN", 50000, "shop_disarmtroopers", 0},
    {"characterstuds",
     &tCHEAT_CHARACTERSTUDS,
     0,
     {0},
     -1,
     -1,
     "PSA7PM",
     75000,
     "shop_characterstuds",
     CHEAT_FLAG_CAN_TURNOFF},
    {"perfectdeflect",
     &tCHEAT_PERFECTDEFLECT,
     0,
     {0},
     -1,
     -1,
     "HTWHIS",
     20000,
     "shop_perfectdeflect",
     CHEAT_FLAG_PERFECT_DEFLECT},
    {"explodingblasterbolts", &tCHEAT_EXPLODINGBLASTERBOLTS, 0, {0}, -1, -1, "GYL04B", 20000, "shop_explodingbolts", 0},
    {"forcepull", &tCHEAT_FORCEPULL, 0, {0}, -1, -1, "7XR7Z1", 12000, "shop_forcepull", 0},
    {"vehiclesmartbomb", &tCHEAT_VEHICLESMARTBOMB, 0, {0}, -1, -1, "OYS2JP", 15000, "shop_vehiclesmart", 0},
    {"superastromech", &tCHEAT_SUPERASTROMECH, 0, {0}, -1, -1, "0OJO5O", 10000, "shop_superastromech", 0},
    {"superjedislam", &tCHEAT_SUPERJEDISLAM, 0, {0}, -1, -1, "6DT2CK", 11000, "shop_superjedislam", 0},
    {"superthermaldetonator", &tCHEAT_SUPERTHERMALDETONATOR, 0, {0}, -1, -1, "ZURPDI", 25000, "shop_superthermal", 0},
    {"deflectbolts",
     &tCHEAT_DEFLECTBOLTS,
     0,
     {0},
     -1,
     -1,
     "TPF1LL",
     100000,
     "shop_deflectbolts",
     CHEAT_FLAG_DEFLECT_BOLTS | CHEAT_FLAG_POWERUP},
    {"darkside", &tCHEAT_DARKSIDE, 0, {0}, -1, -1, "3O1DUB", 25000, "shop_darkside", 0},
    {"superblasters",
     &tCHEAT_SUPERBLASTERS,
     0,
     {0},
     -1,
     -1,
     "CRX1LO",
     15000,
     "shop_tool7",
     CHEAT_FLAG_POWERUP | CHEAT_FLAG_SUPER_WEAPON | CHEAT_FLAG_SUPERBLASTERS},
    {"fastforce",
     &tCHEAT_FASTFORCE,
     0,
     {0},
     -1,
     -1,
     "QTDYVG",
     40000,
     "shop_tool8",
     CHEAT_FLAG_FAST_FORCE | CHEAT_FLAG_POWERUP_ONFOOT | CHEAT_FLAG_SUPER_WEAPON},
    {"supersabres",
     &tCHEAT_PURPLEFORCE,
     0,
     {0},
     -1,
     -1,
     "3X7303",
     40000,
     "shop_tool9",
     CHEAT_FLAG_POWERUP_ONFOOT | CHEAT_FLAG_SUPER_WEAPON | CHEAT_FLAG_SUPERSABRES},
    {"tractorbeam", &tCHEAT_TRACTORBEAM, 0, {0}, -1, -1, "LEMPCP", 15000, "shop_tool10", CHEAT_FLAG_POWERUP_VEHICLE},
    {"invincibility",
     &tCHEAT_INVINCIBILITY,
     0,
     {0},
     -1,
     -1,
     "4T2X0G",
     500000,
     "shop_tool11",
     CHEAT_FLAG_POWERUP | CHEAT_FLAG_SUPER_WEAPON | CHEAT_FLAG_INVINCIBILITY},
    {"scorex2",
     &tCHEAT_X2,
     0,
     {0},
     -1,
     -1,
     "R26FOR",
     625000,
     "shop_tool13",
     CHEAT_FLAG_SCORE_COMMON | CHEAT_FLAG_SCORE_X2},
    {"selfdestruct", &tCHEAT_SELFDESTRUCT, 0, {0}, -1, -1, "5VPUHC", 25000, "shop_tool12", 0},
    {"fastbuild",
     &tCHEAT_FASTBUILD,
     0,
     {0},
     -1,
     -1,
     "E7ZZLD",
     30000,
     "shop_tool14",
     CHEAT_FLAG_POWERUP_ONFOOT | CHEAT_FLAG_FAST_BUILD | CHEAT_FLAG_SUPER_WEAPON},
    {"scorex4",
     &tCHEAT_X4,
     0,
     {0},
     -1,
     -1,
     "YNHS94",
     1250000,
     "shop_tool15",
     CHEAT_FLAG_CAN_TURNOFF | CHEAT_FLAG_SCORE_X4},
    {"regenerate",
     &tCHEAT_REGENERATE,
     0,
     {0},
     -1,
     -1,
     "JEHZU6",
     150000,
     "shop_tool16",
     CHEAT_FLAG_POWERUP | CHEAT_FLAG_REGENERATE},
    {"minikitdetector",
     &tCHEAT_MINIKITDETECTOR,
     0,
     {0},
     -1,
     -1,
     "WGXC8Y",
     125000,
     "shop_tool17",
     CHEAT_FLAG_MINIKIT_DETECTOR},
    {"scorex6",
     &tCHEAT_X6,
     0,
     {0},
     -1,
     -1,
     "YW9S6L",
     2500000,
     "shop_tool18",
     CHEAT_FLAG_CAN_TURNOFF | CHEAT_FLAG_SCORE_X6},
    {"superzapper", &tCHEAT_SUPERZAPPER, 0, {0}, -1, -1, "2T7WBD", 14000, "shop_tool19", CHEAT_FLAG_SUPER_WEAPON},
    {"rockets", &tCHEAT_ROCKETS, 0, {0}, -1, -1, "1P9CGR", 20000, "shop_tool20", CHEAT_FLAG_SUPER_WEAPON},
    {"scorex8",
     &tCHEAT_X8,
     0,
     {0},
     -1,
     -1,
     "VYWFEV",
     5000000,
     "shop_tool21",
     CHEAT_FLAG_CAN_TURNOFF | CHEAT_FLAG_SCORE_X8},
    {"superewokcatapult", &tCHEAT_SUPEREWOKCATAPULT, 0, {0}, -1, -1, "ALQFGF", 25000, "shop_tool22", 0},
    {"infinitetorpedos", &tCHEAT_INFINITETORPEDOS, 0, {0}, -1, -1, "3Z3AFX", 25000, "shop_tool23", 0},
    {"scorex10",
     &tCHEAT_X10,
     0,
     {0},
     -1,
     -1,
     "OC7VIL",
     10000000,
     "shop_tool24",
     CHEAT_FLAG_CAN_TURNOFF | CHEAT_FLAG_SCORE_X10},

    // NAME == NULL terminator (Cheats_Init stops counting here).
    {0},
};

extern i32 ONEPLAYERPOWERUPS;
extern i32 VehicleArea;
extern f32 FRAMETIME;
extern GameObject_s *Player[8];

void Cheats_SetFlags();
void GameAudio_PlaySfx(i32, nuvec_s *, i32, i32);
void ConstantRumble(GameObject_s *, float, float);
void NewRumble(nupad_s *, float, int);
void GameCam_HitRoll();
void *AddGameMessage(char *, nuvec_s *, float, nuvec_s *, float, unsigned char, unsigned char, unsigned char, u32,
                     float);
void NewRumbleAllPlayers(float, float, int, int);
i32 qrand();

void Cheat_SetOn(i32 cheat, i32 on, i32) {
    if (cheat < 0 || cheat >= CheatSystem.cheats_count)
        return;
    CheatSystem.cheats[cheat].enabled = on != 0;
    Cheats_SetFlags();
}

void Cheats_Init(CHEAT *cheats) {
    CheatSystem.cheats = cheats;
    CheatSystem.cheats_count = 0;
    if (cheats == 0)
        return;

    // Count entries up to (but not including) the NULL-name terminator.
    for (CHEAT *entry = cheats; entry->name != 0; entry++)
        CheatSystem.cheats_count++;
}

void Cheats_Reset() {
    Cheat_PowerUpTime = 0.0f;
    Cheats_SetFlags();
}

void Cheats_Update() {
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        Cheat_PowerUpTime -= FRAMETIME;
        if (Cheat_PowerUpTime <= 0.0f)
            GameAudio_PlaySfx(0x52, 0, 0, 0);
        else {
            GameAudio_PlaySfx(0x51, 0, 0, 0);
            ConstantRumble(0, qrand() * 0.0009765625f * 0.03125f, 0.0f);
        }
    }
    Cheats_SetFlags();
}

void Cheats_TurnOff(i32 cheat) {
    i32 count = CheatSystem.cheats_count;
    if (count > 0) {
        CHEAT *p = CheatSystem.cheats;
        if (cheat != 0) {
            for (i32 i = 0; i < count; i++) {
                if (p[i].flag & CHEAT_FLAG_CAN_TURNOFF)
                    p[i].enabled = 0;
            }
        } else {
            for (i32 i = 0; i < count; i++)
                p[i].enabled = 0;
        }
    }
    Cheats_SetFlags();
}

void Cheats_SetFlags() {
    i32 count = CheatSystem.cheats_count;
    CheatSystem.flags = 0;
    if (count > 0) {
        i32 flags = 0;
        CHEAT *const cheats = CheatSystem.cheats;
        f32 powerupTime = Cheat_PowerUpTime;
        i32 vehicleArea = VehicleArea;
        if (ONEPLAYERPOWERUPS == 0) {
            if (powerupTime > 0.0009765625f) {
                if (vehicleArea == 0) {
                    for (i32 i = 0; i < count; i++)
                        if (cheats[i].enabled || (cheats[i].flag & CHEAT_FLAG_POWERUP_ONFOOT))
                            flags |= cheats[i].flag;
                } else {
                    for (i32 i = 0; i < count; i++)
                        if (cheats[i].enabled || (cheats[i].flag & CHEAT_FLAG_POWERUP_VEHICLE))
                            flags |= cheats[i].flag;
                }
            } else {
                for (i32 i = 0; i < count; i++)
                    if (cheats[i].enabled)
                        flags |= cheats[i].flag;
            }
        } else {
            for (i32 i = 0; i < count; i++)
                if (cheats[i].enabled)
                    flags |= cheats[i].flag;
        }
        CheatSystem.flags = flags;
    }
}

void Cheat_StartPowerUp(nuvec_s *nuvec, GameObject_s *gameObject) {
    if (ONEPLAYERPOWERUPS != 0) {
        if (gameObject == 0)
            return;
        gameObject->field_0xdec = 2.0f;
        NewRumble(*(nupad_s **)(*(void **)((char *)gameObject + 0xc94)), 0.7f, 0);
        GameCam_HitRoll();
    } else {
        NewRumbleAllPlayers(0.7f, 0, 0, 0);
        Cheat_PowerUpTime = 2.0f;
    }
    GAMEMESSAGE_s *m = (GAMEMESSAGE_s *)AddGameMessage(0, nuvec, 0.5f, nuvec, 0.75f, 0xff, 0xff, 0xff, 0x4023, 1.0f);
    if (m)
        m->scale = 0.75f;
    m = (GAMEMESSAGE_s *)AddGameMessage(0, nuvec, 0.5f, nuvec, 0.25f, 0xff, 0xff, 0xff, 0x4023, 1.0f);
    if (m)
        m->scale = 0.75f;
    GameAudio_PlaySfx(0x50, 0, 0, 0);
}

u32 Cheat_MultiplyScore(u32 s) {
    if (Cheats_CheckFlags(CHEAT_FLAG_SCORE_X2))
        s *= 2;
    if (Cheats_CheckFlags(CHEAT_FLAG_SCORE_X4))
        s *= 4;
    if (Cheats_CheckFlags(CHEAT_FLAG_SCORE_X6))
        s *= 6;
    if (Cheats_CheckFlags(CHEAT_FLAG_SCORE_X8))
        s *= 8;
    if (Cheats_CheckFlags(CHEAT_FLAG_SCORE_X10))
        s *= 10;

    return s;
}

i32 Cheat_PowerUpActive(i32 index) {
    if (ONEPLAYERPOWERUPS != 0) {
        if ((u32)index <= 1) {
            if (Player[0] && Player[0]->field_0xdec > 0.0009765625f && Player[0]->apiobj.field_0x27c == index)
                return 1;
            if (Player[1] && Player[1]->field_0xdec > 0.0009765625f && Player[1]->apiobj.field_0x27c == index)
                return 1;
        }
        return 0;
    }
    if (Cheat_PowerUpTime > 0.0009765625f)
        return 1;
    return 0;
}

void Cheat_GetOnOffBitfield(i32 *onoffs, i32 count) {
    memset(onoffs, 0, ((count + 31) / 32) << 2);
    for (i32 i = 0; i < count; i++) {
        if (CheatSystem.cheats[i].enabled) {
            onoffs[i >> 5] |= 1 << i;
        }
    }
}

void Cheat_SetOnOffBitfield(i32 *onoffs, i32 count) {
    for (i32 i = 0; i < count; i++) {
        CheatSystem.cheats[i].enabled = ((onoffs[i >> 5] >> i) & 1) ? 1 : 0;
    }
}

i32 FreePlayUnlocked() {
    return 1;
}
