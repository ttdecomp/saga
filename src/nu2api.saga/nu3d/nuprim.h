#pragma once

#include "nu2api.saga/nucore/common.h"

typedef enum {
    NUPRIM_SCALEMODE_PS2 = 0,
    NUPRIM_SCALEMODE_NORMALISED = 1,
    NUPRIM_SCALEMODE_ABSOLUTE = 2
} NUPRIMSCALEMODE;

#ifdef __cplusplus
extern "C" {
#endif
    void NuPrimSetCoordinateSystem(NUPRIMSCALEMODE scale_mode);
    void NuPrimPushCoordSystem(NUPRIMSCALEMODE scale_mode);
    void NuPrimInit(VARIPTR *buffer, VARIPTR buffer_end);
    void NuPrimReset(void);
#ifdef __cplusplus
}
#endif
