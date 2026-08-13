#include "legoapi/episode.h"

#include "globals.h"
#include "legoapi/area.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"

EPISODEDATA *EDataList = NULL;

u32 Episode_FindAreaFromFlags(EPISODEDATA *ep, u32 flags, u32 want) {
    for (i32 i = 0; i < (i32)ep->area_count; i++) {
        AREADATA *a = &ADataList[ep->area_ids[i]];
        if ((a->flags & flags) == want) {
            return (u8)a->field27_0x7c;
        }
    }
    return 0xffffffff;
}
EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 maxCount,
                                    i32 *countDest) {
    NUFPAR *fp = NuFParCreate(file);
    if (fp == NULL) {
        if (countDest != NULL) {
            *countDest = 0;
        }
        return NULL;
    }

    i32 count = 0;
    bool bVar3 = false;
    EPISODEDATA *episodePtr = (EPISODEDATA *)((usize)bufferStart->void_ptr + 3U & 0xfffffffc);
    bufferStart->void_ptr = (void *)episodePtr;
    EPISODEDATA *episode = episodePtr;

    while (NuFParGetLine(fp)) {
    get_word:
        NuFParGetWord(fp);
        char *a = fp->word_buf;
        if (*a == '\0') {
            continue;
        }

        if (!bVar3) {
            if (NuStrICmp(a, "episode_start") == 0 && count < maxCount) {
                episode->name_id = -1;
                episode->text_id = -1;
                episode->area_count = 0;
                episode->index = (u8)count;
                bVar3 = true;
            }
            continue;
        }

        if (NuStrICmp(a, "episode_end") == 0) {
            bVar3 = false;
            if (episode->area_count != 0) {
                count++;
                episode++;
                if (NuFParGetLine(fp) == 0) {
                    break;
                }
                goto get_word;
            }
            continue;
        }

        if (NuStrICmp(a, "area") == 0) {
            if (episode->area_count <= 9 && NuFParGetWord(fp) != 0) {
                i32 areaIndex;
                AREADATA *area = Area_FindByName(fp->word_buf, &areaIndex);
                bVar3 = true;
                if (areaIndex != -1) {
                    u32 areaCount = episode->area_count;
                    bool found = false;
                    if (areaCount != 0) {
                        if (episode->area_ids[0] == areaIndex) {
                            found = true;
                        } else {
                            for (i32 k = 1; k < (i32)areaCount; k++) {
                                if (episode->area_ids[k] == areaIndex) {
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (!found) {
                        for (i32 j = 0; j < count; j++) {
                            EPISODEDATA *prev = &episodePtr[j];
                            for (i32 byteOff = 0; byteOff < (i32)prev->area_count * 2; byteOff += 2) {
                                if (*(i16 *)((u8 *)prev->area_ids + byteOff) == areaIndex) {
                                    found = true;
                                    break;
                                }
                            }
                            if (found) {
                                break;
                            }
                        }
                    }
                    if (!found) {
                        episode->area_ids[areaCount] = (i16)areaIndex;
                        episode->area_count = (u8)(areaCount + 1);
                        if ((area->flags & 6) == 0) {
                            episode->regular_areas += 1;
                        }
                    }
                }
            } else {
                bVar3 = true;
            }
            continue;
        }

        if (NuStrICmp(a, "name_id") == 0) {
            episode->name_id = (i16)NuFParGetInt(fp);
            bVar3 = true;
            continue;
        }

        if (NuStrICmp(a, "text_id") == 0) {
            bVar3 = true;
            episode->text_id = (i16)NuFParGetInt(fp);
            continue;
        }

        bVar3 = true;
        continue;
    }

    NuFParDestroy(fp);
    if (count != 0) {
        bufferStart->void_ptr = (void *)episode;
        if (countDest != NULL) {
            *countDest = count;
        }
        return episodePtr;
    }
    return NULL;
}

i32 Episode_ContainsArea(i32 areaId, i32 *areaIndex) {
    for (i32 i = 0; i < EPISODECOUNT; i++) {
        EPISODEDATA *episode = &EDataList[i];

        for (i32 j = 0; j < episode->area_count; j++) {
            i16 id = episode->area_ids[j];
            if (id == areaId) {
                if (areaIndex != NULL) {
                    *areaIndex = j;
                }

                return i;
            }
        }
    }

    if (areaIndex != NULL) {
        *areaIndex = -1;
    }

    return -1;
}
