#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    void CRC_Init(VARIPTR *buffer_start);
    u32 CRC_ProcessStringIgnoreCase(const char *str);

#ifdef __cplusplus
}
#endif
