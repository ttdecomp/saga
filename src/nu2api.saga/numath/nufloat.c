#include "nu2api.saga/numath/nufloat.h"

#include "nu2api.saga/nucore/common.h"

f32 NuFnabs(f32 f) {
    f32 rv;

    *(int *)&rv = *(int *)&f | 0x80000000;
    return rv;
}

f32 NuFneg(f32 v) {
    f32 rv;

    *(int *)&rv = *(int *)&v ^ 0x80000000;
    return rv;
}

f32 NuFsign(f32 f) {
    if (*(int *)&f < 0) {
        return -1.0f;
    } else {
        return 1.0f;
    }
}

f32 NuFsqrt(f32 f) {
    if (f <= 1e-6f) {
        return 0.0f;
    }

    return sqrtf(f);
}

f32 NuFmax(f32 a, f32 b) {
    if (a > b) {
        return a;
    }

    return b;
}

f32 NuFmin(f32 a, f32 b) {
    if (a < b) {
        return a;
    }

    return b;
}

f32 NuFmod(f32 a, f32 b) {
    return a - (int)(a / b) * b;
}

f32 NuFloor(f32 f) {
    return (f32)(i32)f;
}
