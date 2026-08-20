#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

extern void *LevelHackData;
extern void *OldLevelHackData;
extern i32 LevelHackSize;
extern i32 LevelHackSendTimer;
extern void *LevelProgressData;

extern i32 LevFlag[4];
extern i32 LevHSpecial[264];
extern i32 LevSfxFlag[4];
extern void *dynamic_antinodes;
extern i32 LevInstAnim[12];
extern i32 LevArea[4];
extern i32 LevPathNodes[8];
extern i32 LevPathCnx[16];
extern i32 LevGameObject[8];
extern i32 LevGamePart[8];
extern i32 LevAIMessage[8];
extern i32 LevelLocator;
extern i32 LevGizObst[8];
extern i32 LevBlowUp[5];
extern i32 LevGizmo[12];
extern i32 LevSfxId[4];
extern i32 LevelCodeSpline[8];
extern i32 LevGizForce[4];
extern i32 LevAIPathNode[4];
extern i32 LevBoltIgnorePlatIds[2];
extern i32 LevPlatID[2];
extern i32 LevPathCnxDir;
extern i32 LevDeaths;
extern i32 LevLock[4];
extern i32 LevSafePlatID[2];
extern i32 LEVELOBJECTCOUNT;
extern i32 AREACOUNT;
extern void *ADataList;
extern void CompleteLevel(WORLDINFO_s *);
extern WORLDINFO_s *WORLD;
i32 KillBoss(i32, i32, float);
extern void GoToNewLevel(i32);
extern void NewCutScene(CUTINFO *, CUTSYS *, char *, i32);
extern void ClearLevelProgress(i32, struct WORLDINFO_s *);
extern char Game[0x7e58];
extern char BackupGame[0x7e58];
extern i32 Customiser_AccessoriesLoaded;
extern void *vehicle_scene;
extern void *CharacterCustomiser;
extern i32 HIGHGAMEOBJECT;
extern void *Obj;
extern i32 Area;
extern i32 last_area;
extern void *big_icon_scene;
extern void *area_scene;

struct TORPEDOPACKET_s;
struct GameObject_s;
struct CUSTOMISER;
void FreeTorpedoPacket(struct TORPEDOPACKET_s **);
void RemoveGameObject(struct GameObject_s *, i32);
void IconScenes_Dump(void);
void CharScenes_AreaDump(void);
void Particles_DumpAreaPage(void);
void Customiser_RestoreModelTextureIDs(struct CUSTOMISER *);
void Customiser_DumpAccessories(struct CUSTOMISER *);
extern "C" void APIDumpCharacterModels(i32);
extern "C" void NuGScnRemove(void *);

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ResetLevel(WORLDINFO_s *, char *, i32) {
}

void ClearLevData() {
    LevFlag[0] = 0;
    LevFlag[1] = 0;
    LevFlag[2] = 0;
    LevFlag[3] = 0;
    memset(LevHSpecial, 0, sizeof(LevHSpecial));
    LevSfxFlag[0] = 0;
    LevSfxFlag[1] = 0;
    LevSfxFlag[2] = 0;
    LevSfxFlag[3] = 0;
    memset(dynamic_antinodes, 0, 0x1500);
    memset(LevInstAnim, 0, sizeof(LevInstAnim));
    LevArea[0] = 0;
    LevArea[1] = 0;
    LevArea[2] = 0;
    LevArea[3] = 0;
    memset(LevPathNodes, 0, sizeof(LevPathNodes));
    memset(LevPathCnx, 0, sizeof(LevPathCnx));
    memset(LevGameObject, 0, sizeof(LevGameObject));
    memset(LevGamePart, 0, sizeof(LevGamePart));
    memset(LevAIMessage, 0, sizeof(LevAIMessage));
    LevelLocator = 0;
    memset(LevGizObst, 0, sizeof(LevGizObst));
    LevBlowUp[0] = 0;
    LevBlowUp[1] = 0;
    LevBlowUp[2] = 0;
    LevBlowUp[3] = 0;
    LevBlowUp[4] = 0;
    memset(LevGizmo, 0, sizeof(LevGizmo));
    LevSfxId[0] = 0;
    LevSfxId[1] = 0;
    LevSfxId[2] = 0;
    LevSfxId[3] = 0;
    memset(LevelCodeSpline, 0, sizeof(LevelCodeSpline));
    LevGizForce[0] = 0;
    LevGizForce[1] = 0;
    LevGizForce[2] = 0;
    LevGizForce[3] = 0;
    LevAIPathNode[0] = 0;
    LevAIPathNode[1] = 0;
    LevAIPathNode[2] = 0;
    LevAIPathNode[3] = 0;
    LevBoltIgnorePlatIds[0] = -1;
    LevBoltIgnorePlatIds[1] = -1;
    LevPlatID[0] = -1;
    LevPlatID[1] = -1;
    LevPathCnxDir = 0;
    LevDeaths = 0;
    LevLock[0] = 0;
    LevLock[1] = 0;
    LevLock[2] = 0;
    LevLock[3] = 0;
    LevSafePlatID[0] = -1;
    LevSafePlatID[1] = -1;
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
    i32 i;

    if (HIGHGAMEOBJECT > 0) {
        for (i = 0; i < HIGHGAMEOBJECT; i++) {
            u8 *obj = (u8 *)Obj + i * 0x10e4;
            if (*(u8 *)(obj + 0x1f8) & 1) {
                FreeTorpedoPacket((struct TORPEDOPACKET_s **)(obj + 0xcb4));
                RemoveGameObject((struct GameObject_s *)obj, 1);
            }
        }
    }
    if (Area != -1 && Area == last_area) {
        return;
    }
    switch (Customiser_AccessoriesLoaded) {
        case 2:
            Customiser_RestoreModelTextureIDs((struct CUSTOMISER *)CharacterCustomiser);
            break;
        case 1:
            Customiser_DumpAccessories((struct CUSTOMISER *)CharacterCustomiser);
            break;
    }
    Customiser_AccessoriesLoaded = 0;
    APIDumpCharacterModels(0);
    IconScenes_Dump();
    CharScenes_AreaDump();
    if (big_icon_scene != NULL) {
        NuGScnRemove(big_icon_scene);
        big_icon_scene = NULL;
    }
    if (area_scene != NULL) {
        NuGScnRemove(area_scene);
        area_scene = NULL;
    }
    if (vehicle_scene != NULL) {
        NuGScnRemove(vehicle_scene);
        vehicle_scene = NULL;
    }
    Particles_DumpAreaPage();
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

void ClearAreaProgress(i32 a, i32 b) {
    u8 *area;
    i32 i;
    i32 levelIdx;

    for (i = 0; i < 12; i++) {
        ClearLevelProgress(i, NULL);
    }
    *(i32 *)BackupGame = *(i32 *)((u8 *)Game + 0x7bfc);
    if (b == 0 || a == -1) {
        return;
    }
    *(i32 *)((u8 *)Game + 0x14) = 0;
    area = *(u8 **)&ADataList + a * 0x9c;
    if (*(u8 *)(area + 0x7d) == 0) {
        return;
    }
    for (i = 0; i < *(u8 *)(area + 0x7d); i++) {
        levelIdx = *(i16 *)(area + 0x60 + i * 2);
        memcpy((u8 *)Game + levelIdx * 0x54 + 0x11, (u8 *)BackupGame + levelIdx * 0x54 + 0x11, 0x53);
    }
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
