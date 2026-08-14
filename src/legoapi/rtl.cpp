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

extern "C" {

    void fogAlloc(void) {
    }

    void fogFree(void) {
    }

    void rtlAlloc(void) {
    }

    void rtlApplySetScale(void) {
    }

    void rtlDynamicAlloc(void) {
    }

    void rtlDynamicAllocTemplate(void) {
    }

    void rtlDynamicFree(void) {
    }

    void rtlDynamicMasterEnable(void) {
    }

    void rtlDynamicSetColours(void) {
    }

    void rtlDynamicSetDirection(void) {
    }

    void rtlDynamicSetPos(void) {
    }

    void rtlDynamicSetRadii(void) {
    }

    void rtlDynamicSetType(void) {
    }

    void rtlFrameUpdate(void) {
    }

    void rtlFree(void) {
    }

    void rtlGetCurrentSet(void) {
    }

    void rtlGetEnvPath(void) {
    }

    void rtlGetEnvSceneName(void) {
    }

    void rtlGetEnvSet(void) {
    }

    void rtlGetFogSet(void) {
    }

    void rtlInitDynamic(void) {
    }

    void rtlLoadSet(void) {
    }

    void rtlProcessLights(void) {
    }

    void rtlReset(void) {
    }

    void rtlResetEx(void) {
    }

    void rtlSaveSet(void) {
    }

    void rtlScaleSetMultipliers(void) {
    }

    void rtlSetAssocName(void) {
    }

    void rtlSetExt(void) {
    }

    void rtlSetLights(void) {
    }

    void rtlSetMinR(void) {
    }

    void rtlSetModifiers(void) {
    }

    void rtlSetShadowFlickerBlendTime(void) {
    }

    void rtlSetShadowFlickerScale(void) {
    }

    void rtlSetSpecularLight(void) {
    }

    void rtlSetSpecularValue(void) {
    }

    void rtlSetUndoBuffer(void) {
    }

    void rtlSetUserIdName(void) {
    }

    void rtlSpecularValue(void) {
    }

    void rtlResetDynamic(void) {
    }

    i32 rtlFindByUserId(i32 rtl_set, i32 user_id) {
        (void)rtl_set;
        (void)user_id;
        return -1;
    }

    void rtlGetDirection(i32 rtl_set, i32 id, void **out) {
        (void)rtl_set;
        (void)id;
        (void)out;
    }

} // extern "C"

void SelectNextRTL() {
}

void SelectPrevRTL() {
}

void rtlSwapSetEndianess(rtlset *) {
}
