#include "nu2api.saga/numath/numtx.h"

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

void NuMtxScale(NUMTX *m, NUVEC *s) {}
void NuMtxScaleU(NUMTX *m, float s) {}
void NuMtxPreScaleU(NUMTX *m, float s) {}
NUVEC NuMtxGetScale(NUMTX *m) {}
void NuMtxPreScale(NUMTX *m, NUVEC *s) {}
void NuMtxPreScaleX(NUMTX *m, float ScaleX) {}
void NuMtxRotateX(NUMTX *m, NUANG a) {}
void NuMtxPreRotateX(NUMTX *m, NUANG a) {}
void NuMtxRotateY(NUMTX *m, NUANG a) {}
void NuMtxPreRotateY(NUMTX *m, NUANG a) {}
void NuMtxRotateZ(NUMTX *m, NUANG a) {}
void NuMtxPreRotateZ(NUMTX *m, NUANG a) {}
void NuMtxPreRotateY180(NUMTX *m) {}
void NuMtxPreRotateY180X(NUMTX *m, NUANG a) {}
void NuMtxPreSkewYX(NUMTX *Mtx, float SkewVal) {}
void NuMtxTransposeR(NUMTX *m, NUMTX *m0) {}
void NuMtxTranspose(NUMTX *m, NUMTX *m0) {}
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