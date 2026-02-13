#include "nu2api.saga/nu3d/nucamera.h"

#include <stdlib.h>
#include <string.h>

#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/numath/nufloat.h"
#include "nu2api.saga/numath/numtx.h"
#include "nu2api.saga/numath/nutrig.h"
#include "nu2api.saga/numath/nuvec.h"
#include "nu2api.saga/numath/nuvec4.h"

NUCAMERA global_camera;

NUMTX clip_planes = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUMTX pmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUMTX smtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};
NUMTX vmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUMTX vpmtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUMTX vpc_sci_mtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUMTX vpc_vport_mtx = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

NUCLIPPLANES ClipPlanes;

f32 zx;
f32 zy;

f32 zxs;
f32 zys;

NUCAMERA *NuCameraCreate() {
    NUCAMERA *cam = NU_ALLOC_T(NUCAMERA, 1, "", NUMEMORY_CATEGORY_NONE);

    NuMtxSetIdentity(&cam->mtx);

    cam->near_clip = 0.15f;
    cam->far_clip = 10000.0f;
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

NUMTX *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}

void NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor) {
    if (viewport != NULL) {
        memcpy(viewport, &vpc_vport_mtx, sizeof(NUMTX));
    }

    if (scissor != NULL) {
        memcpy(scissor, &vpc_sci_mtx, sizeof(NUMTX));
    }
}

NUMTX *NuCameraGetProjectionMtx(void) {
    return &pmtx;
}

NUMTX *NuCameraGetScalingMtx(void) {
    return &smtx;
}

NUMTX *NuCameraGetViewMtx(void) {
    return &vmtx;
}

NUMTX *NuCameraGetVPMtx(void) {
    return &vpmtx;
}

NUMTX *NuCameraGetVPCMtx(void) {
    return &vpc_vport_mtx;
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

    if (0.0f > (pnt2.z - cam->near_clip) + radius) {
        return 1;
    }
    if (0.0f > (cam->far_clip - pnt2.z) + radius) {
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

    near_point.x *= cam->near_clip;
    near_point.y *= cam->near_clip;
    near_point.z *= cam->near_clip;
    far_point.x *= cam->far_clip;
    far_point.y *= cam->far_clip;
    far_point.z *= cam->far_clip;

    NuVecMtxTransform(ray_start, &near_point, &cam->mtx);
    NuVecMtxTransform(ray_end, &far_point, &cam->mtx);
}

void NuCameraRayCast(NUVEC *pnt, f32 x, f32 y) {
    pnt->x = x * zx;
    pnt->y = y * zy;
    pnt->z = 1.0f;

    NuVecMtxRotate(pnt, pnt, &global_camera.mtx);
}

void NuCameraBuildClipPlanes(void) {
    f32 near_dist;
    f32 near_dist_sq;

    f32 left_right;
    f32 top_bottom;

    f32 left_right_inv;
    f32 top_bottom_inv;

    NUMTX frustum_planes;
    NUMTX scissor_planes;

    f32 dot;

    near_dist = global_camera.near_clip;
    near_dist_sq = global_camera.near_clip * global_camera.near_clip;

    // Build the frustum planes.
    left_right = zx * global_camera.near_clip;
    top_bottom = zy * global_camera.near_clip;

    left_right_inv = 1.0f / NuFsqrt(left_right * left_right + near_dist_sq);
    top_bottom_inv = 1.0f / NuFsqrt(top_bottom * top_bottom + near_dist_sq);

    NuMtxSetZero(&frustum_planes);

    frustum_planes.m00 = -near_dist * left_right_inv;
    frustum_planes.m12 = -near_dist * top_bottom_inv;
    frustum_planes.m20 = left_right * left_right_inv;
    frustum_planes.m22 = top_bottom * top_bottom_inv;

    frustum_planes.m01 = -frustum_planes.m00;
    frustum_planes.m13 = -frustum_planes.m12;
    frustum_planes.m21 = frustum_planes.m20;
    frustum_planes.m23 = frustum_planes.m22;

    // Build the scissor planes.
    left_right = zxs * near_dist;
    top_bottom = zys * near_dist;

    left_right_inv = 1.0f / NuFsqrt(left_right * left_right + near_dist_sq);
    top_bottom_inv = 1.0f / NuFsqrt(top_bottom * top_bottom + near_dist_sq);

    NuMtxSetZero(&scissor_planes);

    scissor_planes.m00 = -near_dist * left_right_inv;
    scissor_planes.m12 = -near_dist * top_bottom_inv;
    scissor_planes.m20 = left_right * left_right_inv;
    scissor_planes.m22 = near_dist * top_bottom_inv;

    scissor_planes.m01 = -scissor_planes.m00;
    scissor_planes.m13 = -scissor_planes.m12;
    scissor_planes.m21 = scissor_planes.m20;
    scissor_planes.m23 = scissor_planes.m22;

    NuMtxMulH(&ClipPlanes.frustum_planes, &vmtx, &frustum_planes);
    NuMtxMulH(&ClipPlanes.scissor_planes, &vmtx, &scissor_planes);

    dot = global_camera.mtx.m30 * global_camera.mtx.m20 + global_camera.mtx.m31 * global_camera.mtx.m21 +
          global_camera.mtx.m32 * global_camera.mtx.m22;

    ClipPlanes.near_plane.x = global_camera.mtx.m20;
    ClipPlanes.near_plane.y = global_camera.mtx.m21;
    ClipPlanes.near_plane.z = global_camera.mtx.m22;
    ClipPlanes.near_plane.w = -dot - global_camera.near_clip;

    ClipPlanes.near_far_planes.m00 = -global_camera.mtx.m20;
    ClipPlanes.near_far_planes.m10 = -global_camera.mtx.m21;
    ClipPlanes.near_far_planes.m20 = -global_camera.mtx.m22;
    ClipPlanes.near_far_planes.m30 = dot + global_camera.far_clip;

    ClipPlanes.near_far_planes.m01 = ClipPlanes.near_plane.x;
    ClipPlanes.near_far_planes.m11 = ClipPlanes.near_plane.y;
    ClipPlanes.near_far_planes.m21 = ClipPlanes.near_plane.z;
    ClipPlanes.near_far_planes.m31 = ClipPlanes.near_plane.w;

    ClipPlanes.near_far_planes.m02 = NuFabs(ClipPlanes.near_far_planes.m00);
    ClipPlanes.near_far_planes.m12 = NuFabs(ClipPlanes.near_far_planes.m10);
    ClipPlanes.near_far_planes.m22 = NuFabs(ClipPlanes.near_far_planes.m20);
    ClipPlanes.near_far_planes.m32 = NuFabs(ClipPlanes.near_far_planes.m30);

    ClipPlanes.near_far_planes.m03 = NuFabs(ClipPlanes.near_far_planes.m01);
    ClipPlanes.near_far_planes.m13 = NuFabs(ClipPlanes.near_far_planes.m11);
    ClipPlanes.near_far_planes.m23 = NuFabs(ClipPlanes.near_far_planes.m21);
    ClipPlanes.near_far_planes.m33 = NuFabs(ClipPlanes.near_far_planes.m31);

    ClipPlanes.abs_frustum_planes.m00 = NuFabs(ClipPlanes.frustum_planes.m00);
    ClipPlanes.abs_frustum_planes.m01 = NuFabs(ClipPlanes.frustum_planes.m01);
    ClipPlanes.abs_frustum_planes.m02 = NuFabs(ClipPlanes.frustum_planes.m02);
    ClipPlanes.abs_frustum_planes.m03 = NuFabs(ClipPlanes.frustum_planes.m03);

    ClipPlanes.abs_frustum_planes.m10 = NuFabs(ClipPlanes.frustum_planes.m10);
    ClipPlanes.abs_frustum_planes.m11 = NuFabs(ClipPlanes.frustum_planes.m11);
    ClipPlanes.abs_frustum_planes.m12 = NuFabs(ClipPlanes.frustum_planes.m12);
    ClipPlanes.abs_frustum_planes.m13 = NuFabs(ClipPlanes.frustum_planes.m13);

    ClipPlanes.abs_frustum_planes.m20 = NuFabs(ClipPlanes.frustum_planes.m20);
    ClipPlanes.abs_frustum_planes.m21 = NuFabs(ClipPlanes.frustum_planes.m21);
    ClipPlanes.abs_frustum_planes.m22 = NuFabs(ClipPlanes.frustum_planes.m22);
    ClipPlanes.abs_frustum_planes.m23 = NuFabs(ClipPlanes.frustum_planes.m23);

    ClipPlanes.abs_frustum_planes.m30 = 0.0f;
    ClipPlanes.abs_frustum_planes.m31 = 0.0f;
    ClipPlanes.abs_frustum_planes.m32 = 0.0f;
    ClipPlanes.abs_frustum_planes.m33 = 0.0f;

    ClipPlanes.abs_scissor_planes.m00 = NuFabs(ClipPlanes.scissor_planes.m00);
    ClipPlanes.abs_scissor_planes.m01 = NuFabs(ClipPlanes.scissor_planes.m01);
    ClipPlanes.abs_scissor_planes.m02 = NuFabs(ClipPlanes.scissor_planes.m02);
    ClipPlanes.abs_scissor_planes.m03 = NuFabs(ClipPlanes.scissor_planes.m03);

    ClipPlanes.abs_scissor_planes.m10 = NuFabs(ClipPlanes.scissor_planes.m10);
    ClipPlanes.abs_scissor_planes.m11 = NuFabs(ClipPlanes.scissor_planes.m11);
    ClipPlanes.abs_scissor_planes.m12 = NuFabs(ClipPlanes.scissor_planes.m12);
    ClipPlanes.abs_scissor_planes.m13 = NuFabs(ClipPlanes.scissor_planes.m13);

    ClipPlanes.abs_scissor_planes.m20 = NuFabs(ClipPlanes.scissor_planes.m20);
    ClipPlanes.abs_scissor_planes.m21 = NuFabs(ClipPlanes.scissor_planes.m21);
    ClipPlanes.abs_scissor_planes.m22 = NuFabs(ClipPlanes.scissor_planes.m22);
    ClipPlanes.abs_scissor_planes.m23 = NuFabs(ClipPlanes.scissor_planes.m23);

    ClipPlanes.abs_scissor_planes.m30 = 0.0f;
    ClipPlanes.abs_scissor_planes.m31 = 0.0f;
    ClipPlanes.abs_scissor_planes.m32 = 0.0f;
    ClipPlanes.abs_scissor_planes.m33 = 0.0f;
}
