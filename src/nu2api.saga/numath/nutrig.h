#ifndef NU2API_SAGA_NUMATH_NUTRIG_H
#define NU2API_SAGA_NUMATH_NUTRIG_H

#include <math.h>

#define NUTRIGTABLE_COUNT 32768
#define NUTRIGTABLE_INTERVAL (float)(2.0f * M_PI / NUTRIGTABLE_COUNT)

extern float NuTrigTable[NUTRIGTABLE_COUNT];

/// @brief Initializes the sine lookup table
/// @details Initializes the sine lookup table with the values of the sine function for the angles 0 to 2π.
/// @return void
void NuTrigInit(void);

#endif // NU2API_SAGA_NUMATH_NUTRIG_H