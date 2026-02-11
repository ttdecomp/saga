#pragma once

#include "nu2api.saga/numath/numtx.h"

typedef struct nucamera_s {
    NUMTX mtx;
    f32 fov;
    f32 aspect;
    f32 nearclip;
    f32 farclip;

    f32 unknown_50;
    f32 unknown_54;
    f32 unknown_58;
    f32 unknown_5c;
    f32 unknown_60;
    f32 unknown_64;

    f32 portal_nearclip;

    NUVEC scale;
} NUCAMERA;

extern NUMTX clip_planes;
extern NUCAMERA global_camera;
extern NUMTX vmtx;
extern NUMTX pmtx;
extern NUMTX smtx;

#ifdef __cplusplus
void NuCameraBuildClipPlanes(void);

extern "C" {
#endif
    NUCAMERA *NuCameraCreate(void);
    void NuCameraDestroy(NUCAMERA *cam);
    f32 NuCameraDist(NUVEC *v);
    f32 NuCameraDistSqr(NUVEC *v);
    NUCAMERA *NuCameraGetCam(void);
    NUMTX *NuCameraGetViewMtx(void);
    NUMTX *NuCameraGetClipPlanes(void);
    i32 NuCameraClipTestSphere(NUVEC *pnt, float radius, NUMTX *wm);
    void NuCameraGetPosition(NUVEC *v);
    void NuCameraGetTrans(NUVEC *v);
    NUMTX *NuCameraGetProjectionMtx(void);
    NUMTX *NuCameraGetScalingMtx(void);
    NUMTX *NuCameraGetMtx(void);
    void NuCameraCalcRay(float screen_x, float screen_y, NUVEC *ray_start, NUVEC *ray_end, NUCAMERA *cam);
#ifdef __cplusplus
}
#endif
