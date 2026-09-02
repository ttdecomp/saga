#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nu3d/nurndr.h"

extern "C" {

    f32 NuIOS_GetAspectRatio(void) {
        return static_cast<f32>(nurndr_pixel_width) / static_cast<f32>(nurndr_pixel_height);
    }

    i32 NuIOS_IsWidescreen(void) {
        return NuIOS_GetAspectRatio() > 1.5f;
    }

} // extern "C"
