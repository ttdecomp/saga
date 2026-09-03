#include "host/platform/free_camera.hpp"

#include <cstring>

#include "nu2api/nu3d/nucamera.h"

extern NUMTX clip_test_mtx;

extern "C" i32 NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world_mtx, f32 far_clip,
                                       i32 should_clip_to_screen) {
    if (HostFreeCameraActive()) {
        return 1;
    }

    NUVEC extents[8] = {
        {min->x, min->y, min->z}, {max->x, max->y, max->z}, {min->x, min->y, max->z}, {max->x, max->y, min->z},
        {min->x, max->y, min->z}, {min->x, max->y, max->z}, {max->x, min->y, min->z}, {max->x, min->y, max->z},
    };
    NUVEC clip_space_extents[8];
    char results[8];

    if (far_clip == 0.0f) {
        far_clip = global_camera.unknown_64 != 0.0f ? global_camera.unknown_64 : global_camera.far_clip;
    }
    if (world_mtx == NULL || world_mtx == &numtx_identity) {
        clip_test_mtx = vmtx;
    } else {
        NuMtxMul(&clip_test_mtx, world_mtx, &vmtx);
    }
    NuVecMtxTransformBlock(clip_space_extents, extents, &clip_test_mtx, 8);
    memset(results, 0, sizeof(results));

    for (i32 i = 0; i < 8; ++i) {
        if (clip_space_extents[i].x > clip_space_extents[i].z * zx) {
            results[i] |= 0x02;
        }
        if (clip_space_extents[i].y > clip_space_extents[i].z * zy) {
            results[i] |= 0x04;
        }
        if (clip_space_extents[i].z > far_clip) {
            results[i] |= 0x10;
        }
        if (clip_space_extents[i].x < -clip_space_extents[i].z * zx) {
            results[i] |= 0x01;
        }
        if (clip_space_extents[i].y < -clip_space_extents[i].z * zy) {
            results[i] |= 0x08;
        }
        if (clip_space_extents[i].z < global_camera.near_clip) {
            results[i] |= 0x20;
        }
    }

    const char all =
        results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7];
    if (all != 0) {
        return 0;
    }
    const char any =
        results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7];
    return any == 0 ? 1 : 2;
}

extern "C" i32 NuCameraClipTestExtentsAxisAligned(NUVEC *center, NUVEC *extent, f32 far_clip) {
    if (HostFreeCameraActive()) {
        return 1;
    }

    auto transform_planes = [](const NUVEC &point, const NUMTX &planes, f32 out[4]) {
        out[0] = point.x * planes.m00 + point.y * planes.m10 + point.z * planes.m20 + planes.m30;
        out[1] = point.x * planes.m01 + point.y * planes.m11 + point.z * planes.m21 + planes.m31;
        out[2] = point.x * planes.m02 + point.y * planes.m12 + point.z * planes.m22 + planes.m32;
        out[3] = point.x * planes.m03 + point.y * planes.m13 + point.z * planes.m23 + planes.m33;
    };
    auto project_extent = [](const NUVEC &value, const NUMTX &planes, f32 out[4]) {
        out[0] = value.x * planes.m00 + value.y * planes.m10 + value.z * planes.m20;
        out[1] = value.x * planes.m01 + value.y * planes.m11 + value.z * planes.m21;
        out[2] = value.x * planes.m02 + value.y * planes.m12 + value.z * planes.m22;
        out[3] = value.x * planes.m03 + value.y * planes.m13 + value.z * planes.m23;
    };

    f32 distance[6];
    f32 radius[6];
    transform_planes(*center, ClipPlanes.frustum_planes, distance);
    project_extent(*extent, ClipPlanes.abs_frustum_planes, radius);

    NUMTX near_far_planes = ClipPlanes.near_far_planes;
    if (far_clip != 0.0f) {
        near_far_planes.m30 += far_clip - global_camera.far_clip;
    }
    distance[4] = center->x * near_far_planes.m00 + center->y * near_far_planes.m10 + center->z * near_far_planes.m20 +
                  near_far_planes.m30;
    distance[5] = center->x * near_far_planes.m01 + center->y * near_far_planes.m11 + center->z * near_far_planes.m21 +
                  near_far_planes.m31;
    radius[4] = extent->x * near_far_planes.m02 + extent->y * near_far_planes.m12 + extent->z * near_far_planes.m22;
    radius[5] = extent->x * near_far_planes.m03 + extent->y * near_far_planes.m13 + extent->z * near_far_planes.m23;

    for (i32 plane = 0; plane < 6; ++plane) {
        if (distance[plane] < -radius[plane]) {
            return 0;
        }
    }
    for (i32 plane = 0; plane < 4; ++plane) {
        if (radius[plane] > distance[plane]) {
            f32 scissor_distance[4];
            f32 scissor_radius[4];
            transform_planes(*center, ClipPlanes.scissor_planes, scissor_distance);
            project_extent(*extent, ClipPlanes.abs_scissor_planes, scissor_radius);
            for (i32 scissor_plane = 0; scissor_plane < 4; ++scissor_plane) {
                if (scissor_radius[scissor_plane] > scissor_distance[scissor_plane]) {
                    return 2;
                }
            }
            break;
        }
    }
    return 1;
}

extern "C" i32 NuCameraClipTestSphere(NUVEC *point, f32 radius, NUMTX *world_mtx) {
    if (HostFreeCameraActive()) {
        return 0;
    }

    NUVEC camera_point;
    if (world_mtx != NULL) {
        NuVecMtxTransform(&camera_point, point, world_mtx);
        NuVecMtxTransform(&camera_point, &camera_point, NuCameraGetViewMtx());
    } else {
        NuVecMtxTransform(&camera_point, point, NuCameraGetViewMtx());
    }
    NUCAMERA *camera = NuCameraGetCam();
    if (0.0f > (camera_point.z - camera->near_clip) + radius || 0.0f > (camera->far_clip - camera_point.z) + radius) {
        return 1;
    }

    NUVEC4 planes;
    NuVec4MtxTransform(&planes, &camera_point, NuCameraGetClipPlanes());
    return -radius > planes.x || -radius > planes.y || -radius > planes.z || -radius > planes.w;
}
