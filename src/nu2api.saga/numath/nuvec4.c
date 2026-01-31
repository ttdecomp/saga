#include "nu2api.saga/numath/nuvec4.h"

#include "nu2api.saga/numath/numtx.h"

void NuVec4MtxTransform(NUVEC4 *v, NUVEC *v0, NUMTX *m0) {
    float y = v0->x * m0->_01 + v0->y * m0->_11 + v0->z * m0->_21 + m0->_31;
    float z = v0->x * m0->_02 + v0->y * m0->_12 + v0->z * m0->_22 + m0->_32;
    float w = v0->x * m0->_03 + v0->y * m0->_13 + v0->z * m0->_23 + m0->_33;
    v->x = v0->x * m0->_00 + v0->y * m0->_10 + v0->z * m0->_20 + m0->_30;
    v->y = y;
    v->z = z;
    v->w = w;
}