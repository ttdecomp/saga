#include "nu2api/nu3d/nuvport.h"

#include "decomp.h"

NUVIEWPORT vpCurrent = {0};
NUVIEWPORT vpDevice = {0};
i32 vport_inval = 0;
NUMTX vp_smtx = {0}; // scaling matrix

void NuPs2GetViewport(NUVIEWPORT *vp) {
    UNIMPLEMENTED();
}

void NuVpInit(void) {
    NuPs2GetViewport(&vpDevice);

    vpDevice.center_x = 0.5f;
    vpDevice.center_y = 0.5f;
    vpDevice.clip_min_x = 0.0f;
    vpDevice.clip_min_y = 0.0f;
    vpDevice.clip_max_x = 1.0f;
    vpDevice.clip_max_y = 1.0f;
    vpCurrent = vpDevice;
    vport_inval = 1;

    NuVpUpdate();
}

void NuVpRestore(void) {
    vpCurrent = vpDevice;
    vport_inval = 1;
}

void NuVpGetScalingMtx(NUMTX *dest) {
    if (dest == NULL) {
        return;
    }

    *dest = vp_smtx;
}

void NuVpUpdate(void) {
    UNIMPLEMENTED();
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
