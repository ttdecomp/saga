#pragma once

#include "legoapi/core/input/timer.h"
#include "nu2api/nucore/common.h"

typedef struct MISSIONSAVE_s {
    char data[0xc];
} MISSIONSAVE;

// One mission entry in the mission buffer; 0x18 bytes each.
typedef struct MISSIONDATA_s {
    i16 find_char; // 0x00
    i16 level;     // 0x02
    i16 name_id;   // 0x04
    i16 text_id;   // 0x06
    i32 bounty;    // 0x08
    i32 bounty2;   // 0x0c
    i16 time;      // 0x10
    u8 count;      // 0x12
    u8 pad[0x5];   // 0x13 .. 0x18
} MISSIONDATA;

struct MISSIONSYS_s {
    i32 length;                // 0x00
    MISSIONDATA *mission;      // 0x04 (pointer to the active mission entry)
    TIMER timer;               // 0x08
    MISSIONSAVE *mission_save; // 0x18
    u8 count;                  // 0x1c
    u8 field8_0x1d;            // 0x1d (mission state)
    u8 flags;                  // 0x1e
    u8 character_count;        // 0x1f
    i16 character_ids[8];      // 0x20
};
typedef struct MISSIONSYS_s MISSIONSYS;

#ifdef __cplusplus

MISSIONSYS *Missions_Configure(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, MISSIONSAVE *save);
MISSIONDATA *Mission_Active(MISSIONSYS *mission_system);

extern MISSIONSYS *MissionSys;
#endif
