#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct nugspline_s;
struct WORLDINFO_s;

typedef struct doorspline_s DOORSPLINE;

struct DOOR_s {
    char name[0xa0];         // 0x000
    DOORSPLINE *spl;         // 0x0a0
    char pad1[0xd4 - 0xa4];  // 0x0a4
    NUVEC pos;               // 0x0d4
    char pad2[0xf0 - 0xe0];  // 0x0e0
    i16 level;               // 0x0f0
    char pad3[0xf5 - 0xf2];  // 0x0f2
    u8 flags;                // 0x0f5
    char pad4[0x120 - 0xf6]; // 0x0f6
};

// ---- Hub exit-door bookkeeping ----

extern NUVEC Door_CutCamPos0;
extern NUVEC Door_CutCamPos1;
extern i32 Door_UseCutCam;
extern struct nugspline_s *Door_CutSpl;
extern char Door_ExitCameraSplineName[64];
extern char Door_ExitName[64];
extern i32 Door_Start;

extern void *HubStartDoor;
extern void *VEHICLES_ADATA;
extern i32 LEGOSPL_START;
extern i32 hub_from_superstory;
extern i32 hub_from_mission;
extern i32 hub_from_arcade;
extern i32 hub_from_cutsceneplayer;
extern i32 shop_from_cutsceneplayer;
extern i32 hub_startoutsidebonusdoor_area;

void StartDoorPositions(void);

void *Door_FindByIndex(struct WORLDINFO_s *world, i32 a, i32 b, struct nuvec_s *c);

void *Door_FindByName(struct WORLDINFO_s *world, char *name);
void Doors_SetLastDoor(DOOR_s *door);
