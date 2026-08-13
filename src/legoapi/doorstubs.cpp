// Hub/door subsystem globals and stubbed functions referenced by
// Players_InitPositions (and friends). These are not matched by objdiff
// individually; they exist so the legoapi/players object links and so its
// relocations carry the exact ROM symbol names.

#include "legoapi/apiobject.h"
#include "legoapi/area.h"
#include "legoapi/world.h"

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

// ---- Player start positions ----
// One entry per player.  Stride is 0x10 in the original binary.
typedef struct playerstart_s {
    NUVEC *pos; // 0x00
    f32 u4;     // 0x04
    f32 u8;     // 0x08
    i16 angle;  // 0x0c
} PLAYERSTARTENTRY;

PLAYERSTARTENTRY PlayerStart[8];

// ---- Exit-camera / cut-door data ----
NUVEC Door_CutCamPos0;
NUVEC Door_CutCamPos1;
i32 Door_UseCutCam = 0;
NUGSPLINE *Door_CutSpl = NULL;
char Door_ExitCameraSplineName[64] = {0};
char Door_ExitName[64] = {0};

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

// ---- Stubbed functions ----

// C-linkage (plain symbol in the ROM).
extern "C" {
    void ComplexSockPosition(void *a, void *b, i32 c, i32 d, SOCKPOSITION *out) {
        (void)a;
        (void)b;
        (void)c;
        (void)d;
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        out->heading = 0.0f;
    }
}

struct EPISODEDATA; // distinct tag => mangles to P11EPISODEDATA

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

i32 Episode_FindAreaFromFlags(struct EPISODEDATA *ep, u32 a, u32 b) {
    (void)ep;
    (void)a;
    (void)b;
    return -1;
}

void *CutScenePlayer_Available(void) {
    return NULL;
}

void ChatterSfx(struct GameObject_s *g, int a, float b) {
    (void)g;
    (void)a;
    (void)b;
}

void Move_VEHICLE(struct GameObject_s *g) {
    (void)g;
}

// ---- PreResetCode dependency stubs ----
// Real functions of the original binary, not yet decompiled.  Stubbed so the
// players object links; these symbols are matched separately by objdiff.

void DrawOffsetCode(GameObject_s *obj, int param) {
    (void)obj;
    (void)param;
}

// rtlDynamicEnable uses C linkage in the original binary (plain symbol name).
extern "C" void rtlDynamicEnable(int id, int param) {
    (void)id;
    (void)param;
}

float GameObjectNearFloor(GameObject_s *obj, float h, float *out) {
    (void)obj;
    (void)h;
    if (out != NULL) {
        *out = 0.0f;
    }
    return 0.0f;
}

float GetHoverPosY(GameObject_s *obj) {
    (void)obj;
    return 0.0f;
}

int Cheat_IsOn(int cheat) {
    (void)cheat;
    return 0;
}

int Player_HasPurpleForce(GameObject_s *obj) {
    (void)obj;
    return 0;
}

unsigned GAMEPAD_ACTION = 0;
