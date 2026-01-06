#include "nu2api.saga/numath/numtx.h"

NUMTX numtx_zero = {0};

// clang-format off
NUMTX numtx_identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
// clang-format on

void NuMtxSetZero(NUMTX *m) {
    *m = numtx_zero;
}

void NuMtxSetIdentity(NUMTX *m) {
    *m = numtx_identity;
}

void NuMtxSetTranslation(NUMTX *m, NUVEC *t) {
    // clang-format off
    *m = (NUMTX) {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        t->x, t->y, t->z, 1.0f,
    };
    // clang-format on
}

void NuMtxSetTranslationNeg(NUMTX *m, NUVEC *t) {
    // clang-format off
    *m = (NUMTX) {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -t->x, -t->y, -t->z, 1.0f,
    };
    // clang-format on
}

void NuMtxSetScale(NUMTX *m, NUVEC *s) {
    // clang-format off
    *m = (NUMTX) {
        s->x, 0.0f, 0.0f, 0.0f,
        0.0f, s->y, 0.0f, 0.0f,
        0.0f, 0.0f, s->z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    // clang-format on
}