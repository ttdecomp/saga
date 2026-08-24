#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "legoapi/gizmo/base/GizForceObjectInterface.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "legoapi/render/core/render.h"
#include "nu2api/nu3d/nutex.h"

static GIZAIMESSAGE_s *KashyyykA_msg_TotalWookies;
static GIZAIMESSAGE_s *KashyyykA_msg_WookiesToRescue;
static GameObject_s *Grievous_obj; // current Grievous boss object
static i16 vader_a_count;          // Vader A panel guard
static i16 vader_a_sub;            // Vader A countdown subtitle

// Episode 3 level handlers, in the game's Episode_III progression:
// dogfight / cruiser / grievous / kashyyyk / temple / vader / a-new-hope.

// ===========================================================================
// Dogfight (Dogfight_A)
// ===========================================================================

void ChrisDogFightAInit(WORLDINFO_s *) {
}

void ChrisDogFightAReset(WORLDINFO_s *) {
}

void ChrisDogFightAUpdate(WORLDINFO_s *) {
}

void ChrisDogFightADraw(WORLDINFO_s *) {
}

void ChrisDogFightAPanel(WORLDINFO_s *) {
}

// ===========================================================================
// Cruiser (Cruiser_A / Cruiser_C / Cruiser_D)
// ===========================================================================

void CruiserAInit(WORLDINFO_s *) {
}

void CruiserAUpdate(WORLDINFO_s *) {
}

void CruiserCReset(WORLDINFO_s *) {
}

void CruiserCUpdate(WORLDINFO_s *) {
}

void CruiserCPanel(WORLDINFO_s *) {
}

void CruiserDInit(WORLDINFO_s *) {
}

void CruiserDReset(WORLDINFO_s *) {
}

void CruiserDUpdate(WORLDINFO_s *) {
}

// ===========================================================================
// Grievous (Grievous_A)
// ===========================================================================

void GrievousA_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "grievous_1")) != NULL) {
        nuvec_s pos = {5.42f, 2.76f, 1.79f};
        NuSpecialSetDrawPos(b->field_0xac, &pos);
        UpdateMidPos(b);
    }
    if ((b = GizmoBlowUp_FindByName(world, "grievous_2")) != NULL)
        b->field_0x124 = 1;
    if ((b = GizmoBlowUp_FindByName(world, "grievous_3")) != NULL)
        b->field_0x124 = 1;
}

void GrievousA_Reset(WORLDINFO_s *) {
    if (netclient != 0)
        return;
    Grievous_obj = (GameObject_s *)FindGameObject((i32)(i16)id_GRIEVOUS, 1, 1, 0, 0);
    if (Grievous_obj != NULL)
        DrawBossHitPoints(Grievous_obj);
}

void GrievousA_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Kashyyyk (Kashyyyk_A / Kashyyyk_B / Kashyyyk_C / Kashyyyk_D)
// ===========================================================================

void KashyyykA_Init(WORLDINFO_s *) {
}

void KashyyykB_Init(WORLDINFO_s *) {
}

void KashyyykC_Init(WORLDINFO_s *world) {
    GIZFORCE_s *f = GizForces_FindForce(world, "kashyyyk_boss");
    if (f != NULL && f->strength_0x6c == 0.0f)
        f->strength_0x6c = 1.0f;
}

void KashyyykD_Init(WORLDINFO_s *) {
}

void KashyyykA_Panel(WORLDINFO_s *) {
}

void KashyyykA_Reset(WORLDINFO_s *) {
    KashyyykA_msg_TotalWookies = CheckGizAIMessage(gizaimessagesys, "TotalWookies", NULL);
    KashyyykA_msg_WookiesToRescue = CheckGizAIMessage(gizaimessagesys, "WookiesToRescue", NULL);
}

void KashyyykB_Reset(WORLDINFO_s *) {
}

void KashyyykD_Reset(WORLDINFO_s *) {
}

void AnakinGreenSabre(GameObject_s *) {
}

void KashyyykA_Update(WORLDINFO_s *) {
}

void KashyyykB_Update(WORLDINFO_s *) {
}

void KashyyykC_Update(WORLDINFO_s *) {
}

void KashyyykD_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Temple (Temple_A / Temple_C)
// ===========================================================================

void TempleA_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "temple_statue")) != NULL)
        b->field_0xa0 |= 2;
    if ((b = GizmoBlowUp_FindByName(world, "temple_pillar")) != NULL)
        b->field_0xa0 |= 2;
}

void TempleC_Init(WORLDINFO_s *) {
}

void TempleC_AlwaysUpdate(WORLDINFO_s *) {
}

// ===========================================================================
// Vader (Vader_A / Vader_B / Vader_C)
// ===========================================================================

void VaderA_Init(WORLDINFO_s *) {
}

void VaderB_Init(WORLDINFO_s *) {
}

void VaderC_Init(WORLDINFO_s *) {
}

void VaderA_Reset(WORLDINFO_s *) {
}

void VaderB_Reset(WORLDINFO_s *) {
}

void VaderC_Reset(WORLDINFO_s *) {
    vader_c.field_0x94 = 0;
    vader_c.field_0x95 = 0;
}

void VaderA_Update(WORLDINFO_s *) {
}

void VaderB_Update(WORLDINFO_s *) {
}

void VaderC_Update(WORLDINFO_s *) {
}

void VaderA_DrawPanel(WORLDINFO_s *) {
    if (vader_a_count <= 2) {
        if (vader_a_count != 0) {
            DrawTimer(vader_a_sub, 0, 0);
            vader_a_sub = 0;
        }
    }
}

void VaderB_DrawPanel(WORLDINFO_s *) {
}

void VaderC_DrawPanel(WORLDINFO_s *) {
}

void VaderA_GoneThroughDoor(WORLDINFO_s *, DOOR_s *door) {
    if (netclient == 0 && door != NULL)
        door->field_0xf7 = 1;
}

static __used__ void VaderA_StartCollapseStage(WORLDINFO_s *) {
}

// ===========================================================================
// A New Hope (ANewHope_A)
// ===========================================================================

void ANewHopeA_Init(WORLDINFO_s *world) {
    GIZOBSTACLE_s *g;
    if ((g = GizObstacle_FindByName(world->giz_obstacle_sys, "anewhope_1")) != NULL)
        g->field_a1_0xa1 |= 1;
    if ((g = GizObstacle_FindByName(world->giz_obstacle_sys, "anewhope_2")) != NULL)
        g->field_a1_0xa1 |= 1;
    if ((g = GizObstacle_FindByName(world->giz_obstacle_sys, "anewhope_3")) != NULL)
        g->field_a1_0xa1 |= 1;
}
