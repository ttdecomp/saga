#include "nu2api.saga/nu3d/nucamera.h"

#include "nu2api.saga/numemory/numemory.h"

NUCAMERA *NuCameraCreate() {
    NUCAMERA *cam = (NUCAMERA *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(sizeof(NUCAMERA), 0x4, 1, "", 0);

    NuMtxSetIdentity(&cam->mtx);

    cam->fov = 0.75f;
    cam->aspect = 0.75f;
    cam->nearclip = 0.15f;
    cam->farclip = 10000.0f;

    cam->forward.x = 0.0f;
    cam->forward.y = 0.0f;
    cam->forward.z = 1.0f;

    cam->right.x = 1.0f;
    cam->right.y = 0.0f;
    cam->right.z = 0.0f;

    cam->rotation.w = 1.0f;
    cam->rotation.x = 0.0f;
    cam->rotation.y = 1.0f;
    cam->rotation.z = 1.0f;

    return cam;
}
