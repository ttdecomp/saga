#include "nu2api.saga/numath/nufloat.h"

#include <math.h>

float NuFabs(float f) {
    union {
        float f;
        unsigned int i;
    } u;

    u.f = f;
    u.i &= 0x7FFFFFFF;
    return u.f;
}

float NuFnabs(float f) {
    // bit-twiddling that approximates what they did originally
    // personally not a fan, maybe we should remove this for portability reasons
    union {
        float f;
        unsigned int i;
    } u;

    u.f = f;
    u.i |= 0x80000000;
    return u.f;
}

float NuFneg(float v) {
    return -v;
}

float NuFsign(float f) {
    return f < 0.0f ? -1.0f : 1.0f;
}

// void NuEquivTollerance(float f) {
//     fetol = f;
// }

// int NuEquiv(float f1, float f2) {
//     return NuFabs(f1 - f2) < fetol;
// }

float NuFsqrt(float f) {
    return sqrt(f);
}