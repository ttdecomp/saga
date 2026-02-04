#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nu3d/numtl.h"

typedef struct vufnt_s {
    char filler1[0x24];   // 0x00-0x23
    float ic_gap;         // 0x24
    char filler2[0x4];    // 0x28-0x2B
    usize field_0x2c;     // 0x2c
    usize field_0x30;     // 0x30
    char filler3[0x8];    // 0x34-0x3B
    int field_0x3c;       // 0x3c
    NUMTL* mtl;           // 0x40
    usize field_0x44;     // 0x44
    usize field_0x48;     // 0x48
} VUFNT;

typedef VUFNT NUQFNT;

#ifdef __cplusplus

int NuQFntReadPS(VUFNT* font, int t_id, int flags, int render_plane, VARIPTR* buffer, VARIPTR buffer_end);

extern "C" {
#endif

    void NuQFntInit(VARIPTR *buffer, VARIPTR buffer_end);
    NUQFNT *NuQFntReadBuffer(VARIPTR *font, VARIPTR *buffer, VARIPTR buffer_end);
    void NuQFntSetICGap(NUQFNT *font, float ic_gap);

#ifdef __cplusplus
}
#endif
