#pragma once

#include "nu2api.saga/numath/nuvec4.h"

typedef struct nuwind_s {
    int unk0[8];
    int unk1;
    NUVEC4 unk2;
    f32 unk3;
} NUWIND;

#ifdef __cplusplus
extern "C" {
#endif
    void NuWindInitialise(NUWIND *wind);
#ifdef __cplusplus
}
#endif
