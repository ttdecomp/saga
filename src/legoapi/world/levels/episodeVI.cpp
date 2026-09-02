#include "decomp.h"
#include "globals.h"
#include "legoapi/ai/core/ai_sys_stubs.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

extern tertype TerSurface[32];

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" i32 FindPlatInst(void *instance_ix);

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
    TerSurface[14].dwFlags = 0xa040;
    TerSurface[9].dwFlags = 0x2040;
}

void JabbasPalaceB_Reset(WORLDINFO_s *) {
    TerSurface[14].dwFlags = 0xa040;
    TerSurface[9].dwFlags = 0x2040;
}

void JabbasPalaceD_Reset(WORLDINFO_s *world) {
    TerSurface[14].dwFlags = 0xa040;
    TerSurface[9].dwFlags = 0x2040;
    if (world->current_level == JABBASPALACED_LDATA && world->push_block_count > 0) {
        pushblock_s *push_block = world->push_blocks;
        NUDISPLAYSPECIAL_s *display_special = push_block->special.display_special;
        if (display_special != NULL) {
            NUGSCN *scene = push_block->special.scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->visibility_flags[display_special->instance_ix] |= 8;
        }
    }
}

void JabbasPalaceE_Reset(WORLDINFO_s *world) {
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, "AI_rancor");
    LevArea[0] = AISysFindArea(world->ai_sys, "Alcove_1");
    LevArea[1] = AISysFindArea(world->ai_sys, "Alcove_2");
    LevArea[2] = AISysFindArea(world->ai_sys, "Back_Room");
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    TerSurface[14].dwFlags = 0xa040;
    TerSurface[9].dwFlags = 0x2040;
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

void SarlaccPitA_Reset(WORLDINFO_s *world) {
    LevSafePlatID[0] = -1;
    if (NuSpecialFind(world->current_gscn, &LevHSpecial[0], "float_skiff_2", 1) == 0) {
        return;
    }
    if (world->terrain == NULL) {
        return;
    }
    LevSafePlatID[0] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[0]));
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
