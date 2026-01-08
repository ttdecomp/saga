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
    m->_30 = t->x;
    m->_31 = t->y;
    m->_32 = t->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetTranslationNeg(NUMTX *m, NUVEC *t) {
    m->_30 = -t->x;
    m->_31 = -t->y;
    m->_32 = -t->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = 0.0f;
    m->_00 = m->_11 = m->_22 = m->_33 = 1.0f;
}

void NuMtxSetScale(NUMTX *m, NUVEC *s) {
    m->_00 = s->x;
    m->_11 = s->y;
    m->_22 = s->z;
    m->_01 = m->_02 = m->_03 = m->_10 = m->_12 = m->_13 = m->_20 = m->_21 = m->_23 = m->_30 = m->_31 = m->_32 = 0.0f;
    m->_33 = 1.0f;
}