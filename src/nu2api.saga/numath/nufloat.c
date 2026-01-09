#include "nu2api.saga/numath/nufloat.h"

#include <math.h>

float NuFabs(float f) {
    return fabsf(f);
}

float NuFnabs(float f) {
    float rv;

    *(int *)&rv = *(int *)&f | 0x80000000;
    return rv;
}

float NuFneg(float v) {
    float rv;

    *(int *)&rv = *(int *)&v ^ 0x80000000;
    return rv;
}

float NuFsign(float f) {
    if (*(int *)&f < 0) {
        return -1.0f;
    } else {
        return 1.0f;
    }
}

float NuFsqrt(float f) {
    if (1e-6f < f) {
        return sqrtf(f);
    }

    return 0.0f;
}