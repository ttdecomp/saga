#include "game/area.h"

#include <stdlib.h>

#include "game/character.h"
#include "game/cheat.h"
#include "game/level.h"
#include "globals.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

AREADATA *ADataList = NULL;
AREADATA *HUB_ADATA = NULL;

i32 Area = -1;

i32 AREA_DEFAULTBONUSTIMETRIALTIME = 0x12C;
i32 AREA_DEFAULTCHALLENGETIME = 0x258;

AREADATA *Area_FindByName(char *name, i32 *indexDest) {
    for (i32 i = 0; i < AREACOUNT; i++) {
        AREADATA *area = &ADataList[i];

        if (NuStrICmp(area->file, name) == 0) {
            if (indexDest != NULL) {
                *indexDest = i;
            }
            return area;
        }
    }

    if (indexDest != NULL) {
        *indexDest = -1;
    }

    return NULL;
}

AREADATA *Areas_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, int count, int *countDest) {
    byte bVar3;
    undefined2 uVar4;
    i16 sVar5;
    nufpar_s *fp;
    int iVar6;
    uint uVar7;
    undefined4 uVar8;
    uint uVar9;
    AREADATA *area2;
    i16 index;
    AREADATA *area;
    int i;
    int j;
    undefined4 uStack_14;
    char *a;
    bool bVar2;
    uint uVar1;

    uStack_14 = 0x4862eb;
    fp = NuFParCreate(file);
    if (fp == (nufpar_s *)0x0) {
        if (countDest != (int *)0x0) {
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
                                        uVar7 = (uint)bVar3;
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
                                                                                                                (byte)((int)
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
            if (countDest != (int *)0x0) {
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
                            Cheat_SetArea((int)(char)area2[j].cheat, j);
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
