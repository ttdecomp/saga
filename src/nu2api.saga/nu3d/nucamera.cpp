#include "nu2api.saga/nu3d/nucamera.h"

#include "decomp.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nutrig.h"
#include "nu2api.saga/numath/nuvec4.h"

#include <string.h>

NUMTX clip_planes = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUCAMERA global_camera;
NUMTX vmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUMTX pmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUMTX smtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUCAMERA *NuCameraCreate() {
    NUCAMERA *cam = (NUCAMERA *)NuMemoryGet()->GetThreadMem()->_BlockAlloc(sizeof(NUCAMERA), 0x4, 1, "", 0);

    NuMtxSetIdentity(&cam->mtx);

    cam->nearclip = 0.15f;
    cam->farclip = 10000.0f;
    cam->portal_nearclip = 0.0f;
    cam->fov = 0.75f;
    cam->aspect = 0.75f;

    cam->unknown_58 = 1.0f;
    cam->unknown_5c = 1.0f;
    cam->unknown_50 = 0.0f;
    cam->unknown_54 = 0.0f;

    cam->scale.z = 1.0f;
    cam->scale.y = 1.0f;
    cam->scale.x = 1.0f;

    cam->unknown_64 = 0.0f;
    cam->unknown_60 = 0.0f;

    return cam;
}

void NuCameraDestroy(NUCAMERA *cam) {
    if (cam != NULL) {
        NuMemoryGet()->GetThreadMem()->BlockFree(cam, 0);
    }
}

f32 NuCameraDist(NUVEC *v) {
    return NuVecDist(v, NUMTX_GET_ROW_VEC(&global_camera.mtx, 3), NULL);
}

f32 NuCameraDistSqr(NUVEC *v) {
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

i32 NuCameraClipTestSphere(NUVEC *pnt, f32 radius, NUMTX *wm) {
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

void NuCameraGetPosition(NUVEC *v) {
    memcpy(v, &global_camera.mtx.m30, sizeof(NUVEC));
}

void NuCameraGetTrans(NUVEC *v) {
    memcpy(v, &global_camera.mtx.m30, sizeof(NUVEC));
}

NUMTX *NuCameraGetProjectionMtx(void) {
    return &pmtx;
}

NUMTX *NuCameraGetScalingMtx(void) {
    return &smtx;
}

NUMTX *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}

void NuCameraCalcRay(float screen_x, float screen_y, NUVEC *ray_start, NUVEC *ray_end, NUCAMERA *cam) {
    NUVEC near_point;
    NUVEC far_point;

    if (cam == NULL) {
        cam = &global_camera;
    }

    float tan = NU_TAN_LUT(cam->fov * 0.5f * 10430.378f);
    near_point = far_point = {
        .x = (screen_x + screen_x - 1.0f) * tan / cam->aspect,
        .y = (1.0f - (screen_y + screen_y)) * tan,
        .z = 1.0f,
    };

    near_point.x *= cam->nearclip;
    near_point.y *= cam->nearclip;
    near_point.z *= cam->nearclip;
    far_point.x *= cam->farclip;
    far_point.y *= cam->farclip;
    far_point.z *= cam->farclip;

    NuVecMtxTransform(ray_start, &near_point, &cam->mtx);
    NuVecMtxTransform(ray_end, &far_point, &cam->mtx);
}

void NuCameraBuildClipPlanes(void) {
    UNIMPLEMENTED();
}
