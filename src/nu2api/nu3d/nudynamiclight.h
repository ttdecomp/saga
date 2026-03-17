#pragma once

#include "nu2api/nucore/common.h"
#include <stdbool.h>

typedef struct nudynamiclights_s {
    char _pad0[0x7BC];  
    int isEnabled;     
} NUDYNAMICLIGHTS;


#ifdef __cplusplus
extern "C" {
#endif
    bool NuDynamicLightIsEnabled(NUDYNAMICLIGHTS* param_1);
    int NuDynamicLightAddRenderScene(NUDYNAMICLIGHTS* a1, int a2, int a3);
#ifdef __cplusplus
}
#endif


