#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nutrig.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void setnextpoint(float, float) {
}

void BezierLinePos(VuVec &, VuVec &, VuVec &, VuVec &, VuVec &, float) {
}

void nugraph_blend(i32, i32, i32 *, float) {
}

void BezierLineEval(VuVec &, VuVec &, VuVec &, VuVec &, VuVec &, float) {
}

void GetIntCurveVal(ani3_animheader_s *, float *, i32) {
}

void nugraph_linear(i32, nuvec_s *, nuvec_s *, i32) {
}

void CalcSplinePoint(flightspline_s *, _vuv_s *, float) {
}

void nugraph_bspline(i32, i32, nuvec_s *, nuvec_s *, i32) {
}

void BezierLineLength(VuVec &, VuVec &, VuVec &, VuVec &) {
}

void BezierLineLength(VuVec &, VuVec &, VuVec &, VuVec &, float) {
}

void LoadShelfSplines() {
    splshelf = NuSplineFind(WORLD->current_gscn, const_cast<char *>("shelf_top1"));
    splcharshelf = NuSplineFind(WORLD->current_gscn, const_cast<char *>("shelf_bottom"));
    splcodes = NuSplineFind(WORLD->current_gscn, const_cast<char *>("shelf_6"));

    if (splshelf == NULL || splcharshelf == NULL || splcodes == NULL) {
        return;
    }

    memset(SubShelfPos, 0, sizeof(SubShelfPos));
    memset(ShelfPos, 0, sizeof(ShelfPos));
    memset(CodePos, 0, 6 * sizeof(*CodePos));

    for (i32 i = 0; i < splshelf->length; ++i) {
        ShelfPos[i] = splshelf->pts[i];
    }
    for (i32 i = 0; i < splcharshelf->length; ++i) {
        SubShelfPos[i] = splcharshelf->pts[i];
    }
    for (i32 i = 0; i < splcodes->length; ++i) {
        CodePos[i] = splcodes->pts[i];
    }

    ShelfPos[2].x += (ShelfPos[3].x - ShelfPos[2].x) * 0.5f;
    ShelfPos[2].z += (ShelfPos[3].z - ShelfPos[2].z) * 0.5f;

    NUVEC diff;
    NuVecSub(&diff, &CodePos[5], &CodePos[4]);
    NuVecAdd(&CodePos[6], &CodePos[5], &diff);
    CodePos[6].y += 0.02f;
    NuVecScale(&diff, &diff, 0.5f);
    NuVecSub(&CodePos[0], &CodePos[0], &diff);
    NuVecSub(&CodePos[1], &CodePos[1], &diff);
    NuVecSub(&CodePos[2], &CodePos[2], &diff);
    NuVecSub(&CodePos[3], &CodePos[3], &diff);
    NuVecSub(&CodePos[4], &CodePos[4], &diff);
    NuVecSub(&CodePos[5], &CodePos[5], &diff);
    NuVecSub(&CodePos[6], &CodePos[6], &diff);

    NUVEC start = splshelf->pts[0];
    NUVEC end = splshelf->pts[splshelf->length];
    NUVEC direction;
    NuVecSub(&direction, &end, &start);
    shelfang = static_cast<u16>(NuAtan2D(direction.x, direction.z));
}

void PointAlongSpline(nugspline_s *, float, nuvec_s *, u16 *, u16 *, i32) {
}

void getnextdatapoint(float *, i32 *) {
}

void FlightSpline_Init(WORLDINFO_s *, flightspline_s *, i32) {
}

void OutSideSplineArea(nuvec_s *, nugspline_s *, nuvec_s *, nuvec_s *, i32) {
}

void InitSplinePosition(SPLINEPOS_s *, nugspline_s *, float, i32) {
}

void nugraphGetXatIndex(nugraph_s *, i32) {
}

void nugraphGetYatIndex(nugraph_s *, i32) {
}

void nugraph_catmullrom(i32, nuvec_s *, nuvec_s *, i32) {
}

void GetNearestSplinePos(nuvec_s *, SPLINEPOS_s *, nugspline_s *, i32, i16, i16) {
}

void nugraph_compute_point(i32 *, i32, i32, float, nuvec_s *, nuvec_s *) {
}

void CalcSplinePointFromDist(flightspline_s *, _vuv_s *, float) {
}

static LEVELSPLINE *LevSplList;
static i32 LEVELSPLINECOUNT;
static i32 levspl_i_start = -1;
static i32 levspl_i_startcam = -1;

void LevelSplines_InitForGame(LEVELSPLINE *splines) {
    LevSplList = splines;
    LEVELSPLINECOUNT = 0;
    levspl_i_start = -1;
    levspl_i_startcam = -1;

    if (splines == NULL) {
        return;
    }

    for (LEVELSPLINE *spline = splines; spline->name != NULL; ++spline) {
        if (levspl_i_start == -1 && NuStrICmp(spline->name, "start") == 0) {
            levspl_i_start = LEVELSPLINECOUNT;
        }
        if (levspl_i_startcam == -1 && NuStrICmp(spline->name, "start_cam") == 0) {
            levspl_i_startcam = LEVELSPLINECOUNT;
        }
        ++LEVELSPLINECOUNT;
    }
}

void nugraph_compute_intervals(i32 *, i32, i32) {
}

void EvaluateSplineXZIntersection(nugspline_s *, i32, SPLINEPOS_s *, nugspline_s *, i32, SPLINEPOS_s *) {
}

void nugraph_compute_linear_point(i32, float, nuvec_s *, nuvec_s *) {
}

void nugraph_compute_catmull_point(i32, float, nuvec_s *, nuvec_s *) {
}

void setpoint(float) {
}

static __used__ f32 SplineLength(nugspline_s *, i32) {
    return 0.0f;
}

void LevelSplines_InitForLevel(WORLDINFO_s *world) {
    world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
    world->portal_places = reinterpret_cast<PORTALPOS **>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr += LEVELSPLINECOUNT * sizeof(*world->portal_places);
    memset(world->portal_places, 0, LEVELSPLINECOUNT * sizeof(*world->portal_places));

    if (LevSplList == NULL) {
        return;
    }

    for (i32 i = 0; i < LEVELSPLINECOUNT; ++i) {
        LEVELSPLINE *entry = &LevSplList[i];
        if ((entry->area != -1 && world->level_sub_id != entry->area) ||
            (entry->level != -1 && world->level_idx != entry->level)) {
            continue;
        }

        NUGSCN *scene = entry->scene != NULL ? *entry->scene : world->current_gscn;
        if (scene == NULL) {
            continue;
        }

        NUGSPLINE *spline = NuSplineFind(scene, const_cast<char *>(entry->name));
        world->portal_places[i] = reinterpret_cast<PORTALPOS *>(spline);
        if (spline == NULL) {
            continue;
        }

        const i32 point_count = spline->length;
        if ((entry->min_points != 0 && point_count < entry->min_points) ||
            (entry->max_points != 0 && entry->min_points <= entry->max_points && point_count > entry->max_points)) {
            world->portal_places[i] = NULL;
        }
    }
}
