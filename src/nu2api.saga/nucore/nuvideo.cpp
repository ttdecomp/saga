#include "nu2api.saga/nucore/nuvideo.h"

#include <assert.h>

#include "nu2api.saga/nucore/nuapi.h"

int NuVideoGetMode(void) {
    return nuapi.video_mode;
}

void NuVideoSetMode(int mode) {
    nuapi.video_mode = mode;

    // this ordering is required for a match...
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

int NuVideoGetAspect(void) {
    return nuapi.video_aspect;
}

int NuVideoGetAspectPS(void) {
    return 3;
}

NUVIDEO_SWAPMODE NuVideoGetSwapMode(void) {
    return nuapi.video_swap_mode;
}

void NuVideoRollingFrameRateReset(void) {
}

void NuVideoSetBrightness(float brightness) {
    nuapi.video_brightness = brightness; // this has a weird register swap issue with the matching...
    NuVideoSetBrightnessPS();
}

void NuVideoSetBrightnessPS() {
}

void NuVideoSetResolution(int width, int height) {
    // 100% asm match btw...
    __asm__("cmp $0, %0" : : "m" (width));
    __asm__("cmp $0, %0" : : "m" (height));

    ((volatile NUAPI*)&nuapi)->screen_width = width;
    ((volatile NUAPI*)&nuapi)->screen_height = height;
}

void NuVideoSetSwapModePS(NUVIDEO_SWAPMODE video_swap_mode) {
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