#include "decomp.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/mission.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/world/world_shared.h"

extern "C" void PlaySfx(char *, i32);

extern i32 GetMenuID(void);
extern void Cheats_TurnOff(i32);
extern void SetBonusWinner(i32);
extern void ResetGameMessages(void);
extern GameObject_s *FindGameObject(i32, u32, i32, i32, i32);
extern i32 Collection_Got(i32 id);

void EndMission(MISSIONSYS *ms, i32 param1, i32 param2) {
    if (netclient == 0 || param2 == 0) {
        ms->field8_0x1d = (u8)param1;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] != NULL && *(i8 *)((char *)Player[BonusWinner] + 0x1f8) < 0) {
        } else {
            BonusWinner = (BonusWinner == 0);
        }
        BonusWinFlag = 0;
        NewMenu(9, -1, -1);
        ResetGameMessages();
        if (param1 == 3) {
            PlaySfx("MK-Panel", 0);
        }
    }
}

void InitMission(MISSIONSYS *ms, i32 idx) {
    ms->field8_0x1d = 1;
    ms->mission = (MISSIONDATA *)(ms->length + idx * 24);
    ResetTimer(&ms->timer, 0.0f);
    Cheats_TurnOff(0);
}

void EndChallenge(i32 param1, i32 param2) {
    if (netclient == 0 || param2 == 0) {
        ChallengeMode = param1;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] != NULL && *(i8 *)((char *)Player[BonusWinner] + 0x1f8) < 0) {
        } else {
            BonusWinner = (BonusWinner == 0);
        }
        BonusWinFlag = 0;
        NewMenu(10, -1, -1);
        ResetGameMessages();
        if (param1 == 3) {
            PlaySfx("MK-Panel", 0);
        }
    }
}

void InitChallenge(i32) {
    ChallengeMode = 1;
    ResetTimer(&ChallengeTimer, 0.0f);
    Cheats_TurnOff(0);
    *(i32 *)((char *)&AreaGlobals + 0x1c) = 0;
}

void Mission_Clear(MISSIONSYS *ms) {
    if (ms == NULL) {
        ms = MissionSys;
        if (ms == NULL) {
            return;
        }
    }
    ms->mission = NULL;
    ms->field8_0x1d = 0;
    ResetTimer(&ms->timer, 0.0f);
}

i32 Mission_Active(MISSIONSYS *ms) {
    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        return 0;
    }
    if (ms->field8_0x1d == 0) {
        return 0;
    }
    return (i32)(usize)ms->mission;
}

void CheckMissionEnd(MISSIONSYS *ms) {
    GameObject_s *player;
    GameObject_s *obj;
    u32 lo;
    u32 hi;
    i32 i;

    if (ms->timer.time_elapsed < 3.0f) {
        return;
    }

    obj = FindGameObject((u32)ms->mission->find_char, 0, 0, 1, 0);
    if (GetMenuID() == 9) {
        return;
    }

    if (obj == NULL) {
        lo = 0;
        hi = 0;
        for (i = 0; i < HIGHGAMEOBJECT; i++) {
            GameObject_s *candidate = (GameObject_s *)((char *)Obj + i * 0x10e4);
            if ((*(u16 *)((char *)candidate + 0x1f8) & 0x1001) == 0x1001 &&
                *(u8 *)((char *)candidate + 0x287) == 0 && *(u8 *)((char *)candidate + 0x27c) == 0xff) {
                lo |= *(u32 *)((char *)candidate + 0x1e4);
                hi |= *(u32 *)((char *)candidate + 0x1e8);
            }
        }
    } else {
        lo = *(u32 *)((char *)obj + 0x1e4);
        hi = *(u32 *)((char *)obj + 0x1e8);
    }

    for (i = 0; i < 8; i++) {
        player = Player[i];
        if (player != NULL && *(i8 *)((char *)player + 0x1f8) < 0) {
            if ((lo & *(u32 *)((char *)player + 0x1ec)) | (hi & *(u32 *)((char *)player + 0x1f0))) {
                EndMission(ms, 2, 1);
                return;
            }
        }
    }
}

void Mission_FindTarget(MISSIONSYS *ms, u64 *target) {
    GameObject_s *obj;

    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        if (target != NULL) {
            *target = 0;
        }
        return;
    }

    obj = FindGameObject((u32)ms->mission->find_char, 0, 0, 1, 0);
    if (target != NULL) {
        if (obj != NULL) {
            *target = *(u64 *)((char *)obj + 0x1e4);
        } else {
            *target = 0;
        }
    }
}

i32 Missions_PickupsOff(MISSIONSYS *ms) {
    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        return 0;
    }
    if (ms->field8_0x1d == 0) {
        return 0;
    }
    if (ms->mission == NULL) {
        return 0;
    }
    return (ms->flags ^ 1) & 1;
}

i32 Mission_CurrentState(MISSIONSYS *ms) {
    u8 c;

    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        return 0;
    }
    c = ms->field8_0x1d;
    if (c != 0 && ms->mission != NULL) {
        return (i32)c;
    }
    return 0;
}

i32 Missions_NumCompleted(MISSIONSYS *ms, MISSIONSAVE *save, i32 count) {
    i32 i;
    i32 total = 0;

    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        return 0;
    }
    if (ms->count == 0) {
        return 0;
    }

    if (count != 0) {
        for (i = 0; i < ms->count; i++) {
            ((u8 *)save)[0x50 + i] = 1;
            if (((f32 *)save)[i] == 0.0f) {
                u16 t = (u16)ms->mission[i].time;
                ((f32 *)save)[i] = ((f32)(t >> 16) * 98304.0f) + (f32)t - 1.0f;
            }
        }
        return ms->count;
    }

    for (i = 0; i < ms->count; i++) {
        if (((u8 *)save)[0x50 + i] == 1) {
            total++;
        }
    }
    return total;
}

i32 Missions_PartyAvailable(MISSIONSYS *ms) {
    i32 i;

    if (ms == NULL) {
        ms = MissionSys;
    }
    if (ms == NULL) {
        return 1;
    }
    if (ms->character_count == 0) {
        return 1;
    }
    for (i = 0; i < ms->character_count; i++) {
        if (Collection_Got(ms->character_ids[i]) == 0) {
            return 0;
        }
    }
    return 1;
}
