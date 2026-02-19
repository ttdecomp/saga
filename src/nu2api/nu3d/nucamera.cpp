#include "nu2api/nu3d/nucamera.h"

#include <string.h>

NUMTX clip_test_mtx;

i32 NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world_mtx, f32 far_clip, i32 should_clip_to_screen) {
    NUVEC extents[8];
    NUVEC clip_space_extents[8];
    char results[8];
    i32 i;

    if (far_clip == 0.0f) {
        if (global_camera.unknown_64 != 0.0f) {
            far_clip = global_camera.unknown_64;
        } else {
            far_clip = global_camera.far_clip;
        }
    }

    if (world_mtx == NULL || world_mtx == &numtx_identity) {
        clip_test_mtx = vmtx;
    } else {
        NuMtxMul(&clip_test_mtx, world_mtx, &vmtx);
    }

    extents[0].x = min->x;
    extents[0].y = min->y;
    extents[0].z = min->z;

    extents[1].x = max->x;
    extents[1].y = max->y;
    extents[1].z = max->z;

    extents[2].x = min->x;
    extents[2].y = min->y;
    extents[2].z = max->z;

    extents[3].x = max->x;
    extents[3].y = max->y;
    extents[3].z = min->z;

    extents[4].x = min->x;
    extents[4].y = max->y;
    extents[4].z = min->z;

    extents[5].x = min->x;
    extents[5].y = max->y;
    extents[5].z = max->z;

    extents[6].x = max->x;
    extents[6].y = min->y;
    extents[6].z = min->z;

    extents[7].x = max->x;
    extents[7].y = min->y;
    extents[7].z = max->z;

    NuVecMtxTransformBlock(clip_space_extents, extents, &clip_test_mtx, 8);
    memset(results, 0, sizeof(results));

    for (i = 0; i < 8; i++) {
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

    if ((results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7]) != 0) {
        return 0;
    }

    if ((results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7]) == 0) {
        return 1;
    }

    return 2;
}
