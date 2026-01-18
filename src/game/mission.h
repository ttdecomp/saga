#pragma once

#include <stdint.h>

#include "game/timer.h"
#include "nu2api.saga/nucore/common.h"

struct missionsave_s {
    char data[0xc];
};
typedef struct missionsave_s MISSIONSAVE;

struct missionsys_s {
    int length;
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    TIMER timer;
    MISSIONSAVE *missionSave;
    uint8_t count;
    uint8_t field8_0x1d;
    uint8_t flags;
    uint8_t characterCount;
    int16_t characterIds[8];
};
typedef struct missionsys_s MISSIONSYS;

MISSIONSYS *Missions_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, MISSIONSAVE *save);
