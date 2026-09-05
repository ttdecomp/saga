#pragma once

#include "nu2api/nucore/common.h"

typedef struct EPISODEDATA {
    i16 name_id;
    i16 text_id;
    i16 area_ids[10];
    u8 area_count;
    u8 regular_areas;
    u8 index;
    u8 field6_0x1b;
} EPISODEDATA;

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
i32 Episode_IsComplete(EPISODEDATA *episode, i32 *completed_area_count);
i32 Episodes_Completed();

u32 Episode_FindAreaFromFlags(EPISODEDATA *ep, u32 flags, u32 want);

void CoinTotal_Draw(i32 total, f32 y, f32 scale, i32 remember_positions, f32 icon_phase, i32 red, i32 green, i32 blue);

#endif
