#include "nu2api/numath/nutrig.h"

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nufloat.h"

#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 -0.166667f
#define NEG_3_OVER_40 -0.075f
#define NEG_5_OVER_112 -0.0446429f
#define NEG_35_OVER_1152 -0.0303819f
#define MAX_SHORT_OVER_PI 10430.4f

#define POW3(x) (x) * (x) * (x)
#define POW5(x) POW3(x) * (x) * (x)
#define POW7(x) POW5(x) * (x) * (x)
#define POW9(x) POW7(x) * (x) * (x)

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

    unknown_b = CLAMP((abs - 0.70710677f) * 3.40282e+38f, -1.0f, 1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + sin;

    return (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
            POW3(unknown_b * unknown_c * unknown_a) * NEG_1_OVER_6 +
            POW5(unknown_b * unknown_c * unknown_a) * NEG_3_OVER_40 +
            POW7(unknown_b * unknown_c * unknown_a) * NEG_5_OVER_112 +
            POW9(unknown_b * unknown_c * unknown_a) * NEG_35_OVER_1152) *
           MAX_SHORT_OVER_PI;
}

short NuACos(f32 cos) {
    return 0x4000 - NuASin(cos);
}

NUANG NuAngAdd(NUANG a, NUANG b) {
    return a + b;
}

NUANG NuAngSub(NUANG a, NUANG b) {
    return a - b;
}

NUANG NuAng2AltSol(NUANG theta) {
    return theta + NUANG_180DEG;
}

f32 NuAtanf(f32 x) {
    return atanf(x);
}

NUANG NuAtani(f32 x) {
    return (NUANG)(NuAtanf(x) * MAX_SHORT_OVER_PI);
}

NUANG NuAtan2DA(f32 dx, f32 dy) {
    return (NUANG)(NuAtan2(dx, dy) * MAX_SHORT_OVER_PI);
}

f32 NuAtan2DAF(f32 dx, f32 dy) {
    return NuAtan2(dx, dy);
}

f32 NuSinf(NUANG ang) {
    return NU_SIN_LUT(ang);
}

f32 NuCosf(NUANG ang) {
    return NU_COS_LUT(ang);
}
