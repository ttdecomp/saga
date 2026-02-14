#pragma once

#include "nu2api.saga/nucore/common.h"

typedef struct nusound_info_s {
    const char *name;
    u8 flags;
    u8 priority;
    u16 next;
    u16 group;
} NUSOUNDINFO;

#ifdef __cplusplus
extern "C" {
#endif

    void InitSfx(variptr_u *buffer_start, variptr_u buffer_end, const char *file);

    void LoadSfx(const char *file, variptr_u *buffer_start, variptr_u buffer_end);

    i32 GetSfxId(const char *name);

#ifdef __cplusplus
}
#endif
