#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct nugspline_s;
struct WORLDINFO_s;

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
