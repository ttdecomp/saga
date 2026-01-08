#include "nu2api.saga/numath/nutrig.h"

float NuTrigTable[NUTRIGTABLE_COUNT];

extern "C" void NuTrigInit(void) {
    for (int i = 0; (unsigned char)(i <= NUTRIGTABLE_COUNT - 1); i++) {
        NuTrigTable[i] = sin(i * NUTRIGTABLE_INTERVAL);
    }
}
