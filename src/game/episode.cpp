#include "game/episode.h"

#include "game/area.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufpar.h"

EPISODEDATA *EDataList = NULL;

EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, int32_t maxCount,
                                    int32_t *countDest) {
    short *psVar1;
    byte bVar2;
    char *a;
    bool bVar3;
    nufpar_s *fp;
    int iVar4;
    uint uVar5;
    EPISODEDATA *episodePtr;
    uint i;
    EPISODEDATA *episodePtr2;
    uint uVar6;
    EPISODEDATA *episode;
    int j;
    short areaId;
    int count;
    int areaIndex;
    AREADATA *area;

    fp = NuFParCreate(file);
    if (fp == (nufpar_s *)0x0) {
        if (countDest != (int *)0x0) {
            *countDest = 0;
        }
    } else {
        count = 0;
        bVar3 = false;
        episodePtr = (EPISODEDATA *)((int)bufferStart->void_ptr + 3U & 0xfffffffc);
        bufferStart->void_ptr = episodePtr;
        episode = episodePtr;
    LAB_00488f90:
        iVar4 = NuFParGetLine(fp);
        if (iVar4 != 0) {
            while (true) {
                NuFParGetWord(fp);
                a = fp->word_buf;
                if (*a == '\0')
                    break;
                if (!bVar3) {
                    iVar4 = NuStrICmp(a, "episode_start");
                    if (iVar4 == 0 && count < maxCount) {
                        episode->nameId = -1;
                        episode->textId = -1;
                        episode->areaCount = 0;
                        episode->index = (uint8_t)count;
                        bVar3 = true;
                    }
                    break;
                }

                if (NuStrICmp(a, "episode_end") != 0) {
                    if (NuStrICmp(fp->word_buf, "area") == 0) {
                        if (episode->areaCount >= 10 || NuFParGetWord(fp) == 0)
                            goto LAB_004890ae;

                        area = Area_FindByName(fp->word_buf, &areaIndex);

                        bVar3 = true;
                        if (areaIndex != -1) {
                            bVar2 = episode->areaCount;
                            i = (uint)bVar2;
                            if (i == 0)
                                goto LAB_0048910d;
                            if (areaIndex == episode->areaIds[0])
                                goto LAB_004890ae;
                            uVar5 = 0;
                            goto LAB_00489248;
                        }
                    } else {
                        iVar4 = NuStrICmp(fp->word_buf, "name_id");
                        if (iVar4 == 0) {
                            iVar4 = NuFParGetInt(fp);
                            episode->nameId = (short)iVar4;
                            bVar3 = true;
                        } else {
                            iVar4 = NuStrICmp(fp->word_buf, "text_id");
                            bVar3 = true;
                            if (iVar4 == 0) {
                                iVar4 = NuFParGetInt(fp);
                                episode->textId = (short)iVar4;
                            }
                        }
                    }
                    break;
                }

                bVar3 = false;
                if (episode->areaCount == 0)
                    break;
                count = count + 1;
                iVar4 = NuFParGetLine(fp);
                episode = episode + 1;
                if (iVar4 == 0) {
                    goto end;
                }
            }
            goto LAB_00488f90;
        }

    end:
        NuFParDestroy(fp);
        if (count != 0) {
            bufferStart->void_ptr = episode;
            if (countDest == (int *)0x0) {
                return episodePtr;
            }
            *countDest = count;
            return episodePtr;
        }
    }
    return NULL;

    while (iVar4 = uVar5 + 1, uVar5 = uVar6, areaIndex != episode->areaIds[iVar4]) {
    LAB_00489248:
        uVar6 = uVar5 + 1;
        if (uVar5 == bVar2 - 1)
            break;
    }

    if (uVar6 == i) {
    LAB_0048910d:
        j = 0;
        episodePtr2 = episodePtr;
        if (0 < count) {
            do {
                iVar4 = 0;
                while (iVar4 != (uint)episodePtr2->areaCount * 2) {
                    psVar1 = (short *)((int)episodePtr2->areaIds + iVar4);
                    iVar4 = iVar4 + 2;
                    if (areaIndex == *psVar1)
                        goto LAB_004890ae;
                }
                j = j + 1;
                episodePtr2 = episodePtr2 + 1;
            } while (j != count);
        }
        areaId = (short)areaIndex;
        episode->areaIds[i] = areaId;
        bVar3 = true;
        episode->areaCount = bVar2 + 1;
        if ((area->flags & 6) == 0) {
            episode->regularAreas += 1;
        }
    } else {
        bVar3 = true;
    }

    goto LAB_00488f90;
LAB_004890ae:
    bVar3 = true;
    goto LAB_00488f90;
}
