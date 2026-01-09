#ifndef NU2API_SAGA_NUMATH_NUMTX_H
#define NU2API_SAGA_NUMATH_NUMTX_H

#include "nu2api.saga/numath/nuang.h"
#include "nu2api.saga/numath/nuvec.h"

struct numtx_s {
    float _00;
    float _01;
    float _02;
    float _03;
    float _10;
    float _11;
    float _12;
    float _13;
    float _20;
    float _21;
    float _22;
    float _23;
    float _30;
    float _31;
    float _32;
    float _33;
};

typedef struct numtx_s NUMTX;

extern NUMTX numtx_zero;
extern NUMTX numtx_identity;

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Initializes a matrix to the empty matrix
/// @details Initializes the matrix m to the empty matrix.
/// @param m The matrix to initialize to the empty matrix
/// @return void
void NuMtxSetZero(NUMTX *m);

/// @brief Initializes a matrix to the identity matrix
/// @details Initializes the matrix m to the identity matrix.
/// @param m The matrix to initialize to the identity matrix
/// @return void
void NuMtxSetIdentity(NUMTX *m);

/// @brief Initializes a matrix to a translation matrix
/// @details Initializes the matrix m to a translation matrix with the translation vector t.
/// @param m The matrix to initialize to a translation matrix
/// @param t The translation vector to initialize the matrix to
/// @return void
void NuMtxSetTranslation(NUMTX *m, NUVEC *t);

/// @brief Initializes a matrix to a translation matrix with the negative translation vector
/// @details Initializes the matrix m to a translation matrix with the negative translation vector t.
/// @param m The matrix to initialize to a translation matrix with the negative translation vector
/// @param t The translation vector to initialize the matrix to with the negative translation vector
/// @return void
void NuMtxSetTranslationNeg(NUMTX *m, NUVEC *t);

/// @brief Initializes a matrix to a scale matrix
/// @details Initializes the matrix m to a scale matrix with the scale vector s.
/// @param m The matrix to initialize to a scale matrix
/// @param s The scale vector to initialize the matrix to
/// @return void
void NuMtxSetScale(NUMTX *m, NUVEC *s);

/// @brief Initializes a matrix to a rotation matrix around the x-axis
/// @details Initializes the matrix m to a rotation matrix around the x-axis with the angle a.
/// @param m The matrix to initialize to a rotation matrix around the x-axis
/// @param a The angle to initialize the matrix to with the rotation around the x-axis
/// @return void
void NuMtxSetRotationX(NUMTX *m, NUANG a);

/// @brief Initializes a matrix to a rotation matrix around the y-axis
/// @details Initializes the matrix m to a rotation matrix around the y-axis with the angle a.
/// @param m The matrix to initialize to a rotation matrix around the y-axis
/// @param a The angle to initialize the matrix to with the rotation around the y-axis
/// @return void
void NuMtxSetRotationY(NUMTX *m, NUANG a);

/// @brief Initializes a matrix to a rotation matrix around the z-axis
/// @details Initializes the matrix m to a rotation matrix around the z-axis with the angle a.
/// @param m The matrix to initialize to a rotation matrix around the z-axis
/// @param a The angle to initialize the matrix to with the rotation around the z-axis
/// @return void
void NuMtxSetRotationZ(NUMTX *m, NUANG a);

/// @brief Initializes a matrix to a rotation matrix around an axis
/// @details Initializes the matrix m to a rotation matrix around the axis ax with the angle a.
/// @param m The matrix to initialize to a rotation matrix around the axis
/// @param ang The angle to initialize the matrix to with the rotation around the axis
/// @param ax The axis to initialize the matrix to with the rotation around the axis
/// @return void
void NuMtxSetRotationAxis(NUMTX *m, NUANG ang, NUVEC *ax);

/// @brief Translates a matrix by a translation vector
/// @details Translates the matrix m by the translation vector t.
/// @param m The matrix to translate
/// @param t The translation vector to translate the matrix by
/// @return void
void NuMtxTranslate(NUMTX *m, NUVEC *t);

/// @brief Translates a matrix opposite to the direction of the translation vector
/// @details Translates the matrix m opposite to the direction of the translation vector t.
/// @param m The matrix to translate
/// @param t The translation vector to translate the matrix by opposite to the direction of the translation vector
/// @return void
void NuMtxTranslateNeg(NUMTX *m, NUVEC *t);

void NuMtxPreTranslate(NUMTX *m, NUVEC *t);
void NuMtxPreTranslateX(NUMTX *m, float tx);
void NuMtxPreTranslateNeg(NUMTX *m, NUVEC *t);
void NuMtxScale(NUMTX *m, NUVEC *s);
void NuMtxScaleU(NUMTX *m, float s);
void NuMtxPreScaleU(NUMTX *m, float s);
NUVEC NuMtxGetScale(NUMTX *m);
void NuMtxPreScale(NUMTX *m, NUVEC *s);
void NuMtxPreScaleX(NUMTX *m, float ScaleX);
void NuMtxRotateX(NUMTX *m, NUANG a);
void NuMtxPreRotateX(NUMTX *m, NUANG a);
void NuMtxRotateY(NUMTX *m, NUANG a);
void NuMtxPreRotateY(NUMTX *m, NUANG a);
void NuMtxRotateZ(NUMTX *m, NUANG a);
void NuMtxPreRotateZ(NUMTX *m, NUANG a);
void NuMtxPreRotateY180(NUMTX *m);
void NuMtxPreRotateY180X(NUMTX *m, NUANG a);
void NuMtxPreSkewYX(NUMTX *Mtx, float SkewVal);
void NuMtxTransposeR(NUMTX *m, NUMTX *m0);
void NuMtxTranspose(NUMTX *m, NUMTX *m0);
void NuMtxInv(NUMTX *m, NUMTX *m0);
void NuMtxInvR(NUMTX *m, NUMTX *m0);
float NuMtxDet3(NUMTX *m);
void NuMtxLookAtX(NUMTX *m, NUVEC *pnt);
void NuMtxLookAtY(NUMTX *m, NUVEC *pnt);
void NuMtxLookAtZ(NUMTX *m, NUVEC *pnt);
void NuMtxInvLookAtX(NUMTX *m, NUVEC *pnt);
void NuMtxInvLookAtY(NUMTX *m, NUVEC *pnt);
void NuMtxInvLookAtZ(NUMTX *m, NUVEC *pnt);
void NuMtxAddR(NUMTX *m, NUMTX *m0, NUMTX *m1);
void NuMtxSubR(NUMTX *m, NUMTX *m0, NUMTX *m1);
void NuMtxSkewSymmetric(NUMTX *m, NUVEC *v);
void NuMtxGetXAxis(NUMTX *m, NUVEC *x);
void NuMtxGetYAxis(NUMTX *m, NUVEC *y);
void NuMtxGetZAxis(NUMTX *m, NUVEC *z);
void NuMtxGetTranslation(NUMTX *m, NUVEC *t);
int NuMtxCompare(NUMTX *a, NUMTX *b);
void NuMtxTruncate24Bit(NUMTX *trunc, NUMTX *mtx);
void NuMtxRotateAng(int ang, float x, float z, float *rx, float *rz);
void NuMtxGetEulerXYZ(NUMTX *Mat, NUANG *x, NUANG *y, NUANG *z);
void NuMtxLookAtD3D(NUMTX *mtx, NUVEC *eye, NUVEC *center, NUVEC *up);
void NuMtxSetPerspectiveD3D(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar);
void NuMtxSetPerspectiveBlend(NUMTX *mtx, float fovy, float aspect, float zNear, float zFar);
void NuMtxSetFrustumD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f);
void NuMtxSetFrustumBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f);
void NuMtxSetOrthoD3D(NUMTX *mtx, float l, float r, float b, float t, float n, float f);
void NuMtxSetOrthoBlend(NUMTX *mtx, float l, float r, float b, float t, float n, float f);
void NuMtxGetPerspectiveD3D(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar);
void NuMtxGetPerspectiveBlend(NUMTX *mtx, float *fovy, float *aspect, float *zNear, float *zFar);
void NuMtxGetFrustumD3D(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f);
void NuMtxGetFrustumBlend(NUMTX *mtx, float *l, float *r, float *b, float *t, float *n, float *f);

#ifdef __cplusplus
}
#endif

#endif // NU2API_SAGA_NUMATH_NUMTX_H