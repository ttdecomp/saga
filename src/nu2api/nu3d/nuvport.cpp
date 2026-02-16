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
