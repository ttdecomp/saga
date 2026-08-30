// Hub/exit-door subsystem: the door lookups and hub spawn bookkeeping that
// Players_InitPositions uses to place the player at the correct exit door.

#include "legoapi/props/doors/door.h"

#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

// ---- Exit-camera / cut-door data ----
NUVEC Door_CutCamPos0;
NUVEC Door_CutCamPos1;
i32 Door_UseCutCam = 0;
struct nugspline_s *Door_CutSpl = NULL;
char Door_ExitCameraSplineName[64] = {0};
char Door_ExitName[64] = {0};
i32 Door_Start = 0;

// ---- Hub exit-door bookkeeping ----
void *HubStartDoor = NULL;
void *VEHICLES_ADATA = NULL;
i32 LEGOSPL_START = -1;
i32 hub_from_superstory = -1;
i32 hub_from_mission = -1;
i32 hub_from_arcade = -1;
i32 hub_from_cutsceneplayer = 0;
i32 shop_from_cutsceneplayer = 0;
i32 hub_startoutsidebonusdoor_area = -1;

void StartDoorPositions(void) {
    Door_Start = 0;
    if (Door_ExitName[0] == '\0') {
        return;
    }
    DOOR_s *a = WORLD->doors;
    if (a != NULL && WORLD->door_count > 0) {
        i32 i = 0;
        do {
            if (a->spline != NULL && NuStrICmp(a->name, Door_ExitName) == 0) {
                if (a->spline->length < 6) {
                    PlayerStart[0].pos = OldPlrSPos[7].door_fallback.position;
                    PlayerStart[0].angle = OldPlrSPos[7].door_fallback.angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[5], &arr[4]);
                    PlayerStart[0].pos = &arr[4];
                    PlayerStart[0].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 8) {
                    PlayerStart[1].pos = PlayerStart[0].pos;
                    PlayerStart[1].angle = PlayerStart[0].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[7], &arr[6]);
                    PlayerStart[1].pos = &arr[6];
                    PlayerStart[1].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 10) {
                    PlayerStart[2].pos = PlayerStart[1].pos;
                    PlayerStart[2].angle = PlayerStart[1].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[9], &arr[8]);
                    PlayerStart[2].pos = &arr[8];
                    PlayerStart[2].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 12) {
                    PlayerStart[3].pos = PlayerStart[2].pos;
                    PlayerStart[3].angle = PlayerStart[2].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[11], &arr[10]);
                    PlayerStart[3].pos = &arr[10];
                    PlayerStart[3].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 14) {
                    PlayerStart[4].pos = PlayerStart[3].pos;
                    PlayerStart[4].angle = PlayerStart[3].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[13], &arr[12]);
                    PlayerStart[4].pos = &arr[12];
                    PlayerStart[4].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 16) {
                    PlayerStart[5].pos = PlayerStart[4].pos;
                    PlayerStart[5].angle = PlayerStart[4].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[15], &arr[14]);
                    PlayerStart[5].pos = &arr[14];
                    PlayerStart[5].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 18) {
                    PlayerStart[6].pos = PlayerStart[5].pos;
                    PlayerStart[6].angle = PlayerStart[5].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[17], &arr[16]);
                    PlayerStart[6].pos = &arr[16];
                    PlayerStart[6].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spline->length < 20) {
                    PlayerStart[7].pos = PlayerStart[6].pos;
                    PlayerStart[7].angle = PlayerStart[6].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spline->pts;
                    NuVecSub(&tmp, &arr[19], &arr[18]);
                    PlayerStart[7].pos = &arr[18];
                    PlayerStart[7].angle = NuAtan2D(tmp.x, tmp.z);
                }
                Door_Start = 1;
                return;
            }
            i++;
            a++;
        } while (i < WORLD->door_count);
    }
    Door_ExitName[0] = '\0';
}

DOOR_s *Door_FindByIndex(WORLDINFO_s *world, i32 a, i32 b, NUVEC *c) {
    DOOR_s *d = world->doors;
    DOOR_s *out;
    out = NULL;
    if (d != NULL) {
        i32 count = world->door_count;
        if (count > 0) {
            if (a == -1) {
                if (b != -1) {
                    if (c != NULL) {
                        f32 best = 1000000.0f;
                        struct DOOR_s *bestDoor = NULL;
                        i32 i = 0;
                        do {
                            if (((d->flags & 4) == 0) && d->level != -1 && d->level == b) {
                                f32 dist = NuVecDistSqr((NUVEC *)c, &d->pos, NULL);
                                count = world->door_count;
                                if (dist < best) {
                                    best = dist;
                                    bestDoor = d;
                                }
                            }
                            i++;
                            d++;
                        } while (i < count);
                        return bestDoor;
                    }
                    i32 i = 0;
                    do {
                        if (((d->flags & 4) == 0) && d->level != -1 && d->level == b) {
                            return d;
                        }
                        i++;
                        d++;
                    } while (i != count);
                }
            } else {
                if (c != NULL) {
                    if (b != -1) {
                        f32 best = 1000000.0f;
                        struct DOOR_s *bestDoor = NULL;
                        i32 i = 0;
                        do {
                            i16 s = d->level;
                            if (((d->flags & 4) == 0) && s != -1 && (s == b || LDataList[s].area_index == a)) {
                                f32 dist = NuVecDistSqr((NUVEC *)c, &d->pos, NULL);
                                count = world->door_count;
                                if (dist < best) {
                                    best = dist;
                                    bestDoor = d;
                                }
                            }
                            i++;
                            d++;
                        } while (i < count);
                        return bestDoor;
                    }
                    f32 best = 1000000.0f;
                    struct DOOR_s *bestDoor = NULL;
                    i32 i = 0;
                    do {
                        if (((d->flags & 4) == 0) && d->level != -1 && LDataList[d->level].area_index == a) {
                            f32 dist = NuVecDistSqr((NUVEC *)c, &d->pos, NULL);
                            count = world->door_count;
                            if (dist < best) {
                                best = dist;
                                bestDoor = d;
                            }
                        }
                        i++;
                        d++;
                    } while (i < count);
                    return bestDoor;
                }
                if (b == -1) {
                    i32 i = 0;
                    do {
                        if (((d->flags & 4) == 0) && d->level != -1 && LDataList[d->level].area_index == a) {
                            return d;
                        }
                        i++;
                        d++;
                    } while (i != count);
                } else {
                    i32 i = 0;
                    do {
                        i16 s = d->level;
                        if ((d->flags & 4) == 0 && s != -1) {
                            if (s == b) {
                                return d;
                            }
                            if (LDataList[s].area_index == a) {
                                return d;
                            }
                        }
                        i++;
                        d++;
                    } while (i != count);
                }
            }
        }
    }
    return out;
}

DOOR_s *Door_FindByName(WORLDINFO_s *world, char *name) {
    if (name != NULL && world->doors != NULL && world->door_count > 0) {
        i32 i = 0;
        DOOR_s *b = world->doors;
        do {
            if (NuStrICmp(name, (char *)b) == 0) {
                return b;
            }
            i++;
            b++;
        } while (i < world->door_count);
    }
    return NULL;
}
static __used__ void GoThroughDoor_ExtraCode(WORLDINFO_s *, DOOR_s *) {
}
