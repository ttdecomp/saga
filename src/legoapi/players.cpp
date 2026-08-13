#include "legoapi/world.h"

#include <stdio.h>
#include <string.h>

#include "gameapi/edtools/edstubs.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/area.h"
#include "legoapi/character.h"
#include "legoapi/cheat.h"
#include "legoapi/episode.h"
#include "legoapi/level.h"
#include "legoapi/players.h"
#include "legoapi/qrand.h"
#include "legoapi/socksys.h"
#include "legoapi/timer.h"
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

        if (LevelChangesInArea == 0 && UsePlayerList != 0 && PlayerProgress[0].field_0x6 == 0 &&
            PlayerProgress[1].field_0x6 != 0 && FreePlay == 0 && (WORLD->area->flags & 5) == 1 && list0 != -1 &&
            list0 == Area_StoryModelList[0] && list[1] != -1 && list[1] == Area_StoryModelList[2] && list0 != list[1]) {
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
                    g->apiobj.field252_0x1f8 = (u8)((g->apiobj.field252_0x1f8 & 0x7f) | ((slot == 0) << 7));
                    PlayerProgress[slot].hitpoints = g->current_hp;
                    g->field_0x106e = 0;
                } else {
                    char c = g->apiobj.field_0x27c;
                    g->apiobj.field252_0x1f8 =
                        (u8)((g->apiobj.field252_0x1f8 & 0x7f) | (PlayerProgress[c].field_0x6 << 7));

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

typedef struct portalpos_s {
    i16 count; // 0x00
    i16 pad_2;
    f32 *field_0x04;
    f32 *positions; // 0x08
} PORTALPOS;

typedef struct spawnsys_s {
    char pad[0xa0];
    PORTALPOS *portal; // 0xa0
} SPAWNSYS;

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
    SPAWNSYS *np = world->spawn_sys;
    if (np != NULL && bonus == 0) {
        A = np->portal;
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
        if (world->unknown_0120 != last_area && hub_from_cutsceneplayer == 0) {
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

    void *camArg = world->api_object_sys;
    for (i32 i = 0; i < 8; i++) {
        SOCKPOSITION sp;
        ComplexSockPosition(camArg, (void *)PlayerStart[i].pos, -1, -1, &sp);
        PlayerStart[i].u4 = sp.x;
        PlayerStart[i].u8 = sp.y;
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
    if (b != -1 && (CDataList[b].field1_0x4 & 0x2000) == 0 && PlayerID[0] != b && Collection_Got(b) == 1 &&
        GCDataList[b].field275_0x116 != 0 && PlayerID[1] != b) {
        PlayerID[0] = b;
    }
    if (c != -1 && (CDataList[c].field1_0x4 & 0x2000) == 0 && PlayerID[1] != c && Collection_Got(c) == 1 &&
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

extern "C" void ComplexSockPosition(void *a, void *b, i32 c, i32 d, SOCKPOSITION *out) {
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    out->heading = 0.0f;
}

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

unsigned GAMEPAD_ACTION = 0;
void PlayerTakeHit(GameObject_s *, GameObject_s *) {
}

void PlayerItem_Set(PLAYERITEM_s *, PLAYERITEMTYPE_s *) {
}

void Player_FindByID(i32) {
}

void Player_StartPos(GameObject_s *) {
}

void PlayersDropInOut() {
}

void PlayerItem_GotAmmo(PLAYERITEM_s *) {
}

void Players_AveragePos(nuvec_s *, SOCKPOSITION_s *) {
}

void Players_BothActive() {
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
