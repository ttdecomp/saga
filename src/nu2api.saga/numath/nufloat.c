#include "nu2api.saga/numath/nufloat.h"

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
    if (f <= 1e-6f) {
        return 0.0f;
    }

    return sqrtf(f);
}

float NuFmod(float a, float b) {
    return a - (int)(a / b) * b;
}
