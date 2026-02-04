#include "nu2api.saga/numath/nutrig.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/numath/nufloat.h"

#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 -0.166667f
#define NEG_3_OVER_40 -0.075f
#define NEG_5_OVER_112 -0.0446429f
#define NEG_35_OVER_1152 -0.0303819f
#define MAX_SHORT_OVER_PI 10430.4f

static short NuASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 value;
    f32 something_else;
    f32 transformed_sin;

    abs = NuFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    value = MIN(sqrt, abs);

    if ((abs - 0.70710677f) * 3.40282e+38f < 1.0f && (abs - 0.70710677f) * 3.40282e+38f >= -1.0f) {
        something_else = -1.0f;
    } else if ((abs - 0.70710677f) * 3.40282e+38f > 1.0f) {
        something_else = 1.0f;
    } else {
        something_else = (abs - 0.70710677f) * 3.40282e+38f;
    }

    if (sin * 3.40282e+38f < 1.0f) {
        transformed_sin = sin * 3.40282e+38f;
    } else {
        transformed_sin = 1.0f;
    }

    if (sin < -1.0f) {
        transformed_sin = -1.0f;
    }

    return (PI_OVER_4 * (something_else * transformed_sin + sin) - (something_else * transformed_sin * value) +
            (NEG_1_OVER_6 * ((something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                             (something_else * transformed_sin * value))) +
            (NEG_3_OVER_40 * ((something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                              (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                              (something_else * transformed_sin * value))) +
            (NEG_5_OVER_112 * ((something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                               (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                               (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
                               (something_else * transformed_sin * value))) +
            (NEG_35_OVER_1152 *
             ((something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
              (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
              (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
              (something_else * transformed_sin * value) * (something_else * transformed_sin * value) *
              (something_else * transformed_sin * value)))) *
           MAX_SHORT_OVER_PI;
}

short NuACos(f32 cos) {
    return 0x4000 - NuASin(cos);
}
