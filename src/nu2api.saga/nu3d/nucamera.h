#pragma once

#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nuquat.h"

typedef struct nucamera_s {
    NUMTX mtx;
    f32 fov;
    f32 aspect;
    f32 nearclip;
    f32 farclip;
    NUVEC forward;
    NUVEC right;
    NUQUAT rotation;
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
    f32 NuCameraDist(nuvec_s *v);
    f32 NuCameraDistSqr(nuvec_s *v);
    NUCAMERA *NuCameraGetCam(void);
    NUMTX *NuCameraGetViewMtx(void);
    NUMTX *NuCameraGetClipPlanes(void);
    i32 NuCameraClipTestSphere(NUVEC *pnt, float radius, NUMTX *wm);
    void NuCameraGetPosition(NUVEC *v);
    void NuCameraGetTrans(NUVEC *v);
    NUMTX *NuCameraGetProjectionMtx(void);
    NUMTX *NuCameraGetScalingMtx(void);
    NUMTX *NuCameraGetMtx(void);
    void NuCameraCalcRay(float screen_x, float screen_y, nuvec_s *ray_start, nuvec_s *ray_end, nucamera_s *cam);
#ifdef __cplusplus
}
#endif
