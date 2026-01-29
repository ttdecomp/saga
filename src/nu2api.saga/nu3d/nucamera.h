#pragma once

#include "decomp.h"

#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nuvec4.h"

struct nucamera_s {
    numtx_s mtx;
    float fov;
    float aspect;
    float nearclip;
    float farclip;
    NUVEC forward;
    NUVEC right;
    NUQUAT rotation;
};

typedef struct nucamera_s NUCAMERA;

C_API_START

NUCAMERA *NuCameraCreate(void);

C_API_END