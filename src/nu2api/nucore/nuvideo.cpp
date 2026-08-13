#include "nu2api/nucore/nuvideo.h"

#include "nu2api/nucore/nuapi.h"

i32 NuVideoGetMode(void) {
    return nuapi.video_mode;
}

void NuVideoSetMode(i32 mode) {
    nuapi.video_mode = mode;

    switch (nuapi.video_mode) {
        case NUVIDEOMODE_PAL:
        case NUVIDEOMODE_PALFF:
            nuapi.video_is_pal = 1;
            break;
        case NUVIDEOMODE_NTSC:
        case NUVIDEOMODE_NTSCFF:
        case NUVIDEOMODE_NTSC_J:
            nuapi.video_is_pal = 0;
            break;
        default:
            break;
    }
}

i32 NuVideoGetAspect(void) {
    return nuapi.video_aspect;
}

NUVIDEO_SWAPMODE NuVideoGetSwapMode(void) {
    return nuapi.video_swap_mode;
}

void NuVideoRollingFrameRateReset(void) {
}

void NuVideoSetBrightness(f32 brightness) {
    nuapi.video_brightness = brightness;

    NuVideoSetBrightnessPS();
}

void NuVideoSetResolution(i32 width, i32 height) {
    // It's unclear exactly what was here, but this matches and makes a sort of
    // sense.
    if (width == 0 | height == 0) {
    }

    nuapi.screen_width = width;
    nuapi.screen_height = height;
}

NUVIDEO_SWAPMODE NuVideoSetSwapMode(NUVIDEO_SWAPMODE video_swap_mode) {
    NUVIDEO_SWAPMODE swap_mode = nuapi.video_swap_mode;
    nuapi.video_swap_mode = video_swap_mode;

    switch (nuapi.video_swap_mode) {
        // notably ternarys DO NOT MATCH here :)
        case NUVIDEO_SWAPMODE_ASYNC:
            if (nuapi.video_is_pal) {
                nuapi.fps = 50.0f;
            } else {
                nuapi.fps = 60.0f;
            }
            break;
        case NUVIDEO_SWAPMODE_FIELDSYNC:
            if (nuapi.video_is_pal) {
                nuapi.fps = 50.0f;
            } else {
                nuapi.fps = 60.0f;
            }
            break;
        case NUVIDEO_SWAPMODE_FRAMESYNC:
            if (nuapi.video_is_pal) {
                nuapi.fps = 25.0f;
            } else {
                nuapi.fps = 30.0f;
            }
            break;
        case NUVIDEO_SWAPMODE_MIN2FIELD:
            if (nuapi.video_is_pal) {
                nuapi.fps = 25.0f;
            } else {
                nuapi.fps = 30.0f;
            }
            break;
        case NUVIDEO_SWAPMODE_ROLLING:
            if (nuapi.video_is_pal) {
                nuapi.fps = 50.0f;
            } else {
                nuapi.fps = 60.0f;
            }
            break;
        case NUVIDEO_SWAPMODE_ROLLING2FRAMES:
            if (nuapi.video_is_pal) {
                nuapi.fps = 25.0f;
            } else {
                nuapi.fps = 30.0f;
            }
            break;
        default:
            break;
    }

    NuVideoSetSwapModePS(video_swap_mode);
    return swap_mode;
}
