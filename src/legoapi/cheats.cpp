#include "legoapi/cheat.h"
#include "legoapi/apiobject.h"

#include <string.h>

struct nuvec_s;
struct nupad_s;

extern CHEATSYSTEM CheatSystem;
extern f32 Cheat_PowerUpTime;
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
    CheatSystem.cheats_count = 0;
    CheatSystem.cheats = cheats;
    if (cheats != 0 && cheats[0].name != 0) {
        i32 count = 0;
        do
            count++;
        while (cheats[count].name != 0);
        CheatSystem.cheats_count = count;
    }
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
                if (p[i].flag & 0x200000)
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
        f32 powerupTime = Cheat_PowerUpTime;
        i32 vehicleArea = VehicleArea;
        if (ONEPLAYERPOWERUPS == 0) {
            if (powerupTime > 0.0009765625f) {
                if (vehicleArea != 0) {
                    for (i32 i = 0; i < count; i++)
                        if (CheatSystem.cheats[i].enabled || (CheatSystem.cheats[i].flag & 0x20000))
                            flags |= CheatSystem.cheats[i].flag;
                } else {
                    for (i32 i = 0; i < count; i++)
                        if (CheatSystem.cheats[i].enabled || (CheatSystem.cheats[i].flag & 0x10000))
                            flags |= CheatSystem.cheats[i].flag;
                }
            } else {
                for (i32 i = 0; i < count; i++)
                    if (CheatSystem.cheats[i].enabled)
                        flags |= CheatSystem.cheats[i].flag;
            }
        } else {
            for (i32 i = 0; i < count; i++)
                if (CheatSystem.cheats[i].enabled)
                    flags |= CheatSystem.cheats[i].flag;
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
    void *m = AddGameMessage(0, nuvec, 0.5f, nuvec, 0.75f, 0xff, 0xff, 0xff, 0x4023, 1.0f);
    if (m)
        *((f32 *)((char *)m + 0xd4)) = 0.75f;
    m = AddGameMessage(0, nuvec, 0.5f, nuvec, 0.25f, 0xff, 0xff, 0xff, 0x4023, 1.0f);
    if (m)
        *((f32 *)((char *)m + 0xd4)) = 0.75f;
    GameAudio_PlaySfx(0x50, 0, 0, 0);
}

u32 Cheat_MultiplyScore(u32 s) {
    s = Cheats_CheckFlags(0x4) ? s * 2 : s;
    s = Cheats_CheckFlags(0x8) ? s * 4 : s;
    s = Cheats_CheckFlags(0x10) ? s * 6 : s;
    s = Cheats_CheckFlags(0x20) ? s * 8 : s;
    s = Cheats_CheckFlags(0x40) ? s * 10 : s;
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

void FreePlayUnlocked() {
}
