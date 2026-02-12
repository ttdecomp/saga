#pragma once

#include "nu2api.saga/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 g_isLowEndDevice;

    i32 NuIOS_IsLowEndDevice(void);

    char *NuIOS_GetDocumentsPath(void);
    char *NuIOS_GetAppBundlePath(void);

    u32 NuIOS_YieldThread(void);

    void NuIOS_InitRenderThread();
    i32 NuIOS_ShouldUseMSAA(void);
#ifdef __cplusplus
}
#endif
