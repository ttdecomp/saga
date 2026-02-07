#pragma once

#include "nu2api.saga/numath/nuvec.h"

typedef struct nuvec4_s {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} NUVEC4;

#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4MtxTransform(NUVEC4 *v, NUVEC *v0, NUMTX *m0);
#ifdef __cplusplus
}
#endif
