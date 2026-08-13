#pragma once

#include "nu2api/numath/nuvec4.h"

typedef struct nuwind_s {
    i32 unk0[8];
    i32 unk1;
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
