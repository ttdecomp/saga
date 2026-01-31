#include "nu2api.saga/nu3d/nucamera.h"

#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nuvec4.h"
#include "nu2api.saga/numemory/numemory.h"

NUMTX clip_planes = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUCAMERA global_camera;
NUMTX vmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUCAMERA *NuCameraCreate() {
    NUCAMERA *cam = (NUCAMERA *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(sizeof(NUCAMERA), 0x4, 1, "", 0);

    NuMtxSetIdentity(&cam->mtx);

    cam->nearclip = 0.15f;
    cam->farclip = 10000.0f;
    cam->rotation.x = 0.0f;
    cam->fov = 0.75f;
    cam->aspect = 0.75f;
    cam->forward.z = 1.0f;
    cam->right.x = 1.0f;
    cam->forward.x = 0.0f;
    cam->forward.y = 0.0f;
    cam->rotation.w = 1.0f;
    cam->rotation.z = 1.0f;
    cam->rotation.y = 1.0f;
    cam->right.z = 0.0f;
    cam->right.y = 0.0f;

    return cam;
}

void NuCameraDestroy(NUCAMERA *cam) {
    if (cam != NULL) {
        NuMemoryGet()->GetThreadMem()->BlockFree(cam, 0);
    }
}

float NuCameraDist(NUVEC *v) {
    return NuVecDist(v, NUMTX_GET_ROW_VEC(&global_camera.mtx, 3), NULL);
}

float NuCameraDistSqr(NUVEC *v) {
    return NuVecDistSqr(v, NUMTX_GET_ROW_VEC(&global_camera.mtx, 3), NULL);
}

NUCAMERA *NuCameraGetCam(void) {
    return &global_camera;
}

NUMTX *NuCameraGetViewMtx(void) {
    return &vmtx;
}

NUMTX *NuCameraGetClipPlanes(void) {
    return &clip_planes;
}

int NuCameraClipTestSphere(NUVEC *pnt, float radius, NUMTX *wm) {
    NUCAMERA *cam = NuCameraGetCam();
    NUMTX *view = NuCameraGetViewMtx();

    NUVEC pnt2;
    if (wm != NULL) {
        NuVecMtxTransform(&pnt2, pnt, wm);
        NuVecMtxTransform(&pnt2, &pnt2, view);
    } else {
        NuVecMtxTransform(&pnt2, pnt, view);
    }

    if (0.0f > (pnt2.z - cam->nearclip) + radius) {
        return 1;
    }
    if (0.0f > (cam->farclip - pnt2.z) + radius) {
        return 1;
    }

    NUVEC4 g_dot;
    NuVec4MtxTransform(&g_dot, &pnt2, NuCameraGetClipPlanes());
    if (-radius > g_dot.x) {
        return 1;
    }
    if (-radius > g_dot.y) {
        return 1;
    }
    if (-radius > g_dot.z) {
        return 1;
    }
    if (-radius > g_dot.w) {
        return 1;
    }
    
    return 0;
}