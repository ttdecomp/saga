#ifndef NU2API_SAGA_NUMATH_NUTRIG_H
#define NU2API_SAGA_NUMATH_NUTRIG_H

#include <math.h>

#include "nu2api.saga/numath/nuang.h"

#define NUTRIGTABLE_COUNT 32768
#define NUTRIGTABLE_INTERVAL (float)(2.0f * M_PI / NUTRIGTABLE_COUNT)

/// @brief The sine lookup table
/// @details The sine lookup table is a table of the sine function for the angles 0 to 2π.
extern float NuTrigTable[NUTRIGTABLE_COUNT];

#define NU_SIN_LUT(ang) NuTrigTable[ang >> 1 & 0x7fff]
#define NU_COS_LUT(ang) NuTrigTable[(ang + NUANG_90DEG) >> 1 & 0x7fff]

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initializes the sine lookup table
/// @details Initializes the sine lookup table with the values of the sine function for the angles 0 to 2π.
/// @return void
void NuTrigInit(void);

#ifdef __cplusplus
}
#endif

#endif // NU2API_SAGA_NUMATH_NUTRIG_H