#include "legoapi/world/world.h"
struct HINT_s;

#include <stdio.h>
#include <string.h>

#include "gameapi/edtools/edstubs.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/world/area.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/level.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/core/input/timer.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nutrig.h"

struct MechTouchUIElement;
struct PLAYERITEM_s;
struct PLAYERITEMTYPE_s;
struct PLAYERPACKET_s;
struct SOCKPOSITION_s;
struct TouchHolder;

void CheckForPlayersTurnedOff();

extern NUVEC plr_lastpos;

extern void GetTopBot(GameObject_s *obj);
extern void GameObjectDimensions(GameObject_s *obj);
extern void GameObjectOrigin(GameObject_s *obj);
extern void ResetRumble(RUMBLEPACKET *packet);
extern void ResetLights(NUVEC *position, rtldata_s *data, void *set);
extern void CurrentStart(GameObject_s *obj, i32 mode, i32 start);
extern void InitSurfaceInfo(GameObject_s *obj);
extern void SetObjOnSurface(GameObject_s *obj, i32 mode);
extern void GizForce_ResetLOS(GameObject_s *obj);
extern void PortalGameObject(GameObject_s *obj, i32 enable, i32 immediate, i16 portal, nugscn_s *scene);

extern "C" void ResetMiniAnimPacket(void *packet, i32 animation);
extern "C" void ComplexSockAngles(SOCKPOSITION *position);

void ResetPlayerAI(GameObject_s *obj);
void ResetPlayerMoves(GameObject_s *obj);

void Players_Init(void) {
    memset(Player, 0, sizeof(Player));
    GameObject_s **p = Player;

    if (netclient != 0) {
        return;
    }

    PLAYERCOUNT = 0;

    if ((WORLD->current_level->flags & 2) != 0) {
        i16 *list;
        i16 list0;

        if (FreePlay == 0 && (Hub_UsePlayerList == 0 || HUB_ADATA == NULL || HUB_ADATA != WORLD->area) &&
            (UsePlayerList != 1 ||
             (PlayerList[0] != -1 && (apicharsys->playermodelids[PlayerList[0]] == -1 ||
                                      (PlayerList[1] != -1 && apicharsys->playermodelids[PlayerList[1]] == -1))))) {
            list = Area_PlayerIDList;
            list0 = Area_PlayerIDList[0];
        } else {
            list = PlayerList;
            list0 = PlayerList[0];
        }

        if (LevelChangesInArea == 0 && UsePlayerList != 0 && PlayerProgress[0].active == 0 &&
            PlayerProgress[1].active != 0 && FreePlay == 0 && (WORLD->area->flags & 5) == 1 && list0 != -1 &&
            list0 == Area_StoryModelList[0].model_id && list[1] != -1 && list[1] == Area_StoryModelList[2].model_id &&
            list0 != list[1]) {
            list[0] = list[1];
            list[1] = list0;
        }

        i32 idx = 0;
        while (list[idx] != -1 && idx != 8) {
            GameObject_s *g = AddCreature(list[idx], 1);
            if (g != NULL) {
                i32 pi;
                u8 slot;
                GAMECHARACTERDATA *cd;

                PreResetCode(g);
                PostResetCode(g);

                pi = PLAYERCOUNT;
                g->oldpos = &OldPlrSPos[PLAYERCOUNT];
                p[PLAYERCOUNT] = g;
                p[pi]->batarang = Batarang + pi * 0xb4;
                g->apiobj.field_0x27c = (char)pi;
                p[pi]->torpedo = GetTorpedoPacket();

                g->field_0x1050 |= 1;
                g->field_0x108e = 0;
                PLAYERCOUNT = PLAYERCOUNT + 1;
                g->hitpoints = DEFAULT_PLAYERHITPOINTS;

                if (UsePlayerList == 0) {
                    slot = g->apiobj.field_0x289;
                    *(u8 *)&g->apiobj.field_0x1f8 = (u8)((*(u8 *)&g->apiobj.field_0x1f8 & 0x7f) | ((slot == 0) << 7));
                    PlayerProgress[slot].hitpoints = g->current_hp;
                    g->field_0x106e = 0;
                } else {
                    char c = g->apiobj.field_0x27c;
                    *(u8 *)&g->apiobj.field_0x1f8 =
                        (u8)((*(u8 *)&g->apiobj.field_0x1f8 & 0x7f) | (PlayerProgress[c].active << 7));

                    if (Area == last_area) {
                        if (UsePlayerList == 1) {
                            g->field_0xe22 = (u8)((g->field_0xe22 & 0xfe) | (PlayerProgress[c].field_0x7 & 1));
                            SetHitPoints(g, PlayerProgress[c].hitpoints);
                            c = g->apiobj.field_0x27c;
                            g->field_0x108e = PlayerProgress[c].field_0xa;
                            g->field_0x106e = PlayerProgress[c].field_0x4;
                            g->field_0xdec = PlayerProgress[c].field_0xc;
                            g->suit = PlayerSuit[c];
                            if (g->torpedo != NULL) {
                                g->torpedo->count = PlayerTorpedoCount[c];
                            }
                        }
                    } else {
                        g->field_0xe22 &= 0xfe;
                        g->field_0x106e = 0;
                    }
                }

                cd = (GAMECHARACTERDATA *)apicharsys->char_data[g->id].field11_0x24;
                g->apiobj.viewdistance = cd->viewdistance;
                g->apiobj.heardistance = cd->heardistance;
                g->apiobj.maxviewheight = cd->maxviewheight;
                g->apiobj.minviewheight = cd->minviewheight;

                {
                    g->ai.nearest_opponent = NULL;
                    g->ai.field_0xdc = 0;
                    g->ai.opponent = NULL;
                    g->ai.field_0xec = 0;
                    g->ai.field_0xe0 = 0x4e6e6b28;
                    g->ai.field_0xf0 = 0x4e6e6b28;
                    g->apiobj.field387_0x2a0 = 0;
                    g->apiobj.field388_0x2a4 = 0;
                    g->field_0xebc = 0;
                    g->field_0xec0 = 0;
                    g->opponent = NULL;
                    g->last_attacker = NULL;
                    g->field_0xecc = 0;
                    g->field_0xed0 = 0;
                    g->ai.antinode_timer = 0.0f;
                    g->field_0xec4 = 0;
                    g->field_0xec8 = 0;
                    g->field_0xed8 = 0;
                    g->ai.field_0x1e5 &= 0xaf;
                    g->field_0xef9 &= 0xf7;
                    g->field_0xef8 &= 0xfe;
                    g->field_0xf00 |= 0x40;
                }
            }
            idx++;
            if (bonusmodearcade != 0 || (HUB_ADATA != NULL && HUB_ADATA == WORLD->area) || VehicleArea != 0)
                break;
        }
    }

    UsePlayerList = 0;

    if (p[0] != NULL) {
        COINPACKET *cp = p[0]->coinpacket = CoinPacket;
        cp->lastcoin = LEGOOBJ_DEFAULTLASTCOIN;
        u32 coins = PlayerProgress[0].coins;
        if (Area != last_area) {
            coins = 0;
        }
        cp->coins = coins;
        p[0]->gizforce_los_info = GizForceLOSInfo;
        memcpy(BackUpPlayers, p[0], 0x439 * 4);
    }

    if (p[1] != NULL) {
        COINPACKET *cp = p[1]->coinpacket = CoinPacket + 1;
        cp->lastcoin = LEGOOBJ_DEFAULTLASTCOIN;
        u32 coins = 0;
        if (Area == last_area) {
            coins = PlayerProgress[1].coins;
        }
        cp->coins = coins;
        p[1]->gizforce_los_info = (char *)GizForceLOSInfo + 0x630;
        memcpy(BackUpPlayers + 0x439, p[1], 0x439 * 4);
    }

    i32 id0 = (p[0] != NULL) ? p[0]->id : -1;
    i32 id1 = (p[1] != NULL) ? p[1]->id : -1;
    RememberPlayerIDs(0, id0, id1);
}

// --- Helpers moved from world.cpp ---

static char sMissionStartDoor[] = "MissionStartDoor";
static char sArcadeStartDoor[] = "ArcadeStartDoor";

static NUVEC HubVehiclesDoorPos[2] = {{0}};
static NUVEC HubMinikitDoorPos[2] = {{0}};

void Players_InitPositions(WORLDINFO *world) __attribute__((optimize("unroll-loops")));
void Players_InitPositions(WORLDINFO *world) {
    i32 bonus = 0;
    if (world->area != NULL) {
        bonus = (i32)((world->area->flags >> 2) & 1);
    }

    for (i32 i = 0; i < 8; i++) {
        PlayerStart[i].pos = &v000;
        PlayerStart[i].angle = 0;
    }

    PORTALPOS *A = NULL;
    i32 ninit = 2;
    DOOR_s *start_door = world->start_door;
    if (start_door != NULL && bonus == 0) {
        A = reinterpret_cast<PORTALPOS *>(start_door->spline);
    } else if (LEGOSPL_START != -1) {
        A = world->portal_places[LEGOSPL_START];
        ninit = 0;
    }

    if (A != NULL) {
        f32 *ps = A->positions;
        NUVEC tmp;
        i32 n = ninit;
        for (i32 i = 0; i < 8; i++) {
            PlayerStart[i].pos = (NUVEC *)&ps[6 * n];
            NuVecSub(&tmp, (NUVEC *)&ps[6 * n + 3], (NUVEC *)&ps[6 * n]);
            PlayerStart[i].angle = NuAtan2D(tmp.x, tmp.z);
            if (2 * n + 4 > A->count)
                n = ninit;
            else
                n = n + 1;
        }
        if (bonus != 0 && A->count > 3) {
            i32 nc = A->count >> 2;
            i32 r = qrand() / (i32)(0xffff / nc + 1);
            PlayerStart[0].pos = (NUVEC *)&ps[12 * r];
            NuVecSub(&tmp, (NUVEC *)&ps[12 * r + 3], (NUVEC *)&ps[12 * r]);
            PlayerStart[0].angle = NuAtan2D(tmp.x, tmp.z);
            PlayerStart[1].pos = (NUVEC *)&ps[12 * r + 6];
            tmp.z = ps[12 * r + 9] - ps[12 * r + 6];
            tmp.x = ps[12 * r + 9] - PlayerStart[0].pos->x;
            PlayerStart[1].angle = NuAtan2D(tmp.x, tmp.z);
        }
    }

    // --- hub exit-door lookup ---
    if (HUB_ADATA != NULL && HUB_ADATA == world->area) {
        if (world->level_sub_id != last_area && hub_from_cutsceneplayer == 0) {
            if (hub_from_superstory != -1) {
                i32 area =
                    Episode_FindAreaFromFlags((EPISODEDATA *)((char *)EDataList + hub_from_superstory * 0x1c), 5, 5);
                void *door = Door_FindByIndex(world, area, -1, NULL);
                if (door != NULL) {
                    NuStrCpy(Door_ExitName, (char *)door);
                }
            } else if (hub_from_mission != -1) {
                void *door = Door_FindByName(world, sMissionStartDoor);
                if (door != NULL) {
                    NuStrCpy(Door_ExitName, (char *)door);
                }
            } else if (hub_from_arcade != -1) {
                void *door = Door_FindByName(world, sArcadeStartDoor);
                if (door != NULL) {
                    NuStrCpy(Door_ExitName, (char *)door);
                }
            } else if (hub_startoutsidebonusdoor_area != -1) {
                void *door = Door_FindByIndex(world, hub_startoutsidebonusdoor_area, -1, NULL);
                if (door != NULL) {
                    NuStrCpy(Door_ExitName, (char *)door);
                }
            } else if (VEHICLES_ADATA == NULL || (u8)((char *)VEHICLES_ADATA)[0x7c] == 0xa09) {
                if (VEHICLES_ADATA != NULL && (i32)(u8)((char *)VEHICLES_ADATA)[0x7c] == 0xa09) {
                    PlayerStart[0].pos = (NUVEC *)&HubVehiclesDoorPos[0];
                    PlayerStart[1].pos = (NUVEC *)&HubVehiclesDoorPos[1];
                } else if ((u16)(ADataList[0xa09].flags) & 0x5) {
                    PlayerStart[0].pos = (NUVEC *)&HubMinikitDoorPos[0];
                    PlayerStart[1].pos = (NUVEC *)&HubMinikitDoorPos[1];
                }
            }
        }
    }
    StartDoorPositions();

    Door_CutSpl = NULL;
    if (Door_ExitCameraSplineName[0] != 0) {
        NUGSPLINE *spl = NuSplineFind(world->current_gscn, Door_ExitCameraSplineName);
        Door_CutSpl = spl;
        if (spl == NULL) {
            Door_UseCutCam = 0;
        } else {
            Door_CutCamPos0 = spl->pts[0];
            Door_CutCamPos1 = spl->pts[1];
        }
    }
    Door_ExitCameraSplineName[0] = 0;

    for (i32 i = 0; i < 8; i++) {
        SOCKPOSITION sp;
        ComplexSockPosition(world->sock_sys, PlayerStart[i].pos, -1, -1, &sp);
        PlayerStart[i].sock_location = sp.location;
        PlayerStart[i].sock_ratio = sp.ratio;
    }
    HubStartDoor = NULL;

    if (HUB_ADATA != NULL && HUB_ADATA == world->area) {
        hub_from_superstory = -1;
        hub_from_mission = -1;
        hub_from_arcade = -1;
        if (hub_from_cutsceneplayer != 0) {
            void *av = CutScenePlayer_Available();
            if (av != NULL && (i16)(*(i16 *)((char *)CutScenePlayer_Available() + 0xa)) != -1) {
                void *door =
                    Door_FindByIndex(world, -1, (i32)(*(i16 *)((char *)CutScenePlayer_Available() + 0xa)), NULL);
                HubStartDoor = door;
                if (door != NULL) {
                    f32 *vps = *(f32 **)((char *)(*(void **)((char *)door + 0xa0)) + 0x8);
                    NUVEC v2;
                    NuVecSub(&v2, (NUVEC *)&vps[15], (NUVEC *)&vps[12]);
                    PlayerStart[0].pos = (NUVEC *)&vps[12];
                    PlayerStart[0].angle = NuAtan2D(v2.x, v2.z);
                    NuVecSub(&v2, (NUVEC *)&vps[21], (NUVEC *)&vps[18]);
                    PlayerStart[1].pos = (NUVEC *)&vps[18];
                    PlayerStart[1].angle = NuAtan2D(v2.x, v2.z);
                } else {
                    shop_from_cutsceneplayer = hub_from_cutsceneplayer;
                }
            } else {
                shop_from_cutsceneplayer = hub_from_cutsceneplayer;
            }
        }
        hub_from_cutsceneplayer = 0;
    }
}

typedef struct {
    i32 field_0;
    char *name;
    u8 field_0x8;
    u8 field_0x9;
    u8 field_0xa;
    u8 field_0xb;
} PlayerItemTypeEntry;

extern PlayerItemTypeEntry *PlayerItemType;
extern i32 PLAYERITEMTYPECOUNT;

extern i8 BoltType_FindIDByName(char *name, WORLDINFO *world);

void PlayerItemTypes_Reset(WORLDINFO *world) {
    if (PlayerItemType != 0 && PLAYERITEMTYPECOUNT > 0) {
        for (i32 i = 0; i < PLAYERITEMTYPECOUNT; i++) {
            PlayerItemType[i].field_0x9 = (u8)BoltType_FindIDByName(PlayerItemType[i].name, world);
        }
    }
}

GameObject_s *AddCreature(i32 id, i32 param) {
    if ((u32)id >= 0x154) {
        return NULL;
    }
    if (apicharsys->playermodelids[id] == -1) {
        return NULL;
    }
    GameObject_s *g = AddGameObject(id);
    if (g == NULL) {
        return NULL;
    }
    InitCreature(g, id, param);
    return g;
}

static f32 sPreResetMulA = 0.0f;
static f32 sPreResetMulB = 0.0f;
static f32 sPreResetSubC = 0.0f;
static f32 sPreResetD18Scale = 0.0f;
static f32 sPreReset1048Scale = 0.0f;
static f32 sPreResetDivF = 0.0f;
static f32 sPreResetMulG = 0.0f;

void PreResetCode(GameObject_s *obj) {
    u8 *b = (u8 *)obj;

    u8 t23 = (u8)(b[0xe23] & 0xf8);
    b[0xe20] &= 0xef;
    u8 t22 = (u8)(b[0xe22] & 0x3f);
    u8 t25 = (u8)(b[0xe25] & 0xaf);
    b[0xe24] &= 0xcf;
    b[0xf03] &= 0xdf;
    b[0xe3e] = 0xff;
    b[0xe3f] = 0xff;
    b[0xe3c] = 0x00;
    b[0xe3d] = 0xff;
    b[0xe25] = t25;
    b[0xe22] = t22;
    b[0xe23] = t23;

    DrawOffsetCode(obj, 0);

    if ((*(u32 *)&b[0xf00] & 8) != 0) {
        b[0xe22] &= 0xf7;
        *(f32 *)&b[0xc40] = (f32)qrand() * sPreResetMulA * sPreResetMulB - sPreResetSubC;
        *(f32 *)&b[0xc44] = (f32)qrand() * sPreResetMulA * sPreResetMulB - sPreResetSubC;
        b[0xe20] &= 0xfb;
        *(f32 *)&b[0xc48] = (f32)qrand() * sPreResetMulA * sPreResetMulB - sPreResetSubC;
        *(u32 *)&b[0xcf4] = 0;
        *(u32 *)&b[0xcec] = 0;
        *(u32 *)&b[0xcf0] = 0;
        *(u32 *)&b[0xca0] = 0;
        if (b[0x7a5] == 0xa) {
            b[0xe21] &= 0xf7;
        }
        b[0xe21] &= 0xdf;
        b[0xe33] = 0x01;
        b[0xe22] &= 0xfd;
        b[0xe46] = 0xff;
        b[0xe45] = 0xff;
        b[0xe44] = 0xff;
        b[0xe43] = 0xff;
        *(u32 *)&b[0xd0c] = 0;
        *(u16 *)&b[0x4a] = 0xffff;
        *(u32 *)&b[0xd7c] = 0;
        if ((*(u32 *)(*(u32 *)&b[0x54] + 4) & 0x8040) == 0) {
            b[0xe31] = 0;
        }

        {
            i32 e04 = *(i32 *)&b[0xe04];
            b[0xe23] &= 0x7f;
            if (e04 != -1) {
                rtlDynamicEnable(e04, 0);
            }
        }

        {
            u8 al = b[0x27d];
            b[0xe25] &= 0xfd;
            b[0xe24] &= 0xbf;
            b[0x1089] = 0;

            if (al != 0) {
                *(u32 *)&b[0xd18] = 0x3e4ccccd; /* 0.2f */
            } else if (*(f32 *)&b[0xd18] > 0.0f) {
                *(f32 *)&b[0xd18] -= FRAMETIME;
            }

            if ((i8)b[0x1f8] >= 0 || VehicleArea != 0) {
                goto finish_dfd;
            }

            if (b[0x7a5] == 0 &&
                sPreReset1048Scale * *(f32 *)(*(u32 *)(*(u32 *)&b[0x54] + 0x24) + 0x30) > *(f32 *)&b[0x76c]) {
                goto finish_dfd;
            }

            {
                f32 f28v = *(f32 *)(*(u32 *)&b[0xc94] + 0x28);
                if (f28v > 0.0f) {
                    if ((al & 2) != 0) {
                        goto finish_dfd;
                    }
                    if (b[0x1084] == 0) {
                        goto finish_dfd;
                    }
                } else {
                    if (*(f32 *)&b[0x68] == 0.0f && *(f32 *)&b[0x70] == 0.0f) {
                        goto finish_dfd;
                    }
                    if ((al & 2) != 0) {
                        goto finish_dfd;
                    }
                    if (b[0x1084] == 0) {
                        goto finish_dfd;
                    }
                }

                {
                    f32 t1 = *(f32 *)&b[0x6a8];
                    if (t1 <= NuTrigTable[0x4000]) {
                        goto finish_dfd;
                    }
                    f32 t2 = NuTrigTable[0x3000];
                    if (t2 <= t1) {
                        goto finish_dfd;
                    }
                    t1 = *(f32 *)&b[0x1e0] / sPreResetDivF;
                    f32 diff = *(f32 *)&b[0x17c] - *(f32 *)&b[0x218];
                    t1 *= sPreResetMulG;
                    if (diff <= t1) {
                        goto finish_dfd;
                    }
                    if (f28v <= 0.0f) {
                        NUVEC dir;
                        dir.x = *(f32 *)&b[0x68];
                        dir.y = 0.0f;
                        dir.z = *(f32 *)&b[0x70];
                        NuVecNorm(&dir, &dir);
                        if ((*(f32 *)&b[0x698] - *(f32 *)&b[0x5c]) * dir.x +
                                (*(f32 *)&b[0x6a0] - *(f32 *)&b[0x64]) * dir.z <=
                            0.0f) {
                            goto finish_dfd;
                        }
                    } else {
                        NUVEC dir;
                        NuVecRotateY(&dir, &v001, *(u16 *)&b[0x5a]);
                        if ((*(f32 *)&b[0x698] - *(f32 *)&b[0x5c]) * dir.x +
                                (*(f32 *)&b[0x6a0] - *(f32 *)&b[0x64]) * dir.z <=
                            0.0f) {
                            goto finish_dfd;
                        }
                    }
                    *(u32 *)&b[0x1048] = 0x3dcccccd; /* 0.1f */
                }
            }
            goto finish_e1e;

        finish_dfd:
            if (*(f32 *)&b[0x1048] > 0.0f) {
                *(f32 *)&b[0x1048] -= FRAMETIME;
            }
            goto finish_e1e;

        finish_e1e:
            GameObjectNearFloor(obj, 1.0f, (f32 *)&b[0xda0]);
            *(f32 *)&b[0xdb8] = GetHoverPosY(obj);
            {
                u8 v = *(u8 *)(*(u32 *)(*(u32 *)&b[0x54] + 0x24) + 0x117);
                if (v != 0xff) {
                    if (b[0x27c] == 0xff) {
                        b[0xe42] = v;
                    } else {
                        if (Cheat_IsOn(0x19) != 0) {
                            b[0xe42] = 0x00;
                        } else if (b[0x27c] != 0xff && Player_HasPurpleForce(obj) != 0) {
                            b[0xe42] = 0x03;
                        } else {
                            b[0xe42] = *(u8 *)(*(u32 *)(*(u32 *)&b[0x54] + 0x24) + 0x117);
                        }
                    }
                } else {
                    b[0xe42] = 0xff;
                }

                {
                    u32 mask = GAMEPAD_ACTION;
                    f32 xmm0 = *(f32 *)&b[0xde4];
                    if ((*(u32 *)(*(u32 *)&b[0xc94] + 4) & mask) != 0) {
                        *(f32 *)&b[0xde4] = xmm0 + FRAMETIME;
                    } else {
                        if (xmm0 > 0.0f && sPreResetD18Scale > xmm0) {
                            b[0xe24] |= 0x40;
                        }
                        *(f32 *)&b[0xde4] = 0.0f;
                    }
                }
            }
            b[0xe25] &= 0x5f;
        }
    }

    return;
}

void PostResetCode(GameObject_s *obj) {
    u8 *p = (u8 *)obj;
    void *pp = *(void **)(p + 0x54);
    *(u8 *)(p + 0x1091) = 0;
    void *q = *(void **)((u8 *)pp + 0x24);
    i16 v = *(i16 *)((u8 *)q + 0xe8);
    if (v != -1) {
        u8 b = *(u8 *)((u8 *)q + 0x11e);
        f32 f = (f32)(u32)b;
        ChatterSfx(obj, v, f);
    }
    if (*(void **)(*(u8 **)(p + 0x54) + 0x18) != (void *)&Move_VEHICLE && *(u16 *)(p + 0xe1c) != 0 &&
        (*(u8 *)(p + 0xe25) & 0x20) == 0) {
        *(u16 *)(p + 0xe1c) = 0;
    }
}

static TORPEDOPACKET TorpedoPackets[16];

TORPEDOPACKET *GetTorpedoPacket(void) __attribute__((optimize("unroll-loops")));
TORPEDOPACKET *GetTorpedoPacket(void) {
    for (i32 i = 0; i < 16; i++) {
        if ((TorpedoPackets[i].field_0x1 & 1) == 0) {
            TorpedoPackets[i].field_0x1 |= 1;
            return &TorpedoPackets[i];
        }
    }
    return NULL;
}

void SetHitPoints(GameObject_s *obj, i32 hp) {
    obj->current_hp = (u8)hp;
    if ((i8)hp > (i32)(u32)obj->hitpoints) {
        obj->current_hp = obj->hitpoints;
    }
}

extern i32 Collection_Got(i32 id);

void RememberPlayerIDs(i32 a, i32 b, i32 c) __attribute__((force_align_arg_pointer));
void RememberPlayerIDs(i32 a, i32 b, i32 c) {
    if (VehicleArea != 0 || GAMEDEMO != 0) {
        return;
    }
    if (a == 0) {
        if ((WORLD->current_level->flags & 0x4e2) != 2) {
            return;
        }
    }
    if (b != -1 && (CDataList[b].model_flags & 0x2000) == 0 && PlayerID[0] != b && Collection_Got(b) == 1 &&
        GCDataList[b].field275_0x116 != 0 && PlayerID[1] != b) {
        PlayerID[0] = b;
    }
    if (c != -1 && (CDataList[c].model_flags & 0x2000) == 0 && PlayerID[1] != c && Collection_Got(c) == 1 &&
        GCDataList[c].field275_0x116 != 0 && PlayerID[0] != c) {
        PlayerID[1] = c;
    }
    if (b != c && PlayerID[1] == b && PlayerID[0] == c) {
        PlayerID[0] = b;
        PlayerID[1] = c;
    }
}

// ---- Player start spawn entries ----
PLAYERSTARTENTRY PlayerStart[8];

// ---- Misc player/gameobject helpers relocated from doorstubs.cpp ----

void *CutScenePlayer_Available(void) {
    return NULL;
}

void ChatterSfx(GameObject_s *g, i32 a, float b) {
    (void)g;
    (void)a;
    (void)b;
}

void Move_VEHICLE(GameObject_s *g) {
    (void)g;
}

void DrawOffsetCode(GameObject_s *obj, i32 param) {
    (void)obj;
    (void)param;
}

// rtlDynamicEnable uses C linkage in the original binary (plain symbol name).
extern "C" void rtlDynamicEnable(i32 id, i32 param) {
    (void)id;
    (void)param;
}

float GameObjectNearFloor(GameObject_s *obj, float h, float *out) {
    (void)obj;
    (void)h;
    if (out != NULL) {
        *out = 0.0f;
    }
    return 0.0f;
}

float GetHoverPosY(GameObject_s *obj) {
    (void)obj;
    return 0.0f;
}

i32 Player_HasPurpleForce(GameObject_s *obj) {
    (void)obj;
    return 0;
}

// Directional/button mask constants (original .data @0x667bf0-0x667c40).
u32 GAMEPAD_DRIGHT = 0x2000;
u32 GAMEPAD_DLEFT = 0x8000;
u32 GAMEPAD_DDOWN = 0x4000;
u32 GAMEPAD_DUP = 0x1000;
u32 GAMEPAD_TOGGLERIGHT = 10;
u32 GAMEPAD_TOGGLELEFT = 5;
u32 GAMEPAD_TAG = 16;
u32 GAMEPAD_SPECIAL = 32;
u32 GAMEPAD_ACTION = 128;
u32 GAMEPAD_JUMP = 64;
u32 GAMEPAD_START = 2048;
u32 GAMEPAD_SELECT = 256;
u32 GAMEPAD_MENUSELECT = 64;
u32 GAMEPAD_MENUCANCEL = 16;

void PlayerTakeHit(GameObject_s *, GameObject_s *) {
}

void PlayerItem_Set(PLAYERITEM_s *, PLAYERITEMTYPE_s *) {
}

void Player_FindByID(i32) {
}

NUVEC *Player_StartPos(GameObject_s *obj) {
    i32 index = obj->apiobj.field_0x27c;
    if (index < 0 || index > 7) {
        index = obj->apiobj.field_0x289;
    }
    index &= 7;
    return PlayerStart[index].pos != NULL ? PlayerStart[index].pos : PlayerStart[0].pos;
}

i32 PlayersDropInOut() {
    CheckForPlayersTurnedOff();
    return 0;
}

void PlayerItem_GotAmmo(PLAYERITEM_s *) {
}

i32 Players_AveragePos(nuvec_s *, SOCKPOSITION_s *) {
    return 0;
}

i32 Players_BothActive() {
    return Player[0] != NULL && static_cast<i8>(Player[0]->apiobj.field_0x1f8) < 0 && Player[1] != NULL &&
           static_cast<i8>(Player[1]->apiobj.field_0x1f8) < 0;
}

void PlayerItemType_Find(i32) {
}

void Player_ClearContext(GameObject_s *, i32) {
}

void Player_HasFastBuild(GameObject_s *) {
}

void PlayerItemTypes_Init(PLAYERITEMTYPE_s *) {
}

void Player_ResetContexts(PLAYERPACKET_s *) {
}

void Player_CopyEssentials(GameObject_s *, GameObject_s *) {
}

void Player_HasDeflectBolts(GameObject_s *) {
}

void Player_ToggleCharacter(GameObject_s *, i32, i32) {
}

void Player_HasInvincibility(GameObject_s *) {
}

void Player_HasDoubleBoltDamage(GameObject_s *) {
}

void PlayerButton_OnHold_Callback(MechTouchUIElement &, TouchHolder &) {
}

void Player_HasDoubleWeaponDamage(GameObject_s *) {
}

void PlayerButton_OnLeave_Callback(MechTouchUIElement &, TouchHolder &) {
}

void Player_HasDoubleBoltDamage_FromBolt(BOLT_s *) {
}

void PlayerButton_OnClick_Callback_NextButton(MechTouchUIElement &, TouchHolder &) {
}

static __used__ i32 SelectOpponent(GameObject_s *, f32, f32, i32, i32) {
    return 0;
}

static __used__ i32 ShootThisFrame(GameObject_s *, i32, i32) {
    return 0;
}

static __used__ void SetComboOpponent(GameObject_s *, f32, i32, i32) {
}

static __used__ void Player_ClearContext_Game(GameObject_s *, i32) {
}

static __used__ u16 CanGlide_Game(GameObject_s *) {
    return {};
}

static __used__ unsigned int CanStartHold_Game(GameObject_s *) {
    return {};
}

static __used__ unsigned int CanPushBlocks_Game(GameObject_s *) {
    return {};
}

static __used__ unsigned int CanPushObstacles_Game(GameObject_s *) {
    return {};
}

static __used__ void PlayerButton_PlayHint(HINT_s *) {
}

static __used__ void PlayerButton_UpdateHint(HINT_s *) {
}

void KillPlayer(GameObject_s *, i32, i32, nuvec_s *) {
}

namespace {

    constexpr f32 kInvalidSurfaceHeight = 2000000.0f;

    void SyncPlayerSpawnPosition(GameObject_s *obj) {
        const NUVEC &position = obj->apiobj.position;

        obj->apiobj.pos_x = position.x;
        obj->apiobj.pos_y = position.y;
        obj->apiobj.pos_z = position.z;

        obj->apiobj.start_position[0] = position.x;
        obj->apiobj.start_position[1] = position.y;
        obj->apiobj.start_position[2] = position.z;
        obj->apiobj.initial_position[0] = position.x;
        obj->apiobj.initial_position[1] = position.y;
        obj->apiobj.initial_position[2] = position.z;

        plr_lastpos = position;
    }

} // namespace

void ResetPlayer(GameObject_s *obj, i32 reset_moves, nuvec_s *position, i32 snap_to_surface) {
    if (obj == NULL) {
        return;
    }

    const bool restore_hitpoints = obj->apiobj.field_0x27c != -1 && obj->field_0x7a5 == 0x2b;

    if (reset_moves != 0) {
        if (position == NULL) {
            position = Player_StartPos(obj);
        }
        if (position != NULL) {
            obj->apiobj.position = *position;
        }

        if (obj->apiobj.field_0x27c != -1) {
            obj->field_0xf00 |= 0x40;
        }

        ResetPlayerMoves(obj);
        SyncPlayerSpawnPosition(obj);

        obj->apiobj.field_0x68 = v000.x;
        obj->apiobj.field_0x6c = v000.y;
        obj->apiobj.field_0x70 = v000.z;
        obj->reset_velocity = v000;

        GetTopBot(obj);
        GameObjectDimensions(obj);
        obj->field_0xe23 &= static_cast<u8>(~8u);
        obj->use_model_origin = 0;
        obj->apiobj.field_0x288 = 0;
        GameObjectOrigin(obj);

        ResetRumble(&obj->pad_gamepad->rumble_packet);
        ResetLights(&obj->apiobj.position, &obj->light_data, WORLD->rtl_set);
        ResetMiniAnimPacket(obj->mini_anim_packet, -1);

        obj->sock_position.location.sock = -1;
        obj->sock_position.location.segment = -1;
        if (WORLD->sock_sys != NULL) {
            ComplexSockPosition(WORLD->sock_sys, &obj->apiobj.position, -1, -1, &obj->sock_position);
            ComplexSockAngles(&obj->sock_position);
        }

        u8 player_index = static_cast<u8>(obj->apiobj.field_0x27c);
        if (player_index > 7) {
            player_index = obj->apiobj.field_0x289;
        }
        player_index &= 7;

        const u16 facing = static_cast<u16>(PlayerStart[player_index].angle);
        obj->apiobj.pitch_angle = 0;
        obj->apiobj.roll_angle = 0;
        obj->apiobj.facing_angle = facing;
        obj->apiobj.movement_facing_angle = facing;
        obj->apiobj.field_0x276 = facing;
        NuVecRotateY(&obj->facing_direction, &v001, facing);

        obj->field_0xc34 = 0x3f800000;
        obj->field_0xc38 = 0.0f;
        CurrentStart(obj, 0, 1);
        obj->field_0xe23 &= static_cast<u8>(~8u);
        obj->field_0xefe &= static_cast<u8>(~4u);
        obj->field_0xeff &= static_cast<u8>(~2u);
        obj->apiobj.model_draw_result = 1;
        obj->use_model_origin = 0;
        obj->apiobj.field_0x288 = 0;
        obj->field_0x1084 = 0;

        InitSurfaceInfo(obj);
        f32 surface_y = GetHoverPosY(obj);
        if (snap_to_surface == 0 || (obj->apiobj.character_data->model_flags & 0x2000) != 0) {
            if (surface_y != kInvalidSurfaceHeight) {
                obj->apiobj.position.y = surface_y;
                SyncPlayerSpawnPosition(obj);
            }
        } else {
            SetObjOnSurface(obj, 0);
        }

        GizForce_ResetLOS(obj);
        PortalGameObject(obj, 1, 1, -1, WORLD->current_gscn);

        if ((ResetBits & 2) != 0 || obj->apiobj.field_0x287 != 0 || restore_hitpoints) {
            SetHitPoints(obj, DEFAULT_PLAYERHITPOINTS);
            obj->field_0xe38 = 4;
        }
        if ((ResetBits & 8) != 0) {
            const i32 progress_index = static_cast<i8>(obj->apiobj.field_0x27c);
            if (progress_index >= 0 && progress_index < 8) {
                if (PlayerProgress[progress_index].field_0xb != 0) {
                    obj->field_0xe31 = 1;
                }
                obj->field_0xe38 = PlayerProgress[progress_index].field_0x9;
            }
        }

        ResetPlayerAI(obj);
        obj->apiobj.previous_position[0] = obj->apiobj.position.x;
        obj->apiobj.previous_position[1] = obj->apiobj.position.y;
        obj->apiobj.previous_position[2] = obj->apiobj.position.z;
        obj->field_0x10c8 = obj->apiobj.position.x;
        obj->field_0x10cc = obj->apiobj.position.y;
        obj->field_0x10d0 = obj->apiobj.position.z;
        obj->field_0xdc8 = 0.0f;
    } else {
        obj->field_0xdc8 = 0.0f;
    }

    obj->field_0x1004 = 1.0f;
    obj->field_0x101c = 0.0f;
    obj->field_0xda8 = 0.0f;
    obj->field_0xd78 = 1.0f;
    obj->apiobj.field_0x214 = kInvalidSurfaceHeight;
    obj->field_0xdbc = 0.0f;
    obj->field_0xf1c = 0.0f;
    obj->field_0xc54 = -1.0f;
    obj->field_0xde0 = 0.0f;
    obj->apiobj.field_0x287 = 0;
    obj->field_0xe36 = 0;
    obj->apiobj.field_0x285 = 0;
    obj->apiobj.field_0x1f8 &= 0xfa83;
    obj->apiobj.field_0x1fa &= static_cast<u8>(~2u);
    obj->apiobj.field_0x1f4 &= ~0x100u;
    obj->field_0xefc |= 0x80;
    obj->field_0xe21 |= 0x80;
}

void ResetPlayerAI(GameObject_s *) {
}

void ActivatePlayer(GameObject_s *) {
}

i32 MakePlayerList(i32 count) {
    i32 player_count = 0;

    for (i32 i = 0; i < count; i++) {
        GameObject_s *player = Player[i];
        if (player == NULL) {
            if (makeplayerlist_freeplay == 1 && player_count == 1 && WORLD != NULL &&
                WORLD->current_level == HUB_LDATA && HUB_LDATA != NewLData) {
                PlayerList[1] = FreePlayModelList[i].model_id;
                PlayerProgress[1].active = 0;
                player_count = 2;
            }
            continue;
        }

        if (makeplayerlist_freeplay == 1) {
            PlayerList[player_count] = FreePlayModelList[i].model_id;
        } else if (makeplayerlist_freeplay != 2) {
            PlayerList[player_count] = player->id;
        }

        PLAYERPROGRESS *progress = &PlayerProgress[player_count];
        progress->active = (u8)player->apiobj.field_0x1f8 >> 7;

        bool unavailable = false;
        if (player->field_0x7a5 != 6 && player->field_0xe32 != 2) {
            unavailable = true;
            if ((player->field_0xe22 & 1) == 0) {
                unavailable = player->field_0xe32 != 0;
            }
        }
        progress->field_0x7 = unavailable;
        progress->coins = player->coinpacket != NULL ? player->coinpacket->coins : 0;
        progress->field_0xb = player->field_0xe31 == 1;

        if (player->apiobj.field_0x287 != 0) {
            progress->hitpoints = player->hitpoints;
            progress->field_0x9 = 4;
        } else {
            progress->hitpoints = player->current_hp;
            progress->field_0x9 = player->field_0xe38;
        }
        progress->field_0xa = player->field_0x108e;
        progress->field_0x4 = player->field_0x106e;
        progress->field_0xc = player->field_0xdec;

        PlayerSuit[player_count] = player->suit;
        PlayerTorpedoCount[player_count] = player->torpedo != NULL ? player->torpedo->count : 0;
        player_count++;
    }

    for (i32 i = player_count; i < 8; i++) {
        PlayerList[i] = -1;
    }
    makeplayerlist_freeplay = 0;
    return player_count;
}

void CollectHitPoint(GameObject_s *, nuvec_s *, i32) {
}

void DeactivatePlayer(GameObject_s *, float, GameObject_s *) {
}

void ResetPlayerMoves(GameObject_s *) {
}

void SetToLastSafePos(GameObject_s *) {
}

void AvailableToPlayer(u32, i32, i32, i32) {
}

void GetNumLocalPlayers() {
}

void UnderPlayerControl(GameObject_s *) {
}

void ActivePlayerInRange(nuvec_s *, float, float *) {
}

void GetOtherActivePlayer(GameObject_s *) {
}

void FindNearestPlayerToVec(nuvec_s *, GameObject_s **, float &, bool, u32) {
}

void SetPlayerGroupPosition(float, float, float) {
}

void UpdateLastSafePosition(GameObject_s *) {
}

void CheckForPlayersTurnedOff() {
}

void FindFurthestPlayerFromVec(nuvec_s *, GameObject_s **, float &, bool, u32) {
}

void AveragePlayerCurrentSpeedMul() {
}

void SetPlayer() {
    if (Player[0] != NULL && static_cast<i8>(Player[0]->apiobj.field_0x1f8) < 0) {
        player = Player[0];
        if (Player[1] != NULL && static_cast<i8>(Player[1]->apiobj.field_0x1f8) < 0) {
            player2 = Player[1];
            return;
        }
    } else if (Player[1] != NULL && static_cast<i8>(Player[1]->apiobj.field_0x1f8) < 0) {
        player = Player[1];
    } else {
        player = NULL;
        return;
    }

    player2 = NULL;
}
