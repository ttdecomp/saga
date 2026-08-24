#ifndef LEGOAPI_MISC_H
#define LEGOAPI_MISC_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Miscellaneous shared game helpers (module legoapi/misc).

i32 XZLinesIntersect(nuvec_s *a0, nuvec_s *a1, nuvec_s *b0, nuvec_s *b1, float *ia, float *ib);

#endif
