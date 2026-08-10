#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    void CRC_Init(VARIPTR *buffer_start, VARIPTR buffer_end);
    u32 CRC_ProcessStringIgnoreCase(const char *str);

#ifdef __cplusplus
}
#endif
