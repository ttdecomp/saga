#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/level.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// Episode 4 level handlers, in the game's Episode_IV progression:
// blockade runner / tatooine / mos eisley / death star rescue / escape /
// battle.

// ===========================================================================
// Blockade runner (BlockadeRunner_B / BlockadeRunner_C / BlockadeRunner_D)
// ===========================================================================

void BlockadeRunnerB_Init(WORLDINFO_s *) {
}

void BlockadeRunnerC_Init(WORLDINFO_s *world) {
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle8");
}

void BlockadeRunnerB_Update(WORLDINFO_s *) {
}

void BlockadeRunnerD_Update(WORLDINFO_s *) {
}

void BlockadeRunnerD_Reset(WORLDINFO_s *) {
}

// ===========================================================================
// Tatooine (Tatooine_A / B / C / D)
// ===========================================================================

void TatooineA_Init(WORLDINFO_s *) {
}

void TatooineB_Init(WORLDINFO_s *) {
}

void TatooineC_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b = GizmoBlowUp_FindByName(world, "boxblowup_3");
    if (b != NULL)
        b->field_0xa0 |= 0x10000;
}

void TatooineD_Init(WORLDINFO_s *) {
}

void TatooineA_Update(WORLDINFO_s *) {
}

void TatooineD_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Mos Eisley (MosEisley_A / B / D / E)
// ===========================================================================

void MosEisleyA_Init(WORLDINFO_s *) {
}

void MosEisleyB_Init(WORLDINFO_s *) {
}

void MosEisleyD_Init(WORLDINFO_s *) {
}

void MosEisleyE_Init(WORLDINFO_s *) {
}

void MosEisleyB_Update(WORLDINFO_s *) {
}

void MosEisleyE_Update(WORLDINFO_s *) {
}

void MosEisleyE_Reset(WORLDINFO_s *) {
    texanimbits &= ~2;
}

void MosEisleyD_AlwaysUpdate(WORLDINFO_s *world) {
    LevelStreaming_DoorOverride(world, MOSEISLEYE_LDATA, 7.5f, NULL);
}

void MosEisleyC_PastBarrier(GameObject_s *) {
}

// ===========================================================================
// Death Star rescue (DeathStarRescue_B / C)
// ===========================================================================

void DeathStarRescueB_Init(WORLDINFO_s *) {
}

void DeathStarRescueC_Init(WORLDINFO_s *) {
}

void DeathStarRescueB_Update(WORLDINFO_s *) {
}

void DeathStarRescueB_AlwaysUpdate(WORLDINFO_s *) {
}

void DeathStarRescueC_AlwaysUpdate(WORLDINFO_s *) {
}

// ===========================================================================
// Death Star escape (DeathStarEscape_A / B / C / D)
// ===========================================================================

void DeathStarShieldDown() {
}

void DeathStarEscapeA_Init(WORLDINFO_s *) {
}

void DeathStarEscapeB_Init(WORLDINFO_s *) {
}

void DeathStarEscapeB_Draw(WORLDINFO_s *) {
}

void DeathStarEscapeC_Init(WORLDINFO_s *) {
}

void DeathStarEscapeA_Update(WORLDINFO_s *) {
}

void DeathStarEscapeB_Update(WORLDINFO_s *) {
}

void DeathStarEscapeC_Update(WORLDINFO_s *) {
}

void DeathStarEscapeD_Update(WORLDINFO_s *) {
}

void DeathStarEscapeC_Reset(WORLDINFO_s *) {
}

void DeathStarEscapeB_AlwaysUpdate(WORLDINFO_s *) {
}

void KillParts_TIEFIGHTER(ADDPART_s *, i32, i32, GameObject_s *, i32, u16, u16, nuvec_s *) {
}

// ===========================================================================
// Death Star battle (DeathStarBattle_C / D)
// ===========================================================================

void DeathStarBattleC_AlwaysUpdate(WORLDINFO_s *) {
    if (DEATHSTARBATTLEMIDTRO_LDATA != NULL)
        other_level_override = DEATHSTARBATTLEMIDTRO_LDATA->idx;
}

void DeathStarBattleDDraw(WORLDINFO_s *) {
}

void DeathStarBattleDInit(WORLDINFO_s *) {
}

void DeathStarBattleDReset(WORLDINFO_s *) {
    memset(trenchrun, 0, sizeof(trenchrun));
}

void DeathStarBattleDUpdate(WORLDINFO_s *) {
}
