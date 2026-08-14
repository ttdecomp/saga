#pragma once

#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"

typedef struct nuquat_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} NUQUAT;

#ifdef __cplusplus
extern "C" {
#endif
    void NuQuatFromEulerXYZ(NUQUAT *out, NUANG psi, NUANG theta, NUANG phi);

    void NuQuatToMtx(NUQUAT *quat, NUMTX *out);

    void NuQuatInv(NUQUAT *out, NUQUAT *quat);

    void NuQuatLerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
    void NuQuatSlerp(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
    void NuQuatCubicInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, f32 t);
    void NuQuatHermiteInt(NUQUAT *out, NUQUAT *m, NUQUAT *a, NUQUAT *b, NUQUAT *c, f32 t);

    void NuQuatHarmonize(NUQUAT *a, NUQUAT *b);

    void NuQuatAdd(NUQUAT *out, NUQUAT *q0, NUQUAT *q1);
    void NuQuatSub(NUQUAT *out, NUQUAT *q0, NUQUAT *q1);
    void NuQuatMul(NUQUAT *out, NUQUAT *q0, NUQUAT *q1);
    f32 NuQuatMagnitude(NUQUAT *q);
    void NuQuatNormalise(NUQUAT *out, NUQUAT *q);
#ifdef __cplusplus
}

void NuQuatNeg2(NUQUAT *out, NUQUAT *in);
void NuQuatBlend(NUQUAT *out, NUQUAT *q0, NUQUAT *q1, f32 blendA, f32 blendB);
void NuQuatLerp2(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
void NuQuatSlerpFast(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
void NuQuatSlerp_Accurate(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t);
f32 NuQuatDot(NUQUAT *q0, NUQUAT *q1);

#endif

f32 NuCubicInterpolation(f32 m, f32 y0, f32 y1, f32 y2, f32 t);
f32 NuHermiteInterpolation(f32 m, f32 y0, f32 y1, f32 y2, f32 t);
