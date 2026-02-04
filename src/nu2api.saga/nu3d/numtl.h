#pragma once

#include "nu2api.saga/numath/nuvec.h"

typedef struct numtl_s {
    char filler1[0x40];
    unsigned char unk_flag1;   // 0x40
    unsigned char unk_flag2;   // 0x41
    unsigned char unk_flag3;   // 0x42
    char filler2[0x11];        // 0x43-0x53
    NUVEC scale;               // 0x54
    char filler3[0x10];        // 0x60-0x6F
    float opacity;             // 0x70
    u16 tex_id;                // 0x74
    char filler4[0x24E];       // 0x76-0x2C3
} NUMTL;

typedef struct nushadermtldesc_s {
    u32 flags;              // 0x0
    i32 tex_id;             // 0x4
    char filler1[0xC];      // 0x8-0x13
    u32 mask;               // 0x14
    char filler2[0xC];      // 0x18-0x23
    float value;            // 0x24
    char filler3[0x114];    // 0x28-0x13B
    u8 byte1;               // 0x13c
    u8 byte2;               // 0x13d
    u8 byte3;               // 0x13e
    char filler4[0x7A];     // 0x13F-0x1B8
    u8 byte4;               // 0x1b9
    char filler5[0x4E];     // 0x1BA-0x207
} NUSHADERMTLDESC;

#ifdef __cplusplus

void DefaultMtl(NUMTL* mtl);
void NuMtlCreatePS(NUMTL* mtl, int unk_flag);

extern "C" {
#endif

void NuShaderMtlDescInit(NUSHADERMTLDESC* shader_mtl_desc);
void NuMtlSetShaderDescPS(NUMTL* mtl, NUSHADERMTLDESC* shader_mtl_desc);
NUMTL* NuMtlCreate(int count);
void NuMtlUpdate(NUMTL* mtl);

#ifdef __cplusplus
}
#endif
