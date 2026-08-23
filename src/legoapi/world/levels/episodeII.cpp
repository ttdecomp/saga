#include <stdio.h>

#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/gizmo/base/GizForceObjectInterface.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/ai/core/ai_sys_stubs.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/render.h"
#include "legoapi/world/level_shared.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// --- Cross-file entry points / id globals ---
//
// AIPAthFindPathCnx remains local here because it is called with a different
// argument arity than in episodeI (both are byte-matched as-is), so it cannot
// live in a shared header.

extern "C" {
    void *AIPAthFindPathCnx(AISYS_s *, i32, void *, void *, void *); // legoapi/ai pathfinding
}

// --- File-local statics (original _ZL... symbols; not renamed) ---------------

// Kamino disco-room state (original _ZL11kaminodisco). A byte flag (0/1/2)
// that KaminoC_Init clears via memset of the enclosing disco struct, which is
// why readers cannot be constant-folded.
static u8 kaminodisco;
static i32 dooku_c;            // _ZL7dooku_c
static i32 dooku_state[4];     // dooku level state (hit counters / node)
static void *kamino_e_state;   // kamino_e level state block
static void *kamino_e_special; // kamino_e named scene object

// Episode 2 level handlers, in the game's Episode_II progression:
// pursuit (coruscant bounty-hunter) / kamino / factory (geonosis droid
// factory) / jedi / gunship / bonus gunship / dooku, then the NewTown bonus.
//
// The bounty-hunter pursuit and bonus-gunship functions came from a separate
// pursuit.cpp translation unit in the original binary (_GLOBAL__sub_I_pursuit.cpp);
// JediB currently lives here per the Episode II level grouping.

// ===========================================================================
// Coruscant — bounty-hunter pursuit (Zam Wesell)
// ===========================================================================

void BountyHunterPursuitA_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "Jango")) != NULL)
        *((u8 *)b + 0x9f) |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b1")) != NULL)
        *((u8 *)b + 0x9f) |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b2")) != NULL)
        *((u8 *)b + 0x9f) |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b3")) != NULL)
        *((u8 *)b + 0x9f) |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b4")) != NULL)
        *((u8 *)b + 0x9f) |= 0x20;
}

void BountyHunterPursuitB_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitC_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitD_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitA_Reset(WORLDINFO_s *) {
}

void BountyHunterPursuitB_Reset(WORLDINFO_s *) {
}

void BountyHunterPursuitC_Reset(WORLDINFO_s *) {
}

void BountyHunterPursuitD_Reset(WORLDINFO_s *) {
}

void BountyHunterPursuitA_Update(WORLDINFO_s *) {
}

void BountyHunterPursuitB_Update(WORLDINFO_s *) {
}

void BountyHunterPursuitC_Update(WORLDINFO_s *) {
}

void BountyHunterPursuitD_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Kamino
// ===========================================================================

i32 KaminoInside() {
    if (WORLD->area != NULL && WORLD->area == KAMINO_ADATA) {
        if (WORLD->current_level == KAMINOA_LDATA) {
            if (CUTSTOPGAME == 0) {
                if (*((u8 *)GameCam + 1) == 5)
                    return 1;
                if (*((u8 *)GameCam + 1) == 6)
                    return 1;
            } else {
                return 1;
            }
        } else if (WORLD->current_level == KAMINOE_LDATA) {
            if (CUTSTOPGAME == 0 && *((u8 *)GameCam + 1) != 0x1e)
                return 1;
        }
    }
    return 0;
}

i32 KaminoDiscoOn() {
    return kaminodisco == 2;
}

i32 KaminoInDiscoRoom() {
    i32 r = 0;
    if (WORLD->current_level == KAMINOC_LDATA)
        r = (*((u8 *)GameCam + 1) == 0x15);
    return r;
}

void KaminoA_AlwaysUpdate(WORLDINFO_s *) {
    bool v = 0;
    if (CUTSTOPGAME == 0) {
        u8 b = *((u8 *)GameCam + 1);
        if (b != 5)
            v = (b != 6);
    }
    object_switches[1] = v;
}

void KaminoC_Init(WORLDINFO_s *) {
}

void KaminoC_Reset(WORLDINFO_s *) {
}

void KaminoC_Update(WORLDINFO_s *) {
}

void KaminoD_Init(WORLDINFO_s *) {
}

void KaminoE_Init(WORLDINFO_s *world) {
    kaminoe_netpacket = SetLevelHack(0x14);
    GIZMO_s *g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "Force");
    if (g != NULL)
        LevForce = *(i32 *)g;
}

void KaminoE_Reset(WORLDINFO_s *) {
}

void KaminoE_Update(WORLDINFO_s *) {
}

void KaminoE_AlwaysUpdate(WORLDINFO_s *) {
    bool v = 1;
    if (CUTSTOPGAME == 0)
        v = (*((u8 *)GameCam + 1) == 0x1e);
    object_switches[1] = v;
}

void KaminoE_Draw(WORLDINFO_s *world) {
    if (netclient == 0) {
        if (kamino_e_state != NULL && *(float *)((u8 *)kamino_e_state + 0x28) > 0.0f) {
            GameObject_s *obj = (GameObject_s *)FindGameObject((i32)(i16)id_JANGOFETT, 1, 1, 1, 0);
            if (obj != NULL && kamino_e_state != NULL && *(float *)((u8 *)obj + 0x28) == 1.0f)
                DrawBossHitPoints(obj);
        }
    }
    NuSpecialSetDrawMtx(&kamino_e_special, NuSpecialGetDrawMtx(&kamino_e_special));
    NuSpecialSetVisibility(&kamino_e_special, 1);
}

void KaminoE_CheckPlatHit(BOLT_s *) {
}

void KaminoF_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "f1")) != NULL) {
        b->field_0x128 = 0.3f;
        b->field_0x124 = 1;
    }
    if ((b = GizmoBlowUp_FindByName(world, "f2")) != NULL) {
        b->field_0x128 = 0.3f;
        b->field_0x124 = 1;
    }
    if ((b = GizmoBlowUp_FindByName(world, "f3")) != NULL) {
        b->field_0x128 = 0.3f;
        b->field_0x124 = 1;
    }
}

void KaminoOutro_Init(WORLDINFO_s *) {
    bool v = 0;
    if (CUTSTOPGAME == 0) {
        u8 b = *((u8 *)GameCam + 1);
        if (b != 5)
            v = (b != 6);
    }
    object_switches[1] = v;
}

void NbKaminoA_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "nb1")) != NULL) {
        b->field_0x128 = 1.0f;
        b->field_0x124 = 1;
    }
    if ((b = GizmoBlowUp_FindByName(world, "nb2")) != NULL) {
        b->field_0x128 = 1.0f;
        b->field_0x124 = 1;
    }
}

// ===========================================================================
// Geonosis — droid factory (Factory_B / Factory_G)
// ===========================================================================

void FactoryB_Init(WORLDINFO_s *) {
}

void FactoryB_Reset(WORLDINFO_s *world) {
    ResetPaintPuzzle(world);
    factoryb_cut = (void *)(usize)NewCutScene(NULL, world->cutscene_sys, "fb_cut", 0);
    if (factoryb_cut != NULL) {
        void *scene = *(void **)((u8 *)factoryb_cut + 4);
        if (scene != NULL) {
            *((u8 *)scene + 0x88) |= 2;
            *((u8 *)scene + 0x88) |= 8;
        }
    }
    factoryb_conveyor_stopped_msg = CheckGizAIMessage(gizaimessagesys, "conv_stopped", NULL);
}

void FactoryB_Update(WORLDINFO_s *) {
}

void FactoryB_Draw(WORLDINFO_s *) {
    DrawPaintLights();
}

void FactoryG_Init(WORLDINFO_s *) {
}

void FactoryG_Update(WORLDINFO_s *) {
}

// ===========================================================================
// Jedi (Jedi_B)
// ===========================================================================

void JediB_Init(WORLDINFO_s *) {
}

void JediB_Reset(WORLDINFO_s *) {
}

void JediB_Update(WORLDINFO_s *) {
}

void JediB_DrawPanel(WORLDINFO_s *) {
}

// ===========================================================================
// Gunship (Gunship_A / Gunship_B)
// ===========================================================================

void GunshipA_Init(WORLDINFO_s *) {
}

void GunshipA_Update(WORLDINFO_s *) {
}

void GunshipA_Draw(WORLDINFO_s *) {
}

void GunshipB_Reset(WORLDINFO_s *world) {
    LevGizmo[0] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun1");
    LevGizmo[1] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun2");
    LevGizmo[2] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun3");
    LevGizmo[3] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun4");
    LevGizmo[4] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun5");
    LevGizmo[5] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun6");
    LevGizmo[6] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun7");
    LevGizmo[7] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun8");
}

i32 GunshipInLevel(LEVELDATA_s *level) {
    if (BONUS_GUNSHIPA_LDATA == NULL)
        return 0;
    return BONUS_GUNSHIPA_LDATA == level;
}

void GunShip_DragBombSeekBlowUp(GameObject_s *) {
}

// ===========================================================================
// Bonus gunship (Bonus_Gunship_A / Bonus_Gunship_B)
// ===========================================================================

void BonusGunshipA_Reset(WORLDINFO_s *) {
    gunship_player_dead = 0;
    if (*((char *)LevFlag) == 1)
        *((char *)LevFlag) = 2;
    *((char *)LevFlag + 1) = 0;
    bonus_gunship_store_progress_flag = 0;
}

void BonusGunshipA_Update(WORLDINFO_s *world) {
    if (*((char *)LevFlag) == 0) {
        bool found = false;
        if (Player[0] != NULL && *((u8 *)Player[0] + 0x661) == 0 && *(float *)((u8 *)Player[0] + 0x68c) > 0.001f) {
            found = true;
        } else if (Player[1] != NULL && *((u8 *)Player[1] + 0x661) == 0 &&
                   *(float *)((u8 *)Player[1] + 0x68c) > 0.001f) {
            found = true;
        }
        if (found) {
            if (netclient != 0) {
                *((char *)LevFlag) = 1;
            } else {
                Doors_SetLastDoor((DOOR_s *)Door_FindByName(world, "bonus_door"));
                bonus_gunship_store_progress_flag = 1;
                StoreLevelProgress(world);
                bonus_gunship_store_progress_flag = 0;
                *((char *)LevFlag) = 1;
            }
        }
    }
    if (gunship_player_dead == 0) {
        if ((Player[0] != NULL && *((u8 *)Player[0] + 0x287) != 0) ||
            (Player[1] != NULL && *((u8 *)Player[1] + 0x287) != 0)) {
            gunship_player_dead = 1;
            ResetLevel(world, "bonus", 1);
        }
    }
}

void BonusGunshipB_Init(WORLDINFO_s *world) {
    bonusgunshipb_netpacket = SetLevelHack(0xc);
    LevGizObst[0] = (i32)(usize)GizObstacle_FindByName((GIZOBSTACLESYS_s *)*(void **)((u8 *)world + 0x46a8), "obs");
}

void BonusGunshipB_Reset(WORLDINFO_s *) {
    *((char *)LevFlag) = 0;
    *((char *)LevFlag + 1) = 0;
    MiscTime = 0;
    gunship_player_dead = 0;
    bonus_gunship_store_progress_flag = 0;
}

void BonusGunshipB_Update(WORLDINFO_s *) {
}

void BonusGunshipB_Panel(WORLDINFO_s *) {
    if (*((char *)LevFlag) == 1) {
        if (MiscTime > 60.0f)
            DrawTimer((i32)MiscTime + 1, 0, 0);
    }
}

// ===========================================================================
// Dooku (Dooku_C)
// ===========================================================================

void DookuC_Init(WORLDINFO_s *world) {
    LevGizForce[0] = GizForce_FindByName(world->giz_force_sys, "dooku");
    LevGizForce[1] = GizForce_FindByName(world->giz_force_sys, "dooku1");
    LevGizForce[2] = GizForce_FindByName(world->giz_force_sys, "dooku2");
    void *path1 = AIPathFindNode(world->ai_sys, "path1", 0);
    LevAIPathNode[0] = (i32)(usize)path1;
    void *path2 = AIPathFindNode(world->ai_sys, "path2", 0);
    LevAIPathNode[1] = (i32)(usize)path2;
    void *path3 = AIPathFindNode(world->ai_sys, "path3", 0);
    LevAIPathNode[2] = (i32)(usize)path3;
    void *path4 = AIPathFindNode(world->ai_sys, "path4", 0);
    LevAIPathNode[3] = (i32)(usize)path4;
    char buf[0x40];
    LevPathCnx[0] = AIPAthFindPathCnx(world->ai_sys, 0, path1, path2, buf);
    LevPathCnx[1] = AIPAthFindPathCnx(world->ai_sys, 0, path2, path3, buf);
    LevPathCnx[2] = AIPAthFindPathCnx(world->ai_sys, 0, path3, path4, buf);
    LevPathCnx[3] = AIPAthFindPathCnx(world->ai_sys, 0, path4, (void *)"conn", buf);
    dookuC_nodesNeedUpdating = 1;
}

void DookuC_Reset(WORLDINFO_s *world) {
    dooku_c = 0;
    dooku_state[0] = 0;
    dooku_state[1] = 0;
    dooku_state[2] = 0;
    dooku_state[3] = 0;
    if (netclient == 0) {
        dooku_c = (i32)(usize)SetGizAIMessage(gizaimessagesys, "dooku_total", 0.0f, NULL);
        dooku_state[0] = (i32)(usize)CheckGizAIMessage(gizaimessagesys, "dooku_hits", NULL);
    }
    NuSpecialFind(world->current_gscn, (void **)&dooku_state[1], "dooku_node", 1);
}

void DookuC_Update(WORLDINFO_s *) {
}

void DookuC_DrawPanel(WORLDINFO_s *) {
    if (netclient != 0)
        return;
    GameObject_s *obj = (GameObject_s *)FindGameObject((i32)(i16)id_COUNTDOOKU, 1, 1, 1, 0);
    if (obj != NULL && dooku_c != 0 && *((float *)((u8 *)obj + 0x28)) == 1.0f)
        DrawBossHitPoints(obj);
}

// ===========================================================================
// New Town (bonus; original episodeII.cpp TU member)
// ===========================================================================

void NewTown_Init(WORLDINFO_s *world) {
    LevGizmo[0] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "newtown");
    char buf[0x18];
    i32 i = 1;
    for (;;) {
        sprintf(buf, "%i", i);
        GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, buf);
        if (g == NULL)
            break;
        g->field_0xa0 |= 2;
        i++;
    }
}

void NewTown_Reset(WORLDINFO_s *) {
}

void NewTown_Update(WORLDINFO_s *) {
}
