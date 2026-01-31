#pragma once

#include <stdint.h>

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif

    enum areaflags_e {
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
    typedef enum areaflags_e AREAFLAGS;

    struct areadata_s { /* PlaceHolder Structure */
        char dir[64];
        char file[32];
        short field2_0x60[1];
        undefined field3_0x62;
        undefined field4_0x63;
        undefined field5_0x64;
        undefined field6_0x65;
        undefined field7_0x66;
        undefined field8_0x67;
        undefined field9_0x68;
        undefined field10_0x69;
        undefined field11_0x6a;
        undefined field12_0x6b;
        undefined field13_0x6c;
        undefined field14_0x6d;
        undefined field15_0x6e;
        undefined field16_0x6f;
        undefined field17_0x70;
        undefined field18_0x71;
        undefined field19_0x72;
        undefined field20_0x73;
        undefined field21_0x74;
        undefined field22_0x75;
        undefined field23_0x76;
        undefined field24_0x77;
        undefined2 field25_0x78;
        AREAFLAGS flags;
        undefined1 field27_0x7c;
        byte field28_0x7d;
        byte cheat;
        undefined1 field30_0x7f;
        int field31_0x80;
        short challenge_trial_time;
        byte episode_index;
        byte area_index;
        short area_music;
        undefined2 minikit_id;
        undefined4 field37_0x8c;
        undefined4 field38_0x90;
        undefined2 field39_0x94;
        byte field40_0x96;
        undefined field41_0x97;
        undefined4 field42_0x98;
    };
    typedef struct areadata_s AREADATA;

    extern AREADATA *ADataList;
    extern AREADATA *HUB_ADATA;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

AREADATA *Area_FindByName(char *name, int32_t *indexDest);

AREADATA *Areas_ConfigureList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, int count, int *countDest);

static inline AREAFLAGS operator|(AREAFLAGS a, AREAFLAGS b) {
    return static_cast<AREAFLAGS>(static_cast<int>(a) | static_cast<int>(b));
}
static inline AREAFLAGS operator|=(AREAFLAGS &a, AREAFLAGS b) {
    a = a | b;
    return a;
}

#endif
