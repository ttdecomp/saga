#include <stdio.h>

#include "decomp.h"
#include "legoapi/world/level.h"
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
#include "legoapi/world/levels/levels.h"
#include "legoapi/render/core/render.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"
// This level's view of the shared 16-byte LevFlag scratch. byte0 holds the
// bonus-gunship milestone state; byte1 a secondary state.
enum GUNSHIP_STATE_e {
    GUNSHIP_INACTIVE = 0, // never entered
    GUNSHIP_ACTIVE = 1,   // player aboard / stage running
    GUNSHIP_WON = 2,      // stage finished
};
struct GUNSHIP_LEVFLAG_s {
    u8 progress; // 0x00 -> GUNSHIP_STATE_e
    u8 exit;     // 0x01
    u8 pad[14];  // 0x02
};
static_assert(sizeof(struct GUNSHIP_LEVFLAG_s) == 16, "LevFlag must be 16 bytes");
extern struct GUNSHIP_LEVFLAG_s LevFlag;

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
static i32 dooku_c;        // _ZL7dooku_c
static i32 dooku_state[4]; // dooku level state (hit counters / node)

// kamino_e level state block and hud scene object.
struct kamino_e_state_s {
    char pad_0x00[0x28];
    f32 field_0x28; // 0x28
};
static struct kamino_e_state_s *kamino_e_state;
static void *kamino_e_special;   // kamino_e named scene object
static i32 pursuit_state[0x20];  // bounty-hunter pursuit state
static i16 gunship_bolts[2];     // gun-ship bolt type ids
static u8 gunship_flags[0xa];    // gun-ship weapon-select flags
static void *gunship_weapons[4]; // gun-ship gizmo weapons

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
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b1")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b2")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b3")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "b4")) != NULL)
        b->field_0x9f |= 0x20;
}

void BountyHunterPursuitB_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitC_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitD_Init(WORLDINFO_s *) {
}

void BountyHunterPursuitA_Reset(WORLDINFO_s *world) {
    pursuit_state[0] = 0;
    pursuit_state[1] = 0;
    pursuit_state[0] = (i32)(usize)GetNamedGameObject(world->ai_sys, "pursuit_a");
    GIZMOBLOWUP_s *b;
    if ((b = GizmoBlowUp_FindByName(world, "za1")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "za2")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "za3")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "za4")) != NULL)
        b->field_0x9f |= 0x20;
    if ((b = GizmoBlowUp_FindByName(world, "za5")) != NULL)
        b->field_0x9f |= 0x20;
}

void BountyHunterPursuitB_Reset(WORLDINFO_s *world) {
    LevGizmo[0] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_1");
    LevGizmo[1] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_2");
    LevGizmo[2] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_3");
    LevGizmo[3] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_4");
    LevGizmo[4] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_5");
    LevGizmo[5] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitb_6");
    pursuit_state[0] = (i32)(usize)GetNamedGameObject(world->ai_sys, "pursuitb_exit");
}

void BountyHunterPursuitC_Reset(WORLDINFO_s *) {
}

void BountyHunterPursuitD_Reset(WORLDINFO_s *world) {
    pursuit_state[0] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_1");
    pursuit_state[1] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_2");
    pursuit_state[2] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_3");
    pursuit_state[3] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_4");
    pursuit_state[4] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_5");
    pursuit_state[5] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_6");
    pursuit_state[6] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_7");
    pursuit_state[7] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_8");
    pursuit_state[8] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_9");
    pursuit_state[9] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_10");
    pursuit_state[10] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_11");
    pursuit_state[11] = (i32)(usize)GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "pursuitd_12");
    pursuit_state[12] = (i32)(usize)GetNamedGameObject(world->ai_sys, "pursuitd_last");
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
                if (GameCam->field_0x01 == 5)
                    return 1;
                if (GameCam->field_0x01 == 6)
                    return 1;
            } else {
                return 1;
            }
        } else if (WORLD->current_level == KAMINOE_LDATA) {
            if (CUTSTOPGAME == 0 && GameCam->field_0x01 != 0x1e)
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
        r = (GameCam->field_0x01 == 0x15);
    return r;
}

void KaminoA_AlwaysUpdate(WORLDINFO_s *) {
    bool v = 0;
    if (CUTSTOPGAME == 0) {
        u8 b = GameCam->field_0x01;
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

void KaminoD_Init(WORLDINFO_s *world) {
    for (i32 i = 1; i < 13; i++) {
        char buf[0x10];
        sprintf(buf, "DOT%i", i);
        GIZOBSTACLE_s *g = GizObstacle_FindByName(world->giz_obstacle_sys, buf);
        if (g->field_0x3c != 0.0f) {
            break;
        }

        g->field_0x3c = 13.5f;
    }

    GIZMOBLOWUP_s *target = GizmoBlowUp_FindByName(world, "target_a11");
    if (target != NULL) {
        target->field_0x124 = 1;
    }
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
        v = (GameCam->field_0x01 == 0x1e);
    object_switches[1] = v;
}

void KaminoE_Draw(WORLDINFO_s *world) {
    if (netclient == 0) {
        if (kamino_e_state != NULL && kamino_e_state->field_0x28 > 0.0f) {
            GameObject_s *obj = (GameObject_s *)FindGameObject((i32)(i16)id_JANGOFETT, 1, 1, 1, 0);
            if (obj != NULL && kamino_e_state != NULL && obj->apiobj.field_0x28 == 1.0f)
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
        u8 b = GameCam->field_0x01;
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

void FactoryB_Init(WORLDINFO_s *world) {
    factoryb_netpacket = SetLevelHack(0x4);
    InitPaintPuzzle(world);
    GIZOBSTACLE_s *conv[9] = {0};
    conv[1] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv1");
    conv[2] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv2");
    conv[3] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv3");
    conv[4] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv4");
    conv[5] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv5");
    conv[6] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv6");
    conv[7] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv7");
    conv[8] = GizObstacle_FindByName(world->giz_obstacle_sys, "conv8");
    for (i32 i = 1; i <= 8; i++) {
        if (conv[i] != NULL)
            conv[i]->field_0xdc = 0;
    }
}

void FactoryB_Reset(WORLDINFO_s *world) {
    ResetPaintPuzzle(world);
    factoryb_cut = (CUTINFO *)NewCutScene(NULL, world->cutscene_sys, "fb_cut", 0);
    if (factoryb_cut != NULL) {
        CUTSCENEDATA_s *scene = (CUTSCENEDATA_s *)factoryb_cut->scene;
        if (scene != NULL) {
            scene->field_0x88 |= 2;
            scene->field_0x88 |= 8;
        }
    }
    factoryb_conveyor_stopped_msg = CheckGizAIMessage(gizaimessagesys, "conv_stopped", NULL);
}

void FactoryB_Update(WORLDINFO_s *) {
}

void FactoryB_Draw(WORLDINFO_s *) {
    DrawPaintLights();
}

void FactoryG_Init(WORLDINFO_s *world) {
    if (netclient != 0)
        return;
    GIZMO *g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force_g1");
    if (g != NULL)
        force_array[0] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force_g2");
    if (g != NULL)
        force_array[1] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force_g3");
    if (g != NULL)
        force_array[2] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force_g4");
    if (g != NULL)
        force_array[3] = (GIZFORCE_s *)g->object;
}

void FactoryG_Update(WORLDINFO_s *world) {
    if (netclient != 0)
        return;
    i32 complete = 0;
    if (GizForce_Complete(force_array[0]))
        complete++;
    if (GizForce_Complete(force_array[1]))
        complete++;
    if (GizForce_Complete(force_array[2]))
        complete++;
    if (GizForce_Complete(force_array[3]))
        complete++;
    if (ObiWan == NULL) {
        ObiWan = (GameObject_s *)FindGameObject((i32)(i16)id_OBIWANKENOBIJEDIMASTER, 0x400, 0, 1, 0);
        return;
    }
    if (complete == 4) {
        if (FreePlay == 0)
            NewCutScene(NULL, world->cutscene_sys, "factory_escape", 1);
    } else {
        ObiWan->apiobj.field_0x5c = 79.2f;
        ObiWan->apiobj.field_0x60 = 0.75f;
        ObiWan->apiobj.field_0x64 = -10.5f;
    }
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

void GunshipA_Init(WORLDINFO_s *world) {
    gunship_bolts[1] = (i16)BoltType_FindIDByName("gunbolt2", world);
    gunship_bolts[0] = (i16)BoltType_FindIDByName("gunbolt1", world);
    gunship_flags[0] = 0;
    gunship_flags[1] = 0;
    gunship_flags[2] = 0;
    gunship_flags[3] = 0;
    gunship_flags[4] = 0;
    gunship_flags[5] = 1;
    gunship_flags[6] = 1;
    gunship_flags[7] = 1;
    gunship_flags[8] = 1;
    gunship_flags[9] = 1;
    InitMiniSnowTroopers(world, 0xa, 0x20, 0);
    gunship_weapons[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gunw1");
    gunship_weapons[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gunw2");
    gunship_weapons[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gunw3");
    gunship_weapons[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gunw4");
}

void GunshipA_Update(WORLDINFO_s *world) {
    UpdateMiniSnowTroopers(world);
}

void GunshipA_Draw(WORLDINFO_s *world) {
    if (TimingBarSet == 5) {
        TBOPENFN("gun_timing", 5);
        DrawMiniSnowTroopers(world);
    } else {
        DrawMiniSnowTroopers(world);
        if (TimingBarSet == 5)
            TBCLOSEFN("gun_timing", 5);
    }
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
    if (LevFlag.progress == GUNSHIP_ACTIVE)
        LevFlag.progress = GUNSHIP_WON;
    LevFlag.exit = 0;
    bonus_gunship_store_progress_flag = 0;
}

void BonusGunshipA_Update(WORLDINFO_s *world) {
    if (LevFlag.progress == GUNSHIP_INACTIVE) {
        bool found = false;
        if (Player[0] != NULL && Player[0]->field_0x661 == 0 && Player[0]->field_0x68c > 0.001f) {
            found = true;
        } else if (Player[1] != NULL && Player[1]->field_0x661 == 0 && Player[1]->field_0x68c > 0.001f) {
            found = true;
        }
        if (found) {
            if (netclient != 0) {
                LevFlag.progress = GUNSHIP_ACTIVE;
            } else {
                Doors_SetLastDoor((DOOR_s *)Door_FindByName(world, "bonus_door"));
                bonus_gunship_store_progress_flag = 1;
                StoreLevelProgress(world);
                bonus_gunship_store_progress_flag = 0;
                LevFlag.progress = GUNSHIP_ACTIVE;
            }
        }
    }
    if (gunship_player_dead == 0) {
        if ((Player[0] != NULL && Player[0]->apiobj.field_0x287 != 0) ||
            (Player[1] != NULL && Player[1]->apiobj.field_0x287 != 0)) {
            gunship_player_dead = 1;
            ResetLevel(world, "bonus", 1);
        }
    }
}

void BonusGunshipB_Init(WORLDINFO_s *world) {
    bonusgunshipb_netpacket = (struct BONUSGUNSHIP_NETPACKET_s *)SetLevelHack(0xc);
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obs");
}

void BonusGunshipB_Reset(WORLDINFO_s *) {
    LevFlag.progress = GUNSHIP_INACTIVE;
    LevFlag.exit = 0;
    MiscTime = 0;
    gunship_player_dead = 0;
    bonus_gunship_store_progress_flag = 0;
}

void BonusGunshipB_Update(WORLDINFO_s *world) {
    if (netclient != 0) {
        LevFlag.progress = bonusgunshipb_netpacket->state;
        LevFlag.exit = bonusgunshipb_netpacket->sub;
        MiscTime = bonusgunshipb_netpacket->time;
    } else {
        if (gunship_player_dead == 0 && ((Player[0] != NULL && Player[0]->apiobj.field_0x287 != 0) ||
                                         (Player[1] != NULL && Player[1]->apiobj.field_0x287 != 0))) {
            gunship_player_dead = 1;
            ResetLevel(world, "bonus_gunship", 1);
        }
        bonusgunshipb_netpacket->state = LevFlag.progress;
        bonusgunshipb_netpacket->sub = LevFlag.exit;
        bonusgunshipb_netpacket->time = MiscTime;
    }
    if (LevFlag.progress == 0) {
        if (LevDeaths > 0) {
            float x = (float)LevDeaths * LevDeaths + 1.0f;
            if (GameTimer[0] >= x)
                LevFlag.progress = GUNSHIP_ACTIVE;
        }
    } else if (LevFlag.progress == GUNSHIP_ACTIVE) {
        if (MiscTime > 5.0f)
            MiscTime = 5.0f;
    }
}

void BonusGunshipB_Panel(WORLDINFO_s *) {
    if (LevFlag.progress == GUNSHIP_ACTIVE) {
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

void DookuC_Update(WORLDINFO_s *world) {
    if (netclient == 0) {
        if (FreePlay != 0) {
            KillBossCompleteLevel((i32)(i16)id_COUNTDOOKU, 0, 0.0f);
        } else if (DOOKUOUTRO_LDATA != NULL) {
            KillBossNewLevel((i32)(i16)id_COUNTDOOKU, 0, 0.0f, DOOKUOUTRO_LDATA->idx);
        }
    }
    DrawForceBackEffect((nuhspecial_s *)dooku_state[1]);
}

void DookuC_DrawPanel(WORLDINFO_s *) {
    if (netclient != 0)
        return;
    GameObject_s *obj = (GameObject_s *)FindGameObject((i32)(i16)id_COUNTDOOKU, 1, 1, 1, 0);
    if (obj != NULL && dooku_c != 0 && obj->apiobj.field_0x28 == 1.0f)
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
