#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void HashString(unsigned char *) {
}

void ResetSeeds() {
}

void makenuvec4(float, float, float, float) {
}

void FindAnglesXY(nuvec_s *, u16 *, u16 *) {
}

void FindAnglesZX(nuvec_s *, u16 *, u16 *) {
}

void getNumDigits(i32) {
}

void LineCrossedXZ(float, float, float, float, float, float, float, float) {
}

void ScaleAndClamp(i32) {
}

void VecRotateAxis(nuvec_s *, u16, nuvec_s *) {
}

void SolveQuadratic(float, float, float, float *, float *) {
}

void XZLinesClosest(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float *, float *) {
}

void LineIntersectXY(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void MakeThrowVector(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, float) {
}

void OnOrInsidePlane(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float, float *) {
}

void PackCharIntoInt(char, char, char, char) {
}

void TerCrossProduct(nuvec_s *, nuvec_s *) {
}

void DistanceToLineXZ(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void MatrixReflection(numtx_s *, i32, float, float, numtx_s *) {
}

void OnOrOutsidePlane(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void PackShortIntoInt(i16, i16) {
}

void RatioAlongLineXZ(nuvec_s *, nuvec_s *, nuvec_s *) {
}

void XZLinesIntersect(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, float *, float *) {
}

void GetRotationAngles(nuvec_s *, u16 *, u16 *) {
}

void UnpackCharFromInt(i32, char &, char &, char &, char &) {
}

void RatioBetweenPlanes(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void UnpackShortFromInt(i32, i16 &, i16 &) {
}

void AnglesBetweenPoints(nuvec_s *, nuvec_s *, u16 *, u16 *) {
}

void LineIntersectCircle(nuvec_s *, nuvec_s *, nuvec_s *, float) {
}

void LineIntersectSphere(nuvec_s *, nuvec_s *, nuvec_s *, float, float *) {
}

void LineToPlaneDistance(VuVec &, VuVec &, VuVec &) {
}

void LineToPointDistance(VuVec &, VuVec &, VuVec &, VuVec *) {
}

void RatioBetweenEdgesXZ(nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *, nuvec_s *) {
}

void SphereSphereOverlap(nuvec_s *, float, nuvec_s *, float) {
}

void CalcAveragePosAndRad(GIZBUILDIT_s &, VuVec &, float &, bool) {
}

void LineToPlaneIntersecion(VuVec &, VuVec &, VuVec &, VuVec *) {
}

void CalculateInterceptVector(nuvec_s *, nuvec_s *, nuvec_s *, float, nuvec_s *, nuvec_s *) {
}

void LineToSphereIntersection(VuVec &, VuVec &, VuVec &, float, VuVec *, VuVec *) {
}

void MatrixReflectionVU0_AXISY(numtx_s *, float, float, numtx_s *) {
}

void SphereSphereOverlapScaleY(nuvec_s *, float, float, nuvec_s *, float, float) {
}

void IToX(char *, i32) {
}

void XToI(char *) {
}

void IsTok(char const *, char const *) {
}

void CapVec(nuvec_s *, float, nuvec_s *) {
}

void I64ToX(char *, i64) {
}

void XToI64(char *) {
}

void RotDiff(u16, u16) {
}

void memmove(void *, void const *, i32) {
}

void rawClip(VuVec const *, VuVec *, i32, VuVec const &) {
}

void getqseed() {
}

void RotateVec(nuvec_s *, nuvec_s *) {
}

void clampBias(float) {
}

void findrange(nugscn_s *, i32) {
}

void makenuvec(float, float, float) {
}

static __used__ i32 MatchExtension(char *, char *, i32) {
    return 0;
}

static __used__ float fxy(unsigned int, unsigned int) {
    return 0;
}

static __used__ unsigned int fxyda(float, float) {
    return 0;
}

static __used__ float fxydaf(float, float) {
    return 0;
}

static __used__ int icomp(const void *, const void *) {
    return 0;
}

static __used__ i32 sort32a(void const *, void const *) {
    return 0;
}
