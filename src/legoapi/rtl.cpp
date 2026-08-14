#include "decomp.h"
#include "legoapi_types.h"

struct nuqtdim_s;
struct nuqthdr_s;
struct rtl_s;
struct rtlidata_s;
struct NUFRUSTRUM;

static __used__ int ElOverlaps(nuqtdim_s *, nuqtdim_s *) {
    return 0;
}

static __used__ rtl_s *GetNextRTL(void *, rtl_s *, char *, int *) {
    return nullptr;
}

static __used__ int InsertData(nuqthdr_s *, int, void *) {
    return 0;
}

static __used__ void InsertLight(rtl_s *, rtlidata_s *, float) {
}

static __used__ void InsertAntiLight(rtl_s *, rtlidata_s *, float) {
}

static __used__ int FindNearestRTL(nuvec_s *, int) {
    return 0;
}

static __used__ bool InsideLineXZ(float, float, float, float, float, float) {
    return false;
}

static __used__ int FindNearestFog(nuvec_s *) {
    return 0;
}

static __used__ i32 rtlCalcLights(nuvec_s *, numtx_s *, f32, rtlidata_s *) {
    return 0;
}

static __used__ void rtlCalcShadow(rtlidata_s *) {
}

static __used__ void rtlProcessLight(rtl_s *, f32) {
}

static __used__ void rtlSwapEndianess32(void *) {
}

static __used__ void rtlApplySetScaleLoop(void *, rtlidata_s *, nuvec_s *, numtx_s *, i32, f32) {
}

static __used__ void rtlApplyModifiersToChainLight(rtl_s *) {
}

static __used__ void rtlApplyModifiersToSingleLight(rtl_s *) {
}

static __used__ i32 rtlCmp(rtl_s *, rtl_s *) {
    return 0;
}

static __used__ void transposeClipPlanes(NUFRUSTRUM *) {
}

static __used__ void roomRecursive(nugscn_s *, NUFRUSTRUM *, i16, i16, i32) {
}
