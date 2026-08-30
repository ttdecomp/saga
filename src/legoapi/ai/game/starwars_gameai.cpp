#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct AISYS_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

typedef void (*PREPARINGSPECIALMOVEFN)(AIPACKET_s *, APIOBJECT_s *, i32);
typedef void (*MIDSPECIALMOVEFN)(AISYS_s *, AIPACKET_s *, APIOBJECT_s *);
typedef void (*HUBCALLBACK)(WORLDINFO_s *);

extern "C" void InitFn_PreparingForSpecialMove(PREPARINGSPECIALMOVEFN);
extern "C" void InitFn_MidSpecialMove(MIDSPECIALMOVEFN);

extern i32 LEGO_AIPATHCNX_JUMP;
extern i32 LEGO_AIPATHCNX_DOUBLE_JUMP;
extern i32 LEGO_AIPATHCNX_HIGH_JUMP;
extern i32 LEGO_AIPATHCNX_R2D2GLIDE;
extern i32 LEGO_AIPATHCNX_FORGOODIES;
extern i32 LEGO_AIPATHCNX_FORBADDIES;
extern i32 LEGO_AIPATHCNX_BLOCKAGE;
extern i32 LEGO_AIPATHCNX_DONTTOGGLE;
extern i32 LEGO_AIPATHCNX_FULLTERRAIN;
extern i32 LEGO_AIPATHCNX_BIGJUMP;
extern i32 LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern i32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
extern i32 LEGO_AIPATHCNX_JUMP_NOW;
extern i32 LEGO_AIPATHCNX_DONT_JUMP_NOW;
extern i32 mechAutoJumpFlags;
extern i32 mechAutoJumpCantReachFlags;

extern HUBCALLBACK Hub_InitAIFn;
extern HUBCALLBACK Hub_ResetAIFn;
extern HUBCALLBACK Hub_UpdateAIFn;

extern void LSW_Hub_InitAI(WORLDINFO_s *);
extern void LSW_Hub_ResetAI(WORLDINFO_s *);
extern void LSW_Hub_UpdateAI(WORLDINFO_s *);

static void StarWars_PreparingForSpecialMove(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static void StarWars_MidSpecialMove(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
}

void StarWars_PrepareJump(AIPACKET_s *, APIOBJECT_s *, i32) {
}

void StarWars_GameAISysInit() {
    LEGO_AIPATHCNX_JUMP = 1;
    LEGO_AIPATHCNX_DOUBLE_JUMP = 2;
    LEGO_AIPATHCNX_HIGH_JUMP = 0x20;
    LEGO_AIPATHCNX_R2D2GLIDE = 4;
    LEGO_AIPATHCNX_FORGOODIES = 0x80;
    LEGO_AIPATHCNX_FORBADDIES = 0x100;
    LEGO_AIPATHCNX_BLOCKAGE = 0x40000;
    LEGO_AIPATHCNX_DONTTOGGLE = 0x80000;
    LEGO_AIPATHCNX_FULLTERRAIN = 0x100000;
    LEGO_AIPATHCNX_BIGJUMP = 0x1000;
    LEGO_AIPATHCNX_REQUIRESPERMISSION = 0x2000;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x200;
    LEGO_AIPATHCNX_JUMP_NOW = 0x400;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x800;

    InitFn_PreparingForSpecialMove(StarWars_PreparingForSpecialMove);
    InitFn_MidSpecialMove(StarWars_MidSpecialMove);

    Hub_InitAIFn = LSW_Hub_InitAI;
    Hub_ResetAIFn = LSW_Hub_ResetAI;
    Hub_UpdateAIFn = LSW_Hub_UpdateAI;
    mechAutoJumpFlags = 0xe00023;
    mechAutoJumpCantReachFlags = 0xffedfe7f;
}

void StarWars_PrepareR2D2Glide(AIPACKET_s *, APIOBJECT_s *, i32) {
}

void StarWars_ParseAIPathCnxFlag(char *) {
}

void StarWars_AutoSetAICapabilities(GameObject_s *) {
}
