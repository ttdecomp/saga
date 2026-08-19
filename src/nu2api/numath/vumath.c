#include "decomp.h"

#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"

static void VuQuatCopy(NUQUAT *dst, NUQUAT *src) {
    (void)dst;
    (void)src;
}

static void VuQuatBlend(NUQUAT *out, NUQUAT *a, NUQUAT *b, f32 t, f32 w) {
    (void)out;
    (void)a;
    (void)b;
    (void)t;
    (void)w;
}

static f32 VuQuatDot(NUQUAT *a, NUQUAT *b) {
    (void)a;
    (void)b;
    return 0;
}

static void VuQuatLerp(NUQUAT *out, NUQUAT *a, NUQUAT *b, f32 t) {
    (void)out;
    (void)a;
    (void)b;
    (void)t;
}

static void VuQuatNeg2(NUQUAT *out, NUQUAT *in) {
    (void)out;
    (void)in;
}

static void VuQuatNormalise(NUQUAT *out, NUQUAT *in) {
    (void)out;
    (void)in;
}

static void VuVecMtxMul(NUVEC *out, NUVEC *v, NUMTX *m) {
    (void)out;
    (void)v;
    (void)m;
}

static void VuVecSet(f32 *out, f32 x, f32 y, f32 z, f32 w) {
    (void)out;
    (void)x;
    (void)y;
    (void)z;
    (void)w;
}

static void VuMtxTranspose(NUMTX *dst, NUMTX *src) {
    (void)dst;
    (void)src;
}

static void NuVecClear(NUVEC *v) {
    (void)v;
}
