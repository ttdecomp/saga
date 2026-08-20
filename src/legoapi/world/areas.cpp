#include "legoapi/world/level_shared.h"

#include <string.h>

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

void Areas_ConfigureResidents(variptr_u *, variptr_u *) {
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
    if (*(void **)((char *)WORLD + 0x130) != NULL && (*(u8 *)(*(char **)((char *)WORLD + 0x130) + 0x2800) & 1) == 0) {
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
