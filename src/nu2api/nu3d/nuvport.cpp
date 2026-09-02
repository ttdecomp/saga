#include "nu2api/nu3d/nuvport.h"

#include "decomp.h"
#include "nu2api/nu3d/nurndr.h"

extern "C" void NuRndrStateUpdateCameraState(void);

static constexpr f32 kVirtualWidth = 640.0f;
static constexpr f32 kVirtualHeight = 224.0f;
static constexpr f32 kViewportCenterX = 2048.0f;
static constexpr f32 kViewportCenterY = 2048.0f;

NUVIEWPORT vpCurrent = {0};
NUVIEWPORT vpDevice = {0};
i32 vport_inval = 0;
NUMTX vp_smtx = {0}; // scaling matrix
NUVPREGION g_NuVpRegion = {
    0.0f, 0.0f, kVirtualWidth, kVirtualHeight, 0.0f, 0.0f, kVirtualWidth, kVirtualHeight,
    1.0f, 1.0f, 0.0f,          0.0f,           1.0f, 1.0f, 0.0f,          0.0f,
};

void NuPs2GetViewport(NUVIEWPORT *vp) {
    vp->x = (i32)((kViewportCenterX - (f32)nurndr_pixel_width * 0.5f) * 16.0f);
    vp->y = (i32)((kViewportCenterY - (f32)nurndr_pixel_height * 0.5f) * 16.0f);
    vp->width = (i32)((f32)nurndr_pixel_width * 10240.0f / kVirtualWidth);
    vp->height = (i32)((f32)nurndr_pixel_height * 3584.0f / kVirtualHeight);
    vp->min_z = 0.0f;
    vp->max_z = 1.0f;
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

void NuVpSetRegions(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x, f32 dest_y,
                    f32 dest_right, f32 dest_bottom) {
    g_NuVpRegion.source_x = source_x;
    g_NuVpRegion.source_y = source_y;
    g_NuVpRegion.source_width = source_right;
    g_NuVpRegion.source_height = source_bottom;
    g_NuVpRegion.dest_x = dest_x;
    g_NuVpRegion.dest_y = dest_y;
    g_NuVpRegion.dest_width = dest_right;
    g_NuVpRegion.dest_height = dest_bottom;
    g_NuVpRegion.width_scale = (dest_right - dest_x) / kVirtualWidth;
    g_NuVpRegion.height_scale = (dest_bottom - dest_y) / kVirtualHeight;
    g_NuVpRegion.projection_x_scale = kVirtualWidth / (source_right - source_x);
    g_NuVpRegion.projection_y_scale = kVirtualHeight / (source_bottom - source_y);
    g_NuVpRegion.projection_x_offset = ((kVirtualWidth - source_right) - source_x) * 2.0f / kVirtualWidth;
    g_NuVpRegion.projection_y_offset = ((kVirtualHeight - source_bottom) - source_y) * -2.0f / kVirtualHeight;
}

void NuVpResetRegions(void) {
    f32 height = (f32)PS2_VREZ_H;
    f32 width = (f32)PS2_VREZ_W;

    NuVpSetRegions(0.0f, 0.0f, width, height, 0.0f, 0.0f, width, height);
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
    if (vport_inval != 0) {
        vport_inval = 0;
        NuVpSetScalingMtx();
        vpCurrent.clip_height = 1.0f;
        vpCurrent.clip_width = 1.0f;
        NuRndrStateUpdateCameraState();
    }
}

NUVIEWPORT *NuVpGetCurrentViewport(void) {
    return &vpCurrent;
}

void NuVpGetPosition2(f32 *x, f32 *y) {
    const f32 x_scale = (f32)nurndr_pixel_width / kVirtualWidth;
    const f32 y_scale = (f32)nurndr_pixel_height / kVirtualHeight;
    *x = ((f32)vpCurrent.x * 0.0625f - (kViewportCenterX - (f32)(nurndr_pixel_width >> 1))) / x_scale;
    *y = ((f32)vpCurrent.y * 0.0625f - (kViewportCenterY - (f32)(nurndr_pixel_height >> 1))) / y_scale;
}

void NuVpGetSize2(f32 *width, f32 *height) {
    *width = ((f32)vpCurrent.width * 0.0625f) / ((f32)nurndr_pixel_width / kVirtualWidth);
    *height = ((f32)vpCurrent.height * 0.0625f) / ((f32)nurndr_pixel_height / kVirtualHeight);
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

static __used__ void NuVpSetClippingMtx(void) {
}
