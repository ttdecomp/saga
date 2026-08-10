#include "legoapi/episode.h"

#include "globals.h"
#include "legoapi/area.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"

EPISODEDATA *EDataList = NULL;

EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 maxCount,
                                    i32 *countDest) {
    nufpar_s *fp = NuFParCreate(file);
    if (fp == NULL) {
        if (countDest != NULL) {
            *countDest = 0;
        }
        return NULL;
    }

    i32 count = 0;
    bool inEpisode = false;
    EPISODEDATA *episodes = (EPISODEDATA *)((bufferStart->addr + 3) & ~3);
    bufferStart->void_ptr = episodes;
    EPISODEDATA *episode = episodes;

    while (NuFParGetLine(fp) != 0) {
        NuFParGetWord(fp);
        if (fp->word_buf[0] == '\0') {
            continue;
        }

        if (!inEpisode) {
            if (NuStrICmp(fp->word_buf, "episode_start") == 0 && count < maxCount) {
                episode->name_id = -1;
                episode->text_id = -1;
                episode->area_count = 0;
                episode->index = (u8)count;
                inEpisode = true;
            }
            continue;
        }

        if (NuStrICmp(fp->word_buf, "episode_end") == 0) {
            inEpisode = false;
            if (episode->area_count != 0) {
                count++;
                episode++;
            }
            continue;
        }

        if (NuStrICmp(fp->word_buf, "area") == 0) {
            if (episode->area_count >= 10 || NuFParGetWord(fp) == 0) {
                continue;
            }

            i32 areaIndex;
            AREADATA *area = Area_FindByName(fp->word_buf, &areaIndex);
            if (areaIndex == -1) {
                continue;
            }

            bool duplicate = false;
            for (i32 i = 0; i < episode->area_count; i++) {
                if (episode->area_ids[i] == areaIndex) {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) {
                continue;
            }

            for (i32 i = 0; i < count && !duplicate; i++) {
                for (i32 j = 0; j < episodes[i].area_count; j++) {
                    if (episodes[i].area_ids[j] == areaIndex) {
                        duplicate = true;
                        break;
                    }
                }
            }
            if (duplicate) {
                continue;
            }

            episode->area_ids[episode->area_count] = (i16)areaIndex;
            episode->area_count++;
            if ((area->flags & 6) == 0) {
                episode->regular_areas++;
            }
        } else if (NuStrICmp(fp->word_buf, "name_id") == 0) {
            episode->name_id = (i16)NuFParGetInt(fp);
        } else if (NuStrICmp(fp->word_buf, "text_id") == 0) {
            episode->text_id = (i16)NuFParGetInt(fp);
        }
    }

    NuFParDestroy(fp);
    if (count == 0) {
        return NULL;
    }

    bufferStart->void_ptr = episode;
    if (countDest != NULL) {
        *countDest = count;
    }

    return episodes;
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
