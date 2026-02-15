#pragma once

#include "nu2api.saga/nucore/common.h"

struct episodedata_s {
    i16 name_id;
    i16 text_id;
    i16 area_ids[10];
    u8 area_count;
    u8 regular_areas;
    u8 index;
    u8 field6_0x1b;
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

EPISODEDATA *Episodes_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 maxCount, i32 *countDest);

i32 Episode_ContainsArea(i32 areaId, i32 *areaIndex);

#endif
