
#include <string.h>
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/world/level.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/mission.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"

i32 openlevels = 0;

// Cross-module entry points used by this file (declared locally since they have
// no single shared header in the reconstructed source).  The NuFPar* helpers are
// taken as void* (untyped parser handle) to match the open/close contract.
extern void ReCalculateCompletionPoints(void);
extern void Hub_LockUnlockDoors(struct WORLDINFO_s *);
extern void FreeTorpedoPacket(struct TORPEDOPACKET_s **);
extern void RemoveGameObject(struct GameObject_s *, i32);
extern void IconScenes_Dump(void);
extern void CharScenes_AreaDump(void);
extern void Particles_DumpAreaPage(void);
extern void Customiser_RestoreModelTextureIDs(struct CUSTOMISER *);
extern void Customiser_DumpAccessories(struct CUSTOMISER *);
extern "C" void APIDumpCharacterModels(i32);
extern void SuperCounters_Reset(i32);
extern void NewAreaMusicChanges(void);
extern void ClearTakeOverObjectSys(void);
extern void Door_Reset(void);
extern void ResetMinikitCounter(void);

void Areas_OpenAll(i32 mode) {
    i32 area_index;
    AREADATA *area;
    AREASAVE_s *save = Game.area_save;

    if (mode != 0 && !Store_IsPackUnlocked(STORE_PACK_OPEN_ALL_AREAS)) {
        return;
    }

    for (area_index = 0; area_index < AREACOUNT; area_index++, save++) {
        area = &ADataList[area_index];

        if ((area->flags & AREAFLAG_MINIKIT) != 0) {
            if ((area->episode_index == AREA_EPISODE_I && !Store_IsPackUnlocked(STORE_PACK_EPISODE_I)) ||
                (area->episode_index == AREA_EPISODE_II && !Store_IsPackUnlocked(STORE_PACK_EPISODE_II)) ||
                (area->episode_index == AREA_EPISODE_III && !Store_IsPackUnlocked(STORE_PACK_EPISODE_III)) ||
                (area->episode_index == AREA_EPISODE_IV && !Store_IsPackUnlocked(STORE_PACK_EPISODE_IV)) ||
                (area->episode_index == AREA_EPISODE_V && !Store_IsPackUnlocked(STORE_PACK_EPISODE_V))) {
                continue;
            }
        } else if ((area->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_SUPER_BONUS_AREA)) == AREAFLAG_BONUS_AREA &&
                   area->episode_index != AREA_EPISODE_NONE && !Store_IsPackUnlocked(STORE_PACK_BONUS_AREA)) {
            continue;
        }

        if (mode == 0) {
            save->complete = 1;
            save->area_complete = 1;
            if (area->challenge_trial_time != 0) {
                save->challenge_trial_time = (f32)area->challenge_trial_time - 0.5f;
            }
        } else {
            save->area_complete = 1;
        }
    }

    Game.field_0x2[1] = 1;
    if (WORLD != NULL && WORLD->current_level == HUB_LDATA) {
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
    Area_StoryModelList[0].model_id = -1;
    Area_FreePlayModelCount = 0;
    Area_FreePlayModelList[0] = -1;
    Area_MissionModelCount = 0;
    Area_MissionModelList[0].model_id = -1;
    LevelLoad[0] = -1;
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
        NUFPAR *fp;
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
                if (fp->word_buf[0] == '\0') {
                    continue;
                }
                if (in_area) {
                    if (NuStrICmp(fp->word_buf, "area_end") == 0) {
                        in_area = 0;
                        if (*(i16 *)(area_buf + 0x1e0) != 0) {
                            count++;
                        }
                        continue;
                    }
                } else {
                    if (NuStrICmp(fp->word_buf, "area_start") == 0) {
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
    i32 object_index;

    if (HIGHGAMEOBJECT > 0) {
        for (object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
            GameObject_s *obj = &Obj[object_index];
            if ((obj->apiobj.field_0x1f8 & 1) != 0) {
                FreeTorpedoPacket((TORPEDOPACKET_s **)&obj->torpedo);
                RemoveGameObject(obj, 1);
            }
        }
    }
    if (Area != -1 && Area == last_area) {
        return;
    }
    if (Customiser_AccessoriesLoaded == 2) {
        Customiser_RestoreModelTextureIDs(CharacterCustomiser);
    } else if (Customiser_AccessoriesLoaded == 1) {
        Customiser_DumpAccessories(CharacterCustomiser);
    }
    Customiser_AccessoriesLoaded = 0;
    APIDumpCharacterModels(0);
    IconScenes_Dump();
    CharScenes_AreaDump();
    if (big_icon_scene != NULL) {
        NuGScnRemove(big_icon_scene);
    }
    big_icon_scene = NULL;
    if (area_scene != NULL) {
        NuGScnRemove(area_scene);
    }
    area_scene = NULL;
    if (vehicle_scene != NULL) {
        NuGScnRemove(vehicle_scene);
    }
    vehicle_scene = NULL;
    Particles_DumpAreaPage();
}

void ClearAreaProgress(i32 a, i32 b) {
    const i32 LEVEL_SAVE_SIZE = 0x54;

    for (i32 i = 0; i < 12; i++) {
        ClearLevelProgress(i, NULL);
    }
    areaSuitBits = Game.initial_store_pack_flags;
    if (b == 0 || a == -1) {
        return;
    }
    AreaGlobals.values.field_0x14 = 0;
    Game.area_save[a] = BackupGame.area_save[a];

    AREADATA *area = &ADataList[a];
    if (area->level_count == 0) {
        return;
    }
    for (i32 i = 0; i < area->level_count; i++) {
        i32 level_index = area->levels[i];
        memcpy(Game.level_save + level_index * LEVEL_SAVE_SIZE, BackupGame.level_save + level_index * LEVEL_SAVE_SIZE,
               LEVEL_SAVE_SIZE);
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
        if ((*(u16 *)(area + 0x7a) & (AREAFLAG_TRUE_JEDI | AREAFLAG_MINIKIT)) && *((u8 *)save) != 0) {
            ((u8 *)save)[2] = 1;
            ((u8 *)save)[3] = 1;
        }
        area += 0x9c;
        save = (AREASAVE_s *)((u8 *)save + 0xc);
    }
}

void Areas_ConfigureResidents(VARIPTR *buffer, VARIPTR *) {
    if (ADataList == NULL) {
        return;
    }

    buffer->addr = ALIGN(buffer->addr, 4);
    AREADATA *area = ADataList;
    for (i32 area_index = 0; area_index < AREACOUNT; ++area_index, ++area) {
        area->hub_player_ids = NULL;

        if ((area->flags & AREAFLAG_ENDING_AREA) != 0) {
            continue;
        }

        char path[0x100];
        NuStrCpy(path, "levels\\");
        NuStrCat(path, area->dir);
        NuStrCat(path, "\\");
        NuStrCat(path, area->file);
        NuStrCat(path, ".txt");

        NUFPAR *parser = NuFParCreate(path);
        if (parser == NULL) {
            continue;
        }

        i32 resident_count = 0;
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            if (NuStrICmp(parser->word_buf, "character") != 0) {
                continue;
            }
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            i32 character_id = CharIDFromName(parser->word_buf);
            if (character_id == -1) {
                continue;
            }
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            if (NuStrICmp(parser->word_buf, "resident") != 0) {
                continue;
            }

            if (area->hub_player_ids == NULL) {
                area->hub_player_ids = buffer->i16_ptr;
            }
            area->hub_player_ids[resident_count++] = static_cast<i16>(character_id);
        }
        NuFParDestroy(parser);

        if (resident_count != 0) {
            area->hub_player_ids[resident_count] = -1;
            buffer->i16_ptr = &area->hub_player_ids[resident_count + 1];
        }
    }
    buffer->addr = ALIGN(buffer->addr, 4);
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
    AreaGlobals.values.field_0x10 = 0;
    if (Area == -1) {
        area_ep = 0;
    } else {
        area_ep = *(u8 *)((char *)&Game + 0x7831 + Area * 12);
    }
    AreaGlobals.bytes[0x0c] = area_ep;
    AreaGlobals.bytes[0x14] = area_ep;
    Door_UseCutCam = 0;
    AreaGlobals.values.field_0x00 = 0;
    AreaGlobals.values.field_0x1c = 0;
    AreaGlobals.values.field_0x08 = 0;
    AreaGlobals.values.field_0x18 = 0;
    AreaGlobals.values.field_0x24 = 0;
    BuildUpTotal = 0;
    BuildUpDone = 0;
    if (*(void **)((char *)WORLD + 0x12c) == HOTHBATTLE_ADATA) {
        ResetMinikitCounter();
    }
    for (i = 0; i < 8; i++) {
        if (Player[i] != NULL) {
            Player[i]->current_hp = Player[i]->hitpoints;
        }
        ((u8 *)PlayerProgress)[i * 0x10 + 0x8] = DEFAULT_PLAYERHITPOINTS;
    }
    ResetTimer(&AreaTimer, 0.0f);
    memcpy(&BackupGame, (char *)WORLD + 0x15c, 0x7e58);
    NewAreaMusicChanges();
    VehicleAreaRememberSpeed = 0.0f;
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
