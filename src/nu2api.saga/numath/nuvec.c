#include "nu2api.saga/numath/nuvec.h"

void NuVecAdd(NUVEC* v, NUVEC* v0, NUVEC* v1) {
    v->x = v0->x + v1->x;
    v->y = v0->y + v1->y;
    v->z = v0->z + v1->z;
}

void NuVecSub(NUVEC* v, NUVEC* v0, NUVEC* v1) {
    v->x = v0->x - v1->x;
    v->y = v0->y - v1->y;
    v->z = v0->z - v1->z;
}