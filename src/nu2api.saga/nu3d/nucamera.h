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

extern NUMTX clip_planes;
extern NUCAMERA global_camera;
extern NUMTX vmtx;

#ifdef __cplusplus
extern "C" {
#endif
    NUCAMERA *NuCameraCreate(void);
    void NuCameraDestroy(NUCAMERA* cam);
    float NuCameraDist(nuvec_s* v);
    float NuCameraDistSqr(nuvec_s* v);
    NUCAMERA* NuCameraGetCam(void);
    NUMTX* NuCameraGetViewMtx(void);
    NUMTX* NuCameraGetClipPlanes(void);
    int NuCameraClipTestSphere(NUVEC* pnt, float radius, NUMTX* wm);
#ifdef __cplusplus
}
#endif
