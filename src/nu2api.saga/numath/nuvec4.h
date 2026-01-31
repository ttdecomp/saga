#pragma once

#include "nu2api.saga/numath/nuvec.h"

struct nuvec4_s {
    float x;
    float y;
    float z;
    float w;
};

typedef struct nuvec4_s NUVEC4;

#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4MtxTransform(NUVEC4 *v, NUVEC *v0, NUMTX *m0);
#ifdef __cplusplus
}
#endif