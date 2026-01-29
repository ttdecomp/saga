#include "nu2api.saga/nu3d/nucamera.h"

#include "nu2api.saga/numemory/numemory.h"

nucamera_s *NuCameraCreate(void) {
    nucamera_s *cam = (nucamera_s *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(0x78, 4, 1, "", 0);

    NuMtxSetIdentity((numtx_s *)cam);

    NuMtxSetIdentity(&cam->mtx);
    cam->nearclip = 0.15;
    cam->farclip = 10000.0;
    cam->fov = 0.75;
    cam->aspect = 0.75;
    cam->rotation.x = 0.0;
    cam->forward.z = 1.0;
    cam->right.x = 1.0;
    cam->forward.x = 0.0;
    cam->forward.y = 0.0;
    cam->rotation.w = 1.0;
    cam->rotation.z = 1.0;
    cam->rotation.y = 1.0;
    cam->right.z = 0.0;
    cam->right.y = 0.0;

    return cam;
}