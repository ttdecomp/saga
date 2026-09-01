#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"

extern "C" {
    i32 NuRndrShadowCnt = 0;
    NURND_SHADOW_s NuRndrShadPolDat[128] = {};

    void NuRndrAddShadow(NUVEC *position, f32 radius, i32 opacity, i32 x_rotation, i32 y_rotation, i32 z_rotation) {
        i32 clipped = NuCameraClipTestPoints(position, 1, NULL);
        if (clipped == 0 && NuRndrShadowCnt <= 127) {
            NuRndrShadPolDat[NuRndrShadowCnt].position = *position;
            NuRndrShadPolDat[NuRndrShadowCnt].radius = radius;
            NuRndrShadPolDat[NuRndrShadowCnt].opacity = opacity;
            NuRndrShadPolDat[NuRndrShadowCnt].x_rotation = x_rotation;
            NuRndrShadPolDat[NuRndrShadowCnt].y_rotation = y_rotation;
            NuRndrShadPolDat[NuRndrShadowCnt].z_rotation = z_rotation;
            ++NuRndrShadowCnt;
        }
    }
}
