#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// Episode 6 level handlers, in the game's Episode_VI progression:
// jabbas palace / sarlacc pit / speeder chase / endor battle / death star 2
// battle / emperor fight, plus the senate bonus.

// ===========================================================================
// Jabba's Palace (JabbasPalace_A / B / D / E)
// ===========================================================================

void JabbasPalaceA_Init(WORLDINFO_s *) {
}

void JabbasPalaceB_Init(WORLDINFO_s *) {
}

void JabbasPalaceE_Init(WORLDINFO_s *) {
}

void JabbasPalaceA_Reset(WORLDINFO_s *) {
}

void JabbasPalaceB_Reset(WORLDINFO_s *) {
}

void JabbasPalaceD_Reset(WORLDINFO_s *) {
}

void JabbasPalaceE_Reset(WORLDINFO_s *) {
}

void JabbasPalaceE_Panel(WORLDINFO_s *) {
}

void JabbasPalaceA_Update(WORLDINFO_s *) {
}

void JabbasPalaceE_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Sarlacc Pit (SarlaccPit_A / B / C)
// ===========================================================================

void SarlaccPitA_Draw(WORLDINFO_s *) {
}

void SarlaccPitA_Reset(WORLDINFO_s *) {
}

void SarlaccPitB_Init(WORLDINFO_s *) {
}

void SarlaccPitB_Reset(WORLDINFO_s *) {
}

void SarlaccPitB_Update(WORLDINFO_s *) {
}

void SarlaccPitB_SpecialUpdate(WORLDINFO_s *) {
}

void SarlaccPitC_Init(WORLDINFO_s *) {
}

void SarlaccPitC_Reset(WORLDINFO_s *) {
}

void SarlaccPitC_Update(WORLDINFO_s *) {
}

void SarlaccPitDiscoActive(WORLDINFO_s *) {
}

// ===========================================================================
// Endor battle (EndorBattle_A / C)
// ===========================================================================

void EndorBattleA_Init(WORLDINFO_s *) {
}

void EndorBattleC_Init(WORLDINFO_s *) {
}

void EndorBattleA_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Death Star 2 battle
// ===========================================================================

void DeathStar2BattleD_Init(WORLDINFO_s *) {
}

void DeathStar2BattleD_Update(WORLDINFO_s *) {
}

void DeathStar2BattleD_InZapRange(GameObject_s *) {
}

void DeathStar2BattleA_AlwaysUpdate(WORLDINFO_s *) {
}

// ===========================================================================
// Emperor fight (EmperorFight_A)
// ===========================================================================

void EmperorFightA_Init(WORLDINFO_s *) {
}

void EmperorFightA_Reset(WORLDINFO_s *) {
}

void EmperorFightA_Update(WORLDINFO_s *) {
}

void EmperorFightA_Panel(WORLDINFO_s *) {
}

// ===========================================================================
// Fire / slow-down helpers (Death Star 2 fire)
// ===========================================================================

void DeathStar2BattleFire_Draw(WORLDINFO_s *) {
}

void DeathStar2BattleFire_Init(WORLDINFO_s *) {
}

void DeathStar2BattleFire_Update(WORLDINFO_s *) {
}

void DeathStar2BattleFire_GetSlowDownMul(GameObject_s *) {
}

void DeathStar2BattleFire_SetSlowDownMul(GameObject_s *) {
}

void DeathStar2BattleFire_UpdateSlowDownMul(float) {
}
