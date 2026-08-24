#ifndef LEGOAPI_RENDER_FX_H
#define LEGOAPI_RENDER_FX_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Debris / particle / flight-spline helpers (module legoapi/render/fx).

#ifdef __cplusplus
extern "C" {
#endif
    i16 FindGameDebris(void *debris_sys, char *name);
    void *AddGameDebris(void *debris_sys, i32 type, void *pos);
    i32 PARTLookupType(char *name);
    void AddFiniteShotPART(i32 part_type, void *pos, i32 unknown);
#ifdef __cplusplus
}
#endif
void FlightSpline_Init(WORLDINFO_s *world, flightspline_s *spline, i32 unknown);

#endif
