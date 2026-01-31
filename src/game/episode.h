#pragma once

#include <stdint.h>

#include "nu2api.saga/nucore/common.h"

struct episodedata_s {
    int16_t name_id;
    int16_t text_id;
    int16_t area_ids[10];
    uint8_t area_count;
    uint8_t regular_areas;
    uint8_t index;
    uint8_t field6_0x1b;
};
typedef struct episodedata_s EPISODEDATA;

#ifdef __cplusplus
extern "C" {
#endif

    extern EPISODEDATA *EDataList;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, int32_t maxCount,
                                    int32_t *countDest);

#endif
