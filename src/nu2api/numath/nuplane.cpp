#include "nu2api/numath/nuplane.h"

#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nuvec.h"

void NuPlnEqn(NUPLANE *out, NUVEC *pnt0, NUVEC *pnt1, NUVEC *pnt2) {
    NUVEC *pln;
    NUVEC v1_minus_0;
    NUVEC v2_minus_0;
    NUVEC normal;

    pln = (NUVEC *)out;

    NuVecSub(&v1_minus_0, pnt1, pnt0);
    NuVecSub(&v2_minus_0, pnt2, pnt0);

    NuVecCross(&normal, &v1_minus_0, &v2_minus_0);

    NuVecNorm(pln, &normal);

    out->d = -(out->a * pnt0->x + out->b * pnt0->y + out->c * pnt0->z);
}

i32 NuPlnLine(NUPLANE *plane, NUVEC *s, NUVEC *e, NUVEC *out) {
    f32 dot_sp;
    f32 dot_ep;
    NUVEC dir;
    f32 inv_magnitude;
    i32 is_acute;

    dot_sp = NuVecDot(s, (NUVEC *)plane) + plane->d;
    dot_ep = NuVecDot(e, (NUVEC *)plane) + plane->d;

    if (NuFsign(dot_sp) != NuFsign(dot_ep)) {
        NuVecSub(&dir, e, s);

        inv_magnitude = -dot_sp / (dot_ep - dot_sp);
        NuVecScale(out, &dir, inv_magnitude);

        NuVecAdd(out, out, s);

        return 1;
    }

    return 0;
}

f32 NuPlnLine3(NUPLANE *plane, NUVEC *pnt, NUVEC *v, NUVEC *out) {
    NUVEC *pln;
    f32 dist;
    f32 dot;

    NuVecNorm(v, v);

    // It's not entirely clear why the plane is stored in a separate variable,
    // or that this cast is what originally took place.
    pln = (NUVEC *)plane;
    dot = v->x * pln->x + v->y * pln->y + v->z * pln->z;

    dist = ((plane->a * -plane->d - pnt->x) * pln->x + (plane->b * -plane->d - pnt->y) * pln->y +
            (plane->c * -plane->d - pnt->z) * pln->z) /
           dot;

    out->x = pnt->x + v->x * dist;
    out->y = pnt->y + v->y * dist;
    out->z = pnt->z + v->z * dist;

    return dist;
}

i32 NuPtInPoly(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2, NUPLANE *plane) {
    f32 abs_a;
    f32 abs_b;
    f32 abs_c;
    i32 result;

    abs_a = NuFabs(plane->a);
    abs_b = NuFabs(plane->b);
    abs_c = NuFabs(plane->c);

    if (abs_a > abs_b) {
        if (abs_a > abs_c) {
            if (plane->a <= 0.0f) {
                result = NuPtInPolyYZ(pnt, t0, t1, t2);
            } else {
                result = NuPtInPolyZY(pnt, t0, t1, t2);
            }
        } else {
            if (plane->c <= 0.0f) {
                result = NuPtInPolyXY(pnt, t0, t1, t2);
            } else {
                result = NuPtInPolyYX(pnt, t0, t1, t2);
            }
        }
    } else {
        if (abs_b > abs_c) {
            if (plane->b <= 0.0f) {
                result = NuPtInPolyZX(pnt, t0, t1, t2);
            } else {
                result = NuPtInPolyXZ(pnt, t0, t1, t2);
            }
        } else {
            if (plane->c <= 0.0f) {
                result = NuPtInPolyXY(pnt, t0, t1, t2);
            } else {
                result = NuPtInPolyYX(pnt, t0, t1, t2);
            }
        }
    }

    return result;
}

i32 NuPtInPolyXY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->x - t0->x) * (pnt->y - t1->y) - (pnt->x - t1->x) * (t1->y - t0->y) <= 0.0f &&
        (t2->x - t1->x) * (pnt->y - t2->y) - (pnt->x - t2->x) * (t2->y - t1->y) <= 0.0f &&
        (t0->x - t2->x) * (pnt->y - t0->y) - (pnt->x - t0->x) * (t0->y - t2->y) <= 0.0f) {
        if ((t0->x != t1->x || t0->y != t1->y) && (t0->x != t2->x || t0->y != t2->y) &&
            (t1->x != t2->x || t1->y != t2->y)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyYX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->y - t0->y) * (pnt->x - t1->x) - (pnt->y - t1->y) * (t1->x - t0->x) <= 0.0f &&
        (t2->y - t1->y) * (pnt->x - t2->x) - (pnt->y - t2->y) * (t2->x - t1->x) <= 0.0f &&
        (t0->y - t2->y) * (pnt->x - t0->x) - (pnt->y - t0->y) * (t0->x - t2->x) <= 0.0f) {
        if ((t0->y != t1->y || t0->x != t1->x) && (t0->y != t2->y || t0->x != t2->x) &&
            (t1->y != t2->y || t1->x != t2->x)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyXZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->x - t0->x) * (pnt->z - t1->z) - (pnt->x - t1->x) * (t1->z - t0->z) <= 0.0f &&
        (t2->x - t1->x) * (pnt->z - t2->z) - (pnt->x - t2->x) * (t2->z - t1->z) <= 0.0f &&
        (t0->x - t2->x) * (pnt->z - t0->z) - (pnt->x - t0->x) * (t0->z - t2->z) <= 0.0f) {
        if ((t0->x != t1->x || t0->z != t1->z) && (t0->x != t2->x || t0->z != t2->z) &&
            (t1->x != t2->x || t1->z != t2->z)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyZX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->z - t0->z) * (pnt->x - t1->x) - (pnt->z - t1->z) * (t1->x - t0->x) <= 0.0f &&
        (t2->z - t1->z) * (pnt->x - t2->x) - (pnt->z - t2->z) * (t2->x - t1->x) <= 0.0f &&
        (t0->z - t2->z) * (pnt->x - t0->x) - (pnt->z - t0->z) * (t0->x - t2->x) <= 0.0f) {
        if ((t0->z != t1->z || t0->x != t1->x) && (t0->z != t2->z || t0->x != t2->x) &&
            (t1->z != t2->z || t1->x != t2->x)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyYZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->y - t0->y) * (pnt->z - t1->z) - (pnt->y - t1->y) * (t1->z - t0->z) <= 0.0f &&
        (t2->y - t1->y) * (pnt->z - t2->z) - (pnt->y - t2->y) * (t2->z - t1->z) <= 0.0f &&
        (t0->y - t2->y) * (pnt->z - t0->z) - (pnt->y - t0->y) * (t0->z - t2->z) <= 0.0f) {
        if ((t0->y != t1->y || t0->z != t1->z) && (t0->y != t2->y || t0->z != t2->z) &&
            (t1->y != t2->y || t1->z != t2->z)) {
            return 1;
        }
    }

    return 0;
}

i32 NuPtInPolyZY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2) {
    if ((t1->z - t0->z) * (pnt->y - t1->y) - (pnt->z - t1->z) * (t1->y - t0->y) <= 0.0f &&
        (t2->z - t1->z) * (pnt->y - t2->y) - (pnt->z - t2->z) * (t2->y - t1->y) <= 0.0f &&
        (t0->z - t2->z) * (pnt->y - t0->y) - (pnt->z - t0->z) * (t0->y - t2->y) <= 0.0f) {
        if ((t0->z != t1->z || t0->y != t1->y) && (t0->z != t2->z || t0->y != t2->y) &&
            (t1->z != t2->z || t1->y != t2->y)) {
            return 1;
        }
    }

    return 0;
}

f32 NuLineToPointDistSqrEx(NUVEC *s, NUVEC *e, NUVEC *pnt, NUVEC *out) {
    NUVEC s_to_e;
    f32 len;
    NUVEC pnt_to_s;
    f32 dot;
    NUVEC nearest;
    f32 dist;
    NUVEC dir;

    NuVecSub(&s_to_e, e, s);
    len = NuVecMag(&s_to_e);

    NuVecScale(&s_to_e, &s_to_e, 1.0f / len);

    NuVecSub(&pnt_to_s, pnt, s);
    dot = NuVecDot(&s_to_e, &pnt_to_s);

    if (dot <= 0.0f) {
        nearest = *s;
    } else if (dot >= len) {
        nearest = *e;
    } else {
        NuVecScale(&nearest, &s_to_e, dot);
        NuVecAdd(&nearest, &nearest, s);
    }

    dist = NuVecDistSqr(&nearest, pnt, &dir);

    if (out != NULL) {
        *out = nearest;
    }

    return dist;
}
