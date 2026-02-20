#include "nu2api/numath/nuvec.h"

#include <stddef.h>

#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"

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

void NuVecScale(NUVEC *v, NUVEC *v0, f32 k) {
    v->x = v0->x * k;
    v->y = v0->y * k;
    v->z = v0->z * k;
}

void NuVecAddScale(NUVEC *v, NUVEC *v0, NUVEC *v1, f32 k) {
    v->x = v0->x + v1->x * k;
    v->y = v0->y + v1->y * k;
    v->z = v0->z + v1->z * k;
}

void NuVecScaleAccum(NUVEC *v, NUVEC *v0, f32 k) {
    v->x += v0->x * k;
    v->y += v0->y * k;
    v->z += v0->z * k;
}

void NuVecInvScale(NUVEC *v, NUVEC *v0, f32 k) {
    f32 ki;

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
    f32 y, z;
    y = v0->z * v1->x - v0->x * v1->z;
    z = v0->x * v1->y - v0->y * v1->x;
    v->x = v0->y * v1->z - v0->z * v1->y;

    v->y = y;
    v->z = z;
}

void NuVecCrossRel(NUVEC *v, NUVEC *basepnt, NUVEC *v0, NUVEC *v1) {
    f32 y, z;
    y = (v0->z - basepnt->z) * (v1->x - basepnt->x) - (v0->x - basepnt->x) * (v1->z - basepnt->z);
    z = (v0->x - basepnt->x) * (v1->y - basepnt->y) - (v0->y - basepnt->y) * (v1->x - basepnt->x);
    v->x = (v0->y - basepnt->y) * (v1->z - basepnt->z) - (v0->z - basepnt->z) * (v1->y - basepnt->y);
    v->y = y;
    v->z = z;
}

f32 NuVecDot(NUVEC *v0, NUVEC *v1) {
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

f32 NuVecMagSqr(NUVEC *v0) {
    return v0->x * v0->x + v0->y * v0->y + v0->z * v0->z;
}

f32 NuVecMag(NUVEC *v0) {
    return NuFsqrt(NuVecMagSqr(v0));
}

f32 NuVecMagXZ(NUVEC *v0) {
    return NuFsqrt(v0->x * v0->x + v0->z * v0->z);
}

f32 NuVecNorm(NUVEC *v, NUVEC *v0) {
    f32 len = NuFsqrt(NuVecMagSqr(v0));
    f32 leni;

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

f32 NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        NuVecSub(d, v0, v1);
        return NuVecMag(d);
    } else {
        NuVecSub(&dist, v0, v1);
        return NuVecMag(&dist);
    }
}

f32 NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d) {
    NUVEC dist;

    if (d != NULL) {
        NuVecSub(d, v0, v1);
        return NuVecMagSqr(d);
    } else {
        NuVecSub(&dist, v0, v1);
        return NuVecMagSqr(&dist);
    }
}

f32 NuVecXZDist(NUVEC *v0, NUVEC *v1, NUVEC *d) {
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

f32 NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *d) {
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

void NuVecLerp(NUVEC *vt, NUVEC *v1, NUVEC *v0, f32 t) {
    vt->x = ((v1->x - v0->x) * t) + v0->x;
    vt->y = ((v1->y - v0->y) * t) + v0->y;
    vt->z = ((v1->z - v0->z) * t) + v0->z;
}

int NuVecCompareTolerance(NUVEC *a, NUVEC *b, f32 tolerance) {
    NUVEC diff;
    NuVecSub(&diff, a, b);

    f32 result = NuVecMag(&diff);
    if (result > tolerance) {
        return 0;
    } else {
        return 1;
    }
}

void NuVecMtxTransform(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;

    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
    out->y = y;
    out->z = z;
}

void NuVecMtxRotate(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22;

    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20;
    out->y = y;
    out->z = z;
}

void NuVecMtxScale(NUVEC *out, NUVEC *v, NUMTX *m) {
    out->x = v->x * m->m00;
    out->y = v->y * m->m11;
    out->z = v->z * m->m22;
}

void NuVecMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *m) {
    out->x = v->x + m->m30;
    out->y = v->y + m->m31;
    out->z = v->z + m->m32;
}

void NuVecMtxTransformBlock(NUVEC *out, NUVEC *v, NUMTX *m, i32 count) {
    i32 i;

    for (i = 0; i < count; i++) {
        out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
        out->y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
        out->z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;

        out++;
        v++;
    }
}

i32 NuLineLineIntersect(NUVEC *pnt0, NUVEC *v0, NUVEC *pnt1, NUVEC *v1, f32 *s, f32 *t) {
    NUVEC cross;
    f32 len;
    f32 inv_len;
    NUMTX m;

    NuVecCross(&cross, v0, v1);
    len = NuVecMag(&cross);

    if (len == 0.0f) {
        inv_len = 0.0f;
    } else {
        inv_len = 1.0f / len;
    }

    inv_len *= inv_len;

    m.m00 = pnt1->x - pnt0->x;
    m.m01 = pnt1->y - pnt0->y;
    m.m02 = pnt1->z - pnt0->z;

    m.m10 = v1->x;
    m.m11 = v1->y;
    m.m12 = v1->z;

    m.m20 = cross.x;
    m.m21 = cross.y;
    m.m22 = cross.z;

    *s = NuMtxDet3(&m) * inv_len;

    m.m10 = v0->x;
    m.m11 = v0->y;
    m.m12 = v0->z;

    *t = NuMtxDet3(&m) * inv_len;

    if (inv_len != 0.0f) {
        return 1;
    }

    return 0;
}
