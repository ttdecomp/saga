#pragma once

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

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
    void NuVec4Add(NUVEC4 *v, NUVEC4 *v0, NUVEC4 *v1);
    void NuVec4Scale(NUVEC4 *v, NUVEC4 *v0, f32 k);
    f32 NuVec4Dot(NUVEC4 *v0, NUVEC4 *v1);
    void NuVec4MtxTransformH(NUVEC4 *v, NUVEC4 *v0, NUMTX *m0);
#ifdef __cplusplus
}
#endif
