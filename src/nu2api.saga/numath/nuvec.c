#include "nu2api.saga/numath/nuvec.h"

#include <stddef.h>

#include "nu2api.saga/numath/nufloat.h"

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
    v->x = v0->y * v1->z - v0->z * v1->y;
    v->y = v0->z * v1->x - v0->x * v1->z;
    v->z = v0->x * v1->y - v0->y * v1->x;
}

void NuVecCrossRel(NUVEC *v, NUVEC *basepnt, NUVEC *v0, NUVEC *v1) {
    v->x = (v0->y - basepnt->y) * (v1->z - basepnt->z) - (v1->y - basepnt->y) * (v0->z - basepnt->z);
    v->y = (v0->z - basepnt->z) * (v1->x - basepnt->x) - (v1->z - basepnt->z) * (v0->x - basepnt->x);
    v->z = (v0->x - basepnt->x) * (v1->y - basepnt->y) - (v1->x - basepnt->x) * (v0->y - basepnt->y);
}

float NuVecDot(NUVEC *v0, NUVEC *v1) {
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

void NuVecMax(NUVEC *v, NUVEC *v0, NUVEC *v1) {
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

void NuVecMin(NUVEC *v, NUVEC *v0, NUVEC *v1) {
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

float NuVecMagSqr(NUVEC *v0) {
    return v0->x * v0->x + v0->y * v0->y + v0->z * v0->z;
}

float NuVecMag(NUVEC* v0) {
    return NuFsqrt(NuVecMagSqr(v0));
}

float NuVecMagXZ(NUVEC* v0) {
    return NuFsqrt(v0->x * v0->x + v0->z * v0->z);
}

float NuVecNorm(NUVEC* v, NUVEC* v0) {
    float len = NuFsqrt(NuVecMagSqr(v0));
    float leni;

    if (len <= 0.0f) {
        leni = 0.0f;
    } else {
        leni = 1.0f / len;
    }

    v->x = v0->x * leni;
    v->y = v0->y * leni;
    v->z = v0->z * leni;

    return len;
}

void NuVecSurfaceNormal(NUVEC* v, NUVEC* v0, NUVEC* v1, NUVEC* v2) {
    NUVEC vecA, vecB;

    NuVecSub(&vecA, v0, v1);
    NuVecSub(&vecB, v0, v2);
    NuVecCross(v, &vecA, &vecB);
    NuVecNorm(v, v);
}

float NuVecDist(NUVEC* v0, NUVEC* v1, NUVEC* d) {
    NUVEC dist;
    float result;

    if (d == NULL) {
        NuVecSub(&dist, v0, v1);
        result = NuVecMag(&dist);
    } else {
        NuVecSub(d, v0, v1);
        result = NuVecMag(d);
    }

    return result;
}

float NuVecDistSqr(NUVEC* v0, NUVEC* v1, NUVEC* d) {
    NUVEC dist;
    float result;

    if (d == NULL) {
        NuVecSub(&dist, v0, v1);
        result = NuVecMagSqr(&dist);
    } else {
        NuVecSub(d, v0, v1);
        result = NuVecMagSqr(d);
    }

    return result;
}

float NuVecXZDist(NUVEC* v0, NUVEC* v1, NUVEC* d) {
    NUVEC dist;
    float result;

    if (d == NULL) {
        dist.x = v0->x - v1->x;
        dist.y = 0.0f;
        dist.z = v0->z - v1->z;
        result = NuVecMag(&dist);
    } else {
        d->x = v0->x - v1->x;
        d->y = 0.0f;
        d->z = v0->z - v1->z;
        result = NuVecMag(d);
    }

    return result;
}

float NuVecXZDistSqr(NUVEC* v0, NUVEC* v1, NUVEC* d) {
    NUVEC dist;
    float result;

    if (d == NULL) {
        dist.x = v0->x - v1->x;
        dist.y = 0.0f;
        dist.z = v0->z - v1->z;
        result = NuVecMagSqr(&dist);
    } else {
        d->x = v0->x - v1->x;
        d->y = 0.0f;
        d->z = v0->z - v1->z;
        result = NuVecMagSqr(d);
    }

    return result;
}

void NuVecLerp(NUVEC* vt, NUVEC* v1, NUVEC* v0, float t) {
    vt->x = v0->x + (v1->x - v0->x) * t;
    vt->y = v0->y + (v1->y - v0->y) * t;
    vt->z = v0->z + (v1->z - v0->z) * t;
}

int NuVecCompareTolerance(NUVEC *a, NUVEC *b, float tolerance) {
    NUVEC diff;
    NuVecSub(&diff, a, b);

    float result = NuVecMag(&diff);

    return result <= tolerance;
}

// int NuLineLineIntersect(NUVEC *p0, NUVEC *v0, NUVEC *p1, NUVEC *v1, float *s, float *t) {

// }