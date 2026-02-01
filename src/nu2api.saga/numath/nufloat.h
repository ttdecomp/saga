#pragma once

#include <math.h>

// /// @brief The epsilon for float equivalence in NuEquiv
// extern float fetol;

#ifdef __cplusplus
extern "C" {
#endif
    /// @brief Compute the absolute value of a float
    /// @details Computes the absolute value of the float f and returns the result.
    /// @param f The float to compute the absolute value of
    /// @return The absolute value of the float
    static float NuFabs(float f) {
        return fabsf(f);
    }

    /// @brief Compute the negative absolute value of a float
    /// @details Computes the negative absolute value of the float f and returns the result.
    /// @param f The float to compute the negative absolute value of
    /// @return The negative absolute value of the float
    float NuFnabs(float f);

    /// @brief Compute the negation of a float
    /// @details Computes the negation of the float v and returns the result.
    /// @param v The float to compute the negation of
    /// @return The negation of the float v
    float NuFneg(float v);

    /// @brief Compute the sign of a float
    /// @details Computes the sign of the float v and returns the result as a unit float.
    /// @param v The float to compute the sign of
    /// @return The sign of the float v as a unit float (-1.0f or 1.0f)
    float NuFsign(float f);

    // /// @brief Set the epsilon for float equivalence
    // /// @details Sets the epsilon for float equivalence to the value of the float e.
    // /// @param e The epsilon to set
    // /// @return void
    // void NuEquivTollerance(float f);

    // /// @brief Compute if two floats are equivalent
    // /// @details Computes if the floats f1 and f2 are equivalent within a small epsilon and returns the result.
    // /// @param f1 The first float to compare
    // /// @param f2 The second float to compare
    // /// @return 1 if the floats are equivalent, 0 otherwise
    // int NuEquiv(float f1, float f2);

    /// @brief Compute the square root of a float
    /// @details Computes the square root of the float f and returns the result.
    /// @param f The float to compute the square root of
    /// @return The square root of the float
    float NuFsqrt(float f);

    /// @brief Compute the division of two floats
    /// @details Computes the division of the float a divided by b and returns the result. Returns 0 if b is 0.
    /// @param a The float to divide
    /// @param b The float to divide by
    /// @return The division of the float a and b
    static float NuFdiv(float a, float b) {
        if (a == 0.0f || b == 0.0f) {
            return 0.0f;
        }

        return a / b;
    }

    float NuFmod(float a, float b);
#ifdef __cplusplus
}
#endif
