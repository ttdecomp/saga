#include "nu2api.saga/numath/nuvec.h"

void NuVecNeg(NUVEC* v, NUVEC* v0) {
    v->x = -v0->x;
    v->y = -v0->y;
    v->z = -v0->z;
}

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

void NuVecScale(NUVEC* v, NUVEC* v0, float k) {
    v->x = v0->x * k;
    v->y = v0->y * k;
    v->z = v0->z * k;
}

void NuVecAddScale(NUVEC* v, NUVEC* v0, NUVEC* v1, float k) {
    v->x = v0->x + v1->x * k;
    v->y = v0->y + v1->y * k;
    v->z = v0->z + v1->z * k;
}

void NuVecScaleAccum(NUVEC* v, NUVEC* v0, float k) {
    v->x += v0->x * k;
    v->y += v0->y * k;
    v->z += v0->z * k;
}

void NuVecInvScale(NUVEC* v, NUVEC* v0, float k) {
    float ki;

    if (k == 0.0f) {
        ki = 0.0f;
    } else {
        ki = 1.0f / k;
    }

    v->x = v0->x * ki;
    v->y = v0->y * ki;
    v->z = v0->z * ki;
}

void NuVecCross(NUVEC* v, NUVEC* v0, NUVEC* v1) {
    v->x = v0->y * v1->z - v0->z * v1->y;
    v->y = v0->z * v1->x - v0->x * v1->z;
    v->z = v0->x * v1->y - v0->y * v1->x;
}

void NuVecCrossRel(NUVEC* v, NUVEC* basepnt, NUVEC* v0, NUVEC* v1) {
    v->x = (v0->y - basepnt->y) * (v1->z - basepnt->z) - (v1->y - basepnt->y) * (v0->z - basepnt->z);
    v->y = (v0->z - basepnt->z) * (v1->x - basepnt->x) - (v1->z - basepnt->z) * (v0->x - basepnt->x);
    v->z = (v0->x - basepnt->x) * (v1->y - basepnt->y) - (v1->x - basepnt->x) * (v0->y - basepnt->y);
}

float NuVecDot(NUVEC* v0, NUVEC* v1) {
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

void NuVecMax(NUVEC* v, NUVEC* v0, NUVEC* v1) {
    if (v0->x <= v1->x) {
        v->x = v1->x;
    } else {
        v->x = v0->x;
    }

    if (v0->y <= v1->y) {
        v->y = v1->y;
    } else {
        v->y = v0->y;
    }

    if (v0->z <= v1->z) {
        v->z = v1->z;
    } else {
        v->z = v0->z;
    }
}

void NuVecMin(NUVEC* v, NUVEC* v0, NUVEC* v1) {
    if (v1->x <= v0->x) {
        v->x = v1->x;
    } else {
        v->x = v0->x;
    }

    if (v1->y <= v0->y) {
        v->y = v1->y;
    } else {
        v->y = v0->y;
    }

    if (v1->z <= v0->z) {
        v->z = v1->z;
    } else {
        v->z = v0->z;
    }
}

float NuVecMagSqr(NUVEC* v0) {
    return v0->x * v0->x + v0->y * v0->y + v0->z * v0->z;
}