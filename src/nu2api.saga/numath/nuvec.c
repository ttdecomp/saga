#include "nu2api.saga/numath/nuvec.h"

#include <stddef.h>

#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numath/numtx.h"

NUVEC v000 = {0};
NUVEC v100 = {1.0f, 0.0f, 0.0f};
NUVEC v010 = {0.0f, 1.0f, 0.0f};
NUVEC v001 = {0.0f, 0.0f, 1.0f};
NUVEC v111 = {1.0f, 1.0f, 1.0f};

void NuVecNeg(NUVEC *v, NUVEC *v0) {
    v->x = -v0->x;
    v->y = -v0->y;
    v->z = -v0->z;
}

void NuVecAdd(NUVEC *v, NUVEC *v0, NUVEC *v1) {
    v->x = v0->x + v1->x;
    v->y = v0->y + v1->y;
    v->z = v0->z + v1->z;
}

void NuVecSub(NUVEC *v, NUVEC *v0, NUVEC *v1) {
    v->x = v0->x - v1->x;
    v->y = v0->y - v1->y;
    v->z = v0->z - v1->z;
}

void NuVecScale(NUVEC *v, NUVEC *v0, float k) {
    v->x = v0->x * k;
    v->y = v0->y * k;
    v->z = v0->z * k;
}

void NuVecAddScale(NUVEC *v, NUVEC *v0, NUVEC *v1, float k) {
    v->x = v0->x + v1->x * k;
    v->y = v0->y + v1->y * k;
    v->z = v0->z + v1->z * k;
}

void NuVecScaleAccum(NUVEC *v, NUVEC *v0, float k) {
    v->x += v0->x * k;
    v->y += v0->y * k;
    v->z += v0->z * k;
}

void NuVecInvScale(NUVEC *v, NUVEC *v0, float k) {
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

void NuVecCross(NUVEC *v, NUVEC *v0, NUVEC *v1) {
    float y, z;
    y = v0->z * v1->x - v0->x * v1->z;
    z = v0->x * v1->y - v0->y * v1->x;
    v->x = v0->y * v1->z - v0->z * v1->y;

    v->y = y;
    v->z = z;
}

void NuVecCrossRel(NUVEC *v, NUVEC *basepnt, NUVEC *v0, NUVEC *v1) {
    float y, z;
    y = (v0->z - basepnt->z) * (v1->x - basepnt->x) - (v0->x - basepnt->x) * (v1->z - basepnt->z);
    z = (v0->x - basepnt->x) * (v1->y - basepnt->y) - (v0->y - basepnt->y) * (v1->x - basepnt->x);
    v->x = (v0->y - basepnt->y) * (v1->z - basepnt->z) - (v0->z - basepnt->z) * (v1->y - basepnt->y);
    v->y = y;
    v->z = z;
}

float NuVecDot(NUVEC *v0, NUVEC *v1) {
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

void NuVecMax(NUVEC *v, NUVEC *v0, NUVEC *v1) {
    if (v0->x > v1->x) {
        v->x = v0->x;
    } else {
        v->x = v1->x;
    }

    if (v0->y > v1->y) {
        v->y = v0->y;
    } else {
        v->y = v1->y;
    }

    if (v0->z > v1->z) {
        v->z = v0->z;
    } else {
        v->z = v1->z;
    }
}

void NuVecMin(NUVEC *v, NUVEC *v0, NUVEC *v1) {
    if (v0->x < v1->x) {
        v->x = v0->x;
    } else {
        v->x = v1->x;
    }

    if (v0->y < v1->y) {
        v->y = v0->y;
    } else {
        v->y = v1->y;
    }

    if (v0->z < v1->z) {
        v->z = v0->z;
    } else {
        v->z = v1->z;
    }
}

float NuVecMagSqr(NUVEC *v0) {
    return v0->x * v0->x + v0->y * v0->y + v0->z * v0->z;
}

float NuVecMag(NUVEC *v0) {
    return NuFsqrt(NuVecMagSqr(v0));
}

float NuVecMagXZ(NUVEC *v0) {
    return NuFsqrt(v0->x * v0->x + v0->z * v0->z);
}

float NuVecNorm(NUVEC *v, NUVEC *v0) {
    float len = NuFsqrt(NuVecMagSqr(v0));
    float leni;

    if (len > 0.0f) {
        leni = 1.0f / len;
    } else {
        leni = 0.0f;
    }

    v->x = v0->x * leni;
    v->y = v0->y * leni;
    v->z = v0->z * leni;

    return len;
}

void NuVecSurfaceNormal(NUVEC *v, NUVEC *v0, NUVEC *v1, NUVEC *v2) {
    NUVEC vecA, vecB;

    NuVecSub(&vecA, v0, v1);
    NuVecSub(&vecB, v0, v2);
    NuVecCross(v, &vecB, &vecA);
    NuVecNorm(v, v);
}

float NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        NuVecSub(d, v0, v1);
        return NuVecMag(d);
    } else {
        NuVecSub(&dist, v0, v1);
        return NuVecMag(&dist);
    }
}

float NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        NuVecSub(d, v0, v1);
        return NuVecMagSqr(d);
    } else {
        NuVecSub(&dist, v0, v1);
        return NuVecMagSqr(&dist);
    }
}

float NuVecXZDist(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        d->x = v0->x - v1->x;
        d->y = 0.0f;
        d->z = v0->z - v1->z;
        return NuVecMag(d);
    } else {
        dist.x = v0->x - v1->x;
        dist.y = 0.0f;
        dist.z = v0->z - v1->z;
        return NuVecMag(&dist);
    }
}

float NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        d->x = v0->x - v1->x;
        d->y = 0.0f;
        d->z = v0->z - v1->z;
        return NuVecMagSqr(d);
    } else {
        dist.x = v0->x - v1->x;
        dist.y = 0.0f;
        dist.z = v0->z - v1->z;
        return NuVecMagSqr(&dist);
    }
}

void NuVecLerp(NUVEC *vt, NUVEC *v1, NUVEC *v0, float t) {
    vt->x = ((v1->x - v0->x) * t) + v0->x;
    vt->y = ((v1->y - v0->y) * t) + v0->y;
    vt->z = ((v1->z - v0->z) * t) + v0->z;
}

int NuVecCompareTolerance(NUVEC *a, NUVEC *b, float tolerance) {
    NUVEC diff;
    NuVecSub(&diff, a, b);

    float result = NuVecMag(&diff);
    if (result > tolerance) {
        return 0;
    } else {
        return 1;
    }
}

void NuVecMtxTransform(NUVEC *v, NUVEC *v0, NUMTX *m0) {
    float y = v0->x * m0->_01 + v0->y * m0->_11 + v0->z * m0->_21 + m0->_31;
    float z = v0->x * m0->_02 + v0->y * m0->_12 + v0->z * m0->_22 + m0->_32;
    v->x = v0->x * m0->_00 + v0->y * m0->_10 + v0->z * m0->_20 + m0->_30;
    v->y = y;
    v->z = z;
}