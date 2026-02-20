#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

typedef struct nuplane_s {
    f32 a;
    f32 b;
    f32 c;
    f32 d;
} NUPLANE;

#ifdef __cplusplus
extern "C" {
#endif
    /// @brief Constructs a plane equation from three non-collinear points.
    /// @param out The constructed plane.
    /// @param pnt0 The first point in the plane.
    /// @param pnt1 The second point in the plane.
    /// @param pnt2 The third point in the plane.
    void NuPlnEqn(NUPLANE *out, NUVEC *pnt0, NUVEC *pnt1, NUVEC *pnt2);

    i32 NuPlnLine(NUPLANE *plane, NUVEC *s, NUVEC *e, NUVEC *out);
    f32 NuPlnLine3(NUPLANE *plane, NUVEC *pnt, NUVEC *v, NUVEC *out);

    i32 NuPtInPoly(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2, NUPLANE *plane);
    i32 NuPtInPolyXY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyYX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyXZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyZX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyYZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyZY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);

    f32 NuLineToPointDistSqrEx(NUVEC *s, NUVEC *e, NUVEC *pnt, NUVEC *out);
#ifdef __cplusplus
}
#endif
