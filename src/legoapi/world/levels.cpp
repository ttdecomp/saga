#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

extern void *LevelHackData;
extern void *OldLevelHackData;
extern i32 LevelHackSize;
extern i32 LevelHackSendTimer;
extern void *LevelProgressData;
extern i32 LEVELOBJECTCOUNT;
extern i32 AREACOUNT;
extern void *ADataList;
extern void CompleteLevel(WORLDINFO_s *);
extern WORLDINFO_s *WORLD;
i32 KillBoss(i32, i32, float);
extern void GoToNewLevel(i32);
extern void NewCutScene(CUTINFO *, CUTSYS *, char *, i32);

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ResetLevel(WORLDINFO_s *, char *, i32) {
}

void ClearLevData() {
}

void *SetLevelHack(i32 size) {
    memset(LevelHackData, 0, 0x80);
    memset(OldLevelHackData, 0, 0x80);
    LevelHackSize = size;
    LevelHackSendTimer = 0;
    return LevelHackData;
}

void Areas_OpenAll(i32) {
}

void Area_Configure(i32, i32, EXTRAMODEL *, i16 *) {
}

void ClearUpAreaData() {
}

i32 GetTableLocator(void) {
    return 0;
}

void getSpawnLocator(float, char *) {
}

void KillBossNewLevel(i32 a, i32 b, float c, i32 d) {
    if (KillBoss(a, b, c) != 0) {
        GoToNewLevel(d);
    }
}

void NewLevelFromMenu(LEVELDATA_s *, i32, i32, i32) {
}

void ClearAreaProgress(i32, i32) {
}

i32 GetCounterLocator(i32) {
    return 0;
}

void *LevObj_FindByPlatID(WORLDINFO_s *world, i32 platID) {
    i32 count = LEVELOBJECTCOUNT;
    u8 *obj;
    i32 i;

    if (count <= 0) {
        return NULL;
    }
    obj = *(u8 **)((char *)world + 0x2ac0);
    if (*(i16 *)(obj + 0xc) == platID) {
        return obj;
    }
    for (i = 1; i < count; i++) {
        obj += 0x10;
        if (*(i16 *)(obj + 0xc) == platID) {
            return obj;
        }
    }
    return NULL;
}

i32 KillBossPlayCutScene(i32 a, i32 b, float c, char *name) {
    if (KillBoss(a, b, c) != 0) {
        NewCutScene(NULL, (CUTSYS *)(*(void **)((char *)WORLD + 0x2ab0)), name, 1);
        return 1;
    }
    return 0;
}

void KillBossCompleteLevel(i32 a, i32 b, float c) {
    if (KillBoss(a, b, c) != 0) {
        CompleteLevel(WORLD);
    }
}

void LevelStreaming_Update(WORLDINFO_s *) {
}

void Areas_ConfigureResidents(variptr_u *, variptr_u *) {
}

void LevelObjects_InitForGame(LEVELOBJECT *, variptr_u *, variptr_u *, i32, i32) {
}

void Areas_CompleteAllBuildUps(AREASAVE_s *save) {
    i32 count = AREACOUNT;
    u8 *area;
    u8 *end;

    if (save == NULL || count <= 0) {
        return;
    }
    area = *(u8 **)&ADataList;
    end = area + count * 0x9c;
    while (area != end) {
        if ((*(u16 *)(area + 0x7a) & 0x4010) && *((u8 *)save) != 0) {
            ((u8 *)save)[2] = 1;
            ((u8 *)save)[3] = 1;
        }
        area += 0x9c;
        save = (AREASAVE_s *)((u8 *)save + 0xc);
    }
}

void LevelStreaming_DoorOverride(WORLDINFO_s *, LEVELDATA_s *, float, float *) {
}

void LevelProgress_ReserveBufferSpace(variptr_u *buf, variptr_u) {
    LevelProgressData = (void *)((buf->addr + 0xf) & ~0xf);
    buf->addr = (usize)LevelProgressData + 0x229b0;
}

void NewArea() {
}

void OffPlat(i32) {
}

i32 KillBoss(i32, i32, float) {
    return 0;
}

void LevObj_FixUpPlatIDs(WORLDINFO_s *world) {
    (void)world;
}
