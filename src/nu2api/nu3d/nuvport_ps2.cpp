#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuvport.h"

i32 PS2_VCNTR_X = 640;
i32 PS2_VCNTR_Y = 360;
i32 PS2_VREZ_W = 640;
i32 PS2_VREZ_H = 224;

extern NUVIEWPORT vpCurrent;
extern NUMTX vp_smtx;

void NuPs2GetViewport(NUVIEWPORT *vp) {
    vp->x = (i32)(((f32)PS2_VCNTR_X - (f32)nurndr_pixel_width * 0.5f) * 16.0f);
    vp->y = (i32)(((f32)PS2_VCNTR_Y - (f32)nurndr_pixel_height * 0.5f) * 16.0f);
    vp->width = (i32)((f32)nurndr_pixel_width * 10240.0f / (f32)PS2_VREZ_W);
    vp->height = (i32)((f32)nurndr_pixel_height * 3584.0f / (f32)PS2_VREZ_H);
    vp->min_z = 0.0f;
    vp->max_z = 1.0f;
}

void NuVpSetScalingMtx(void) {
    f32 vp_x = (f32)(vpCurrent.x >> 4);
    f32 vp_y = (f32)(vpCurrent.y >> 4);
    f32 vp_w = (f32)(vpCurrent.width >> 4);
    f32 vp_h = (f32)(vpCurrent.height >> 4);
    f32 vp_minz = vpCurrent.min_z;
    f32 vp_maxz = vpCurrent.max_z;

    vp_smtx.m00 = vp_w * 0.5f;
    vp_smtx.m01 = 0.0f;
    vp_smtx.m02 = 0.0f;
    vp_smtx.m03 = 0.0f;
    vp_smtx.m10 = 0.0f;
    vp_smtx.m11 = -vp_h * 0.5f;
    vp_smtx.m12 = 0.0f;
    vp_smtx.m13 = 0.0f;
    vp_smtx.m20 = 0.0f;
    vp_smtx.m21 = 0.0f;
    vp_smtx.m22 = vp_maxz - vp_minz;
    vp_smtx.m23 = 0.0f;
    vp_smtx.m30 = vpCurrent.center_x * vp_w + vp_x;
    vp_smtx.m31 = vpCurrent.center_y * vp_h + vp_y;
    vp_smtx.m32 = vp_minz;
    vp_smtx.m33 = 1.0f;
}
