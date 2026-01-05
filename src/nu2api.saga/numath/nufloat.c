#include "nu2api.saga/numath/nufloat.h"

float NuFnabs(float f) {
    // bit-twiddling that approximates what they did originally
    union {
        float f;
        unsigned int i;
    } u;

    u.f = f;
    u.i |= 0x80000000;
    return u.f;
}

float NuFneg(float f) {
    return -f;
}