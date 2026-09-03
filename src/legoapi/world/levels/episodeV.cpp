#include "decomp.h"
#include "globals.h"
#include "legoapi/ai/core/ai_sys_stubs.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/traps/gizbombgen.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

extern TERRAIN_SURFACE_s TerSurface[32];
AILOCATOR_s *locator;
GameObject_s *gameobj;
extern u8 troopercannons_beenReset;
extern "C" i32 FindPlatInst(i32 instance_ix);

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" {
    GIZBOMBGEN *HothBattleC_BombGenerator = NULL;
}

void DagobahA_Init(WORLDINFO_s *) {
}

void DagobahB_Init(WORLDINFO_s *) {
}

void DagobahC_Init(WORLDINFO_s *) {
}

void DagobahE_Init(WORLDINFO_s *) {
}

void DagobahB_Reset(WORLDINFO_s *world) {
    LevSafePlatID[1] = -1;
    LevSafePlatID[0] = -1;

    if (NuSpecialFind(world->current_gscn, &LevHSpecial[0], "pad_2_base_2", 1) != 0) {
        if (world->terrain != NULL) {
            LevSafePlatID[0] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[0]));
        }
    }

    if (NuSpecialFind(world->current_gscn, &LevHSpecial[1], "pad_4_base_2", 1) != 0) {
        if (world->terrain != NULL) {
            LevSafePlatID[1] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[1]));
        }
    }
}

void DagobahC_Panel(WORLDINFO_s *) {
}

void KillParts_ATAT(ADDPART_s *, i32, i32, GameObject_s *) {
}

void BobaRocket_Kill(PART_s *, i32) {
}

void BobaRocket_Move(PART_s *, float) {
}

void DagobahA_Update(WORLDINFO_s *) {
}

void HothBattleA_Draw(WORLDINFO_s *) {
}

void HothBattleA_Init(WORLDINFO_s *) {
}

void HothBattleB_Init(WORLDINFO_s *) {
}

void HothBattleC_Draw(WORLDINFO_s *) {
}

void HothBattleC_Init(WORLDINFO_s *) {
}

void HothBattleE_Draw(WORLDINFO_s *) {
}

void HothBattleE_Init(WORLDINFO_s *) {
}

void HothEscapeA_Init(WORLDINFO_s *) {
}

void HothEscapeB_Init(WORLDINFO_s *) {
}

void HothEscapeC_Init(WORLDINFO_s *) {
}

void HothEscapeD_Init(WORLDINFO_s *) {
}

void HothBattleA_Reset(WORLDINFO_s *world) {
    GIZMO *gizmo = LevGizmo[0];
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
    if (minikitCounter_A == 10 && (pickup->state_flags & 8) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void HothBattleC_Reset(WORLDINFO_s *world) {
    GIZMO *bomb_generator = GizmoFindByName(world->gizmo_sys, bombgen_gizmotype_id, "bomb_generator1");
    if (bomb_generator != NULL && bomb_generator->object != NULL) {
        HothBattleC_BombGenerator = static_cast<GIZBOMBGEN *>(bomb_generator->object);
    }

    GIZMO *gizmo = LevGizmo[1];
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    GIZMOPICKUP_s *pickup = static_cast<GIZMOPICKUP_s *>(gizmo->object);
    if (minikitCounter_C == 10 && (pickup->state_flags & 8) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void HothBattleE_Panel(WORLDINFO_s *) {
}

void HothEscapeA_Reset(WORLDINFO_s *) {
}

void HothEscapeB_Reset(WORLDINFO_s *world) {
    locator = AIPathFindLocator(world->ai_sys, "snow_mob");
    gameobj = GetNamedGameObject(world->ai_sys, "snowmob_1");
    TerSurface[9].movement_scale = TerSurface[17].movement_scale;
    TerSurface[9].flags = TerSurface[17].flags & ~2u;
}

void HothEscapeC_Reset(WORLDINFO_s *) {
}

void HothEscapeD_Reset(WORLDINFO_s *) {
}

void BobaRocket_Deflect(PART_s *) {
}

void HothBattleA_Update(WORLDINFO_s *) {
}

void HothBattleC_Update(WORLDINFO_s *) {
}

void HothBattleE_Update(WORLDINFO_s *) {
}

void HothEscapeA_Update(WORLDINFO_s *) {
}

void HothEscapeB_Update(WORLDINFO_s *) {
}

void HothEscapeC_Update(WORLDINFO_s *) {
}

void HothEscapeD_Update(WORLDINFO_s *) {
}

void InitTrooperCannons(WORLDINFO_s *) {
}

void CloudCityTrapA_Init(WORLDINFO_s *) {
}

void CloudCityTrapB_Init(WORLDINFO_s *) {
}

void CloudCityTrapA_Reset(WORLDINFO_s *) {
    if (netclient == 0)
        troopercannons_beenReset = 0;
}

void CloudCityTrapC_Panel(WORLDINFO_s *) {
}

void CloudCityTrapC_Reset(WORLDINFO_s *) {
}

void InitMiniSnowTroopers(WORLDINFO_s *, i32, i32, i32) {
}

void CloudCityEscapeA_Init(WORLDINFO_s *) {
}

void CloudCityEscapeC_Init(WORLDINFO_s *) {
}

void CloudCityTrapA_Update(WORLDINFO_s *) {
}

void CloudCityTrapB_Update(WORLDINFO_s *) {
}

void CloudCityTrapC_Update(WORLDINFO_s *) {
}

void HothBattle_Melee_init(HOTHBATTLE_MELEE_s *) {
}

void CloudCityEscapeA_Panel(WORLDINFO_s *) {
}

void CloudCityEscapeA_Reset(WORLDINFO_s *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "BobaFightStarted", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "Built_C3PO", NULL);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizbuildit_gizmotype_id, "buildit2");
}

void HothBattleE_UpdateWave() {
}

void CloudCityEscapeA_Update(WORLDINFO_s *) {
}

void CloudCityEscapeC_Update(WORLDINFO_s *) {
}

void HothBattle_StartNewWave() {
}

void HothEscapeC_AlwaysUpdate(WORLDINFO_s *) {
}

void isHothBattleWaveCreature(GameObject_s *) {
}

void HothBattle_ManageBackgroundCreatures() {
}

// ===========================================================================
// Asteroid chase (AsteroidChase_A / B / C / D)
// ===========================================================================

struct ASTEROID_s {
    nuhspecial_s special;
    GIZMOBLOWUP_s *blowup;
    i16 rotation_speed_x;
    i16 rotation_speed_y;
    i16 rotation_speed_z;
    u8 activated;
    u8 reserved_17;
};
DECOMP_ASSERT(sizeof(ASTEROID_s) == 0x18, "ASTEROID_s size");

i32 nasteroids;
ASTEROID_s asteroids[128];

static void Asteroids_Reset(WORLDINFO_s *world) {
    static const i32 maxrotspd[3] = {0x1555, 0x38e, 0x16c};
    nuhspecial_s specials[128];

    memset(asteroids, 0, sizeof(asteroids));
    nasteroids = 0;

    i32 special_count = NuSpecialFindMulti(world->current_gscn, specials, "asteroid", 128, 0);
    if (special_count == 0)
        return;

    if (special_count > 0) {
        for (i32 special_index = 0; special_index < special_count; ++special_index) {
            for (i32 type_index = 0; type_index < world->gizmo_blowup_type_count; ++type_index) {
                NuSpecialCompare(&world->gizmo_blowup_types[type_index].special, &specials[special_index]);
            }

            if (NuSpecialGetVisibilityFn(&specials[special_index]) != 0) {
                ASTEROID_s *asteroid = &asteroids[nasteroids];
                asteroid->special = specials[special_index];

                char *name = NuSpecialGetName(&asteroid->special);
                i32 asteroid_type;
                if (name == NULL || NuStrIStr(name, "asteroid_a") != NULL || NuStrIStr(name, "asteroid_pop") != NULL) {
                    asteroid_type = 0;
                } else if (NuStrIStr(name, "asteroid_b") != NULL) {
                    asteroid_type = 1;
                } else if (NuStrIStr(name, "asteroid_c") != NULL) {
                    asteroid_type = 2;
                } else {
                    asteroid_type = 0;
                }

                i32 max_speed = maxrotspd[asteroid_type];
                asteroid->rotation_speed_x = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
                asteroid->rotation_speed_y = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
                asteroid->rotation_speed_z = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
                ++nasteroids;
            }
        }
    }

    for (i32 blowup_index = 0; blowup_index < world->gizmo_blowup_count; ++blowup_index) {
        ASTEROID_s *asteroid = &asteroids[nasteroids];
        GIZMOBLOWUP_s *blowup = &world->gizmo_blowups[blowup_index];
        char *name = blowup->name;
        i32 asteroid_type;
        if (name == NULL || NuStrIStr(name, "asteroid_a") != NULL || NuStrIStr(name, "asteroid_pop") != NULL) {
            asteroid_type = 0;
        } else if (NuStrIStr(name, "asteroid_mid") != NULL) {
            asteroid_type = 1;
        } else {
            continue;
        }

        asteroid->blowup = blowup;
        i32 max_speed = maxrotspd[asteroid_type];
        asteroid->rotation_speed_x = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
        asteroid->rotation_speed_y = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
        asteroid->rotation_speed_z = static_cast<i16>(qrand() / ~(0xffff / (max_speed * 2)) + max_speed);
        ++nasteroids;
    }
}

void AsteroidChaseA_Init(WORLDINFO_s *) {
}

void AsteroidChaseB_Init(WORLDINFO_s *) {
}

void AsteroidChaseB_Draw(WORLDINFO_s *) {
}

void AsteroidChaseC_Init(WORLDINFO_s *) {
}

void AsteroidChaseD_Init(WORLDINFO_s *) {
}

void AsteroidChaseA_Reset(WORLDINFO_s *world) {
    Asteroids_Reset(world);
}

void AsteroidChaseB_Reset(WORLDINFO_s *world) {
    Asteroids_Reset(world);
}

void AsteroidChaseC_Reset(WORLDINFO_s *world) {
    Asteroids_Reset(world);
}

void AsteroidChaseD_Panel(WORLDINFO_s *) {
}

void AsteroidChaseA_Update(WORLDINFO_s *) {
}

void AsteroidChaseB_Update(WORLDINFO_s *) {
}

void AsteroidChaseC_Update(WORLDINFO_s *) {
}

void AsteroidChaseD_Update(WORLDINFO_s *) {
}
