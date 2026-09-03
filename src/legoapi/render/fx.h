#ifndef LEGOAPI_RENDER_FX_H
#define LEGOAPI_RENDER_FX_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Debris / particle / flight-spline helpers (module legoapi/render/fx).

struct GAMEDEBRISENTRY_s {
    i32 effect;
    char name[16];
};

struct APIDEBRISSYS_s {
    i32 named_count;
    i32 capacity;
    GAMEDEBRISENTRY_s *entries;
};

#ifdef __cplusplus
extern "C" {
#endif
    i32 FindGameDebris(APIDEBRISSYS_s *debris_sys, char *name);
    void *AddGameDebris(APIDEBRISSYS_s *debris_sys, i32 type, void *pos);
    APIDEBRISSYS_s *InitGameDebris(VARIPTR *cursor, VARIPTR end, i32 count, i32 named_count, char **names, char page);
    i32 PARTLookupType(char *name);
    i32 ParticlesPerFrame(f32 particles_per_frame, f32 frame_time);
    i32 ParticlesPerSecond(f32 particles_per_second, f32 frame_time);
    void AddFiniteShotPART(i32 part_type, void *pos, i32 unknown);
#ifdef __cplusplus
}
#endif
void FlightSpline_Init(WORLDINFO_s *world, flightspline_s *spline, i32 unknown);

#endif
