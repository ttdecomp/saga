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
extern i16 Area_PlayerModelList[24];
extern APICHARACTERMODELLIST_s Area_MissionModelList[52];
extern APICHARACTERMODELLIST_s Area_StoryModelList[52];
extern APICHARACTERMODELLIST_s FreePlayModelList[52];
extern APICHARACTERMODELLIST_s Hub_ModelList[8];
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

i32 AREA_DEFAULTBONUSTIMETRIALTIME = 300;
i32 AREA_DEFAULTCHALLENGETIME = 600;

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
    nufpar_s *fp = NuFParCreate(file);
    if (fp == NULL) {
        if (countDest != NULL)
            *countDest = 0;
        return NULL;
    }

    i32 area_count = 0;
    i32 in_area = 0;
    AREADATA *area = (AREADATA *)ALIGN((usize)bufferStart->void_ptr, 4);
    bufferStart->void_ptr = area;
    AREADATA *area_base = area;

    while (NuFParGetLine(fp)) {
        NuFParGetWord(fp);
        char *word = fp->word_buf;
        if (*word == '\0')
            continue;

        if (in_area) {
            if (NuStrICmp(word, "area_end") == 0) {
                in_area = 0;
                if (area->dir[0] != '\0' && area->file[0] != '\0' && area->level_count != 0 &&
                    (area->flags & AREAFLAG_TEST_AREA) == 0) {
                    area++;
                    area_count++;
                }
            } else if (NuStrICmp(fp->word_buf, "dir") == 0) {
                if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buf) <= 0x3f)
                    NuStrCpy(area->dir, fp->word_buf);
                in_area = 1;
            } else if (NuStrICmp(fp->word_buf, "file") == 0) {
                if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buf) <= 0x1f)
                    NuStrCpy(area->file, fp->word_buf);
                in_area = 1;
            } else if (NuStrICmp(fp->word_buf, "level") == 0) {
                if (area->level_count > 0xb || NuFParGetWord(fp) == 0) {
                    in_area = 1;
                } else {
                    i32 li;
                    Level_FindByName(fp->word_buf, &li);
                    in_area = 1;
                    if (li != -1) {
                        in_area = area->level_count;
                        if (in_area == 0) {
                            area->levels[0] = (i16)li;
                            area->level_count = 1;
                        } else {
                            i32 k;
                            if (area->levels[0] != li) {
                                for (k = 1; k < in_area; k++) {
                                    if (area->levels[k] == li)
                                        break;
                                }
                                if (k == in_area) {
                                    area->levels[in_area] = (i16)li;
                                    area->level_count = (u8)(in_area + 1);
                                }
                            }
                        }
                        in_area = 1;
                    }
                }
            } else if (NuStrICmp(fp->word_buf, "single_buffer") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_SINGLE_BUFFER;
            } else if (NuStrICmp(fp->word_buf, "minikit") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_MINIKIT;
                if (NuFParGetWord(fp) != 0)
                    area->minikit_id = CharIDFromName(fp->word_buf);
            } else if (NuStrICmp(fp->word_buf, "true_jedi") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_TRUE_JEDI;
            } else if (NuStrICmp(fp->word_buf, "test_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_TEST_AREA;
            } else if (NuStrICmp(fp->word_buf, "hub_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_HUB_AREA;
            } else if (NuStrICmp(fp->word_buf, "override_things_scene") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_OVERRIDE_THINGS_SCENE;
            } else if (NuStrICmp(fp->word_buf, "vehicle_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_VEHICLE_AREA;
            } else if (NuStrICmp(fp->word_buf, "ending_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_ENDING_AREA;
            } else if (NuStrICmp(fp->word_buf, "bonus_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_BONUS_AREA;
            } else if (NuStrICmp(fp->word_buf, "super_bonus_area") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_SUPER_BONUS_AREA;
            } else if (NuStrICmp(fp->word_buf, "nocharactercollision") == 0 ||
                       NuStrICmp(fp->word_buf, "nocharactercollisions") == 0 ||
                       NuStrICmp(fp->word_buf, "no_character_collision") == 0 ||
                       NuStrICmp(fp->word_buf, "no_character_collisions") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_NO_CHARACTER_COLLISION;
            } else if (NuStrICmp(fp->word_buf, "nopickupgravity") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_NOPICKUPGRAVITY;
            } else if (NuStrICmp(fp->word_buf, "no_gold_brick") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_NO_GOLDBRICK;
            } else if (NuStrICmp(fp->word_buf, "no_completion_points") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_NO_COMPLETION_POINTS;
            } else if (NuStrICmp(fp->word_buf, "no_freeplay") == 0) {
                in_area = 1;
                area->flags |= AREAFLAG_NO_FREEPLAY;
            } else if (NuStrICmp(fp->word_buf, "name_id") == 0) {
                in_area = 1;
                area->name_id = NuFParGetInt(fp);
            } else if (NuStrICmp(fp->word_buf, "text_id") == 0) {
                area->text_id = NuFParGetInt(fp);
                in_area = 1;
                if (NuFParGetWord(fp) != 0)
                    area->text_id_value = (byte)abs(NuAToI(fp->word_buf));
            } else if (NuStrICmp(fp->word_buf, "timetrial_time") == 0) {
                in_area = 1;
                area->challenge_trial_time = NuFParGetInt(fp);
            } else if (NuStrICmp(fp->word_buf, "redbrick_cheat") == 0 ||
                       NuStrICmp(fp->word_buf, "redbrick_extra") == 0) {
                if (NuFParGetWord(fp) == 0) {
                    in_area = 1;
                } else {
                    area->cheat = Cheat_FindByName(fp->word_buf);
                    in_area = 1;
                }
            }
        } else {
            if (NuStrICmp(word, "area_start") != 0 || count <= area_count)
                continue;
            in_area = 1;
            area->dir[0] = '\0';
            area->file[0] = '\0';
            area->levels[0] = -1;
            area->name_id = 0xffff;
            area->flags = AREAFLAG_NONE;
            area->index = (u8)area_count;
            area->level_count = 0;
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
            area->text_id = 0xffff;
            area->text_id_value = 1;
            area->field42_0x98 = NULL;
        }
    }

    NuFParDestroy(fp);
    if (area_count != 0) {
        bufferStart->void_ptr = area;
        if (countDest != NULL)
            *countDest = area_count;
        i32 j = 0;
        if (0 < area_count) {
            do {
                while (true) {
                    if (area_base[j].challenge_trial_time == 0) {
                        if ((area_base[j].flags & AREAFLAG_SUPER_BONUS_AREA) == AREAFLAG_BONUS_AREA)
                            area_base[j].challenge_trial_time = (i16)AREA_DEFAULTBONUSTIMETRIALTIME;
                        else if ((area_base[j].flags & AREAFLAG_MINIKIT) != 0)
                            area_base[j].challenge_trial_time = (i16)AREA_DEFAULTCHALLENGETIME;
                    }
                    if (area_base[j].cheat != 0xff)
                        Cheat_SetArea((i32)(char)area_base[j].cheat, j);
                    if (area_base[j].challenge_trial_time != 0 &&
                        (area_base[j].flags & (AREAFLAG_SUPER_BONUS_AREA | AREAFLAG_MINIKIT)) == AREAFLAG_MINIKIT)
                        break;
                    j++;
                    if (area_count <= j)
                        return area_base;
                }
                area_base[j].challenge_trial_time = 1200;
                j++;
            } while (j < area_count);
            return area_base;
        }
    }
    return NULL;
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
    if (indexDest != NULL) {
        *indexDest = -1;
    }

    if (area == NULL || area->level_count == 0) {
        return NULL;
    }

    for (i32 i = 0; i < area->level_count; i++) {
        i32 levelIdx = area->levels[i];
        LEVELDATA *level = &LDataList[levelIdx];
        if (level->flags & LEVEL_STATUS) {
            if (indexDest != NULL) {
                *indexDest = levelIdx;
            }
            return level;
        }
    }

    return NULL;
}

LEVELDATA *Area_FindNextPlayLevel(i32 levelIdx) {
    LEVELDATA *level = &LDataList[levelIdx];
    i32 areaIdx = level->area_index;
    i32 areaLevelIdx = level->area_level_index;
    LEVELDATA *result = level;

    if (areaIdx != -1) {
        if (areaLevelIdx < ADataList[areaIdx].level_count - 1) {
            result = &LDataList[ADataList[areaIdx].levels[areaLevelIdx]];
            if (result->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) {
                for (i32 i = areaLevelIdx; i != ADataList[areaIdx].level_count - 2; i++) {
                    LEVELDATA *candidate = &LDataList[ADataList[areaIdx].levels[i + 1]];
                    if (!(candidate->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)))
                        return candidate;
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
    APICHARACTERMODELLIST_s ml[3];
    i32 level;
    i32 saved_max_fps;
    i32 area;
    NUGSCN *scene;
    APICHARACTERMODELLIST_s *list;
    APICHARACTERMODELLIST_s *story_list;

    area = Area;
    saved_max_fps = nuapi.max_fps;
    nuapi.max_fps = 30;
    level = Level;

    if (area != -1 && area != last_area) {
        characterbuffer_ptr.addr = ALIGN(characterbuffer_ptr.addr, 4);
        NuStrCpy(pathbuf, "levels\\");
        NuStrCat(pathbuf, ADataList[area].dir);
        NuStrCat(pathbuf, "\\");
        NuStrCat(pathbuf, ADataList[area].file);
        NuStrCat(pathbuf, ".gsc");
        scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, pathbuf);
        area_scene = scene;
        if (scene != NULL && scene->display_list != NULL)
            scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;

        if ((ADataList[area].flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) != 0 && area != last_area) {
            characterbuffer_ptr.addr = ALIGN(characterbuffer_ptr.addr, 4);
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\vehicle_things.gsc");
            vehicle_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;
        }
    }

    if (HUB_ADATA != NULL && HUB_ADATA->index == area) {
        if (area != last_area) {
            characterbuffer_ptr.addr = ALIGN(characterbuffer_ptr.addr, 0x40);
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\icons\\starwars_icons_all.gsc");
            big_icon_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->flags |= NU_DISPLAYSCENE_FLAG_NEEDS_BUILD;
        }
    }

    if (area != -1 && area != last_area) {
        ml[0].count = 1;
        ml[1].model_id = -1;
        ml[1].count = 1;
        ml[2].model_id = -1;
        ml[2].count = 0;
        ml[0].model_id = id_ANAKINJEDISCARRED;
        if ((ADataList[area].flags & AREAFLAG_HUB_AREA) != 0)
            ml[1].model_id = id_HANINCARBONITE;
        IconScenes_Load(ml, 1, &characterbuffer_ptr, &characterbuffer_end);
    }

    if (makefreeplaymodellist != 0) {
        if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0 || (Area_PlayerModelCount > 1 && BonusArea == 0)) {
            MakeFreePlayModelList(Area_PlayerModelList[0], Area_PlayerModelList[2], area, level, 0);
        } else {
            MakeFreePlayModelList(id_DEFAULTCHARACTER[0], id_DEFAULTCHARACTER[1], area, level, 0);
        }
        story_list = Area_StoryModelList;
        makefreeplaymodellist = 0;
        list = FreePlayModelList;
        goto icon_scenes;
    } else {
        if (area != -1 && area == last_area) {
            CharacterDataLoad = 2;
            goto loadareadata_check;
        }
    }
model_select:
    if (HUB_ADATA != NULL && HUB_ADATA->index == area) {
        list = Hub_ModelList;
        story_list = Hub_ModelList;
    } else {
        list = FreePlayModelList;
        if (FreePlay == 0) {
            if (Mission_Active(0) != 0)
                list = Area_MissionModelList;
            else
                list = Area_StoryModelList;
        }
        if (HUB_ADATA != NULL && HUB_ADATA->index == area) {
            story_list = Hub_ModelList;
        } else {
            if (Mission_Active(0) != 0)
                story_list = Area_MissionModelList;
            else
                story_list = Area_StoryModelList;
        }
    }

icon_scenes:
    CurrentCList = list;
    IconScenes_Load(list, 1, &characterbuffer_ptr, &characterbuffer_end);

    if (HUB_ADATA != NULL && HUB_ADATA->index == area)
        Customiser_SetAnimsToLoad(CharacterCustomiser, 1);
    else
        Customiser_SetAnimsToLoad(CharacterCustomiser, 0);

    if (area != -1 &&
        (ADataList[area].flags & (AREAFLAG_ENDING_AREA | AREAFLAG_TEST_AREA | AREAFLAG_NO_CHARACTER_COLLISION)) == 0) {
        if (ADataList[area].episode_index <= 2 || (ANEWHOPE_ADATA != NULL && ANEWHOPE_ADATA->index == area) ||
            (PODSPRINT_ADATA != NULL && PODSPRINT_ADATA->index == area) ||
            (BONUS_GUNSHIP_ADATA != NULL && BONUS_GUNSHIP_ADATA->index == area)) {
            Particles_LoadAreaPage("stuff\\char_lsw1.ptl");
        }
    }

    apiloadcharactermodels_nopakfile = (CHARPAK == 0);
    APIResetCharacterRemap();
    CharacterDataLoad = 1;
    GameLoadCharacterModels(list, 0, &characterbuffer_ptr, &characterbuffer_end, 1, area);
    CharacterDataLoad = 2;
    CharScenes_AreaLoad(list, &characterbuffer_end, characterbuffer_ptr);
    if (!(HUB_ADATA != NULL && HUB_ADATA->index == area))
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
