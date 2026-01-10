#include "nu2api.saga/numath/numtx.h"

#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numath/nutrig.h"
#include "nu2api.saga/numath/nuvec.h"

NUMTX numtx_zero = {0};

// clang-format off
NUMTX numtx_identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
// clang-format on

void NuMtxSetZero(NUMTX *m) {
    *m = numtx_zero;
}

void NuMtxSetIdentity(NUMTX *m) {
    *m = numtx_identity;
}

void NuMtxSetTranslation(NUMTX *m, NUVEC *t) {
    m->_30 = t->x;
    m->_31 = t->y;
    m->_32 = t->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetTranslationNeg(NUMTX *m, NUVEC *t) {
    m->_30 = -t->x;
    m->_31 = -t->y;
    m->_32 = -t->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetScale(NUMTX *m, NUVEC *s) {
    m->_00 = s->x;
    m->_11 = s->y;
    m->_22 = s->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxSetRotationX(NUMTX *m, NUANG a) {
    m->_11 = m->_22 = NU_COS_LUT(a);
    m->_12 = NU_SIN_LUT(a);
    m->_21 = -m->_12;
    m->_00 = 1.0f;
    m->_01 = m->_02 = m->_03 = m->_23 = m->_10 = m->_20 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxSetRotationY(NUMTX *m, NUANG a) {
    m->_00 = m->_22 = NU_COS_LUT(a);
    m->_20 = NU_SIN_LUT(a);
    m->_02 = -m->_20;
    m->_11 = 1.0f;
    m->_01 = m->_10 = m->_03 = m->_23 = m->_12 = m->_21 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxSetRotationZ(NUMTX *m, NUANG a) {
    m->_00 = m->_11 = NU_COS_LUT(a);
    m->_01 = NU_SIN_LUT(a);
    m->_10 = -m->_01;
    m->_22 = 1.0;
    m->_02 = m->_12 = m->_03 = m->_23 = m->_20 = m->_21 = m->_13 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0;
}

void NuMtxSetRotationAxis(NUMTX *m, NUANG ang, NUVEC *ax) {
    NUANG angle = ang >> 1; // get half angle
    float s = NU_SIN_LUT(angle);
    float t = NU_COS_LUT(angle);

    float a = ax->x * s;
    float _2a = a + a;
    float b = ax->y * s;
    float _2b = b + b;
    float c = ax->z * s;
    float _2c = c + c;

    float _2sa = t * _2a;
    float _2aa = a * _2a;
    float _2sb = t * _2b;
    float _2ab = a * _2b;
    float _2bb = b * _2b;
    float _2sc = t * _2c;
    float _2ac = a * _2c;
    float _2bc = b * _2c;
    float _2cc = c * _2c;

    m->_00 = 1.0f - (_2bb + _2cc);
    m->_11 = 1.0f - (_2aa + _2cc);
    m->_22 = 1.0f - (_2aa + _2bb);
    m->_33 = 1.0f;
    m->_01 = _2ab + _2sc;
    m->_02 = _2ac - _2sb;
    m->_03 = 0.0f;
    m->_10 = _2ab - _2sc;
    m->_12 = _2bc + _2sa;
    m->_13 = 0.0f;
    m->_20 = _2ac + _2sb;
    m->_21 = _2bc - _2sa;
    m->_23 = 0.0f;
    m->_30 = m->_31 = m->_32 = 0.0f;
}

void NuMtxTranslate(NUMTX *m, NUVEC *t) {
    m->_30 += t->x;
    m->_31 += t->y;
    m->_32 += t->z;
}

void NuMtxTranslateNeg(NUMTX *m, NUVEC *t) {
    m->_30 -= t->x;
    m->_31 -= t->y;
    m->_32 -= t->z;
}

void NuMtxPreTranslate(NUMTX *m, NUVEC *t) {
    m->_30 += t->x * m->_00 + t->y * m->_10 + t->z * m->_20;
    m->_31 += t->x * m->_01 + t->y * m->_11 + t->z * m->_21;
    m->_32 += t->x * m->_02 + t->y * m->_12 + t->z * m->_22;
}

void NuMtxPreTranslateX(NUMTX *m, float tx) {
    m->_30 = m->_30 + m->_00 * tx;
    m->_31 = m->_31 + m->_01 * tx;
    m->_32 = m->_32 + m->_02 * tx;
}

void NuMtxPreTranslateNeg(NUMTX *m, NUVEC *t) {
    m->_30 -= t->x * m->_00 + t->y * m->_10 + t->z * m->_20;
    m->_31 -= t->x * m->_01 + t->y * m->_11 + t->z * m->_21;
    m->_32 -= t->x * m->_02 + t->y * m->_12 + t->z * m->_22;
}

void NuMtxScale(NUMTX *m, NUVEC *s) {
    m->_00 *= s->x;
    m->_01 *= s->y;
    m->_02 *= s->z;
    m->_10 *= s->x;
    m->_11 *= s->y;
    m->_12 *= s->z;
    m->_20 *= s->x;
    m->_21 *= s->y;
    m->_22 *= s->z;
    m->_30 *= s->x;
    m->_31 *= s->y;
    m->_32 *= s->z;
}

void NuMtxScaleU(NUMTX *m, float s) {
    m->_00 *= s;
    m->_01 *= s;
    m->_02 *= s;
    m->_10 *= s;
    m->_11 *= s;
    m->_12 *= s;
    m->_20 *= s;
    m->_21 *= s;
    m->_22 *= s;
    m->_30 *= s;
    m->_31 *= s;
    m->_32 *= s;
}

void NuMtxPreScaleU(NUMTX *m, float s) {
    m->_00 *= s;
    m->_01 *= s;
    m->_02 *= s;
    m->_10 *= s;
    m->_11 *= s;
    m->_12 *= s;
    m->_20 *= s;
    m->_21 *= s;
    m->_22 *= s;
}

NUVEC NuMtxGetScale(NUMTX *m) {
    NUVEC scale;

    scale.x = NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
    scale.y = NuFsqrt(m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12);
    scale.z = NuFsqrt(m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22);

    return scale;
}

void NuMtxPreScale(NUMTX *m, NUVEC *s) {
    m->_00 *= s->x;
    m->_01 *= s->x;
    m->_02 *= s->x;
    m->_10 *= s->y;
    m->_11 *= s->y;
    m->_12 *= s->y;
    m->_20 *= s->z;
    m->_21 *= s->z;
    m->_22 *= s->z;
}

void NuMtxPreScaleX(NUMTX *m, float ScaleX) {
    m->_00 = m->_00 * ScaleX;
    m->_01 = m->_01 * ScaleX;
    m->_02 = m->_02 * ScaleX;
}

void NuMtxRotateX(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _01 = m->_01;
    float _11 = m->_11;
    float _21 = m->_21;
    float _31 = m->_31;

    m->_01 = _01 * cosx - m->_02 * sinx;
    m->_02 = _01 * sinx + m->_02 * cosx;
    m->_11 = _11 * cosx - m->_12 * sinx;
    m->_12 = _11 * sinx + m->_12 * cosx;
    m->_21 = _21 * cosx - m->_22 * sinx;
    m->_22 = _21 * sinx + m->_22 * cosx;
    m->_31 = _31 * cosx - m->_32 * sinx;
    m->_32 = _31 * sinx + m->_32 * cosx;
}

void NuMtxPreRotateX(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _10 = m->_10;
    float _11 = m->_11;
    float _12 = m->_12;

    m->_10 = cosx * _10 + m->_20 * sinx;
    m->_11 = cosx * _11 + m->_21 * sinx;
    m->_12 = cosx * _12 + m->_22 * sinx;
    m->_20 = m->_20 * cosx - sinx * _10;
    m->_21 = m->_21 * cosx - sinx * _11;
    m->_22 = m->_22 * cosx - sinx * _12;
}

void NuMtxRotateY(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _00 = m->_00;
    float _10 = m->_10;
    float _20 = m->_20;
    float _30 = m->_30;

    m->_00 = _00 * cosx + m->_02 * sinx;
    m->_02 = m->_02 * cosx - _00 * sinx;
    m->_10 = _10 * cosx + m->_12 * sinx;
    m->_12 = m->_12 * cosx - _10 * sinx;
    m->_20 = _20 * cosx + m->_22 * sinx;
    m->_22 = m->_22 * cosx - _20 * sinx;
    m->_30 = _30 * cosx + m->_32 * sinx;
    m->_32 = m->_32 * cosx - _30 * sinx;
}

void NuMtxPreRotateY(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _00 = m->_00;
    float _01 = m->_01;
    float _02 = m->_02;

    m->_00 = cosx * _00 - m->_20 * sinx;
    m->_01 = cosx * _01 - m->_21 * sinx;
    m->_02 = cosx * _02 - m->_22 * sinx;
    m->_20 = sinx * _00 + m->_20 * cosx;
    m->_21 = sinx * _01 + m->_21 * cosx;
    m->_22 = sinx * _02 + m->_22 * cosx;
}

void NuMtxRotateZ(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _00 = m->_00;
    float _10 = m->_10;
    float _20 = m->_20;
    float _30 = m->_30;

    m->_00 = _00 * cosx - m->_01 * sinx;
    m->_01 = _00 * sinx + m->_01 * cosx;
    m->_10 = _10 * cosx - m->_11 * sinx;
    m->_11 = _10 * sinx + m->_11 * cosx;
    m->_20 = _20 * cosx - m->_21 * sinx;
    m->_21 = _20 * sinx + m->_21 * cosx;
    m->_30 = _30 * cosx - m->_31 * sinx;
    m->_31 = _30 * sinx + m->_31 * cosx;
}

void NuMtxPreRotateZ(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _00 = m->_00;
    float _01 = m->_01;
    float _02 = m->_02;

    m->_00 = cosx * _00 + m->_10 * sinx;
    m->_01 = cosx * _01 + m->_11 * sinx;
    m->_02 = cosx * _02 + m->_12 * sinx;
    m->_10 = m->_10 * cosx - sinx * _00;
    m->_11 = m->_11 * cosx - sinx * _01;
    m->_12 = m->_12 * cosx - sinx * _02;
}

void NuMtxPreRotateY180(NUMTX *m) {
    m->_00 = -m->_00;
    m->_01 = -m->_01;
    m->_02 = -m->_02;
    m->_20 = -m->_20;
    m->_21 = -m->_21;
    m->_22 = -m->_22;
}

void NuMtxPreRotateY180X(NUMTX *m, NUANG a) {
    float cosx = NU_COS_LUT(a);
    float sinx = NU_SIN_LUT(a);
    float _10 = m->_10;
    float _11 = m->_11;
    float _12 = m->_12;

    m->_00 = -m->_00;
    m->_01 = -m->_01;
    m->_02 = -m->_02;
    m->_10 = cosx * _10 - m->_20 * sinx;
    m->_11 = cosx * _11 - m->_21 * sinx;
    m->_12 = cosx * _12 - m->_22 * sinx;
    m->_20 = -(m->_20 * cosx) - sinx * _10;
    m->_21 = -(m->_21 * cosx) - sinx * _11;
    m->_22 = -(m->_22 * cosx) - sinx * _12;
}

void NuMtxPreSkewYX(NUMTX *Mtx, float SkewVal) {
    Mtx->_00 = Mtx->_00 + Mtx->_10 * SkewVal;
    Mtx->_01 = Mtx->_01 + Mtx->_11 * SkewVal;
    Mtx->_02 = Mtx->_02 + Mtx->_12 * SkewVal;
}

void NuMtxTransposeR(NUMTX *m, NUMTX *m0) {
    float t;

    t = m0->_01;
    m->_01 = m0->_10;
    m->_10 = t;
    t = m0->_02;
    m->_02 = m0->_20;
    m->_20 = t;
    t = m0->_12;
    m->_12 = m0->_21;
    m->_21 = t;
    m->_00 = m0->_00;
    m->_11 = m0->_11;
    m->_22 = m0->_22;
    m->_30 = m0->_30;
    m->_31 = m0->_31;
    m->_32 = m0->_32;
    m->_33 = m0->_33;
}

void NuMtxTranspose(NUMTX *m, NUMTX *m0) {
    float t;

    t = m0->_01;
    m->_01 = m0->_10;
    m->_10 = t;
    t = m0->_02;
    m->_02 = m0->_20;
    m->_20 = t;
    t = m0->_03;
    m->_03 = m0->_30;
    m->_30 = t;
    t = m0->_12;
    m->_12 = m0->_21;
    m->_21 = t;
    t = m0->_13;
    m->_13 = m0->_31;
    m->_31 = t;
    t = m0->_23;
    m->_23 = m0->_32;
    m->_32 = t;
    m->_00 = m0->_00;
    m->_11 = m0->_11;
    m->_22 = m0->_22;
    m->_33 = m0->_33;
}

void NuMtxInv(NUMTX *m, NUMTX *m0) {
    float t;

    float tx = -m0->_30;
    float ty = -m0->_31;
    float tz = -m0->_32;
    t = m0->_01;
    m->_01 = m0->_10;
    m->_10 = t;
    t = m0->_02;
    m->_02 = m0->_20;
    m->_20 = t;
    t = m0->_12;
    m->_12 = m0->_21;
    m->_21 = t;
    m->_00 = m0->_00;
    m->_11 = m0->_11;
    m->_22 = m0->_22;
    m->_30 = m->_00 * tx + m->_10 * ty + m->_20 * tz;
    m->_31 = m->_01 * tx + m->_11 * ty + m->_21 * tz;
    m->_32 = m->_02 * tx + m->_12 * ty + m->_22 * tz;
    m->_23 = 0.0f;
    m->_13 = m->_23;
    m->_03 = m->_13;
    m->_33 = 1.0f;
}

void NuMtxInvR(NUMTX *m, NUMTX *m0) {
    float t;

    t = m0->_01;
    m->_01 = m0->_10;
    m->_10 = t;
    t = m0->_02;
    m->_02 = m0->_20;
    m->_20 = t;
    t = m0->_12;
    m->_12 = m0->_21;
    m->_21 = t;
    m->_00 = m0->_00;
    m->_11 = m0->_11;
    m->_22 = m0->_22;
    m->_23 = 0.0f;
    m->_13 = m->_23;
    m->_03 = m->_13;
    m->_32 = 0.0f;
    m->_31 = m->_32;
    m->_30 = m->_31;
    m->_33 = 1.0f;
}

float NuMtxDet3(NUMTX *m) {
    return m->_00 * (m->_11 * m->_22 - m->_12 * m->_21) - m->_01 * (m->_10 * m->_22 - m->_12 * m->_20) +
           m->_02 * (m->_10 * m->_21 - m->_11 * m->_20);
}

void NuMtxLookAtX(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = pnt->x - m->_30;
    v.y = pnt->y - m->_31;
    v.z = pnt->z - m->_32;

    NuVecNorm(&v, &v);
    NuMtxAlignX(m, &v);
}

void NuMtxLookAtY(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = pnt->x - m->_30;
    v.y = pnt->y - m->_31;
    v.z = pnt->z - m->_32;

    NuVecNorm(&v, &v);
    NuMtxAlignY(m, &v);
}

void NuMtxLookAtZ(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = pnt->x - m->_30;
    v.y = pnt->y - m->_31;
    v.z = pnt->z - m->_32;

    NuVecNorm(&v, &v);
    NuMtxAlignZ(m, &v);
}

void NuMtxInvLookAtX(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->_30 - pnt->x;
    v.y = m->_31 - pnt->y;
    v.z = m->_32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignX(m, &v);
}

void NuMtxInvLookAtY(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->_30 - pnt->x;
    v.y = m->_31 - pnt->y;
    v.z = m->_32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignY(m, &v);
}

void NuMtxInvLookAtZ(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->_30 - pnt->x;
    v.y = m->_31 - pnt->y;
    v.z = m->_32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignZ(m, &v);
}

void NuMtxAddR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    m->_00 = m0->_00 + m1->_00;
    m->_01 = m0->_01 + m1->_01;
    m->_02 = m0->_02 + m1->_02;
    m->_03 = 0.0f;
    m->_10 = m0->_10 + m1->_10;
    m->_11 = m0->_11 + m1->_11;
    m->_12 = m0->_12 + m1->_12;
    m->_13 = 0.0f;
    m->_20 = m0->_20 + m1->_20;
    m->_21 = m0->_21 + m1->_21;
    m->_22 = m0->_22 + m1->_22;
    m->_23 = 0.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxSubR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    m->_00 = m0->_00 - m1->_00;
    m->_01 = m0->_01 - m1->_01;
    m->_02 = m0->_02 - m1->_02;
    m->_03 = 0.0f;
    m->_10 = m0->_10 - m1->_10;
    m->_11 = m0->_11 - m1->_11;
    m->_12 = m0->_12 - m1->_12;
    m->_13 = 0.0f;
    m->_20 = m0->_20 - m1->_20;
    m->_21 = m0->_21 - m1->_21;
    m->_22 = m0->_22 - m1->_22;
    m->_23 = 0.0f;
    m->_30 = 0.0f;
    m->_31 = 0.0f;
    m->_32 = 0.0f;
    m->_33 = 1.0f;
}

void NuMtxSkewSymmetric(NUMTX *m, NUVEC *v) {
    m->_00 = 0.0;
    m->_01 = -v->z;
    m->_02 = v->y;
    m->_03 = 0.0;
    m->_10 = v->z;
    m->_11 = 0.0;
    m->_12 = -v->x;
    m->_13 = 0.0;
    m->_20 = -v->y;
    m->_21 = v->x;
    m->_22 = 0.0;
    m->_23 = 0.0;
    m->_30 = 0.0;
    m->_31 = 0.0;
    m->_32 = 0.0;
    m->_33 = 1.0;
}

void NuMtxGetXAxis(NUMTX *m, NUVEC *x) {
    x->x = m->_00;
    x->y = m->_01;
    x->z = m->_02;
}

void NuMtxGetYAxis(NUMTX *m, NUVEC *y) {
    y->x = m->_10;
    y->y = m->_11;
    y->z = m->_12;
}

void NuMtxGetZAxis(NUMTX *m, NUVEC *z) {
    z->x = m->_20;
    z->y = m->_21;
    z->z = m->_22;
}

void NuMtxGetTranslation(NUMTX *m, NUVEC *t) {
    t->x = m->_30;
    t->y = m->_31;
    t->z = m->_32;
}

int NuMtxCompare(NUMTX *a, NUMTX *b) {
    float* aa = &a->_00;
    float* bb = &b->_00;

    for (int i = 0; i < 16; i++) {
        if (*aa < *bb) {
            return -1;
        }
        if (*aa > *bb) {
            return 1;
        }
        aa += 1;
        bb += 1;
    }

    return 0;
}

void NuMtxTruncate24Bit(NUMTX *trunc, NUMTX *mtx) {
    // was this really worth it?
    // struct padding and uints bigger than 4 bytes be damned
    unsigned int *trunc_int = (unsigned int *)trunc;
    unsigned int *mtx_int = (unsigned int *)mtx;

    trunc_int[0] = mtx_int[0] & 0xffffff00;
    trunc_int[1] = mtx_int[1] & 0xffffff00;
    trunc_int[2] = mtx_int[2] & 0xffffff00;
    trunc_int[3] = mtx_int[3] & 0xffffff00;

    trunc_int[4] = mtx_int[4] & 0xffffff00;
    trunc_int[5] = mtx_int[5] & 0xffffff00;
    trunc_int[6] = mtx_int[6] & 0xffffff00;
    trunc_int[7] = mtx_int[7] & 0xffffff00;
    
    trunc_int[8] = mtx_int[8] & 0xffffff00;
    trunc_int[9] = mtx_int[9] & 0xffffff00;
    trunc_int[10] = mtx_int[10] & 0xffffff00;
    trunc_int[11] = mtx_int[11] & 0xffffff00;
    
    trunc_int[12] = mtx_int[12] & 0xffffff00;
    trunc_int[13] = mtx_int[13] & 0xffffff00;
    trunc_int[14] = mtx_int[14] & 0xffffff00;
    trunc_int[15] = mtx_int[15] & 0xffffff00;
}

void NuMtxRotateAng(NUANG ang, float x, float z, float *rx, float *rz) {
    float sinx = NU_SIN_LUT(ang);
    float cosx = NU_COS_LUT(ang);

    *rx = x * cosx - z * sinx;
    *rz = x * sinx + z * cosx;
}

void NuMtxGetEulerXYZ(NUMTX *Mat, NUANG *x, NUANG *y, NUANG *z) {
    NUVEC XVec;
    NUVEC ZVec;

    NuMtxGetXAxis(Mat, &XVec);
    NuMtxGetZAxis(Mat, &ZVec);
    NuMtxVecToEulerXYZ(&XVec, &ZVec, x, y, z);
}

void NuMtxLookAtD3D(NUMTX *mtx, NUVEC *eye, NUVEC *center, NUVEC *up) {
    NUVEC ax;
    NUVEC ay;
    NUVEC az;

    NuVecSub(&az, center, eye);
    NuVecNorm(&az, &az);
    NuVecCross(&ax, up, &az);
    NuVecNorm(&ax, &ax);
    NuVecCross(&ay, &az, &ax);
    NuVecNorm(&ay, &ay);

    mtx->_00 = ax.x;
    mtx->_01 = ay.x;
    mtx->_02 = az.x;
    mtx->_03 = 0.0;
    mtx->_10 = ax.y;
    mtx->_11 = ay.y;
    mtx->_12 = az.y;
    mtx->_13 = 0.0;
    mtx->_20 = ax.z;
    mtx->_21 = ay.z;
    mtx->_22 = az.z;
    mtx->_23 = 0.0;
    mtx->_30 = (-eye->x * ax.x - eye->y * ax.y) - eye->z * ax.z;
    mtx->_31 = (-eye->x * ay.x - eye->y * ay.y) - eye->z * ay.z;
    mtx->_32 = (-eye->x * az.x - eye->y * az.y) - eye->z * az.z;
    mtx->_33 = 1.0;
}

void NuMtxSetPerspectiveD3D(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar) {
}

void NuMtxSetPerspectiveBlend(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar) {
}
void NuMtxSetFrustumD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {
}
void NuMtxSetFrustumBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {
}
void NuMtxSetOrthoD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {
}
void NuMtxSetOrthoBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {
}
void NuMtxGetPerspectiveD3D(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar) {
}
void NuMtxGetPerspectiveBlend(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar) {
}
void NuMtxGetFrustumD3D(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f) {
}
void NuMtxGetFrustumBlend(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f) {
}

void NuMtxSetRotateXYZ(NUMTX *m, NUANGVEC *a) {
}
void NuMtxMul(NUMTX *m, NUMTX *m0, NUMTX *m1) {
}
void NuMtxMulH(NUMTX *m, NUMTX *m0, NUMTX *m1) {
}
void NuMtxMulR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
}
void NuMtxInvRSS(NUMTX *inv, NUMTX *T) {
}
void NuMtxInvRSSH(NUMTX *inv, NUMTX *T) {
}
void NuMtxInvH(NUMTX *mi, NUMTX *m0) {
    int n = 4;
    float m[16];
    int pivot[4];
    int i, j, k;

    m[0] = m0->_00;
    m[1] = m0->_01;
    m[2] = m0->_02;
    m[3] = m0->_03;
    m[4] = m0->_10;
    m[5] = m0->_11;
    m[6] = m0->_12;
    m[7] = m0->_13;
    m[8] = m0->_20;
    m[9] = m0->_21;
    m[10] = m0->_22;
    m[11] = m0->_23;
    m[12] = m0->_30;
    m[13] = m0->_31;
    m[14] = m0->_32;
    m[15] = m0->_33;

    for (i = 0; i < n; i++) {
        pivot[i] = i;
    }

    for (i = 0; i < n; i++) {
        float maxval = 0.0f;
        int maxrow = i;

        for (j = i; j < n; j++) {
            float tmp = NuFabs(m[j * n + i]);
            if (tmp > maxval) {
                maxval = tmp;
                maxrow = j;
            }
        }

        if (NuFdiv(1.0f, maxval) == maxval) {
            return;
        }

        if (maxrow != i) {
            for (k = 0; k < n; k++) {
                float tmp = m[i * n + k];
                m[i * n + k] = m[maxrow * n + k];
                m[maxrow * n + k] = tmp;
            }
            k = pivot[i];
            pivot[i] = pivot[maxrow];
            pivot[maxrow] = k;
        }

        float divisor = m[i * n + i];
        for (j = 0; j < n; j++) {
            if (j != i) {
                float factor = -m[j * n + i] / divisor;
                for (k = 0; k < n; k++) {
                    m[j * n + k] = m[j * n + k] + factor * m[i * n + k];
                }
            }
        }

        for (k = 0; k < n; k++) {
            m[i * n + k] = m[i * n + k] / divisor;
        }
    }

    for (i = n - 1; i >= 0; i--) {
        if (pivot[i] != i) {
            for (k = 0; k < n; k++) {
                float tmp = m[k * n + i];
                m[k * n + i] = m[k * n + pivot[i]];
                m[k * n + pivot[i]] = tmp;
            }
        }
    }

    mi->_00 = m[0];
    mi->_01 = m[1];
    mi->_02 = m[2];
    mi->_03 = m[3];
    mi->_10 = m[4];
    mi->_11 = m[5];
    mi->_12 = m[6];
    mi->_13 = m[7];
    mi->_20 = m[8];
    mi->_21 = m[9];
    mi->_22 = m[10];
    mi->_23 = m[11];
    mi->_30 = m[12];
    mi->_31 = m[13];
    mi->_32 = m[14];
    mi->_33 = m[15];
}

void NuMtxAlignX(NUMTX *m, NUVEC *v) {
    m->_00 = v->x;
    m->_01 = v->y;
    m->_02 = v->z;
    m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
    m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
    m->_22 = m->_00 * m->_11 - m->_01 * m->_10;

    float s = NuFsqrt(m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22);
    s = NuFdiv(1.0f, s);

    m->_20 = m->_20 * s;
    m->_21 = m->_21 * s;
    m->_22 = m->_22 * s;
    m->_10 = m->_21 * m->_02 - m->_22 * m->_01;
    m->_11 = m->_22 * m->_00 - m->_20 * m->_02;
    m->_12 = m->_20 * m->_01 - m->_21 * m->_00;
}

void NuMtxAlignY(NUMTX *m, NUVEC *v) {
    m->_10 = v->x;
    m->_11 = v->y;
    m->_12 = v->z;
    m->_00 = m->_11 * m->_22 - m->_12 * m->_21;
    m->_01 = m->_12 * m->_20 - m->_10 * m->_22;
    m->_02 = m->_10 * m->_21 - m->_11 * m->_20;

    float s = NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
    s = NuFdiv(1.0f, s);

    m->_00 = m->_00 * s;
    m->_01 = m->_01 * s;
    m->_02 = m->_02 * s;
    m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
    m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
    m->_22 = m->_00 * m->_11 - m->_01 * m->_10;
}

void NuMtxAlignZ(NUMTX *m, NUVEC *v) {
    float xLenSq = m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02;
    float yLenSq = m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12;
    float zLenSq = m->_20 * m->_20 + m->_21 * m->_21 + m->_22 * m->_22;

    float len = NuFsqrt(NuFdiv(zLenSq, v->x * v->x + v->y * v->y + v->z * v->z));
    m->_20 = v->x * len;
    m->_21 = v->y * len;
    m->_22 = v->z * len;

    if (NuFabs(NuVecDot(NUMTX_GET_ROW_VEC(m, 1), NUMTX_GET_ROW_VEC(m, 2))) > 0.86602540378f /* cos(30 deg) */) {
        NuVecCross(NUMTX_GET_ROW_VEC(m, 1), NUMTX_GET_ROW_VEC(m, 2), NUMTX_GET_ROW_VEC(m, 0));
        len = NuFsqrt(NuFdiv(yLenSq, m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12));
        m->_10 = m->_10 * len;
        m->_11 = m->_11 * len;
        m->_12 = m->_12 * len;
        NuVecCross(NUMTX_GET_ROW_VEC(m, 0), NUMTX_GET_ROW_VEC(m, 1), NUMTX_GET_ROW_VEC(m, 2));
        len = NuFsqrt(NuFdiv(xLenSq, m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02));
        m->_00 = m->_00 * len;
        m->_01 = m->_01 * len;
        m->_02 = m->_02 * len;
    } else {
        NuVecCross(NUMTX_GET_ROW_VEC(m, 0), NUMTX_GET_ROW_VEC(m, 1), NUMTX_GET_ROW_VEC(m, 2));
        len = NuFsqrt(NuFdiv(xLenSq, m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02));
        m->_00 = m->_00 * len;
        m->_01 = m->_01 * len;
        m->_02 = m->_02 * len;
        NuVecCross(NUMTX_GET_ROW_VEC(m, 1), NUMTX_GET_ROW_VEC(m, 2), NUMTX_GET_ROW_VEC(m, 0));
        len = NuFsqrt(NuFdiv(yLenSq, m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12));
        m->_10 = m->_10 * len;
        m->_11 = m->_11 * len;
        m->_12 = m->_12 * len;
    }
}

void NuMtxOrth(NUMTX *m) {
    float magnitude;
    
    magnitude = 1.0 / NuFsqrt(m->_00 * m->_00 + m->_01 * m->_01 + m->_02 * m->_02);
    m->_00 = m->_00 * magnitude;
    m->_01 = m->_01 * magnitude;
    m->_02 = m->_02 * magnitude;
    
    magnitude = 1.0 / NuFsqrt(m->_10 * m->_10 + m->_11 * m->_11 + m->_12 * m->_12);
    m->_10 = m->_10 * magnitude;
    m->_11 = m->_11 * magnitude;
    m->_12 = m->_12 * magnitude;
    m->_20 = m->_01 * m->_12 - m->_02 * m->_11;
    m->_21 = m->_02 * m->_10 - m->_00 * m->_12;
    m->_22 = m->_00 * m->_11 - m->_01 * m->_10;
    m->_10 = m->_21 * m->_02 - m->_22 * m->_01;
    m->_11 = m->_22 * m->_00 - m->_20 * m->_02;
    m->_12 = m->_20 * m->_01 - m->_21 * m->_00;
}

void NuMtxVecToEulerXYZ(NUVEC *XVec, NUVEC *ZVec, NUANG *x, NUANG *y, NUANG *z) {
    NUVEC XVec2;
    NUVEC ZVec2;

    *z = NuAtan2D(XVec->y, XVec->x);
    NuMtxRotateAng(-*z, XVec->x, XVec->y, &XVec2.x, &XVec2.y);
    XVec2.z = XVec->z;
    NuMtxRotateAng(-*z, ZVec->x, ZVec->y, &ZVec2.x, &ZVec2.y);
    ZVec2.z = ZVec->z;
    *y = -NuAtan2D(XVec2.z, XVec2.x);
    NuMtxRotateAng(*y, ZVec2.x, ZVec2.z, &ZVec2.x, &ZVec2.z);
    *x = -NuAtan2D(ZVec2.y, ZVec2.z);
}

float NuMtxSSE(NUMTX *a, NUMTX *b) {
    // funnily enough this does not use SIMD at all...
    float sse;

    sse = (a->_00 - b->_00) * (a->_00 - b->_00);
    sse += (a->_01 - b->_01) * (a->_01 - b->_01);
    sse += (a->_02 - b->_02) * (a->_02 - b->_02);
    sse += (a->_03 - b->_03) * (a->_03 - b->_03);
    sse += (a->_10 - b->_10) * (a->_10 - b->_10);
    sse += (a->_11 - b->_11) * (a->_11 - b->_11);
    sse += (a->_12 - b->_12) * (a->_12 - b->_12);
    sse += (a->_13 - b->_13) * (a->_13 - b->_13);
    sse += (a->_20 - b->_20) * (a->_20 - b->_20);
    sse += (a->_21 - b->_21) * (a->_21 - b->_21);
    sse += (a->_22 - b->_22) * (a->_22 - b->_22);
    sse += (a->_23 - b->_23) * (a->_23 - b->_23);
    sse += (a->_30 - b->_30) * (a->_30 - b->_30);
    sse += (a->_31 - b->_31) * (a->_31 - b->_31);
    sse += (a->_32 - b->_32) * (a->_32 - b->_32);
    sse += (a->_33 - b->_33) * (a->_33 - b->_33);

    return sse;
}

void NuMtx24BitCorrection(NUMTX *X, NUMTX *mtx) {
    
}