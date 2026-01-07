#include "nu2api.saga/numath/nutrig.h"

float NuTrigTable[NUTRIGTABLE_COUNT];

void NuTrigInit(void) {
    for (int i = 0; i < NUTRIGTABLE_COUNT; ++i) {
        NuTrigTable[i] = sin(i * NUTRIGTABLE_INTERVAL);
    }
}
