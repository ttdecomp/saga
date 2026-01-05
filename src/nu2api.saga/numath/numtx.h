#ifndef NU2API_SAGA_NUMATH_NUMTX_H
#define NU2API_SAGA_NUMATH_NUMTX_H

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

/// @brief Initializes a matrix to the empty matrix
/// @details Initializes the matrix m to the empty matrix.
/// @param m The matrix to initialize to the empty matrix
/// @return void
void NuMtxSetZero(NUMTX* m);

/// @brief Initializes a matrix to the identity matrix
/// @details Initializes the matrix m to the identity matrix.
/// @param m The matrix to initialize to the identity matrix
/// @return void
void NuMtxSetIdentity(NUMTX* m);

/// @brief Initializes a matrix to a translation matrix
/// @details Initializes the matrix m to a translation matrix with the translation vector t.
/// @param m The matrix to initialize to a translation matrix
/// @param t The translation vector to initialize the matrix to
/// @return void
void NuMtxSetTranslation(NUMTX* m, NUVEC* t);

/// @brief Initializes a matrix to a translation matrix with the negative translation vector
/// @details Initializes the matrix m to a translation matrix with the negative translation vector t.
/// @param m The matrix to initialize to a translation matrix with the negative translation vector
/// @param t The translation vector to initialize the matrix to with the negative translation vector
/// @return void
void NuMtxSetTranslationNeg(NUMTX* m, NUVEC* t);

/// @brief Initializes a matrix to a scale matrix
/// @details Initializes the matrix m to a scale matrix with the scale vector s.
/// @param m The matrix to initialize to a scale matrix
/// @param s The scale vector to initialize the matrix to
/// @return void
void NuMtxSetScale(NUMTX* m, NUVEC* s);

#endif // NU2API_SAGA_NUMATH_NUMTX_H