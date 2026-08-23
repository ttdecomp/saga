#ifndef NU2API_NU3D_NUSPLINE_H
#define NU2API_NU3D_NUSPLINE_H

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

typedef struct nugspline_s {
    i16 length; // signed: the original compares it with jle/movswl
    u16 pt_size;
    char *name;

    // This seems to be typed as `char *` in the original code base, but use as
    // a `NUVEC` is all but hardcoded.
    NUVEC *pts;
} NUGSPLINE;

#ifdef __cplusplus
extern "C" {
#endif

    NUGSPLINE *NuSplineFind(NUGSCN *scene, char *name);

#ifdef __cplusplus
}
#endif

#endif // NU2API_NU3D_NUSPLINE_H