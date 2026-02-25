#pragma once

#include <math.h>

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    /// @brief Compute the absolute value of a float
    /// @details Computes the absolute value of the float f and returns the result.
    /// @param f The float to compute the absolute value of
    /// @return The absolute value of the float
    static f32 NuFabs(f32 f) {
        return fabsf(f);
    }

    /// @brief Compute the negative absolute value of a float
    /// @details Computes the negative absolute value of the float f and returns the result.
    /// @param f The float to compute the negative absolute value of
    /// @return The negative absolute value of the float
    f32 NuFnabs(f32 f);

    /// @brief Compute the negation of a float
    /// @details Computes the negation of the float v and returns the result.
    /// @param v The float to compute the negation of
    /// @return The negation of the float v
    f32 NuFneg(f32 v);

    /// @brief Compute the sign of a float
    /// @details Computes the sign of the float v and returns the result as a unit float.
    /// @param v The float to compute the sign of
    /// @return The sign of the float v as a unit float (-1.0f or 1.0f)
    f32 NuFsign(f32 f);

    /// @brief Compute the square root of a float
    /// @details Computes the square root of the float f and returns the result.
    /// @param f The float to compute the square root of
    /// @return The square root of the float
    f32 NuFsqrt(f32 f);

    /// @brief Returns the larger of two floating point values.
    f32 NuFmax(f32 a, f32 b);

    /// @brief Returns the smaller of two floating point values.
    f32 NuFmin(f32 a, f32 b);

    /// @brief Compute the division of two floats
    /// @details Computes the division of the float a divided by b and returns the result. Returns 0 if b is 0.
    /// @param a The float to divide
    /// @param b The float to divide by
    /// @return The division of the float a and b
    static f32 NuFdiv(f32 a, f32 b) {
        if (a == 0.0f || b == 0.0f) {
            return 0.0f;
        }

        return a / b;
    }

    f32 NuFmod(f32 a, f32 b);

    f32 NuFloor(f32 f);
#ifdef __cplusplus
}
#endif
