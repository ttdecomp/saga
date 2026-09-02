#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"

struct nugspline_s;
struct WORLDINFO_s;

struct DOOR_s {
    char name[0x40];             // 0x000
    char gizmo_name[0x40];       // 0x040
    char camera_spline_name[32]; // 0x080
    NUGSPLINE *spline;           // 0x0a0
    NUVEC point0;                // 0x0a4
    NUVEC point1;                // 0x0b0
    NUVEC opposite_midpoint;     // 0x0bc
    NUVEC point3;                // 0x0c8
    NUVEC pos;                   // 0x0d4
    f32 radius;                  // 0x0e0
    NUVEC normal;                // 0x0e4
    i16 level;                   // 0x0f0
    i16 freeplay_level;          // 0x0f2
    u8 next_sock;                // 0x0f4
    u8 flags;                    // 0x0f5
    u8 vehicle;                  // 0x0f6
    u8 active;                   // 0x0f7
    NUGSPLINE *camera_spline;    // 0x0f8
    f32 camera_wait;             // 0x0fc
    f32 camera_blend_time;       // 0x100
    u32 vehicle_mask;            // 0x104
    i32 vehicle_mode;            // 0x108
    NUPLANE plane;               // 0x10c
    void *cutscene;              // 0x11c
};

DECOMP_ASSERT(sizeof(DOOR_s) == 0x120, "DOOR_s size");

// ---- Hub exit-door bookkeeping ----

extern NUVEC Door_CutCamPos0;
extern NUVEC Door_CutCamPos1;
extern i32 Door_UseCutCam;
extern struct nugspline_s *Door_CutSpl;
extern char Door_ExitCameraSplineName[64];
extern char Door_ExitName[64];
extern i32 Door_Start;
extern i32 Door_NextSock;

extern void *HubStartDoor;
extern void *VEHICLES_ADATA;
extern i32 LEGOSPL_START;
extern i32 LEGOSPL_SPLIT;
extern i32 hub_from_superstory;
extern i32 hub_from_mission;
extern i32 hub_from_arcade;
extern i32 hub_from_cutsceneplayer;
extern i32 shop_from_cutsceneplayer;
extern i32 hub_startoutsidebonusdoor_area;

void StartDoorPositions(void);

DOOR_s *Door_FindByIndex(WORLDINFO_s *world, i32 area, i32 level, NUVEC *position);

DOOR_s *Door_FindByName(WORLDINFO_s *world, char *name);
void Doors_SetLastDoor(DOOR_s *door);
