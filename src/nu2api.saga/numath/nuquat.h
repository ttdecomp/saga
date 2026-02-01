#pragma once

#include "nu2api.saga/numath/nuang.h"
#include "nu2api.saga/numath/numtx.h"

typedef struct nuquat_s {
    float x;
    float y;
    float z;
    float w;
} NUQUAT;

#ifdef __cplusplus
extern "C" {
#endif
    void NuQuatFromEulerXYZ(NUQUAT *out, NUANG psi, NUANG theta, NUANG phi);

    void NuQuatToMtx(NUQUAT *quat, NUMTX *out);

    void NuQuatInv(NUQUAT *out, NUQUAT *quat);

    void NuQuatLerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, float t);
    void NuQuatSlerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, float t);
    void NuQuatCubicInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, float t);
    void NuQuatHermiteInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, float t);

    void NuQuatHarmonize(NUQUAT *a, NUQUAT *b);
#ifdef __cplusplus
}
#endif

float NuCubicInterpolation(float m, float y0, float y1, float y2, float t);
float NuHermiteInterpolation(float m, float y0, float y1, float y2, float t);
