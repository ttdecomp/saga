#include "nu2api/numath/nutrig.h"

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nufloat.h"

#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 -0.166667f
#define NEG_3_OVER_40 -0.075f
#define NEG_5_OVER_112 -0.0446429f
#define NEG_35_OVER_1152 -0.0303819f
#define MAX_SHORT_OVER_PI 10430.4f

static short NuASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    abs = NuFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

#define ASIN_TERM (unknown_b * unknown_c * unknown_a)
    return (((NEG_3_OVER_40 * (ASIN_TERM * ASIN_TERM)) * (ASIN_TERM * (ASIN_TERM * ASIN_TERM))) +
            ((unknown_d * PI_OVER_4 - ASIN_TERM) + ((NEG_1_OVER_6 * ASIN_TERM) * (ASIN_TERM * ASIN_TERM))) +
            ((NEG_5_OVER_112 * (ASIN_TERM * (ASIN_TERM * ASIN_TERM))) *
             ((ASIN_TERM * ASIN_TERM) * (ASIN_TERM * ASIN_TERM))) +
            ((NEG_35_OVER_1152 * ((ASIN_TERM * ASIN_TERM) * (ASIN_TERM * ASIN_TERM))) *
             ((ASIN_TERM * ASIN_TERM) * (ASIN_TERM * (ASIN_TERM * ASIN_TERM))))) *
           MAX_SHORT_OVER_PI;
#undef ASIN_TERM
}

short NuACos(f32 cos) {
    return 0x4000 - NuASin(cos);
}
