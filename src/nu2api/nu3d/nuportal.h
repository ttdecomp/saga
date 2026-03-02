#pragma once

#include "nu2api/nucore/common.h"

typedef struct nuportal_s {
    u8 filler[0x1a];
    i8 id;
    u8 unknown;
    u32 is_active;
} NUPORTAL;

#ifdef __cplusplus
extern "C" {
#endif

    void NuPortalInit(void);
    void NuPortalSetActiveDirect(NUPORTAL *portal, int active);

#ifdef __cplusplus
}
#endif
