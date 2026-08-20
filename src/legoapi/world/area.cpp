#include "decomp.h"
#include "legoapi/world/area.h"

#include <stdlib.h>

#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/world/level.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nu3d/nugscn.h"
#include "legoapi/world/world.h"

struct APICHARACTERMODELLIST_s;
struct CUSTOMISER;
struct MISSIONSYS_s;

extern i32 abort_load;
extern NUGSCN *area_scene;
extern NUGSCN *vehicle_scene;
extern NUGSCN *big_icon_scene;
extern i16 id_ANAKINJEDISCARRED;
extern i16 id_HANINCARBONITE;
extern i16 id_DEFAULTCHARACTER[2];
extern i16 Area_PlayerModelList[18];
extern i16 Area_MissionModelList[98];
extern i16 Area_StoryModelList[98];
extern i16 FreePlayModelList[98];
extern i16 Hub_ModelList[12];
extern i32 Area_PlayerModelCount;
extern void *CurrentCList;
extern void *CurrentStoryCList;
extern CUSTOMISER *CharacterCustomiser;
extern i32 makefreeplaymodellist;
extern i32 BonusArea;
extern AREADATA *ANEWHOPE_ADATA;
extern AREADATA *PODSPRINT_ADATA;
extern AREADATA *BONUS_GUNSHIP_ADATA;
extern i32 CHARPAK;
extern i32 apiloadcharactermodels_nopakfile;
extern i32 CharacterDataLoad;
extern i32 loadareadata_loadlevel;
extern i32 AreaDataLoaded;
extern i32 Level;
extern VARIPTR characterbuffer_ptr;
extern VARIPTR characterbuffer_end;

extern void IconScenes_Load(APICHARACTERMODELLIST_s *, i32, variptr_u *, variptr_u *);
extern void MakeFreePlayModelList(i32, i32, i32, i32, i32);
extern void Customiser_LoadAccessories(CUSTOMISER *, APICHARACTERMODELLIST_s *);
extern void Customiser_ResetModelTextureIDs(CUSTOMISER *);
extern void Customiser_SetAnimsToLoad(CUSTOMISER *, i32);
extern void GameLoadCharacterModels(APICHARACTERMODELLIST_s *, i32, variptr_u *, variptr_u *, i32, i32);
extern void CharScenes_AreaLoad(APICHARACTERMODELLIST_s *, variptr_u *, variptr_u);
extern void Particles_LoadAreaPage(char *);
extern void *Mission_Active(struct MISSIONSYS_s *ms);
extern "C" {
    extern void APIResetCharacterRemap(void);
}

AREADATA *ADataList = NULL;
AREADATA *HUB_ADATA = NULL;

static AREAFIXUP AreaFixUp_LSW[1] = {{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

i32 Area = -1;

i32 AREA_DEFAULTBONUSTIMETRIALTIME = 0x12C;
i32 AREA_DEFAULTCHALLENGETIME = 0x258;

AREADATA *Area_FindByName(char *name, i32 *indexDest) {
    for (i32 i = 0; i < AREACOUNT; i++) {
        if (NuStrICmp(ADataList[i].file, name) == 0) {
            if (indexDest != NULL) {
                *indexDest = i;
            }
            return &ADataList[i];
        }
    }

    if (indexDest != NULL) {
        *indexDest = -1;
    }

    return NULL;
}

AREADATA *Areas_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 count, i32 *countDest) {
    byte bVar3;
    undefined2 uVar4;
    i16 sVar5;
    nufpar_s *fp;
    i32 iVar6;
    u32 uVar7;
    undefined4 uVar8;
    u32 uVar9;
    AREADATA *area2;
    i16 index;
    AREADATA *area;
    i32 i;
    i32 j;
    undefined4 uStack_14;
    char *a;
    bool bVar2;
    u32 uVar1;

    uStack_14 = 0x4862eb;
    fp = NuFParCreate(file);
    if (fp == (nufpar_s *)0x0) {
        if (countDest != (i32 *)0x0) {
            *countDest = 0;
        }
    } else {
        i = 0;
        bVar2 = false;
        area2 = (AREADATA *)((usize)bufferStart->void_ptr + 3U & 0xfffffffc);
        bufferStart->void_ptr = area2;
        area = area2;
    LAB_00486340:
        iVar6 = NuFParGetLine(fp);
        if (iVar6 != 0) {
            while (true) {
                NuFParGetWord(fp);
                a = fp->word_buf;
                if (*a == '\0')
                    break;
                if (bVar2) {
                    iVar6 = NuStrICmp(a, "area_end");
                    if (iVar6 == 0) {
                        bVar2 = false;
                        if ((((area->dir[0] != '\0') && (area->file[0] != '\0')) && (area->field28_0x7d != 0)) &&
                            ((area->flags & 0x20) == 0)) {
                            area = area + 1;
                            i = i + 1;
                        }
                    } else {
                        iVar6 = NuStrICmp(fp->word_buf, "dir");
                        if (iVar6 == 0) {
                            iVar6 = NuFParGetWord(fp);
                            if ((iVar6 == 0) || (iVar6 = NuStrLen(fp->word_buf), 0x3f < iVar6))
                                goto LAB_00486600;
                            NuStrCpy(area->dir, fp->word_buf);
                            bVar2 = true;
                        } else {
                            iVar6 = NuStrICmp(fp->word_buf, "file");
                            if (iVar6 == 0) {
                                iVar6 = NuFParGetWord(fp);
                                if ((iVar6 == 0) || (iVar6 = NuStrLen(fp->word_buf), 0x1f < iVar6))
                                    goto LAB_00486600;
                                NuStrCpy(area->file, fp->word_buf);
                                bVar2 = true;
                            } else {
                                iVar6 = NuStrICmp(fp->word_buf, "level");
                                if (iVar6 == 0) {
                                    if ((0xb < area->field28_0x7d) || (iVar6 = NuFParGetWord(fp), iVar6 == 0))
                                        goto LAB_00486600;
                                    Level_FindByName(fp->word_buf, &j);
                                    bVar2 = true;
                                    if (j != -1) {
                                        bVar3 = area->field28_0x7d;
                                        uVar7 = (u32)bVar3;
                                        if (uVar7 == 0)
                                            goto LAB_004866e6;
                                        uVar9 = 0;
                                        if (j == area->field2_0x60[0])
                                            goto LAB_00486600;
                                        goto LAB_0048672f;
                                    }
                                } else {
                                    iVar6 = NuStrICmp(fp->word_buf, "single_buffer");
                                    if (iVar6 == 0) {
                                        bVar2 = true;
                                        area->flags |= AREAFLAG_SINGLE_BUFFER;
                                    } else {
                                        iVar6 = NuStrICmp(fp->word_buf, "minikit");
                                        if (iVar6 == 0) {
                                            area->flags |= AREAFLAG_MINIKIT;
                                            iVar6 = NuFParGetWord(fp);
                                            bVar2 = true;
                                            if (iVar6 != 0) {
                                                uVar4 = CharIDFromName(fp->word_buf);
                                                bVar2 = true;
                                                area->minikit_id = uVar4;
                                            }
                                        } else {
                                            iVar6 = NuStrICmp(fp->word_buf, "true_jedi");
                                            if (iVar6 == 0) {
                                                bVar2 = true;
                                                area->flags |= AREAFLAG_TRUE_JEDI;
                                            } else {
                                                iVar6 = NuStrICmp(fp->word_buf, "test_area");
                                                if (iVar6 == 0) {
                                                    bVar2 = true;
                                                    area->flags |= AREAFLAG_TEST_AREA;
                                                } else {
                                                    iVar6 = NuStrICmp(fp->word_buf, "hub_area");
                                                    if (iVar6 == 0) {
                                                        bVar2 = true;
                                                        area->flags |= AREAFLAG_HUB_AREA;
                                                    } else {
                                                        iVar6 = NuStrICmp(fp->word_buf, "override_things_scene");
                                                        if (iVar6 == 0) {
                                                            bVar2 = true;
                                                            area->flags |= AREAFLAG_OVERRIDE_THINGS_SCENE;
                                                        } else {
                                                            iVar6 = NuStrICmp(fp->word_buf, "vehicle_area");
                                                            if (iVar6 == 0) {
                                                                bVar2 = true;
                                                                area->flags |= AREAFLAG_VEHICLE_AREA;
                                                            } else {
                                                                iVar6 = NuStrICmp(fp->word_buf, "ending_area");
                                                                if (iVar6 == 0) {
                                                                    bVar2 = true;
                                                                    area->flags |= AREAFLAG_ENDING_AREA;
                                                                } else {
                                                                    iVar6 = NuStrICmp(fp->word_buf, "bonus_area");
                                                                    if (iVar6 == 0) {
                                                                        bVar2 = true;
                                                                        area->flags |= AREAFLAG_BONUS_AREA;
                                                                    } else {
                                                                        iVar6 =
                                                                            NuStrICmp(fp->word_buf, "super_bonus_area");
                                                                        if (iVar6 == 0) {
                                                                            bVar2 = true;
                                                                            area->flags |= AREAFLAG_SUPER_BONUS_AREA;
                                                                        } else {
                                                                            iVar6 = NuStrICmp(fp->word_buf,
                                                                                              "nocharactercollision");
                                                                            if ((((iVar6 == 0) ||
                                                                                  (iVar6 = NuStrICmp(
                                                                                       fp->word_buf,
                                                                                       "nocharactercollisions"),
                                                                                   iVar6 == 0)) ||
                                                                                 (iVar6 = NuStrICmp(
                                                                                      fp->word_buf,
                                                                                      "no_character_collision"),
                                                                                  iVar6 == 0)) ||
                                                                                (iVar6 = NuStrICmp(
                                                                                     fp->word_buf,
                                                                                     "no_character_collisions"),
                                                                                 iVar6 == 0)) {
                                                                                bVar2 = true;
                                                                                area->flags |=
                                                                                    AREAFLAG_NO_CHARACTER_COLLISION;
                                                                            } else {
                                                                                iVar6 = NuStrICmp(fp->word_buf,
                                                                                                  "nopickupgravity");
                                                                                if (iVar6 == 0) {
                                                                                    bVar2 = true;
                                                                                    area->flags |=
                                                                                        AREAFLAG_NOPICKUPGRAVITY;
                                                                                } else {
                                                                                    iVar6 = NuStrICmp(fp->word_buf,
                                                                                                      "no_gold_brick");
                                                                                    if (iVar6 == 0) {
                                                                                        bVar2 = true;
                                                                                        area->flags |=
                                                                                            AREAFLAG_NO_GOLDBRICK;
                                                                                    } else {
                                                                                        iVar6 = NuStrICmp(
                                                                                            fp->word_buf,
                                                                                            "no_completion_points");
                                                                                        if (iVar6 == 0) {
                                                                                            bVar2 = true;
                                                                                            area->flags |=
                                                                                                AREAFLAG_NO_COMPLETION_POINTS;
                                                                                        } else {
                                                                                            iVar6 = NuStrICmp(
                                                                                                fp->word_buf,
                                                                                                "no_freeplay");
                                                                                            if (iVar6 == 0) {
                                                                                                bVar2 = true;
                                                                                                area->flags |=
                                                                                                    AREAFLAG_NO_FREEPLAY;
                                                                                            } else {
                                                                                                iVar6 = NuStrICmp(
                                                                                                    fp->word_buf,
                                                                                                    "name_id");
                                                                                                if (iVar6 == 0) {
                                                                                                    uVar4 =
                                                                                                        NuFParGetInt(
                                                                                                            fp);
                                                                                                    bVar2 = true;
                                                                                                    area->field25_0x78 =
                                                                                                        uVar4;
                                                                                                } else {
                                                                                                    iVar6 = NuStrICmp(
                                                                                                        fp->word_buf,
                                                                                                        "text_id");
                                                                                                    if (iVar6 == 0) {
                                                                                                        uVar4 =
                                                                                                            NuFParGetInt(
                                                                                                                fp);
                                                                                                        area->field39_0x94 =
                                                                                                            uVar4;
                                                                                                        iVar6 =
                                                                                                            NuFParGetWord(
                                                                                                                fp);
                                                                                                        bVar2 = true;
                                                                                                        if (iVar6 !=
                                                                                                            0) {
                                                                                                            uVar8 = NuAToI(
                                                                                                                fp->word_buf);
                                                                                                            bVar3 =
                                                                                                                (byte)((i32)
                                                                                                                           uVar8 >>
                                                                                                                       0x1f);
                                                                                                            area->field40_0x96 =
                                                                                                                ((byte)
                                                                                                                     uVar8 ^
                                                                                                                 bVar3) -
                                                                                                                bVar3;
                                                                                                        }
                                                                                                    } else {
                                                                                                        iVar6 = NuStrICmp(
                                                                                                            fp->word_buf,
                                                                                                            "timetrial_"
                                                                                                            "time");
                                                                                                        if (iVar6 ==
                                                                                                            0) {
                                                                                                            sVar5 =
                                                                                                                NuFParGetInt(
                                                                                                                    fp);
                                                                                                            bVar2 =
                                                                                                                true;
                                                                                                            area->challenge_trial_time =
                                                                                                                sVar5;
                                                                                                        } else {
                                                                                                            iVar6 = NuStrICmp(
                                                                                                                fp->word_buf,
                                                                                                                "redbri"
                                                                                                                "ck_"
                                                                                                                "chea"
                                                                                                                "t");
                                                                                                            if (((iVar6 !=
                                                                                                                  0) &&
                                                                                                                 (iVar6 = NuStrICmp(
                                                                                                                      fp->word_buf,
                                                                                                                      "redbrick_extra"),
                                                                                                                  iVar6 !=
                                                                                                                      0)) ||
                                                                                                                (iVar6 = NuFParGetWord(
                                                                                                                     fp),
                                                                                                                 iVar6 ==
                                                                                                                     0))
                                                                                                                goto LAB_00486600;
                                                                                                            bVar3 = Cheat_FindByName(
                                                                                                                fp->word_buf);
                                                                                                            bVar2 =
                                                                                                                true;
                                                                                                            area->cheat =
                                                                                                                bVar3;
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                iVar6 = NuStrICmp(a, "area_start");
                if ((iVar6 != 0) || (count <= i))
                    break;
                bVar2 = true;
                area->dir[0] = '\0';
                area->file[0] = '\0';
                area->field2_0x60[0] = -1;
                area->field25_0x78 = 0xffff;
                area->flags = AREAFLAG_NONE;
                area->field27_0x7c = (undefined1)i;
                area->field28_0x7d = 0;
                area->cheat = 0xff;
                area->field30_0x7f = 0;
                area->field31_0x80 = 0;
                area->challenge_trial_time = 0;
                area->episode_index = 0xff;
                area->area_index = 0xff;
                area->area_music = -1;
                area->minikit_id = 0xffff;
                area->field37_0x8c = 0;
                area->field38_0x90 = 0;
                area->field39_0x94 = 0xffff;
                area->field40_0x96 = 1;
                area->field42_0x98 = 0;
                iVar6 = NuFParGetLine(fp);
                if (iVar6 == 0)
                    goto LAB_004864a0;
            }
            goto LAB_00486340;
        }
    LAB_004864a0:
        NuFParDestroy(fp);
        if (i != 0) {
            bufferStart->void_ptr = area;
            if (countDest != (i32 *)0x0) {
                *countDest = i;
            }
            j = 0;
            if (0 < i) {
                do {
                    while (true) {
                        if (area2[j].challenge_trial_time == 0) {
                            if ((area2[j].flags & 0x104) == 4) {
                                area2[j].challenge_trial_time = (i16)AREA_DEFAULTBONUSTIMETRIALTIME;
                            } else if ((area2[j].flags & 0x10) != 0) {
                                area2[j].challenge_trial_time = (i16)AREA_DEFAULTCHALLENGETIME;
                            }
                        }
                        if (area2[j].cheat != 0xff) {
                            Cheat_SetArea((i32)(char)area2[j].cheat, j);
                        }
                        if ((area2[j].challenge_trial_time != 0) && ((area2[j].flags & 0x114) == 0x10))
                            break;
                        j = j + 1;
                        if (i <= j) {
                            return area2;
                        }
                    }
                    iVar6 = j + 1;
                    area2[j].challenge_trial_time = 1200;
                    j = iVar6;
                } while (iVar6 < i);
            }
            return area2;
        }
    }
    return (AREADATA *)0x0;
    while (iVar6 = uVar9 + 1, uVar9 = uVar1, j != area->field2_0x60[iVar6]) {
    LAB_0048672f:
        uVar1 = uVar9 + 1;
        if (uVar9 == uVar7 - 1)
            break;
    }
    if (uVar1 == uVar7) {
    LAB_004866e6:
        index = (i16)j;
        area->field2_0x60[uVar7] = index;
        bVar2 = true;
        area->field28_0x7d = bVar3 + 1;
    } else {
    LAB_00486600:
        bVar2 = true;
    }
    goto LAB_00486340;
}

void Areas_FixUp(AREAFIXUP *fixup) {
    struct FIXUP {
        char *name;
        AREADATA **area;
    };

    if (fixup != NULL) {
        for (FIXUP *f = (FIXUP *)fixup; f->name != NULL; f++) {
            if (f->area != NULL) {
                *f->area = Area_FindByName(f->name, NULL);
            }
        }
    }
}

void FixUpAreas(void) {
    Areas_FixUp(AreaFixUp_LSW);
}

struct LEVELDATA_s *Area_FindStatusLevel(AREADATA *area, i32 *indexDest) {
    LEVELDATA *level;
    i32 levelIdx;
    i32 i;

    if (indexDest != NULL) {
        *indexDest = -1;
    }

    if (area == NULL || area->field28_0x7d == 0) {
        return NULL;
    }

    i32 bound = (u32)area->field28_0x7d * 2 - 2;
    levelIdx = (i32)area->field2_0x60[0];
    i = 0;
    level = &LDataList[levelIdx];

    while (1) {
        if ((((u8 *)&level->flags)[1] & 4) != 0) {
            break;
        }
        if (i == bound) {
            return NULL;
        }
        levelIdx = (i32) * (i16 *)((u8 *)area->field2_0x60 + i + 2);
        i += 2;
        level = &LDataList[levelIdx];
    }

    if (indexDest != NULL) {
        *indexDest = levelIdx;
    }

    return level;
}

LEVELDATA *Area_FindNextPlayLevel(i32 levelIdx) {
    LEVELDATA *level;
    u8 areaIdx;
    i32 areaLevelIdx;
    LEVELDATA *result;

    level = &LDataList[levelIdx];
    areaIdx = (u8)level->unknown_0af;
    areaLevelIdx = (i8)level->unknown_0d4;
    result = level;

    if (areaIdx != 0xff) {
        if (areaLevelIdx < (i32)(ADataList[areaIdx].field28_0x7d - 1)) {
            result = &LDataList[ADataList[areaIdx].field2_0x60[areaLevelIdx]];
            if (((u8 *)&result->flags)[0] & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) {
                while (areaLevelIdx != (i32)ADataList[areaIdx].field28_0x7d - 2) {
                    areaLevelIdx++;
                    if ((((u8 *)&LDataList[ADataList[areaIdx].field2_0x60[areaLevelIdx]].flags)[0] &
                         (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0) {
                        return &LDataList[ADataList[areaIdx].field2_0x60[areaLevelIdx]];
                    }
                }
                return level;
            }
        }
    }
    return result;
}

i32 AreaFromMiniKitID(i32 minikitId) {
    i32 i;

    for (i = 0; i < AREACOUNT; i++) {
        if (ADataList[i].minikit_id == minikitId) {
            return i;
        }
    }

    return -1;
}

struct bgprocinfo_s;
static __used__ void LoadAreaData(bgprocinfo_s *) {
    char pathbuf[0x100];
    i16 ml[6];
    i32 level;
    i32 saved_max_fps;
    i32 area;
    NUGSCN *scene;
    APICHARACTERMODELLIST_s *list;
    APICHARACTERMODELLIST_s *story_list;

    area = Area;
    saved_max_fps = nuapi.max_fps;
    nuapi.max_fps = 0x1e;
    level = Level;

    if (area != -1 && area != last_area) {
        characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3U;
        NuStrCpy(pathbuf, "levels\\");
        NuStrCat(pathbuf, ADataList[area].dir);
        NuStrCat(pathbuf, "\\");
        NuStrCat(pathbuf, ADataList[area].file);
        NuStrCat(pathbuf, ".gsc");
        scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, pathbuf);
        area_scene = scene;
        if (scene != NULL && scene->display_list != NULL)
            scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;

        if ((ADataList[area].flags & 5) != 0 && area != last_area) {
            characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3U;
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\vehicle_things.gsc");
            vehicle_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;
        }
    }

    if (HUB_ADATA != NULL && HUB_ADATA->field27_0x7c == area) {
        if (area != last_area) {
            characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 0x3f) & ~0x3fU;
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\icons\\starwars_icons_all.gsc");
            big_icon_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;
        }
    }

    if (area != -1 && area != last_area) {
        ml[1] = 1;
        ml[2] = -1;
        ml[3] = 1;
        ml[4] = -1;
        ml[5] = 0;
        ml[0] = id_ANAKINJEDISCARRED;
        if ((ADataList[area].flags & AREAFLAG_HUB_AREA) != 0)
            ml[2] = id_HANINCARBONITE;
        IconScenes_Load((APICHARACTERMODELLIST_s *)ml, 1, &characterbuffer_ptr, &characterbuffer_end);
    }

    if (makefreeplaymodellist != 0) {
        if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0 || (Area_PlayerModelCount > 1 && BonusArea == 0)) {
            MakeFreePlayModelList(Area_PlayerModelList[0], Area_PlayerModelList[2], area, level, 0);
        } else {
            MakeFreePlayModelList(id_DEFAULTCHARACTER[0], id_DEFAULTCHARACTER[1], area, level, 0);
        }
        story_list = (APICHARACTERMODELLIST_s *)Area_StoryModelList;
        makefreeplaymodellist = 0;
        list = (APICHARACTERMODELLIST_s *)FreePlayModelList;
        goto icon_scenes;
    } else {
        if (area != -1 && area == last_area) {
            CharacterDataLoad = 2;
            goto loadareadata_check;
        }
    }
model_select:
    if (HUB_ADATA != NULL && HUB_ADATA->field27_0x7c == area) {
        list = (APICHARACTERMODELLIST_s *)Hub_ModelList;
        story_list = (APICHARACTERMODELLIST_s *)Hub_ModelList;
    } else {
        list = (APICHARACTERMODELLIST_s *)FreePlayModelList;
        if (FreePlay == 0) {
            if (Mission_Active(0) != 0)
                list = (APICHARACTERMODELLIST_s *)Area_MissionModelList;
            else
                list = (APICHARACTERMODELLIST_s *)Area_StoryModelList;
        }
        if (HUB_ADATA != NULL && HUB_ADATA->field27_0x7c == area) {
            story_list = (APICHARACTERMODELLIST_s *)Hub_ModelList;
        } else {
            if (Mission_Active(0) != 0)
                story_list = (APICHARACTERMODELLIST_s *)Area_MissionModelList;
            else
                story_list = (APICHARACTERMODELLIST_s *)Area_StoryModelList;
        }
    }

icon_scenes:
    CurrentCList = list;
    IconScenes_Load(list, 1, &characterbuffer_ptr, &characterbuffer_end);

    if (HUB_ADATA != NULL && HUB_ADATA->field27_0x7c == area)
        Customiser_SetAnimsToLoad(CharacterCustomiser, 1);
    else
        Customiser_SetAnimsToLoad(CharacterCustomiser, 0);

    if (area != -1 && (ADataList[area].flags & 0x62) == 0) {
        if (ADataList[area].episode_index <= 2 || (ANEWHOPE_ADATA != NULL && ANEWHOPE_ADATA->field27_0x7c == area) ||
            (PODSPRINT_ADATA != NULL && PODSPRINT_ADATA->field27_0x7c == area) ||
            (BONUS_GUNSHIP_ADATA != NULL && BONUS_GUNSHIP_ADATA->field27_0x7c == area)) {
            Particles_LoadAreaPage("stuff\\char_lsw1.ptl");
        }
    }

    apiloadcharactermodels_nopakfile = (CHARPAK == 0);
    APIResetCharacterRemap();
    CharacterDataLoad = 1;
    GameLoadCharacterModels(list, 0, &characterbuffer_ptr, &characterbuffer_end, 1, area);
    CurrentStoryCList = (void *)2;
    CharScenes_AreaLoad(list, &characterbuffer_end, characterbuffer_ptr);
    if (!(HUB_ADATA != NULL && HUB_ADATA->field27_0x7c == area))
        Customiser_LoadAccessories(CharacterCustomiser, list);
    Customiser_ResetModelTextureIDs(CharacterCustomiser);
    CurrentStoryCList = story_list;
loadareadata_check:
    if (loadareadata_loadlevel != 0) {
        next_level = Level;
        abort_load = 0;
        WorldInfo_StreamLevel(0);
    }
    AreaDataLoaded = 1;
    nuapi.max_fps = saved_max_fps;
}
