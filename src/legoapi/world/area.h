#pragma once

#include "decomp.h"
#include "legoapi/world/level.h"

#include "nu2api/nucore/common.h"

enum {
    AREAFLAG_NONE = 0x0,
    AREAFLAG_SINGLE_BUFFER = 0x8,
    AREAFLAG_MINIKIT = 0x10,
    AREAFLAG_NO_GOLDBRICK = 0x800,
    AREAFLAG_TRUE_JEDI = 0x4000,
    AREAFLAG_TEST_AREA = 0x20,
    AREAFLAG_HUB_AREA = 0x40,
    AREAFLAG_OVERRIDE_THINGS_SCENE = 0x400,
    AREAFLAG_VEHICLE_AREA = 0x1,
    AREAFLAG_ENDING_AREA = 0x2,
    AREAFLAG_BONUS_AREA = 0x4,
    AREAFLAG_SUPER_BONUS_AREA = 0x100 | AREAFLAG_BONUS_AREA,
    AREAFLAG_NO_CHARACTER_COLLISION = 0x80,
    AREAFLAG_NOPICKUPGRAVITY = 0x200,
    AREAFLAG_NO_COMPLETION_POINTS = 0x2000,
    AREAFLAG_NO_FREEPLAY = 0x1000,
};

typedef struct AREADATA_s {
    char dir[64];
    char file[32];
    i16 field2_0x60[12];

    i16 field25_0x78;

    u16 flags;

    byte field27_0x7c;
    byte field28_0x7d;
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
    i16 field39_0x94;
    byte field40_0x96;
    byte field41_0x97;
    i16 *field42_0x98;
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
    void *load_fn;
    void *init_fn;
    void *reset_fn;
    void *update_fn;
    void *draw_fn;
    void *draw_status_fn;
} AREAFIXUP;

AREADATA *Area_FindByName(char *name, i32 *indexDest);

AREADATA *Areas_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 count, i32 *countDest);

struct LEVELDATA_s *Area_FindStatusLevel(AREADATA *area, i32 *indexDest);

void FixUpAreas(void);
void Areas_FixUp(AREAFIXUP *fixup);

struct LEVELDATA_s *Area_FindNextPlayLevel(i32 levelIdx);

i32 AreaFromMiniKitID(i32 minikitId);

#endif
