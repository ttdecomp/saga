#pragma once

#include "nu2api/nucore/common.h"

typedef struct numtx_s NUMTX;

/// @brief A 3-dimensional vector
typedef struct nuvec_s {
    /// @brief The x component of the vector
    f32 x;

    /// @brief The y component of the vector
    f32 y;

    /// @brief The z component of the vector
    f32 z;
} NUVEC;

extern NUVEC v000;
extern NUVEC v100;
extern NUVEC v010;
extern NUVEC v001;
extern NUVEC v111;

#ifdef __cplusplus
extern "C" {
#endif
    /// @brief Negate a vector
    /// @details Negates the vector v0 and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The vector to negate
    /// @return void
    void NuVecNeg(NUVEC *v, NUVEC *v0);

    /// @brief Add two vectors
    /// @details Adds the vectors v0 and v1 and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The first vector to add
    /// @param v1 The second vector to add
    /// @return void
    void NuVecAdd(NUVEC *v, NUVEC *v0, NUVEC *v1);

    /// @brief Subtract two vectors
    /// @details Subtracts the vector v1 from v0 and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The vector to subtract from
    /// @param v1 The vector to subtract
    /// @return void
    void NuVecSub(NUVEC *v, NUVEC *v0, NUVEC *v1);

    /// @brief Scale a vector by a scalar
    /// @details Scales the vector v0 by the scalar k and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The vector to scale
    /// @param k The scalar to scale the vector by
    /// @return void
    void NuVecScale(NUVEC *v, NUVEC *v0, f32 k);

    /// @brief Add a vector scaled by a scalar to another vector
    /// @details Adds the vector v1 scaled by the scalar k to v0 and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The vector to add to
    /// @param v1 The vector to scale and add
    /// @param k The scalar to scale the vector by
    /// @return void
    void NuVecAddScale(NUVEC *v, NUVEC *v0, NUVEC *v1, f32 k);

    /// @brief Scale a vector by a scalar and add the result to another vector
    /// @details Scales the vector v0 by the scalar k and adds the result to v and stores the result in v.
    /// @param v The vector to add the result to
    /// @param v0 The vector to scale
    /// @param k The scalar to scale the vector by
    /// @return void
    void NuVecScaleAccum(NUVEC *v, NUVEC *v0, f32 k);

    /// @brief Inverse scale a vector by a scalar
    /// @details Inverse scales the vector v0 by the scalar k and stores the result in v. If k is 0, v0 is scaled by 0.
    /// @param v The vector to store the result in
    /// @param v0 The vector to inverse scale
    /// @param k The scalar to inverse scale the vector by
    /// @return void
    void NuVecInvScale(NUVEC *v, NUVEC *v0, f32 k);

    /// @brief Compute the cross product of two vectors
    /// @details Computes the cross product of the vectors v0 and v1 and stores the result in v.
    /// @param v The vector to store the result in
    /// @param v0 The first vector to cross
    /// @param v1 The second vector to cross
    /// @return void
    void NuVecCross(NUVEC *v, NUVEC *v0, NUVEC *v1);

    /// @brief Compute the cross product of the two vectors formed from the difference between two points and a base
    /// point
    /// @details Computes the cross product of the two vectors formed from the differences between v0 and basepnt and v1
    /// and basepnt and stores the result in v.
    /// @param v The vector to store the result in
    /// @param basepnt The base point to relative to
    /// @param v0 The first point to form a vector from
    /// @param v1 The second point to form a vector from
    /// @return void
    void NuVecCrossRel(NUVEC *v, NUVEC *basepnt, NUVEC *v0, NUVEC *v1);

    /// @brief Compute the dot product of two vectors
    /// @details Computes the dot product of the vectors v0 and v1 and returns the result.
    /// @param v0 The first vector to dot
    /// @param v1 The second vector to dot
    /// @return The dot product of the two vectors
    f32 NuVecDot(NUVEC *v0, NUVEC *v1);

    /// @brief Compute a vector formed of the maximum component from each vector
    /// @details Computes a vector formed of the maximum component from each vector v0 and v1 and stores the result in
    /// v.
    /// @param v The vector to store the result in
    /// @param v0 The first vector
    /// @param v1 The second vector
    /// @return void
    void NuVecMax(NUVEC *v, NUVEC *v0, NUVEC *v1);

    /// @brief Compute a vector formed of the minimum component from each vector
    /// @details Computes a vector formed of the minimum component from each vector v0 and v1 and stores the result in
    /// v.
    /// @param v The vector to store the result in
    /// @param v0 The first vector
    /// @param v1 The second vector
    /// @return void
    void NuVecMin(NUVEC *v, NUVEC *v0, NUVEC *v1);

    /// @brief Compute the magnitude squared of a vector
    /// @details Computes the magnitude squared of the vector v0 and returns the result.
    /// @param v0 The vector to compute the magnitude squared of
    /// @return The magnitude squared of the vector
    f32 NuVecMagSqr(NUVEC *v0);

    /// @brief Compute the magnitude of a vector
    /// @details Computes the magnitude of the vector v0 and returns the result.
    /// @param v0 The vector to compute the magnitude of
    /// @return The magnitude of the vector
    f32 NuVecMag(NUVEC *v0);

    /// @brief Compute the magnitude of only the x and z components of a vector
    /// @details Computes the magnitude of only the x and z components of the vector v0 and returns the result.
    /// @param v0 The vector to compute the magnitude of
    /// @return The magnitude of the x and z components of the vector
    f32 NuVecMagXZ(NUVEC *v0);

    /// @brief Compute the normalized vector of a vector and return the initial length
    /// @details Computes the normalized vector of the vector v0, stores the result in v and returns the initial length
    /// of the vector.
    /// @param v The vector to store the result in
    /// @param v0 The vector to normalize
    /// @return The initial length of the vector
    f32 NuVecNorm(NUVEC *v, NUVEC *v0);

    /// @brief Compute the surface normal of a triangle
    /// @details Computes the surface normal of the triangle formed by the vectors v0, v1 and v2 and stores the result
    /// in v.
    /// @param v The vector to store the result in
    /// @param v0 The first vertex of the triangle
    /// @param v1 The second vertex of the triangle
    /// @param v2 The third vertex of the triangle
    /// @return void
    void NuVecSurfaceNormal(NUVEC *v, NUVEC *v0, NUVEC *v1, NUVEC *v2);

    /// @brief Compute the distance between two point vectors
    /// @details Computes the distance between the two point vectors v0 and v1 and returns the result. Optionally uses
    /// the vector d as storage for the directional vector between the two points.
    /// @param v0 The first point vector
    /// @param v1 The second point vector
    /// @param d The vector to store the directional vector in, can be NULL
    /// @return The distance between the two point vectors
    f32 NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *d);

    /// @brief Compute the distance squared between two point vectors
    /// @details Computes the distance squared between the two point vectors v0 and v1 and returns the result.
    /// Optionally uses the vector d as storage for the directional vector between the two points.
    /// @param v0 The first point vector
    /// @param v1 The second point vector
    /// @param d The vector to store the directional vector in, can be NULL
    /// @return The distance squared between the two point vectors
    f32 NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d);

    /// @brief Compute the distance between only the x and z components of two point vectors
    /// @details Computes the distance between only the x and z components of the two point vectors v0 and v1 and
    /// returns the result. Optionally uses the vector d as storage for the directional vector between the two points.
    /// @param v0 The first point vector
    /// @param v1 The second point vector
    /// @param d The vector to store the directional vector in, can be NULL
    /// @return The distance between the two point vectors
    f32 NuVecXZDist(NUVEC *v0, NUVEC *v1, NUVEC *d);

    /// @brief Compute the distance squared between only the x and z components of two point vectors
    /// @details Computes the distance squared between only the x and z components of the two point vectors v0 and v1
    /// and returns the result. Optionally uses the vector d as storage for the directional vector between the two
    /// points.
    /// @param v0 The first point vector
    /// @param v1 The second point vector
    /// @param d The vector to store the directional vector in, can be NULL
    /// @return The distance squared between the two point vectors
    f32 NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d);

    /// @brief Compute the linear interpolation between two point vectors at a given ratio
    /// @details Computes the linear interpolation between the two point vectors v0 and v1 at the ratio t and stores the
    /// result in vt.
    /// @param vt The vector to store the result in
    /// @param v1 The second point vector
    /// @param v0 The first point vector
    /// @param t The ratio to interpolate at
    /// @return void
    void NuVecLerp(NUVEC *vt, NUVEC *v1, NUVEC *v0, f32 t);

    /// @brief Compare two vectors with a tolerance
    /// @details Compares the two vectors a and b with a tolerance and returns 1 if they are equal, 0 otherwise.
    /// @param a The first vector to compare
    /// @param b The second vector to compare
    /// @param tolerance The tolerance to compare the vectors with
    /// @return 1 if the vectors are equal, 0 otherwise
    int NuVecCompareTolerance(NUVEC *a, NUVEC *b, f32 tolerance);

    void NuVecMtxTransform(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecMtxRotate(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecMtxScale(NUVEC *out, NUVEC *v, NUMTX *m);
    void NuVecMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *m);

    i32 NuLineLineIntersect(NUVEC *pnt0, NUVEC *v0, NUVEC *pnt1, NUVEC *v1, f32 *s, f32 *t);
#ifdef __cplusplus
}

void NuVecMtxTransformBlock(NUVEC *out, NUVEC *v, NUMTX *m, i32 count);

#endif
