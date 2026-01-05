#ifndef NU2API_SAGA_NUMATH_NUVEC_H
#define NU2API_SAGA_NUMATH_NUVEC_H

/// @brief A 3-dimensional vector
typedef struct {
    /// @brief The x component of the vector
    float x;

    /// @brief The y component of the vector
    float y;

    /// @brief The z component of the vector
    float z;
} nuvec_s;

typedef nuvec_s NUVEC;

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
void NuVecScale(NUVEC *v, NUVEC *v0, float k);

/// @brief Add a vector scaled by a scalar to another vector
/// @details Adds the vector v1 scaled by the scalar k to v0 and stores the result in v.
/// @param v The vector to store the result in
/// @param v0 The vector to add to
/// @param v1 The vector to scale and add
/// @param k The scalar to scale the vector by
/// @return void
void NuVecAddScale(NUVEC *v, NUVEC *v0, NUVEC *v1, float k);

/// @brief Scale a vector by a scalar and add the result to another vector
/// @details Scales the vector v0 by the scalar k and adds the result to v and stores the result in v.
/// @param v The vector to add the result to
/// @param v0 The vector to scale
/// @param k The scalar to scale the vector by
/// @return void
void NuVecScaleAccum(NUVEC *v, NUVEC *v0, float k);

/// @brief Inverse scale a vector by a scalar
/// @details Inverse scales the vector v0 by the scalar k and stores the result in v. If k is 0, v0 is scaled by 0.
/// @param v The vector to store the result in
/// @param v0 The vector to inverse scale
/// @param k The scalar to inverse scale the vector by
/// @return void
void NuVecInvScale(NUVEC *v, NUVEC *v0, float k);

/// @brief Compute the cross product of two vectors
/// @details Computes the cross product of the vectors v0 and v1 and stores the result in v.
/// @param v The vector to store the result in
/// @param v0 The first vector to cross
/// @param v1 The second vector to cross
/// @return void
void NuVecCross(NUVEC *v, NUVEC *v0, NUVEC *v1);

/// @brief Compute the cross product of the two vectors formed from the difference between two points and a base point
/// @details Computes the cross product of the two vectors formed from the differences between v0 and basepnt and v1 and
/// basepnt and stores the result in v.
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
float NuVecDot(NUVEC *v0, NUVEC *v1);

/// @brief Compute a vector formed of the maximum component from each vector
/// @details Computes a vector formed of the maximum component from each vector v0 and v1 and stores the result in v.
/// @param v The vector to store the result in
/// @param v0 The first vector
/// @param v1 The second vector
/// @return void
void NuVecMax(NUVEC *v, NUVEC *v0, NUVEC *v1);

/// @brief Compute a vector formed of the minimum component from each vector
/// @details Computes a vector formed of the minimum component from each vector v0 and v1 and stores the result in v.
/// @param v The vector to store the result in
/// @param v0 The first vector
/// @param v1 The second vector
/// @return void
void NuVecMin(NUVEC *v, NUVEC *v0, NUVEC *v1);

/// @brief Compute the magnitude squared of a vector
/// @details Computes the magnitude squared of the vector v0 and returns the result.
/// @param v0 The vector to compute the magnitude squared of
/// @return The magnitude squared of the vector
float NuVecMagSqr(NUVEC *v0);

/// @brief Compute the magnitude of a vector
/// @details Computes the magnitude of the vector v0 and returns the result.
/// @param v0 The vector to compute the magnitude of
/// @return The magnitude of the vector
// float NuVecMag(NUVEC* v0);

#endif // NU2API_SAGA_NUMATH_NUVEC_H