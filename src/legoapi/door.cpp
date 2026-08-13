// Hub/exit-door subsystem: the door lookups and hub spawn bookkeeping that
// Players_InitPositions uses to place the player at the correct exit door.

#include "legoapi/door.h"

#include "legoapi/area.h"
#include "legoapi/level.h"
#include "legoapi/players.h"
#include "legoapi/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

// ---- Local view of DOOR_s / WORLDINFO_s (offsets verified vs. original) ----

// WORLDINFO_s fields used here: door array at +0x469c, count at +0x46a0.
struct doorview_s {
    char pad0[0x469c];
    struct DOOR_s *doors; // 0x469c
    i32 ndoors;           // 0x46a0
};

// Pointer stored in DOOR_s at +0xa0: exit-spline descriptor.
typedef struct doorspline_s {
    i16 count;     // 0x00
    char pad[6];   // 0x02
    NUVEC *arr;    // 0x08
} DOORSPLINE;

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
    struct DOOR_s *a = ((struct doorview_s *)WORLD)->doors;
    if (a != NULL && ((struct doorview_s *)WORLD)->ndoors > 0) {
        i32 i = 0;
        do {
            if (a->spl != NULL && NuStrICmp(a->name, Door_ExitName) == 0) {
                if (a->spl->count < 6) {
                    PlayerStart[0].pos = (NUVEC *)&OldPlrSPos[7].x;
                    PlayerStart[0].angle = *(i16 *)&OldPlrSPos[7].heading;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[5], &arr[4]);
                    PlayerStart[0].pos = &arr[4];
                    PlayerStart[0].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 8) {
                    PlayerStart[1].pos = PlayerStart[0].pos;
                    PlayerStart[1].angle = PlayerStart[0].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[7], &arr[6]);
                    PlayerStart[1].pos = &arr[6];
                    PlayerStart[1].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 10) {
                    PlayerStart[2].pos = PlayerStart[1].pos;
                    PlayerStart[2].angle = PlayerStart[1].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[9], &arr[8]);
                    PlayerStart[2].pos = &arr[8];
                    PlayerStart[2].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 12) {
                    PlayerStart[3].pos = PlayerStart[2].pos;
                    PlayerStart[3].angle = PlayerStart[2].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[11], &arr[10]);
                    PlayerStart[3].pos = &arr[10];
                    PlayerStart[3].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 14) {
                    PlayerStart[4].pos = PlayerStart[3].pos;
                    PlayerStart[4].angle = PlayerStart[3].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[13], &arr[12]);
                    PlayerStart[4].pos = &arr[12];
                    PlayerStart[4].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 16) {
                    PlayerStart[5].pos = PlayerStart[4].pos;
                    PlayerStart[5].angle = PlayerStart[4].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[15], &arr[14]);
                    PlayerStart[5].pos = &arr[14];
                    PlayerStart[5].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 18) {
                    PlayerStart[6].pos = PlayerStart[5].pos;
                    PlayerStart[6].angle = PlayerStart[5].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[17], &arr[16]);
                    PlayerStart[6].pos = &arr[16];
                    PlayerStart[6].angle = NuAtan2D(tmp.x, tmp.z);
                }
                if (a->spl->count < 20) {
                    PlayerStart[7].pos = PlayerStart[6].pos;
                    PlayerStart[7].angle = PlayerStart[6].angle;
                } else {
                    NUVEC tmp;
                    NUVEC *arr = a->spl->arr;
                    NuVecSub(&tmp, &arr[19], &arr[18]);
                    PlayerStart[7].pos = &arr[18];
                    PlayerStart[7].angle = NuAtan2D(tmp.x, tmp.z);
                }
                Door_Start = 1;
                return;
            }
            i++;
            a++;
        } while (i < ((struct doorview_s *)WORLD)->ndoors);
    }
    Door_ExitName[0] = '\0';
}

void *Door_FindByIndex(struct WORLDINFO_s *world, i32 a, i32 b, struct nuvec_s *c) {
    struct doorview_s *w = (struct doorview_s *)world;
    struct DOOR_s *d = w->doors;
    struct DOOR_s *out;
    out = NULL;
    if (d != NULL) {
        i32 count = w->ndoors;
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
                                count = w->ndoors;
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
                            if (((d->flags & 4) == 0) && s != -1 &&
                                (s == b || LDataList[s].unknown_0af == a)) {
                                f32 dist = NuVecDistSqr((NUVEC *)c, &d->pos, NULL);
                                count = w->ndoors;
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
                        if (((d->flags & 4) == 0) && d->level != -1 &&
                            LDataList[d->level].unknown_0af == a) {
                            f32 dist = NuVecDistSqr((NUVEC *)c, &d->pos, NULL);
                            count = w->ndoors;
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
                        if (((d->flags & 4) == 0) && d->level != -1 &&
                            LDataList[d->level].unknown_0af == a) {
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
                            if (LDataList[s].unknown_0af == a) {
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

void *Door_FindByName(struct WORLDINFO_s *world, char *name) {
    if (name != NULL && ((struct doorview_s *)world)->doors != NULL &&
        ((struct doorview_s *)world)->ndoors > 0) {
        i32 i = 0;
        struct DOOR_s *b = ((struct doorview_s *)world)->doors;
        do {
            if (NuStrICmp(name, (char *)b) == 0) {
                return b;
            }
            i++;
            b++;
        } while (i < ((struct doorview_s *)world)->ndoors);
    }
    return NULL;
}