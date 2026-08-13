// Hub/exit-door subsystem: the door lookups and hub spawn bookkeeping that
// Players_InitPositions uses to place the player at the correct exit door.

#include "legoapi/door.h"

#include "legoapi/area.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

// ---- Exit-camera / cut-door data ----
NUVEC Door_CutCamPos0;
NUVEC Door_CutCamPos1;
i32 Door_UseCutCam = 0;
struct nugspline_s *Door_CutSpl = NULL;
char Door_ExitCameraSplineName[64] = {0};
char Door_ExitName[64] = {0};
i32 Door_Start = 0;

// ---- Hub exit-door bookkeeping ----
void *HubStartDoor = NULL;
void *VEHICLES_ADATA = NULL;
i32 LEGOSPL_START = -1;
i32 hub_from_superstory = -1;
i32 hub_from_mission = -1;
i32 hub_from_arcade = -1;
i32 hub_from_cutsceneplayer = 0;
i32 shop_from_cutsceneplayer = 0;
i32 hub_startoutsidebonusdoor_area = -1;

void StartDoorPositions(void) {
}

void *Door_FindByIndex(struct WORLDINFO_s *world, i32 a, i32 b, struct nuvec_s *c) {
    (void)world;
    (void)a;
    (void)b;
    (void)c;
    return NULL;
}

void *Door_FindByName(struct WORLDINFO_s *world, char *name) {
    (void)world;
    (void)name;
    return NULL;
}