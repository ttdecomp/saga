#include "nu2api.saga/numath/numtx.h"

#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numath/nutrig.h"

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

void NuMtxInv(NUMTX *m, NUMTX *m0) {}
void NuMtxInvR(NUMTX *m, NUMTX *m0) {}
float NuMtxDet3(NUMTX *m) {}
void NuMtxLookAtX(NUMTX *m, NUVEC *pnt) {}
void NuMtxLookAtY(NUMTX *m, NUVEC *pnt) {}
void NuMtxLookAtZ(NUMTX *m, NUVEC *pnt) {}
void NuMtxInvLookAtX(NUMTX *m, NUVEC *pnt) {}
void NuMtxInvLookAtY(NUMTX *m, NUVEC *pnt) {}
void NuMtxInvLookAtZ(NUMTX *m, NUVEC *pnt) {}
void NuMtxAddR(NUMTX *m, NUMTX *m0, NUMTX *m1) {}
void NuMtxSubR(NUMTX *m, NUMTX *m0, NUMTX *m1) {}
void NuMtxSkewSymmetric(NUMTX *m, NUVEC *v) {}
void NuMtxGetXAxis(NUMTX *m, NUVEC *x) {}
void NuMtxGetYAxis(NUMTX *m, NUVEC *y) {}
void NuMtxGetZAxis(NUMTX *m, NUVEC *z) {}
void NuMtxGetTranslation(NUMTX *m, NUVEC *t) {}
int NuMtxCompare(NUMTX *a, NUMTX *b) {}
void NuMtxTruncate24Bit(NUMTX *trunc, NUMTX *mtx) {}
void NuMtxRotateAng(int ang, float x, float z, float *rx, float *rz) {}
void NuMtxGetEulerXYZ(NUMTX *Mat, NUANG *x, NUANG *y, NUANG *z) {}
void NuMtxLookAtD3D(NUMTX *mtx, NUVEC *eye, NUVEC *center, NUVEC *up) {}
void NuMtxSetPerspectiveD3D(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar) {}
void NuMtxSetPerspectiveBlend(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar) {}
void NuMtxSetFrustumD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {}
void NuMtxSetFrustumBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {}
void NuMtxSetOrthoD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {}
void NuMtxSetOrthoBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f) {}
void NuMtxGetPerspectiveD3D(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar) {}
void NuMtxGetPerspectiveBlend(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar) {}
void NuMtxGetFrustumD3D(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f) {}
void NuMtxGetFrustumBlend(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f) {}