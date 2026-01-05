#include "nu2api.saga/numath/numtx.h"

NUMTX numtx_zero = { 0 };
NUMTX numtx_identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

void NuMtxSetZero(NUMTX* m) {
    m->_00 = numtx_zero._00;
    m->_01 = numtx_zero._01;
    m->_02 = numtx_zero._02;
    m->_03 = numtx_zero._03;
    m->_10 = numtx_zero._10;
    m->_11 = numtx_zero._11;
    m->_12 = numtx_zero._12;
    m->_13 = numtx_zero._13;
    m->_20 = numtx_zero._20;
    m->_21 = numtx_zero._21;
    m->_22 = numtx_zero._22;
    m->_23 = numtx_zero._23;
    m->_30 = numtx_zero._30;
    m->_31 = numtx_zero._31;
    m->_32 = numtx_zero._32;
    m->_33 = numtx_zero._33;
}

void NuMtxSetIdentity(NUMTX* m) {
    m->_00 = numtx_identity._00;
    m->_01 = numtx_identity._01;
    m->_02 = numtx_identity._02;
    m->_03 = numtx_identity._03;
    m->_10 = numtx_identity._10;
    m->_11 = numtx_identity._11;
    m->_12 = numtx_identity._12;
    m->_13 = numtx_identity._13;
    m->_20 = numtx_identity._20;
    m->_21 = numtx_identity._21;
    m->_22 = numtx_identity._22;
    m->_23 = numtx_identity._23;
    m->_30 = numtx_identity._30;
    m->_31 = numtx_identity._31;
    m->_32 = numtx_identity._32;
    m->_33 = numtx_identity._33;
}

void NuMtxSetTranslation(NUMTX* m, NUVEC* t) {
    *m = (NUMTX) {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        t->x, t->y, t->z, 1.0f,
    };
}

void NuMtxSetTranslationNeg(NUMTX* m, NUVEC* t) {
    *m = (NUMTX) {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -t->x, -t->y, -t->z, 1.0f,
    };
}

void NuMtxSetScale(NUMTX* m, NUVEC* s) {
    *m = (NUMTX) {
        s->x, 0.0f, 0.0f, 0.0f,
        0.0f, s->y, 0.0f, 0.0f,
        0.0f, 0.0f, s->z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
}