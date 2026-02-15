#pragma once

#include "game/timer.h"
#include "nu2api.saga/nucore/common.h"

typedef struct MISSIONSAVE_s {
    char data[0xc];
} MISSIONSAVE;

struct missionsys_s {
    int length;
    u8 field1_0x4;
    u8 field2_0x5;
    u8 field3_0x6;
    u8 field4_0x7;
    TIMER timer;
    MISSIONSAVE *mission_save;
    u8 count;
    u8 field8_0x1d;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
};
typedef struct missionsys_s MISSIONSYS;

#ifdef __cplusplus

MISSIONSYS *Missions_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, MISSIONSAVE *save);

extern "C" {
#endif
    extern MISSIONSYS *MissionSys;
#ifdef __cplusplus
}
#endif
