#include <stdio.h>
#include <string.h>

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "legoapi/world/level_shared.h"
#include "nu2api/nu3d/nutex.h"

extern "C" void *AIPathFindLocator(AISYS_s *, char *);
struct GameObject_s;
extern i32 netclient;
extern i16 id_KAADU;
extern i16 id_GUNGAN;
extern i16 id_FALUMPASET;
extern i16 id_DARTHMAUL;
GameObject_s *FindGameObject(i32, u32, i32, i32, i32);
void DrawBossHitPoints(GameObject_s *);
extern "C" void NuSpecialSetVisibility(void *, i32);
extern GIZAIMESSAGESYS_s *gizaimessagesys;
GIZAIMESSAGE_s *CheckGizAIMessage(GIZAIMESSAGESYS_s *, const char *, GIZAIMESSAGE_s *);
extern "C" struct nuvec_s *NuSpecialGetPos(void *);
void GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE_s *, i32);
i32 SetLevelHack(i32);
extern "C" void *AIPAthFindPathCnx(AISYS_s *, i32, char *, void *);
extern i32 retakeg_netpacket;
static GIZAIMESSAGE_s *RetakeG_TotalGuards_msg;
static GIZAIMESSAGE_s *RetakeG_GuardsToRescue_msg;
extern i16 trooper_boltid;
extern i8 trooper_side[];
extern i32 hothtroopers;
extern i32 TimingBarSet;
i16 BoltType_FindIDByName(char *, WORLDINFO_s *);
void TBOPENFN(char *, i32);
void TBCLOSEFN(char *, i32);
void UpdatePodRaceLapDisplay(float);
extern f32 FRAMETIME;
extern struct AREADATA_s *PODRACE_ADATA;
extern i32 Lap;
static void *PodRace;
static i32 mushroom_collapse;
static i32 mushroom_nattempts_per_increment;
static i32 mushroom_n_attempts;
static i32 podhurry_i;
void PodKeyReset(void);
i32 InStory(void);
extern struct LEVELDATA_s *PODRACEOUTRO1_LDATA;
extern struct LEVELDATA_s *PODRACESTATUS_LDATA;
extern i32 other_level_override;
static i32 pod_pacemaker;
static void *pod_avalanche_cutscene;
i32 pod_lap_start;
CUTINFO *CutScene_Find(CUTSYS *, char *);
void NewCutScene(CUTINFO *, CUTSYS *, char *, i32);
void CutScene_SnapToEnd(CUTINFO *);
extern u32 client_mines[];
extern void *minesys;
extern "C" void NuMtxSetIdentity(void *);
extern "C" void NuMtxTranslate(void *, void *);
extern "C" void NuSpecialDrawAt(void *, void *);
extern "C" i32 NuSpecialExistsFn(void *);

static struct {
    void *field_0x0; // 0x0  MaulA anim message 1
    void *field_0x4;
    void *field_0x8;
    void *field_0xc;
    void *field_0x10; // 0x10 MaulA anim message 2
    void *field_0x14;
    void *field_0x18;
    void *field_0x1c;
    void *field_0x20; // 0x20 Maul boss object
    void *field_0x24;
    void *field_0x28;
} PODRACELEVELS;

static struct {
    undefined field0_0x0[2];
    u16 count_0x2;
    void *table_0x4[0x20];
    void *table2_0x84[0x20];
    undefined field_0x104[8];
    i16 g1_0x10c;
    i16 g2_0x10e;
    i16 g3_0x110;
    i16 g4_0x112;
} gungan_a;
struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;
static __used__ void PodRaceSnipersReset() {
}
static __used__ void PodRaceSnipersUpdate() {
}

void NegotiationsA_Init(WORLDINFO_s *world) {
    GIZFORCE_s *f = GizForce_FindByName(world->giz_force_sys, "Force3");
    if (f != NULL)
        f->strength_0x6c = 0.4f;
    f = GizForce_FindByName(world->giz_force_sys, "Force2");
    if (f != NULL)
        f->strength_0x6c = 0.4f;
    f = GizForce_FindByName(world->giz_force_sys, "Force72");
    if (f != NULL)
        f->strength_0x6c = 0.75f;
}

void NegotiationsB_Init(WORLDINFO_s *world) {
    GIZFORCE_s *f = GizForce_FindByName(world->giz_force_sys, "Force10");
    if (f != NULL)
        f->strength_0x6c = 0.5f;
}

void GunganA_Init(WORLDINFO_s *world) {
    if (netclient == 0) {
        memset(&gungan_a, 0, sizeof(gungan_a));
        gungan_a.g1_0x10c = id_KAADU;
        gungan_a.g2_0x10e = id_GUNGAN;
        gungan_a.g3_0x110 = id_FALUMPASET;
        gungan_a.g4_0x112 = id_GUNGAN;
        for (i32 i = 0; i < 32; i++) {
            char buf[16];
            sprintf(buf, "origin_%d", i);
            gungan_a.table_0x4[i] = AIPathFindLocator(world->ai_sys, buf);
            sprintf(buf, "target_%d", i);
            gungan_a.table2_0x84[i] = AIPathFindLocator(world->ai_sys, buf);
            if (gungan_a.table_0x4[i] == NULL || gungan_a.table2_0x84[i] == NULL)
                break;
            gungan_a.count_0x2++;
        }
    }
    GIZMOBLOWUP_s *b = GizmoBlowUp_FindByName(world, "Trunk2_exp11");
    if (b != NULL) {
        UpdateMidPos(b);
        b->field_0x124 = 1;
        b->field_0x128 = b->field_0xb0;
        GIZMOBLOWUP_s *b2 = GizmoBlowUp_FindByName(world, "leaves_exp11");
        if (b2 != NULL) {
            b2->field_0x124 = 1;
            b2->field_0x120 = (void *)&b->field_0x50;
            b2->field_0x128 = b->field_0xb0;
        }
        GIZMOBLOWUP_s *b3 = GizmoBlowUp_FindByName(world, "branch3_exp11");
        if (b3 != NULL) {
            b3->field_0x124 = 1;
            b3->field_0x120 = (void *)&b->field_0x50;
            b3->field_0x128 = b->field_0xb0;
        }
    }
}

void GunganA_Update(WORLDINFO_s *) {
}

void RescueA_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, "deton_021");
    if (g != NULL)
        g->field_0xa0 |= 2;
    g = GizmoBlowUp_FindByName(world, "deton_011");
    if (g != NULL)
        g->field_0xa0 |= 2;
    g = GizmoBlowUp_FindByName(world, "pod_071");
    if (g != NULL)
        g->field_0xa0 |= 2;
    g = GizmoBlowUp_FindByName(world, "pod_081");
    if (g != NULL)
        g->field_0xa0 |= 2;
}

void RescueB_Init(WORLDINFO_s *) {
}

void RescueC_Init(WORLDINFO_s *) {
}

void RescueE_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, "roof_light61");
    if (g != NULL)
        g->field_0xa0 |= 2;
}

void PodRaceInit(WORLDINFO_s *) {
}

void PodRaceADraw(WORLDINFO_s *world) {
    if (netclient != 0) {
        for (i32 i = 0; i < 0x40; i++) {
            u32 bit = 1u << (i & 0x1f);
            if (((i < 0x20 ? client_mines[0x300 / 4] : client_mines[0x304 / 4]) & bit) != 0) {
                void *mtx = 0; // local matrix placeholder
                NuMtxSetIdentity(mtx);
                NuMtxTranslate(mtx, &client_mines[i * 3]);
                NuSpecialDrawAt(minesys, mtx);
            }
        }
    } else if (NuSpecialExistsFn(minesys) != 0) {
        u8 *entry = (u8 *)minesys + 0xc;
        u8 *end = (u8 *)minesys + 0x70c;
        for (; entry < end; entry += 0x1c) {
            if (*(u32 *)entry != 0) {
                void *mtx = 0;
                NuMtxSetIdentity(mtx);
                NuMtxTranslate(mtx, entry + 0x4);
                NuSpecialDrawAt(minesys, mtx);
            }
        }
    }
}

void PodRaceAInit(WORLDINFO_s *world) {
    PodRaceInit(world);
}

void PodRaceBInit(WORLDINFO_s *world) {
    PodRaceInit(world);
    mushroom_collapse = 0;
    mushroom_nattempts_per_increment = 1;
    mushroom_n_attempts = 0;
    if (Lap <= 1)
        *(float *)((u8 *)PodRace + 0xaf00) = 3.0f;
}

void PodRaceCInit(WORLDINFO_s *) {
}

void PodRacePanel(WORLDINFO_s *) {
}

void PodRaceReset() {
    *(u32 *)((u8 *)PodRace + 0xaf04) = 0;
    memset((u8 *)PodRace + 0xa580, 0, 0x980);
    *(u8 *)((u8 *)PodRace + 0xaf20) &= 0xfc;
    podhurry_i = -1;
    PodKeyReset();
}

void PodRaceAReset(WORLDINFO_s *) {
}

void PodRaceBReset(WORLDINFO_s *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            pod_lap_start = 1;
            break;
        case 2:
            pod_lap_start = 4;
            break;
        case 3:
            pod_lap_start = 7;
            break;
    }
    pod_pacemaker = 0;
    *(u8 *)LevFlag = 0;
    *((u8 *)LevFlag + 1) = 0;
    pod_avalanche_cutscene = CutScene_Find(world->cutscene_sys, "EP1_PODRACE_MUSHROOM0");
    NuSpecialFind(world->current_gscn, (void **)LevHSpecial, "collapsing_mush");
}

void PodRaceCReset(WORLDINFO_s *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            pod_lap_start = 2;
            break;
        case 2:
            pod_lap_start = 5;
            break;
        case 3:
            pod_lap_start = 8;
            break;
        default:
            break;
    }
    pod_pacemaker = 0;
    PodRaceSnipersReset();
    CUTINFO *cs = CutScene_Find(world->cutscene_sys, "ep1_podrace_avalanche");
    if (cs != NULL) {
        if (Lap == 2)
            NewCutScene(cs, world->cutscene_sys, 0, 1);
        else if (Lap == 3)
            CutScene_SnapToEnd(cs);
    }
}

void PodRaceUpdate(WORLDINFO_s *, float) {
}

void PodRaceAUpdate(WORLDINFO_s *) {
}

void PodRaceBUpdate(WORLDINFO_s *) {
}

void PodRaceCUpdate(WORLDINFO_s *) {
}

void PodRaceAlwasyUpdate(WORLDINFO_s *world) {
    UpdatePodRaceLapDisplay(FRAMETIME);
}

void PodRace_IncreaseLap() {
    Lap++;
}

void PodRaceA_AlwaysUpdate(WORLDINFO_s *world) {
    PodRaceAlwasyUpdate(world);
    if (Lap == 3) {
        if (InStory() != 0) {
            if (PODRACEOUTRO1_LDATA != NULL) {
                other_level_override = *(i16 *)((u8 *)PODRACEOUTRO1_LDATA + 0x62);
                return;
            }
        }
        if (PODRACESTATUS_LDATA != NULL)
            other_level_override = *(i16 *)((u8 *)PODRACESTATUS_LDATA + 0x62);
    }
}

i32 PodRace_InStartCountdown(WORLDINFO_s *world) {
    if (world->area != NULL && world->area == PODRACE_ADATA && PodRace != NULL &&
        *(float *)((u8 *)PodRace + 0xaf00) > 4.158760129802644e+21f)
        return 1;
    return 0;
}

i32 PodLevel(AREADATA_s *area) {
    return PODRACE_ADATA != NULL && PODRACE_ADATA == area;
}

void ResetPodStuff() {
}

void SetPodMergeAnims(ANIMPACKET_s *, i32) {
}

void UpdatePodRaceLapDisplay(float) {
}

void PodSprintA_Init(WORLDINFO_s *) {
}

void PodSprintA_Panel(WORLDINFO_s *) {
}

void PodSprintA_Reset(WORLDINFO_s *) {
}

void PodSprintA_Update(WORLDINFO_s *) {
}

void PodSprint_RollMul(GameObject_s *) {
}

void PodSprint_GetIAlongVals(nugspline_s *, i16 *, i16 *) {
}

void PodSprint_InStartCountdown(WORLDINFO_s *) {
}

void AnakinsFlightB_Draw(WORLDINFO_s *world) {
    if (TimingBarSet == 5) {
        TBOPENFN("mini", 5);
        if (TimingBarSet == 5) {
            TBCLOSEFN("mini", 5);
        }
    }
}

void AnakinsFlightB_Init(WORLDINFO_s *world) {
    trooper_boltid = BoltType_FindIDByName("trooper_red", world);
    trooper_side[0] = 0;
    trooper_side[1] = 0;
    trooper_side[2] = 0;
    NuSpecialFind(world->current_gscn, (void **)LevHSpecial, "minifig_1_1");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0xc), "minifig_1_2");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x18), "minifig_1_3");
}

void AnakinsFlightB_Update(WORLDINFO_s *) {
}

void RetakeD_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g;
    g = GizmoBlowUp_FindByName(world, "lattice_a11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
    g = GizmoBlowUp_FindByName(world, "lattice_b11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
    g = GizmoBlowUp_FindByName(world, "lattice_c11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
    g = GizmoBlowUp_FindByName(world, "lattice_d11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
    g = GizmoBlowUp_FindByName(world, "lattice_e11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
    g = GizmoBlowUp_FindByName(world, "lattice_f11");
    if (g != NULL) {
        g->field_0x128 = 0.5f;
        g->field_0x124 = 1;
    }
}

void RetakeE_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, "box_deton_011");
    if (g != NULL)
        g->field_0xa0 |= 2;

    GIZOBSTACLE_s *obs;
    u8 *n;
    struct nuvec_s *pos;

    obs = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle3");
    if (obs != NULL) {
        n = *(u8 **)((u8 *)obs + 0x34);
        n = *(u8 **)(n + 0x18);
        while (n != NULL) {
            pos = NuSpecialGetPos(n + 0x4);
            pos->z -= 0.75f;
            GizObstacle_EvalAveragePosAndRadius(obs, 2);
            *(f32 *)((u8 *)obs + 0x18) = pos->z;
            *(f32 *)((u8 *)obs + 0x24) = pos->z;
            *(u32 *)((u8 *)obs + 0x3c) = 0x41700000;
            n = *(u8 **)n;
        }
    }

    obs = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle12");
    if (obs != NULL) {
        n = *(u8 **)((u8 *)obs + 0x34);
        n = *(u8 **)(n + 0x18);
        while (n != NULL) {
            pos = NuSpecialGetPos(n + 0x4);
            pos->z += 0.75f;
            GizObstacle_EvalAveragePosAndRadius(obs, 2);
            *(f32 *)((u8 *)obs + 0x18) = pos->z;
            *(u32 *)((u8 *)obs + 0x1c) = *(u32 *)pos;
            *(u32 *)((u8 *)obs + 0x20) = *(u32 *)((u8 *)pos + 0x4);
            *(f32 *)((u8 *)obs + 0x24) = pos->z;
            *(u32 *)((u8 *)obs + 0x3c) = 0x41700000;
            n = *(u8 **)n;
        }
    }

    obs = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle11");
    if (obs != NULL) {
        n = *(u8 **)((u8 *)obs + 0x34);
        n = *(u8 **)(n + 0x18);
        while (n != NULL) {
            *(u32 *)((u8 *)obs + 0x1c) = *(u32 *)pos;
            *(u32 *)((u8 *)obs + 0x20) = *(u32 *)((u8 *)pos + 0x4);
            *(f32 *)((u8 *)obs + 0x24) = pos->z;
            *(u32 *)((u8 *)obs + 0x3c) = 0x41700000;
            n = *(u8 **)n;
        }
    }

    obs = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle13");
    if (obs != NULL) {
        n = *(u8 **)((u8 *)obs + 0x34);
        n = *(u8 **)(n + 0x18);
        while (n != NULL) {
            *(u32 *)((u8 *)obs + 0x1c) = *(u32 *)pos;
            *(u32 *)((u8 *)obs + 0x20) = *(u32 *)((u8 *)pos + 0x4);
            *(f32 *)((u8 *)obs + 0x24) = pos->z;
            *(u32 *)((u8 *)obs + 0x3c) = 0x41700000;
            n = *(u8 **)n;
        }
    }
}

void RetakeG_Init(WORLDINFO_s *world) {
    char buf[0x10];
    retakeg_netpacket = SetLevelHack(4);
    RetakeG_TotalGuards_msg = CheckGizAIMessage(gizaimessagesys, "TotalGuards", NULL);
    RetakeG_GuardsToRescue_msg = CheckGizAIMessage(gizaimessagesys, "GuardsToRescue", NULL);
    LevGizForce[0] = (i32)(usize)GizForce_FindByName(world->giz_force_sys, "force6");
    LevPathCnx[0] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack1_b", buf);
    LevPathCnx[1] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack1_a", buf);
    LevPathCnx[2] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack1_c", buf);
    LevPathCnx[3] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack1_d", buf);
    LevGizForce[1] = (i32)(usize)GizForce_FindByName(world->giz_force_sys, "force3");
    LevPathCnx[4] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack2_b", buf);
    LevPathCnx[5] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack2_a", buf);
    LevPathCnx[6] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack2_c", buf);
    LevPathCnx[7] = (i32)(usize)AIPAthFindPathCnx(world->ai_sys, 0, "stack2_d", buf);
    GIZFORCE_s *f = GizForce_FindByName(world->giz_force_sys, "Force18");
    if (f != NULL)
        f->strength_0x6c = 0.85f;
    f = GizForce_FindByName(world->giz_force_sys, "Force19");
    if (f != NULL)
        f->strength_0x6c = 0.85f;
    f = GizForce_FindByName(world->giz_force_sys, "Force20");
    if (f != NULL)
        f->strength_0x6c = 0.85f;
}

void RetakeG_Reset(WORLDINFO_s *) {
}

void RetakeG_Update(WORLDINFO_s *) {
}

void RetakeG_Panel(WORLDINFO_s *) {
}

void MaulA_Init(WORLDINFO_s *world) {
    PODRACELEVELS.field_0x0 = CheckGizAIMessage(gizaimessagesys, "MaulOnTheRun", NULL);
    PODRACELEVELS.field_0x10 = CheckGizAIMessage(gizaimessagesys, "Hits", NULL);
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x30), "engine_1c");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x3c), "engine_2c");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x48), "engine_1d");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x54), "engine_2d");
}

void MaulA_Reset(WORLDINFO_s *world) {
    NuSpecialSetVisibility((char *)LevHSpecial + 0x30, 0);
    NuSpecialSetVisibility((char *)LevHSpecial + 0x3c, 0);
    NuSpecialSetVisibility((char *)LevHSpecial + 0x48, 0);
    NuSpecialSetVisibility((char *)LevHSpecial + 0x54, 0);
    PODRACELEVELS.field_0x20 = FindGameObject(id_DARTHMAUL, 1, 1, 0, 0);
}

void MaulA_Update(WORLDINFO_s *) {
}

void MaulA_Panel(WORLDINFO_s *world) {
    if (netclient == 0) {
        if (PODRACELEVELS.field_0x20 != NULL && PODRACELEVELS.field_0x0 != NULL &&
            *(float *)((u8 *)PODRACELEVELS.field_0x0 + 0x28) == 0.0f && PODRACELEVELS.field_0x10 != NULL) {
            *(u8 *)((u8 *)PODRACELEVELS.field_0x20 + 0x108a) = 3;
            *(u8 *)((u8 *)PODRACELEVELS.field_0x20 + 0x108b) =
                (u8)(i32) * (float *)((u8 *)PODRACELEVELS.field_0x10 + 0x28);
            DrawBossHitPoints((GameObject_s *)PODRACELEVELS.field_0x20);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

void MaulB_Init(WORLDINFO_s *world) {
    GIZOBSTACLE_s *o = GizObstacle_FindByName(world->giz_obstacle_sys, "Obstacle19");
    if (o != NULL)
        o->field_a1_0xa1 |= 1;
}

void MaulD_Init(WORLDINFO_s *) {
}

void MaulD_Update(WORLDINFO_s *) {
}

void MaulE_Init(WORLDINFO_s *) {
}

void MaulE_Update(WORLDINFO_s *) {
}

void MaulF_Init(WORLDINFO_s *world) {
    PODRACELEVELS.field_0x0 = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x0), "throw_object1");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0xc), "throw_object2");
    NuSpecialFind(world->current_gscn, (void **)((char *)LevHSpecial + 0x18), "throw_object3");
}

void MaulF_Reset(WORLDINFO_s *world) {
    PODRACELEVELS.field_0x20 = FindGameObject(id_DARTHMAUL, 1, 1, 0, 0);
}

void MaulF_Update(WORLDINFO_s *) {
}

void MaulF_Panel(WORLDINFO_s *world) {
    if (netclient == 0) {
        if (PODRACELEVELS.field_0x20 != NULL && PODRACELEVELS.field_0x0 != NULL &&
            *(float *)((u8 *)PODRACELEVELS.field_0x0 + 0x28) == 1.0f) {
            DrawBossHitPoints((GameObject_s *)PODRACELEVELS.field_0x20);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}
