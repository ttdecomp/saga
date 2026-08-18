#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

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

void LevelSplines_InitForGame(LEVELSPLINE *) {
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
    (void)world;
}

