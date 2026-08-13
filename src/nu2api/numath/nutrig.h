#pragma once

#include <math.h>

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuang.h"

#define NUTRIGTABLE_COUNT 32768
#define NUTRIGTABLE_INTERVAL (f32)(2.0f * M_PI / NUTRIGTABLE_COUNT)

/// @brief The sine lookup table
/// @details The sine lookup table is a table of the sine function for the angles 0 to 2π.
extern f32 NuTrigTable[NUTRIGTABLE_COUNT];

#define NU_SIN_LUT(ang) NuTrigTable[(i32)(ang) >> 1 & 0x7fff]
#define NU_COS_LUT(ang) NuTrigTable[((i32)(ang) + NUANG_90DEG) >> 1 & 0x7fff]
#define NU_TAN_LUT(ang) (NuTrigTable[(i32)(ang) >> 1 & 0x7fff] / NuTrigTable[((i32)(ang) + NUANG_90DEG) >> 1 & 0x7fff])

#ifdef __cplusplus
extern "C" {
#endif
    /// @brief Initializes the sine lookup table
    /// @details Initializes the sine lookup table with the values of the sine function for the angles 0 to 2π.
    /// @return void
    void NuTrigInit(void);

    i16 NuACos(f32 cos);
    i32 NuAtan2D(f32 dx, f32 dy);
    f32 NuAtan2(f32 dx, f32 dy);

    f32 NuAtanf(f32 x);
    NUANG NuAtani(f32 x);
    NUANG NuAtan2DA(f32 dx, f32 dy);
    f32 NuAtan2DAF(f32 dx, f32 dy);

    f32 NuSinf(NUANG ang);
    f32 NuCosf(NUANG ang);
#ifdef __cplusplus
}

f32 NuSinApprox2(i32 ang);
f32 NuCosApprox2(i32 ang);
f32 NuSin_Accurate(f32 x);

#endif
