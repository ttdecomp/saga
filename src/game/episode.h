#pragma once

#include <stdint.h>

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

struct episodedata_s {
    int16_t nameId;
    int16_t textId;
    int16_t areaIds[10];
    uint8_t areaCount;
    uint8_t regularAreas;
    uint8_t index;
    uint8_t field6_0x1b;
};
typedef struct episodedata_s EPISODEDATA;

C_API_START

extern EPISODEDATA *EDataList;

C_API_END

#ifdef __cplusplus

EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, int32_t maxCount,
                                    int32_t *countDest);

#endif
