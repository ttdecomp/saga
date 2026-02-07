#pragma once

#include "gamelib/nuwind/nuwind.h"
#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nupad.h"
#include "nu2api.saga/nucore/nutime.h"
#include "nu2api.saga/nucore/nuvideo.h"

typedef struct nuapi_s {
    int field0_0x0;
    int video_mode;
    NUVIDEO_SWAPMODE video_swap_mode;
    int video_aspect;
    int screen_width;
    int screen_height;
    int video_is_pal;
    f32 fps;
    int nuframe_begin_cnt;
    char field12_0x24;
    char field13_0x25;
    char field14_0x26;
    char field15_0x27;
    NUTIME time;
    NUTIME time2;
    f32 frametime;
    char field19_0x3c;
    char field20_0x3d;
    char field21_0x3e;
    char field22_0x3f;

    f32 forced_frame_time;
    f32 max_fps;

    NUPADREC pad_record;

    char field56_0x70;
    char field57_0x71;
    char field58_0x72;
    char field59_0x73;
    char field60_0x74;
    char field61_0x75;
    char field62_0x76;
    char field63_0x77;
    NUWIND *wind;
    char field65_0x7c;
    char field66_0x7d;
    char field67_0x7e;
    char field68_0x7f;
    char field69_0x80;
    char field70_0x81;
    char field71_0x82;
    char field72_0x83;
    char field73_0x84;
    char field74_0x85;
    char field75_0x86;
    char field76_0x87;
    int disable_os_menu_freeze;
    f32 video_brightness;
} NUAPI;

extern NUAPI nuapi;

extern int nuapi_use_target_manager;
extern char *nuapi_target_manager_mac_address;

#ifdef __cplusplus
void NuAPIInit(void);

extern "C" {
#endif
    void NuCommandLine(int argc, char **argv);
    void NuDisableOSMenuFreeze(void);

    void NudxFw_D3DBeginCriticalSection();
    void NudxFw_D3DEndCriticalSection();
#ifdef __cplusplus
}
#endif
