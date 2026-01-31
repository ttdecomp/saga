#pragma once

#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nuquat.h"

typedef struct nucamera_s {
    NUMTX mtx;
    float fov;
    float aspect;
    float nearclip;
    float farclip;
    NUVEC forward;
    NUVEC right;
    NUQUAT rotation;
} NUCAMERA;

#ifdef __cplusplus
extern "C" {
#endif
    NUCAMERA *NuCameraCreate(void);
#ifdef __cplusplus
}
#endif
