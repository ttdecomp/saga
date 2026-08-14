#include "decomp.h"
#include "legoapi_types.h"

// Static RTL / level-editor data helpers. Stubbed to satisfy the symbol
// baseline.

struct nuqtdim_s;
struct nuqthdr_s;
struct rtl_s;
struct rtlidata_s;

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
