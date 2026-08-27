#pragma once

#include "nu2api/nucore/common.h"

// edGra terrain swap protection (module gameapi/edtools, edtoolsall_plain.cpp).

#ifdef __cplusplus
extern "C" {
#endif
    void edGraInitTerrainSwapProtection();
    void edGraEnableTerrainSwap();
    void edGraDisableTerrainSwap();
    void edgraStartPage(i32 page);
    void edgraStopPage(i32 page);
#ifdef __cplusplus
}
#endif
