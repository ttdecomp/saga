#include "decomp.h"
#include "stub_decls.h"

// Local (static) render-library (rtl) helpers and the room/frustum walkers.
// Stubbed as local `t` symbols matching res/libTTapp.so.

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
