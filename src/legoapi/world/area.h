#pragma once

#include "decomp.h"
#include "legoapi/world/level.h"

#include "nu2api/nucore/common.h"

typedef struct AREADATA_s {
    char dir[64];
    char file[32];
    i16 levels[12];

    i16 name_id;

    u16 flags;

    byte index;
    byte level_count;
    byte cheat;
    byte field30_0x7f;
    i32 field31_0x80;
    i16 challenge_trial_time;
    byte episode_index;
    byte area_index;
    i16 area_music;
    i16 minikit_id;
    i32 field37_0x8c;
    i32 field38_0x90;
    i16 text_id;
    byte text_id_value;
    byte field41_0x97;
    i16 *hub_player_ids; // 0x98, optional extra hub characters terminated by -1
} AREADATA;

#ifdef __cplusplus
extern "C" {
#endif
    extern AREADATA *ADataList;
    extern AREADATA *HUB_ADATA;
    extern i32 Area;
#ifdef __cplusplus
}

typedef struct AREAFIXUP {
    char *name;
    AREADATA **area;
} AREAFIXUP;

AREADATA *Area_FindByName(char *name, i32 *indexDest);

AREADATA *Areas_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 count, i32 *countDest);

struct LEVELDATA_s *Area_FindStatusLevel(AREADATA *area, i32 *indexDest);

void FixUpAreas(void);
void Areas_FixUp(AREAFIXUP *fixup);

struct LEVELDATA_s *Area_FindNextPlayLevel(i32 levelIdx);

i32 AreaFromMiniKitID(i32 minikitId);

#endif
