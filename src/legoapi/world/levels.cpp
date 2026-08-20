#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"
#include "legoapi/core/input/timer.h"

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
extern void *ObjTabList;
extern void *HUB_LDATA;
extern "C" i32 NuSpecialExistsFn(void *);
extern "C" void *NuSpecialGetInstanceix(void *);
extern "C" i16 FindPlatInst(void *);

i32 Store_IsPackUnlocked(i32);
void ReCalculateCompletionPoints(void);
void Hub_LockUnlockDoors(struct WORLDINFO_s *);

extern i32 LSW1;
extern i32 LSW2;
extern i32 Arcade;
extern f32 HIGHJUMPHEIGHT;
extern i32 BuildUpTotal;
extern i32 BuildUpDone;
extern void *HOTHBATTLE_ADATA;
extern TIMER AreaTimer;
extern i32 VehicleAreaRememberSpeed;
extern i32 OldBonusScore[2];
extern i32 BonusScore[2];
extern i32 BonusCoinTotal;
extern void *Door_Last;
extern i32 LevelChange;
extern i32 BombGenerator_PlayerBomb[2];
extern i32 Lap;
extern f32 LevTime;
extern char AreaGlobals[0x34];
extern i32 Door_UseCutCam;
extern void *Player[8];
extern char PlayerProgress[0x80];
extern u8 DEFAULT_PLAYERHITPOINTS;

extern void SuperCounters_Reset(i32);
extern void NewAreaMusicChanges(void);
extern void ClearTakeOverObjectSys(void);
extern void Door_Reset(void);
extern void ResetMinikitCounter(void);

extern i32 LOADEROFF;
extern i32 BGLOAD;
extern void *NewLData;
extern void *LDataList;
extern i32 no_more_loads;
extern i32 other_level;
extern i32 other_level_override;
extern i32 CUTSTOPGAME;
extern void *CutStopInfo;
extern i32 WaitingForLevelTime;
extern i32 LevelLoadCount;
extern void *LevelLoad;
extern i32 LEGOSPL_SPLIT;
extern f32 g_BgLoadDelayHackTimer;

struct MISSIONSYS_s;
struct nuvec_s;
struct SOCKPOSITION_s;
extern i32 Mission_Active(struct MISSIONSYS_s *);
extern i32 bgGetProcActive(void);
extern i32 Players_AveragePos(struct nuvec_s *, struct SOCKPOSITION_s *);
extern i32 InStory(void);
extern i32 WorldInfo_OtherLevel(void *);
extern i32 CutScenePlayer_Active(void);
extern i32 InsideLineXZ(f32, f32, f32, f32, f32, f32);
extern "C" f32 NuVecDistSqr(void *, void *, i32);
extern void bgPostRequest(void *, void *, void *, i32);
extern void NuTimeGet(void *);
extern f32 NuTimeSeconds(void *);
extern "C" void *NuFParCreate(const char *);
extern "C" i32 NuFParDestroy(void *);
extern "C" i32 NuFParGetLine(void *);
extern "C" i32 NuFParGetWord(void *);
extern "C" void NuStrCpy(char *, const char *);
extern "C" void NuStrCat(char *, const char *);
extern "C" i32 NuStrICmp(const char *, const char *);

extern i32 Area_PlayerModelCount;
extern i32 Area_StoryModelCount;
extern i16 Area_PlayerModelList[1];
extern i32 Area_FreePlayModelCount;
extern i16 Area_FreePlayModelList[1];
extern i32 Area_MissionModelCount;
extern i16 Area_MissionModelList[1];
extern i16 AreaMusic;
extern void *MissionSys;
extern void *Area_StoryModelList;

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

void Areas_OpenAll(i32 mode) {
    i32 i;
    i32 pack;
    i32 open;
    u8 *comp;
    u8 *area;

    if (mode != 0 && Store_IsPackUnlocked(8) == 0) {
        return;
    }
    if (AREACOUNT > 0) {
        comp = (u8 *)Game + 0x7834;
        for (i = 0; i < AREACOUNT; i++) {
            area = *(u8 **)&ADataList + i * 0x9c;
            open = 1;
            if (*(u16 *)(area + 0x7a) & 0x10) {
                u8 ep = *(u8 *)(area + 0x86);
                for (pack = 0; pack < ep; pack++) {
                    if (Store_IsPackUnlocked(pack) == 0) {
                        open = 0;
                        break;
                    }
                }
            } else if ((*(u16 *)(area + 0x7a) & 0x105) == 0x4 && *(u8 *)(area + 0x86) != 0xff) {
                open = (Store_IsPackUnlocked(5) != 0);
            }
            if (open) {
                if (mode == 0) {
                    comp[-2] = 1;
                    comp[-1] = 1;
                    i16 t = *(i16 *)(area + 0x84);
                    if (t != 0) {
                        *(f32 *)comp = ((f32)(t >> 16) * 0.0588f) + (f32)t - 0.5f;
                    }
                } else {
                    comp[-1] = 1;
                }
            }
            comp += 0xc;
        }
    }
    *(u8 *)((u8 *)Game + 0x3) = 1;
    if (WORLD != NULL && *(void **)((char *)WORLD + 0x128) == (void *)HUB_LDATA) {
        Hub_LockUnlockDoors(WORLD);
    } else {
        ReCalculateCompletionPoints();
    }
}

void Area_Configure(i32 area, i32 param, EXTRAMODEL *models, i16 *s) {
    i16 area_music = -1;

    Area_PlayerModelCount = 0;
    Area_StoryModelCount = 0;
    Area_PlayerModelList[0] = -1;
    ((i16 *)Area_StoryModelList)[0] = -1;
    Area_FreePlayModelCount = 0;
    Area_FreePlayModelList[0] = -1;
    Area_MissionModelCount = 0;
    Area_MissionModelList[0] = -1;
    ((i16 *)LevelLoad)[0] = -1;
    LevelLoadCount = 0;
    if (area != -1) {
        u8 *ad = *(u8 **)&ADataList + area * 0x9c;
        area_music = *(i16 *)(ad + 0x88);
        *(i32 *)(ad + 0x80) = 0;
        *(u8 *)(ad + 0x7f) = 0;
    }
    AreaMusic = area_music;
    if (Mission_Active((struct MISSIONSYS_s *)MissionSys) != 0) {
        if (*(u8 *)((char *)MissionSys + 0x1f) != 0) {
            return;
        }
    }
    if (area != -1) {
        char path[256];
        void *fp;
        i32 count = 0;
        i32 in_area = 0;
        char area_buf[0x1e8];
        NuStrCpy(path, "levels\\");
        NuStrCat(path, *(char **)&ADataList + area * 0x9c);
        NuStrCat(path, "\\");
        NuStrCat(path, *(char **)&ADataList + area * 0x9c + 0x40);
        NuStrCat(path, ".txt");
        fp = NuFParCreate(path);
        if (fp != NULL) {
            while (NuFParGetLine(fp) != 0) {
                NuFParGetWord(fp);
                if (*(char *)fp + 0x510 == NULL) {
                    continue;
                }
                if (in_area) {
                    if (NuStrICmp(*(char **)((char *)fp + 0x510), "area_end") == 0) {
                        in_area = 0;
                        if (*(i16 *)(area_buf + 0x1e0) != 0) {
                            count++;
                        }
                        continue;
                    }
                } else {
                    if (NuStrICmp(*(char **)((char *)fp + 0x510), "area_start") == 0) {
                        if (count <= 9) {
                            *(i16 *)(area_buf + 0x1e0) = 0;
                            *(u8 *)(area_buf + 0x1e7) &= 0xfe;
                            *(u8 *)(area_buf + 0x1e4) = 0xff;
                            *(u8 *)(area_buf + 0x1e5) = 0xff;
                            *(u8 *)(area_buf + 0x1e6) = 0xff;
                            in_area = 1;
                        }
                        continue;
                    }
                }
            }
            NuFParDestroy(fp);
        }
    }
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

void LevelStreaming_Update(WORLDINFO_s *world) {
    i32 i;

    if (LOADEROFF == 0 && BGLOAD != 0 && *(void **)((char *)world + 0x12c) != NULL &&
        (*(u16 *)(*(char **)((char *)world + 0x12c) + 0x7a) & 8) == 0 && NewLData == 0) {
        if (Mission_Active(NULL) == 0) {
            no_more_loads = 0;
            other_level = -1;
            if (no_more_loads == 0 && other_level_override != -1) {
                other_level = other_level_override;
            }
            if (CUTSTOPGAME != 0 && CutStopInfo != NULL && *(i16 *)((char *)CutStopInfo + 0xec) != -1) {
                i32 cutlevel = *(i16 *)((char *)CutStopInfo + 0xec);
                i32 curlevel = *(i32 *)((char *)world + 0x11c);
                if (*(u8 *)((char *)LDataList + cutlevel * 0x144 + 0xaf) ==
                    *(u8 *)((char *)LDataList + curlevel * 0x144 + 0xaf)) {
                    other_level = cutlevel;
                }
            }
            if ((*(u8 *)(*(char **)((char *)world + 0x128) + 0x64) & 0xe0) == 0) {
                u8 *pos = (u8 *)world; // placeholder
                if (Players_AveragePos((struct nuvec_s *)(pos + 0x54), NULL) != 0) {
                    other_level = -1;
                    if (*(i32 *)((char *)world + 0x46a0) > 0) {
                        f32 best = 1000000.0f;
                        for (i = 0; i < *(i32 *)((char *)world + 0x46a0); i++) {
                            u8 *entry = *(u8 **)((char *)world + 0x469c) + i * 0x120;
                            i32 lev = *(i16 *)(entry + 0xf0);
                            if (lev != *(i32 *)((char *)world + 0x11c)) {
                                f32 d = NuVecDistSqr(entry + 0xd4, pos + 0x54, 0);
                                if (d < best) {
                                    best = d;
                                    other_level = *(i16 *)(entry + 0xf0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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
    i32 i;
    u8 area_ep;

    LSW1 = 0;
    LSW2 = 0;
    if (WORLD != NULL) {
        if (*(void **)((char *)WORLD + 0x12c) != NULL) {
            i8 ep = *(i8 *)(*(char **)((char *)WORLD + 0x12c) + 0x86);
            if (ep >= 0) {
                if (ep > 2) {
                    LSW2 = 1;
                } else {
                    LSW1 = 1;
                    HIGHJUMPHEIGHT = 1.14f;
                    goto have_jump;
                }
            }
        }
    }
    if (Arcade != 0) {
        HIGHJUMPHEIGHT = 1.14f;
    } else {
        HIGHJUMPHEIGHT = 0.75f;
    }
have_jump:
    ClearAreaProgress(Area, 0);
    if (*(void **)((char *)WORLD + 0x130) != NULL &&
        (*(u8 *)(*(char **)((char *)WORLD + 0x130) + 0x2800) & 1) == 0) {
        memcpy(*(void **)((char *)WORLD + 0x130), (char *)WORLD + 0x15c, 0x2800);
        *(u8 *)(*(char **)((char *)WORLD + 0x130) + 0x2800) |= 1;
    }
    SuperCounters_Reset(Area);
    *(i32 *)((char *)AreaGlobals + 0x10) = 0;
    if (Area == -1) {
        area_ep = 0;
    } else {
        area_ep = *(u8 *)((char *)Game + 0x7831 + Area * 12);
    }
    *(u8 *)((char *)AreaGlobals + 0xc) = area_ep;
    *(u8 *)((char *)AreaGlobals + 0x14) = area_ep;
    Door_UseCutCam = 0;
    *(i32 *)((char *)AreaGlobals + 0x0) = 0;
    *(i32 *)((char *)AreaGlobals + 0x1c) = 0;
    *(i32 *)((char *)AreaGlobals + 0x8) = 0;
    *(i32 *)((char *)AreaGlobals + 0x18) = 0;
    *(i32 *)((char *)AreaGlobals + 0x24) = 0;
    BuildUpTotal = 0;
    BuildUpDone = 0;
    if (*(void **)((char *)WORLD + 0x12c) == (void *)HOTHBATTLE_ADATA) {
        ResetMinikitCounter();
    }
    for (i = 0; i < 8; i++) {
        if (Player[i] != NULL) {
            *(u8 *)((char *)Player[i] + 0x108b) = *(u8 *)((char *)Player[i] + 0x108a);
        }
        ((u8 *)PlayerProgress)[i * 0x10 + 0x8] = DEFAULT_PLAYERHITPOINTS;
    }
    ResetTimer(&AreaTimer, 0.0f);
    memcpy(BackupGame, (char *)WORLD + 0x15c, 0x7e58);
    NewAreaMusicChanges();
    VehicleAreaRememberSpeed = 0;
    ClearTakeOverObjectSys();
    BonusScore[0] = OldBonusScore[0];
    BonusScore[1] = OldBonusScore[1];
    BonusCoinTotal = 0;
    Door_Last = NULL;
    Door_Reset();
    LevelChange = 1;
    BombGenerator_PlayerBomb[0] = 0;
    BombGenerator_PlayerBomb[1] = 0;
    Lap = 1;
}

void OffPlat(i32) {
}

i32 KillBoss(i32, i32, float) {
    return 0;
}

void LevObj_FixUpPlatIDs(WORLDINFO_s *world) {
    i32 i;
    u8 *obj;

    if (ObjTabList == NULL || LEVELOBJECTCOUNT <= 0) {
        return;
    }
    for (i = 0; i < LEVELOBJECTCOUNT; i++) {
        obj = *(u8 **)((char *)world + 0x2ac0) + i * 0x10;
        *(i16 *)(obj + 0xc) = -1;
        if (*(void **)((char *)world + 0x295c) != NULL) {
            if (NuSpecialExistsFn(obj)) {
                if (*(u8 *)((char *)ObjTabList + i * 8) == 1) {
                    i++;
                    *(i16 *)(obj + 0xc) = FindPlatInst(NuSpecialGetInstanceix(obj));
                }
            }
        }
    }
}
