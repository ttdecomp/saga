#include "decomp.h"
#include "legoapi/legoapi_types.h"
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

void BlockadeRunnerC_Init(WORLDINFO_s *) {
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

void TatooineC_Init(WORLDINFO_s *) {
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
}

void MosEisleyD_AlwaysUpdate(WORLDINFO_s *) {
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
}

void DeathStarBattleDDraw(WORLDINFO_s *) {
}

void DeathStarBattleDInit(WORLDINFO_s *) {
}

void DeathStarBattleDReset(WORLDINFO_s *) {
}

void DeathStarBattleDUpdate(WORLDINFO_s *) {
}
