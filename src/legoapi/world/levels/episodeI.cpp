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
extern void *hothtroopers;
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
extern i32 g_lowEndLevelBehaviour;
static i32 max_nsnipers;
static i32 PodRace_nsnipers;
static float PodRace_sniper_fire_time;
static u8 PodRace_snipers[0xa0];
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
extern i32 nethost;
extern i32 clients_mines_bitfield[];
extern i32 pod_mines_bitfield[];
extern i32 mine_count;
struct PLAYERDATA;
extern PLAYERDATA *apicharsys;
extern i16 id_ANAKINSPOD;
static float pod_roll[2];
static float pod_roll_target[2];
static float pod_animtime[2];
static float pod_countdown;
static float pod_092d00;
static float pod_092d10;
extern float FRAMETIME;
extern float gungan_a_time_Normal;
extern float gungan_a_time_LowEnd;
extern i32 active_neutral_count;
extern i32 active_baddy_count;
static float gungan_timer;
static i16 gungan_count;
static i16 gungan_0x92cb8;
static i16 gungan_0x92cbc[8];
static void *gungan_0x92bb4[8];
static void *gungan_0x92c34[8];
struct NURAND;
extern "C" i32 NuRand(NURAND *);
extern "C" float NuFloatRand(NURAND *);
struct nuvec_s;
struct AIPATHINFO_s;
struct AIGROUP_s;
struct nugspline_s;
void *AddDynamicCreature(i32, nuvec_s *, i32, char *, AIPATHINFO_s *, AIGROUP_s *, i32, nugspline_s *, nuvec_s *, i32,
                         i32);
extern i16 id_STAP;
extern void *FadeSys;
extern i32 Paused;
extern i32 CUTSTOPGAME;
extern i32 MiniCutCam;
extern struct LEVELDATA_s *PODRACEB_LDATA;
extern float GameTimer[];
void TickTockSfx(void);
float SeekLinearF(float, float, float);
i32 qrand(void);
static i32 pod_092d40;
static i32 pod_092d48;
static i32 pod_092d44;
static float pod_0xd440;
static float pod_0xd430;
static float pod_0xd460;
static float pod_092d70[5];
extern "C" void NuSpecialSetVisibility(void *, i32);
void Hint_SetComplete(i32);
extern "C" i32 NuSpecialExistsFn(void *);
extern "C" void *NuSpecialGetDrawMtx(void *);
extern "C" i32 NuSpecialClipTestExtents(void *, void *);
extern i32 retakeg_netpacket;
extern void *podrace_netpacket;
extern "C" void Text3DEx(char *, i32, float, float, float, float, float, i32, i32, i32, i32, i32);
extern "C" float NuFmod(float, float);
extern float PodRace_sniper_start_fire_radius;
extern float PodRace_sniper_fire_radius;
extern float PodRace_sniper_fire_range_time;
extern i16 temp_yrot;
extern i16 temp_xrot;
static i32 pod_sniper_toggle;
extern struct LEVELDATA_s *PODSPRINTA_LDATA;
extern void *player2;
extern void *player;
extern void *game_cutscenes;
extern i32 VehicleAreaRememberSpeed;
void CutScene_StoppedFn_LSW(CUTINFO *);
void CutScene_SnapToEnd(CUTINFO *);
extern i32 podsprint_netpacket;
void ResetPodStuff(void);
extern void *GameCam;
extern i32 pause_rndr_on;
extern float podanimendframe;
struct nuhspecial_s;
void DrawPanel3DObject(float, float, float, float, float, float, u16, u16, u16, nuhspecial_s *, i32, float);
extern "C" void *AISysFindArea(void *, char *);
extern "C" i16 FindGameDebris(void *, char *);
extern "C" i32 PARTLookupType(char *);
extern "C" float AnimEndFrame(void *, i32);
struct flightspline_s;
void FlightSpline_Init(WORLDINFO_s *, flightspline_s *, i32);
extern "C" float NuSpecialGetOriginRadius(void *);
static i32 pod_092d30;
static void UpdatePodRaceMines(void);
static void *CreatePodRaceMine(nuvec_s *);
static void UpdatePacemakerDisplay(void *);
i32 SetLevelHack(i32);
static void PodSprint_InitAISpline(WORLDINFO_s *, void *, char *);
void *BoltType_FindByID(i32, WORLDINFO_s *);
void Bolt_Add(GameObject_s *, nuvec_s *, numtx_s *, i32, i32);
extern "C" i32 NuAtan2D(float, float);
extern "C" void NuMtxSetRotationX(void *, i32);
extern "C" void NuMtxRotateY(void *, i32);
extern "C" i32 NuRandInt(void);
extern i16 id_ROYALGUARD;
static void *retakeg_guard_a;
static void *retakeg_guard_b;
void DrawMeleeTargets(i16 *, char *, float *, i32);
extern void *podsprint;
extern i16 id_CLONEARC;
extern i16 id_IMPERIALSHUTTLE;
extern i16 id_NABOOSTARFIGHTER;
extern i16 id_XWING;
extern i16 id_SNOWSPEEDER;
extern i16 id_MILLENNIUMFALCON;
extern i16 id_NEW_REPUBLIC_GUNSHIP;
extern "C" NUGSPLINE *NuSplineFind(NUGSCN *, char *);
extern "C" f32 NuRandFloat(void);
struct PLAYERDATA;
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
    max_nsnipers = (g_lowEndLevelBehaviour >= 1) ? 2 : 5;
    PodRace_nsnipers = 0;
    if (Lap != 0) {
        char buf[0x20];
        sprintf(buf, "Sniper%d", Lap);
        void *spline = NuSplineFind(WORLD->current_gscn, buf);
        if (spline != NULL && *(i16 *)spline > 0 && PodRace_nsnipers <= 9) {
            i32 idx = 0;
            i32 off = 0;
            do {
                u8 *dst = (u8 *)PodRace_snipers + idx * 0x20;
                memcpy(dst, *(u8 **)((u8 *)spline + 0x8) + off, 0x18);
                *(float *)(dst + 0x18) = NuRandFloat() * PodRace_sniper_fire_time + PodRace_sniper_fire_time * 0.5f;
                *(u32 *)(dst + 0x1c) = 0;
                idx++;
                PodRace_nsnipers = idx;
                off += 0x18;
            } while (*(i16 *)spline > off / 0xc && idx <= 9);
        }
    }
}
static __used__ void PodRaceSnipersUpdate() {
    i32 bolttype = (WORLD->current_level == PODSPRINTA_LDATA) ? 0x29 : 0x28;
    void *bt = BoltType_FindByID(bolttype, WORLD);
    i32 n = PodRace_nsnipers;
    if (n <= 0 || max_nsnipers <= 0)
        return;
    for (i32 i = 0; i < n && i < max_nsnipers; i++) {
        u8 *s = (u8 *)PodRace_snipers + i * 0x20;
        void *target = NULL;
        u8 *p0 = (u8 *)Player[0];
        if (p0 != NULL && (*(u16 *)(p0 + 0x1f8) & 0x1001) == 0x1001) {
            target = p0;
        } else {
            u8 *p1 = (u8 *)Player[1];
            if (p1 != NULL && (*(u16 *)(p1 + 0x1f8) & 0x1001) == 0x1001)
                target = p1;
        }
        if (target != NULL) {
            float dx = *(float *)((u8 *)target + 0x80) - *(float *)(s + 0xc);
            float dy = *(float *)((u8 *)target + 0x88) - *(float *)(s + 0x14);
            float dist2 = dx * dx + dy * dy;
            if (dist2 > PodRace_sniper_start_fire_radius * PodRace_sniper_start_fire_radius) {
                if (dist2 <= PodRace_sniper_fire_radius * PodRace_sniper_fire_radius) {
                    *(float *)(s + 0x1c) = PodRace_sniper_fire_range_time;
                } else {
                    *(float *)(s + 0x18) += FRAMETIME;
                    if (*(float *)(s + 0x18) >= PodRace_sniper_fire_time) {
                        *(float *)(s + 0x18) = 0.0f;
                        if (*(float *)(s + 0x1c) > 0.0f) {
                            // fire
                            temp_yrot = NuAtan2D(dx, -dy);
                            temp_xrot = NuAtan2D(-*(float *)(s + 0x64), dx);
                            void *mtx[4];
                            NuMtxSetRotationX(mtx, (i32)(u16)temp_xrot);
                            NuMtxRotateY(mtx, (i32)(u16)temp_yrot);
                            Bolt_Add(NULL, (nuvec_s *)(s + 0xc), (numtx_s *)mtx, bolttype, 0);
                        }
                    }
                }
            }
        } else {
            *(float *)(s + 0x18) += FRAMETIME;
        }
    }
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

void GunganA_Update(WORLDINFO_s *world) {
    if (netclient != 0)
        return;
    gungan_timer += FRAMETIME;
    float maxtime = gungan_a_time_Normal;
    if (g_lowEndLevelBehaviour != 0)
        maxtime = gungan_a_time_LowEnd;
    if (gungan_count == 0 || gungan_timer <= maxtime)
        return;
    NuRand(NULL);
    i32 r = NuRand(NULL) % gungan_count;
    i32 nh = (g_lowEndLevelBehaviour >= 1) ? 14 : 6;
    i32 nb = (g_lowEndLevelBehaviour >= 1) ? 5 : 4;
    if (nb > active_neutral_count) {
        i32 nv = gungan_0x92cb8 + 1;
        i32 t = (nv >> 2) & 3;
        nv = (nv & 3) - t;
        gungan_0x92cb8 = (i16)nv;
        i32 model = gungan_0x92cbc[nv];
        void *grp = gungan_0x92bb4[r];
        i32 r2 = *(i32 *)((u8 *)grp + 0x1c);
        void *obj = AddDynamicCreature(model, (nuvec_s *)((u8 *)grp + 0x10), r2, (char *)"Wildlife", NULL, NULL, 0,
                                       NULL, NULL, 0, 0);
        if (obj != NULL) {
            *(void **)((u8 *)obj + 0x364) = gungan_0x92c34[r];
            if (g_lowEndLevelBehaviour == 0)
                *(u8 *)((u8 *)obj + 0xf04) &= 0x7f;
        }
    } else if (active_baddy_count > nh) {
        i32 pick = NuRand(NULL);
        i32 model = (pick & 1) ? id_STAP : 0;
        char *name = (char *)((pick & 1) ? "STAP" : "Battledroid");
        void *grp = gungan_0x92bb4[r];
        i32 r2 = *(i32 *)((u8 *)grp + 0x1c);
        void *obj = AddDynamicCreature(model, (nuvec_s *)((u8 *)grp + 0x10), r2, name, NULL, NULL, 0, NULL, NULL, 0, 0);
        if (obj != NULL) {
            *(void **)((u8 *)obj + 0x364) = gungan_0x92c34[r];
            if (g_lowEndLevelBehaviour == 0)
                *(u8 *)((u8 *)obj + 0xf04) &= 0x7f;
        }
    } else {
        gungan_timer = 0.25f - NuFloatRand(NULL) * 0.5f;
    }
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

void RescueC_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g;
    if ((g = GizmoBlowUp_FindByName(world, "pod31")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "pod11")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "pod51")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "pod21")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "target_a31")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "target_a51")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "target_a41")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "target_a61")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "flower_tub31")) != NULL)
        g->field_0x124 = 1;
    if ((g = GizmoBlowUp_FindByName(world, "flower_tub1")) != NULL)
        g->field_0xa0 |= 2;
    char buf[0x1c];
    for (i32 i = 1; i <= 9; i++) {
        sprintf(buf, "window_balcony%d", i);
        if ((g = GizmoBlowUp_FindByName(world, buf)) != NULL) {
            g->field_0x124 = 1;
            g->field_0xa0 |= 2;
        }
    }
    for (i32 i = 0xa; i <= 0xc; i++) {
        sprintf(buf, "window_balcon%d", i);
        if ((g = GizmoBlowUp_FindByName(world, buf)) != NULL) {
            g->field_0x124 = 1;
            g->field_0xa0 |= 2;
        }
    }
}

void RescueE_Init(WORLDINFO_s *world) {
    GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, "roof_light61");
    if (g != NULL)
        g->field_0xa0 |= 2;
}

void PodRaceInit(WORLDINFO_s *world) {
    podrace_netpacket = (void *)(i32)SetLevelHack(0x14);
    u8 *buf = *(u8 **)((u8 *)world + 0x5120);
    PodRace = buf;
    if (buf == NULL)
        return;
    memset(buf, 0, 0xaf24);
    if (netclient != 0) {
        memset(minesys, 0, 0x1d2 * 4);
        memset(client_mines, 0, 0xc5 * 4);
        if (NuSpecialFind(vehicle_scene, (void **)minesys, "mine") != NULL) {
            *(float *)(buf + 0x70c) = NuSpecialGetOriginRadius(minesys);
            char b2[0x20];
            for (i32 i = 0; i < 10; i++) {
                sprintf(b2, "nomine_%d", i);
                *(void **)(buf + 0x714 + (i32) * (i16 *)(buf + 0x73c) * 4) = AISysFindArea(WORLD->ai_sys, b2);
                *(i16 *)(buf + 0x73c) += 1;
            }
            *(u16 *)(buf + 0x73e) = FindGameDebris(WORLD->debris_sys, "MINE_POP");
            *(u32 *)(buf + 0x740) = PARTLookupType("POD_MINE_PART");
        }
    } else {
        FlightSpline_Init(world, (flightspline_s *)buf, 0x20);
    }
    PodKeyReset();
    ResetPodStuff();
    u8 *sys = (u8 *)apicharsys;
    i16 id = *(i16 *)((u8 *)*(void **)(sys + 0x1c) + (i32)id_ANAKINSPOD * 2);
    if (id != -1) {
        u8 *entry = *(u8 **)(sys + 0x18) + (i32)id * 0x54;
        if (*(void **)((u8 *)*(void **)(entry + 0xc) + 0x4) != NULL)
            podanimendframe = AnimEndFrame(entry, 1);
    }
}

void PodRaceADraw(WORLDINFO_s *world) {
    if (netclient != 0) {
        float mtx[16];
        for (i32 i = 0; i < 0x40; i++) {
            u32 bit = 1u << (i & 0x1f);
            if (((i < 0x20 ? client_mines[0x300 / 4] : client_mines[0x304 / 4]) & bit) != 0) {
                NuMtxSetIdentity(mtx);
                NuMtxTranslate(mtx, &client_mines[i * 3]);
                NuSpecialDrawAt(minesys, mtx);
            }
        }
    } else if (NuSpecialExistsFn(minesys) != 0) {
        float mtx[16];
        u8 *entry = (u8 *)minesys + 0xc;
        u8 *end = (u8 *)minesys + 0x70c;
        for (; entry < end; entry += 0x1c) {
            if (*(u32 *)entry != 0) {
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

void PodRaceCInit(WORLDINFO_s *world) {
    PodRaceInit(world);
    *(u8 *)LevFlag = 0;
    char buf[0x20];
    for (i32 i = 1; i <= 0xa; i++) {
        sprintf(buf, "boost0%i", i);
        NuSpecialFind(world->current_gscn, (void **)((u8 *)LevHSpecial + (i - 1) * 0xc), buf);
    }
}

void PodRacePanel(WORLDINFO_s *world) {
    if (netclient != 0) {
        pod_countdown = *(float *)((u8 *)podrace_netpacket + 0xc);
        if (pod_countdown > 0.0f) {
            u8 *entry = (u8 *)world->lev_objs + (Lap + 0x135) * 0x10;
            if (entry[0xe] != 0)
                return;
            Text3DEx((char *)0, 0, 1.0f, 0.1f, 0.1f, 0.1f, (u16)0, (u16)0, (u16)0, 0, 0x3f, 0);
        } else {
            if (pod_092d00 > 0.0f && PodRace != NULL && *(float *)((u8 *)PodRace + 0xaf08) > 0.0f) {
                char buf[0x20];
                sprintf(buf, "%i", (i32) * (float *)((u8 *)PodRace + 0xaf08) + 1);
                if (NuFmod(*(float *)((u8 *)PodRace + 0xaf08), 1.0f) < 0.7f)
                    Text3DEx(buf, 0, 0.4f, 1.0f, 0.75f, 0.75f, 0.75f, 0, 0xff, 0x3f, 0, (u8)(i32)(128.0f * pod_092d00));
            }
            if (pod_092d10 > 0.0f && PodRace != NULL && *(float *)((u8 *)PodRace + 0xaf00) > 0.0f) {
                char buf[0x20];
                sprintf(buf, "%i", (i32) * (float *)((u8 *)PodRace + 0xaf00) + 1);
                if (NuFmod(*(float *)((u8 *)PodRace + 0xaf00), 1.0f) < 0.7f)
                    Text3DEx(buf, 0, 0.4f, 1.0f, 0.75f, 0.75f, 0.75f, 0, 0, 0, 0, (u8)(i32)(128.0f * pod_092d10));
            }
        }
    } else {
        *(float *)((u8 *)podrace_netpacket + 0xc) = pod_countdown;
        if (pod_092d00 > 0.0f && PodRace != NULL && *(float *)((u8 *)PodRace + 0xaf08) > 0.0f) {
            char buf[0x20];
            sprintf(buf, "%i", (i32) * (float *)((u8 *)PodRace + 0xaf08) + 1);
            if (NuFmod(*(float *)((u8 *)PodRace + 0xaf08), 1.0f) < 0.7f)
                Text3DEx(buf, 0, 0.4f, 1.0f, 0.75f, 0.75f, 0.75f, 0, 0xff, 0x3f, 0, (u8)(i32)(128.0f * pod_092d00));
        }
    }
}

void PodRaceReset() {
    *(u32 *)((u8 *)PodRace + 0xaf04) = 0;
    memset((u8 *)PodRace + 0xa580, 0, 0x980);
    *(u8 *)((u8 *)PodRace + 0xaf20) &= 0xfc;
    podhurry_i = -1;
    PodKeyReset();
}

void PodRaceAReset(WORLDINFO_s *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            pod_lap_start = 3;
            break;
        case 2:
            pod_lap_start = 6;
            break;
        case 3:
            pod_lap_start = 9;
            break;
        default:
            break;
    }
    pod_pacemaker = 0;
    *(u32 *)((u8 *)minesys + 0x710) = 0x4e6e6b28;
    clients_mines_bitfield[0] = 0;
    clients_mines_bitfield[1] = 0;
    *(u32 *)((u8 *)minesys + 0x744) = 0;
    pod_mines_bitfield[0] = 0;
    pod_mines_bitfield[1] = 0;
    memset((u8 *)minesys + 0xc, 0, 0x1c0 * 4);
    memset(client_mines, 0, 0xc5 * 4);
    mine_count = 0;
    if (Lap == 3 && nethost == 0 && netclient == 0)
        NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_sebulba", 1);
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
            CutScene_SnapToEnd((CUTINFO *)cs);
    }
}

extern i32 avg_currentspeed_mul;
extern "C" void PlaySfx(char *);
void PodRaceUpdate(WORLDINFO_s *world, float dt) {
    if (netclient != 0)
        return;
    if (*(float *)((u8 *)PodRace + 0xaf00) <= 0.0f)
        return;
    avg_currentspeed_mul = 0;
    *(float *)((u8 *)PodRace + 0xaf08) = *(float *)((u8 *)PodRace + 0xaf0c);
    if (Player[0] != NULL && *(i8 *)((u8 *)Player[0] + 0x1f8) < 0)
        return;
    if (Player[1] != NULL && *(i8 *)((u8 *)Player[1] + 0x1f8) >= 0)
        *(u32 *)((u8 *)Player[1] + 0xc34) = 0;
    i32 n = (i32) * (float *)((u8 *)PodRace + 0xaf00);
    i32 o = (i32) * (float *)((u8 *)PodRace + 0xaf08);
    if (n != o)
        PlaySfx("Pod_Race_Light");
    for (i32 i = 0; i < 0x10; i++) {
        u8 *entry = (u8 *)PodRace + 0xa580 + i * 0x98;
        if (*(void **)((u8 *)PodRace + 0xa614 + i * 0x98) == NULL)
            continue;
        u32 *p = *(u32 **)(entry + 0x80);
        if (p != NULL) {
        }
    }
}
void PodSprintA_Update(WORLDINFO_s *world) {
    VehicleAreaRememberSpeed = 0x3f800000;
    if (nethost != 0) {
        u8 *ps = (u8 *)podsprint;
        *(i16 *)podsprint_netpacket = (i16)(i8)ps[0x8e];
        *(i16 *)((u8 *)podsprint_netpacket + 6) = (i16) * (float *)(ps + 0x84);
        *(i16 *)((u8 *)podsprint_netpacket + 8) = (i16) * (float *)(ps + 0x88);
        if (netclient == 0) {
            if (*(float *)(ps + 0x84) > 0.0f && *(float *)((u8 *)FadeSys + 0x4) == 0.0f) {
                float v = *(float *)(ps + 0x84);
                if (Player[0] == NULL) {
                    if (Player[1] != NULL) {
                        if (v > 0.0f) {
                            if (2.9f > v) {
                                *(u32 *)((u8 *)Player[1] + 0xee0) = 0x4e6e6b28;
                                if (1.5f <= v) {
                                    i32 n = (i32)v;
                                    i32 o = (i32) * (float *)(ps + 0x84);
                                    if (n != o)
                                        PlaySfx("Pod_Race_Light");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void PodRaceAUpdate(WORLDINFO_s *world) {
    if (pod_pacemaker != 0) {
        if (*(float *)((u8 *)FadeSys + 0x4) == 0.0f || pause_rndr_on != 0)
            pod_092d30 = 0.0f;
    }
    PodRaceUpdate(world, FRAMETIME);
    if (netclient != 0) {
        UpdatePodRaceMines();
        return;
    }
    if (Lap > 3 || MiniCutCam != 0) {
        UpdatePodRaceMines();
        return;
    }
    float t = *(float *)((u8 *)minesys + 0x744) + FRAMETIME;
    *(float *)((u8 *)minesys + 0x744) = t;
    if (t > 1.0f) {
        if (*(float *)((u8 *)minesys + 0x710) == 1000000000.0f) {
            float v = *(float *)((u8 *)GameCam + 0x2c);
            *(float *)((u8 *)minesys + 0x710) = v;
            if (100.0f > v) {
                float r = 0.5f - NuRandFloat() * 60.0f;
                nuvec_s *vec = (nuvec_s *)((u8 *)&r - 0xc);
                NuVecRotateY((NUVEC *)vec, (NUVEC *)vec, *(u16 *)((u8 *)GameCam + 0x686));
                nuvec_s p;
                NuVecAdd((NUVEC *)&p, (NUVEC *)vec, (NUVEC *)((u8 *)GameCam - 0x80));
                if (CreatePodRaceMine(&p) != NULL)
                    *(float *)((u8 *)minesys + 0x710) = *(float *)((u8 *)GameCam + 0x2c);
            }
        }
    }
}

static void UpdatePodRaceMines(void) {
}
static void *CreatePodRaceMine(nuvec_s *a) {
    return NULL;
}
static void UpdatePacemakerDisplay(void *a) {
}

void PodRaceBUpdate(WORLDINFO_s *world) {
    if (Lap == 2) {
        if (*(float *)((u8 *)PodRace + 0xaf04) > 1.0f && pod_092d40 != 0) {
            switch (*(u8 *)((u8 *)LevFlag + 1)) {
                case 0:
                    if (*(float *)((u8 *)GameCam + 0x2c) > pod_0xd440) {
                        NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_mushroom0", 1);
                        *(u8 *)((u8 *)LevFlag + 1) = 1;
                    }
                    break;
                case 1:
                    if (pod_avalanche_cutscene != NULL &&
                        (*(u8 *)((u8 *)*(void **)((u8 *)pod_avalanche_cutscene + 0x4) + 0x89) & 0x10)) {
                        NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM1", 1);
                        pod_0xd460 = pod_0xd440;
                        *(u8 *)((u8 *)LevFlag + 1) = 2;
                    }
                    break;
                case 2:
                    pod_0xd460 -= FRAMETIME;
                    if (pod_0xd460 > 0.0f) {
                        if (*(float *)((u8 *)GameCam + 0x2c) > pod_0xd440)
                            *(u8 *)((u8 *)LevFlag + 1) = 3;
                    } else {
                        NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM2", 1);
                        pod_092d48++;
                        if (pod_092d44 > 0 && pod_092d48 % pod_092d44 == 0) {
                            pod_0xd440 = pod_0xd440 + pod_0xd440 < pod_0xd430 ? pod_0xd440 + pod_0xd440 : pod_0xd430;
                        }
                        *(u8 *)((u8 *)LevFlag + 1) = 4;
                    }
                    break;
                case 3:
                    if (NuSpecialExistsFn(LevHSpecial) != 0 &&
                        NuSpecialClipTestExtents(LevHSpecial, NuSpecialGetDrawMtx(LevHSpecial)) == 0)
                        *(u8 *)((u8 *)LevFlag + 1) = 4;
                    break;
            }
        }
    }
    if (pod_pacemaker != 0) {
        if (*(float *)((u8 *)FadeSys + 0x4) != 0.0f && pause_rndr_on == 0) {
            float t = GameTimer[2];
            pod_092d30 = pod_092d30 + FRAMETIME * 2.0f < 1.0f ? pod_092d30 + FRAMETIME * 2.0f : 1.0f;
            if (NuFmod(t, 1.0f) > 0.1f)
                UpdatePacemakerDisplay(world->lev_objs);
        } else {
            pod_092d30 = 0.0f;
        }
    }
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
    if (Lap == 1) {
        if (*(u8 *)LevFlag == 0) {
            if (GameTimer[0] > 10.0f) {
                Hint_SetComplete(0x27e);
                *(u8 *)LevFlag = 1;
            }
        }
    }
}

void PodRaceCUpdate(WORLDINFO_s *world) {
    if (pod_pacemaker != 0) {
        if (*(float *)((u8 *)FadeSys + 0x4) != 0.0f && pause_rndr_on == 0) {
            float t = pod_092d30 + FRAMETIME * 2.0f;
            pod_092d30 = t < 1.0f ? t : 1.0f;
            if (NuFmod(GameTimer[2], 1.0f) > 0.1f)
                UpdatePacemakerDisplay(world->lev_objs);
        } else {
            pod_092d30 = 0.0f;
        }
    }
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
    PodRaceSnipersUpdate();
    switch (*(u8 *)LevFlag) {
        case 0: {
            CUTINFO *cs = CutScene_Find(world->cutscene_sys, "Ep1_Podrace_TuskenRaiders");
            if (cs != NULL && (*(u8 *)((u8 *)*(void **)((u8 *)cs + 0x4) + 0x89) & 0x10))
                *(u8 *)LevFlag = 1;
            break;
        }
        case 1: {
            i32 none = 1;
            for (i32 off = 0xc; off <= 0x6c; off += 0xc) {
                void *sp = (u8 *)LevHSpecial + off;
                if (NuSpecialExistsFn(sp) != 0) {
                    NuSpecialSetVisibility(sp, 1);
                    none = 0;
                }
            }
            if (none)
                *(u8 *)LevFlag = 2;
            break;
        }
        default:
            break;
    }
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
    u8 *sys = (u8 *)apicharsys;
    i16 val = *(i16 *)((u8 *)*(void **)(sys + 0x1c) + (i32)id_ANAKINSPOD * 2);
    float *fp = NULL;
    if (val != -1) {
        u8 *entry = *(u8 **)(sys + 0x18) + (i32)val * 0x54;
        fp = *(float **)((u8 *)*(void **)(entry + 0xc) + 0x4);
    }
    if (fp != NULL && *fp > 0.0f) {
        pod_animtime[0] = 1.0f;
        pod_roll_target[0] = 0.0f;
        pod_roll[0] = 0.0f;
        pod_roll_target[1] = 0.0f;
        pod_animtime[1] = 1.0f;
        pod_roll[1] = 0.0f;
        pod_animtime[0] = (float)qrand() * 1.5259021893143654e-05f * (*fp - 1.0f) + pod_animtime[0];
        pod_animtime[1] = (float)qrand() * 1.5259021893143654e-05f * (*fp - 1.0f) + pod_animtime[1];
    } else {
        pod_roll_target[0] = 0.0f;
        pod_roll_target[1] = 0.0f;
        pod_roll[0] = 0.0f;
        pod_roll[1] = 0.0f;
        pod_animtime[0] = 1.0f;
        pod_animtime[1] = 1.0f;
    }
}

void SetPodMergeAnims(ANIMPACKET_s *packet, i32 index) {
    u8 *a = (u8 *)packet;
    i16 frame = (pod_roll[index] > 0.0f) ? 0x26 : 0x4f;
    *(u16 *)(a + 0x3a) = 1;
    *(u16 *)(a + 0x42) = frame;
    float m = pod_animtime[index];
    *(float *)(a + 0x10) = m;
    *(float *)(a + 0x14) = m;
    *(u32 *)(a + 0x44) = *(u32 *)&pod_roll[index] & 0x7fffffff;
}

void UpdatePodRaceLapDisplay(float arg) {
    if (*(float *)((u8 *)FadeSys + 0x4) != 0.0f || MiniCutCam != 0 || CUTSTOPGAME != 0) {
        pod_countdown = 0.0f;
        pod_092d00 = 0.0f;
        pod_092d10 = 0.0f;
        if (Paused != 0)
            goto lapdisplay;
        if (PodRace != NULL && *(float *)((u8 *)PodRace + 0xaf08) < 10.0f) {
            i32 oldhurry = podhurry_i;
            podhurry_i = (i32) * (float *)((u8 *)PodRace + 0xaf08);
            if (pod_092d00 < 1.0f)
                pod_092d00 = pod_092d00 + arg * 2.0f < 1.0f ? pod_092d00 + arg * 2.0f : 1.0f;
            if (podhurry_i > 0 && oldhurry != podhurry_i) {
                TickTockSfx();
                if (Paused != 0)
                    return;
            }
        }
    } else if (Paused == 0) {
        pod_countdown = SeekLinearF(1.0f, arg + arg, pod_countdown);
        return;
    }
lapdisplay:
    if (PodRace != NULL && *(float *)((u8 *)PodRace + 0xaf00) > 0.0f && pod_092d10 < 1.0f)
        pod_092d10 = pod_092d10 + arg * 2.0f < 1.0f ? pod_092d10 + arg * 2.0f : 1.0f;
}

void PodSprintA_Init(WORLDINFO_s *world) {
    u8 *ps = (u8 *)podsprint;
    memset(ps, 0, 0x94);
    podsprint_netpacket = SetLevelHack(0xa);
    ps[0x0] = 0;
    void *s = NuSplineFind(world->current_gscn, "finish_line");
    *(void **)(ps + 0x0) = s;
    if (s != NULL && *(i16 *)s <= 1)
        *(void **)(ps + 0x0) = NULL;
    s = NuSplineFind(world->current_gscn, "halfway");
    *(void **)(ps + 0x4) = s;
    if (s != NULL && *(i16 *)s <= 1)
        *(void **)(ps + 0x4) = NULL;
    *(void **)(ps + 0x68) = CheckGizAIMessage(gizaimessagesys, "Lap", NULL);
    *(u32 *)((u8 *)*(void **)(ps + 0x68) + 0x28) = 0;
    *(void **)(ps + 0x6c) = CheckGizAIMessage(gizaimessagesys, "sebulba_max_speed", NULL);
    *(void **)(ps + 0x70) = CheckGizAIMessage(gizaimessagesys, "sebulba_min_speed", NULL);
    *(void **)(ps + 0x74) = CheckGizAIMessage(gizaimessagesys, "sebulba_speed_step", NULL);
    *(u32 *)(ps + 0x8) = 0;
    *(u32 *)(ps + 0xc) = 0;
    *(u32 *)(ps + 0x10) = 0;
    *(u32 *)(ps + 0x14) = 0;
    s = NuSplineFind(world->current_gscn, "ai_sebulba");
    *(void **)(ps + 0x8) = s;
    if (s != NULL && *(void **)(ps + 0x0) != NULL && *(void **)(ps + 0x4) != NULL)
        PodSprint_InitAISpline(world, ps + 0x8, (char *)0);
    *(u32 *)(ps + 0x18) = 0;
    *(u32 *)(ps + 0x1c) = 0;
    *(u32 *)(ps + 0x20) = 0;
    *(u32 *)(ps + 0x24) = 0;
    s = NuSplineFind(world->current_gscn, "ai_general");
    *(void **)(ps + 0x18) = s;
    if (s != NULL && *(void **)(ps + 0x0) != NULL && *(void **)(ps + 0x4) != NULL)
        PodSprint_InitAISpline(world, ps + 0x18, (char *)0);
    NuSpecialFind(world->current_gscn, (void **)((u8 *)LevHSpecial + 0x258), "bigrock_five");
    NuSpecialFind(world->current_gscn, (void **)((u8 *)LevHSpecial + 0x264), "bigrock_eight");
}

i32 XZLinesIntersect(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float *, float *);
static void PodSprint_InitAISpline(WORLDINFO_s *world, void *sp, char *b) {
    u8 *spl = *(u8 **)sp;
    i32 i = 0;
    for (i32 esi = 0; esi < (i32)(i16)(*(u16 *)spl) - 1; esi++) {
        if (i <= 5) {
            u8 *pts = *(u8 **)(spl + 0x8);
            u8 *p0 = pts + esi * 0xc;
            u8 *p1 = pts + (esi + 1) * 0xc;
            if (i & 1) {
                u8 *sp2 = *(u8 **)podsprint;
                u8 *pts2 = *(u8 **)(sp2 + 0x8);
                float x0, y0;
                if (XZLinesIntersect((nuvec_s *)p0, (nuvec_s *)p1, (nuvec_s *)(pts2 + 0xc), (nuvec_s *)(pts2 + 0x18),
                                     &x0, &y0)) {
                    *(u16 *)((u8 *)sp + 0x4 + i * 2) = (u16)esi;
                    i++;
                }
            } else {
                u8 *sp2 = *(u8 **)((u8 *)podsprint + 4);
                u8 *pts2 = *(u8 **)(sp2 + 0x8);
                float x0, y0;
                if (XZLinesIntersect((nuvec_s *)p0, (nuvec_s *)p1, (nuvec_s *)(pts2 + 0xc), (nuvec_s *)(pts2 + 0x18),
                                     &x0, &y0)) {
                    *(u16 *)((u8 *)sp + 0x4 + i * 2) = (u16)esi;
                    i++;
                }
            }
        }
    }
    if (*(u16 *)((u8 *)sp + 0xe) == 0)
        *(u16 *)((u8 *)sp + 0xe) = *(u16 *)spl;
}
void PodSprintA_Panel(WORLDINFO_s *world) {
    u8 *ps = (u8 *)podsprint;
    if (*(float *)((u8 *)FadeSys + 0x4) != 0.0f || MiniCutCam != 0 || CUTSTOPGAME != 0) {
        pod_countdown = 0.0f;
        pod_092d00 = 0.0f;
        pod_092d10 = 0.0f;
        if (Paused == 0) {
            float v = *(float *)(ps + 0x84);
            if (v > 0.0f) {
                char buf[0x20];
                i32 n = (i32)v + 1;
                if (n > 3)
                    n = 3;
                sprintf(buf, "%i", n);
                float m = NuFmod(v, 1.0f);
                if (m < 0.7f) {
                    m = (m - 0.7f) / -0.1f + 1.0f;
                    Text3DEx(buf, 0, 0.4f, 1.0f, m * 0.75f, m * 0.75f, m * 0.75f, 0, 0xff, 0, 0,
                             (u8)(i32)(128.0f * pod_092d10));
                }
            }
        }
        if (pod_countdown > 0.0f) {
            i32 idx = 0x135 + (i8)ps[0x8e];
            if (((u8 *)world->lev_objs)[idx * 0x10 + 0xe] == 0) {
                float c = *(float *)((u8 *)&pod_092d10);
                float f = *(float *)((u8 *)FadeSys + 0x4);
                DrawPanel3DObject(0.0f, f, 1.0f, 0.1f, 0.1f, 0.1f, (u16)0, (u16)0, (u16)0,
                                  (nuhspecial_s *)((u8 *)world->lev_objs + idx * 0x10), 0, c);
            }
        }
    } else {
        if (MiniCutCam == 0 && CUTSTOPGAME == 0) {
            if (*(float *)(ps + 0x84) > 0.0f) {
                if (Paused == 0) {
                    float d = pod_092d10;
                    if (1.0f > d)
                        pod_092d10 = d + FRAMETIME * 2.0f < 1.0f ? d + FRAMETIME * 2.0f : 1.0f;
                }
                pod_countdown = 0.0f;
            } else {
                pod_countdown = SeekLinearF(1.0f, FRAMETIME * 2.0f, pod_countdown);
            }
        }
    }
}

void PodSprintA_Reset(WORLDINFO_s *world) {
    u8 *ps = (u8 *)podsprint;
    pod_092d70[4] = -1.0f;
    pod_092d70[1] = -1.0f;
    pod_092d70[0] = -1.0f;
    pod_092d70[2] = -1.0f;
    pod_092d70[3] = -1.0f;
    u8 b = ps[0x92];
    ps[0x78] = 0;
    ps[0x92] = b & 0xef;
    ps[0x88] = 0;
    if ((b & 0xc) != 0 || (netclient != 0 && *(i16 *)podsprint_netpacket > 2)) {
        ps[0x8e] = 3;
        ps[0x8f] = 4;
        ps[0x92] &= 0xf2;
        VehicleAreaRememberSpeed = 0x3f800000;
        void *p = player;
        if (p != NULL && (*(u8 *)((u8 *)p + 0x1f9) & 0x10)) {
            *(float *)((u8 *)p + 0xdc8) = 1.0f;
            *(float *)((u8 *)p + 0x68) = 0.0f;
            *(float *)((u8 *)p + 0x6c) = 0.0f;
            *(float *)((u8 *)p + 0x70) = *(float *)((u8 *)*(void **)((u8 *)*(void **)((u8 *)p + 0x54) + 0x24) + 0x1c);
            NuVecRotateY((NUVEC *)((u8 *)p + 0x68), (NUVEC *)((u8 *)p + 0x68), *(u16 *)((u8 *)p + 0x276));
        } else if (player2 != NULL && (*(u8 *)((u8 *)player2 + 0x1f9) & 0x10)) {
            *(float *)((u8 *)player2 + 0xdc8) = 1.0f;
            *(float *)((u8 *)player2 + 0x68) = 0.0f;
            *(float *)((u8 *)player2 + 0x6c) = 0.0f;
            *(float *)((u8 *)player2 + 0x70) =
                *(float *)((u8 *)*(void **)((u8 *)*(void **)((u8 *)player2 + 0x54) + 0x24) + 0x1c);
            NuVecRotateY((NUVEC *)((u8 *)player2 + 0x68), (NUVEC *)((u8 *)player2 + 0x68),
                         *(u16 *)((u8 *)player2 + 0x276));
        }
        void *cs = *(void **)((u8 *)game_cutscenes + 0x1c);
        if (cs != NULL) {
            CutScene_SnapToEnd((CUTINFO *)cs);
            CutScene_StoppedFn_LSW((CUTINFO *)cs);
        }
    } else {
        ps[0x8e] = 1;
        ps[0x8f] = 0;
        ps[0x92] &= 0xfe;
        *(float *)(ps + 0x84) = 3.0f;
        pod_092d10 = 0.0f;
    }
    PodRaceSnipersReset();
    *(void **)(ps + 0x7c) = AISysFindArea(world->ai_sys, "Boulders");
}

float PodSprint_RollMul(GameObject_s *obj) {
    u16 id = obj->id;
    if (id == id_CLONEARC || id == id_IMPERIALSHUTTLE || id == id_NABOOSTARFIGHTER)
        return 0.6f;
    if (id == id_XWING || id == id_SNOWSPEEDER || id == id_MILLENNIUMFALCON || id == id_NEW_REPUBLIC_GUNSHIP)
        return 0.8f;
    return 1.0f;
}

void PodSprint_GetIAlongVals(nugspline_s *spline, i16 *out1, i16 *out2) {
    if (spline == NULL)
        return;
    u8 *ps = (u8 *)podsprint;
    i32 idx = -1;
    if (spline == *(void **)(ps + 0x8))
        idx = 0;
    else if (spline == *(void **)(ps + 0x18))
        idx = 1;
    else if (spline == *(void **)(ps + 0x28))
        idx = 2;
    else if (spline == *(void **)(ps + 0x38))
        idx = 3;
    else if (spline == *(void **)(ps + 0x48))
        idx = 4;
    else if (spline == *(void **)(ps + 0x58))
        idx = 5;
    else
        return;
    i32 b = (i8)ps[0x8f];
    if (b != 0) {
        i32 a = b, esi;
        if (b <= 4)
            esi = b - 1;
        else {
            esi = 4;
            a = 5;
        }
        i32 base = idx << 3;
        *out1 = *(i16 *)(ps + 0xc + (base + esi) * 2);
        *out2 = *(i16 *)(ps + 0xc + (base + a) * 2);
    } else {
        *out1 = 0;
        *out2 = *(i16 *)(ps + 0xc + (idx << 4));
    }
}

float PodSprint_InStartCountdown(WORLDINFO_s *world) {
    if (world->current_level != PODSPRINTA_LDATA)
        return 0.0f;
    return *(float *)((u8 *)podsprint + 0x84);
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
    i32 count = NuSpecialFind(world->current_gscn, (void **)LevHSpecial, "minifig_1_1") != NULL;
    count += NuSpecialFind(world->current_gscn, (void **)((u8 *)LevHSpecial + 0xc), "minifig_1_2") != NULL;
    count += NuSpecialFind(world->current_gscn, (void **)((u8 *)LevHSpecial + 0x18), "minifig_1_3") != NULL;
    if (count == 3)
        hothtroopers = (void *)LevHSpecial;
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

void RetakeG_Update(WORLDINFO_s *world) {
    i16 *np = (i16 *)&retakeg_netpacket;
    if (netclient != 0) {
        *(float *)((u8 *)retakeg_guard_b + 0x28) = (float)np[1];
        *(float *)((u8 *)retakeg_guard_a + 0x28) = (float)np[0];
    } else {
        np[1] = (i16) * (float *)((u8 *)retakeg_guard_b + 0x28);
        np[0] = (i16) * (float *)((u8 *)retakeg_guard_a + 0x28);
    }
    u8 *g0 = (u8 *)LevGizForce[0];
    u8 *g1 = (u8 *)LevGizForce[1];
    if (g0 != NULL && *(u32 *)(g0 + 0x40) != 0 && g1 != NULL && *(u32 *)(g1 + 0x40) != 0) {
        for (i32 i = 0; i < 6; i++) {
            u8 *obj = (i < 3) ? g0 : g1;
            u8 *obj40 = *(u8 **)(obj + 0x40);
            u8 *cnx = (u8 *)LevPathCnx[i];
            if (cnx != NULL) {
                if (*(u32 *)(obj40 + 0x24) & 4) {
                    *(u32 *)cnx &= 0x7fffffff;
                    *(u32 *)(cnx + 4) &= 0x7fffffff;
                } else {
                    *(u32 *)cnx |= 0x80000000;
                    *(u32 *)(cnx + 4) |= 0x80000000;
                }
            }
        }
    }
}

void RetakeG_Panel(WORLDINFO_s *world) {
    char buf[0x10];
    i16 countbuf[6];
    for (i32 i = 0; i < 6; i++) {
        buf[i] = 1;
        countbuf[i] = id_ROYALGUARD;
    }
    if (retakeg_guard_a != NULL && *(float *)((u8 *)retakeg_guard_a + 0x28) > 0.0f && retakeg_guard_b != NULL &&
        *(float *)((u8 *)retakeg_guard_b + 0x28) > 0.0f) {
        i32 n = (i32) * (float *)((u8 *)retakeg_guard_b + 0x28);
        if (n > 6)
            n = 6;
        if (n > 0)
            memset(buf, 0, n);
    }
    i32 m = (i32) * (float *)((u8 *)retakeg_guard_a + 0x28);
    if (m > 6)
        m = 6;
    DrawMeleeTargets(countbuf, buf, NULL, m);
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
