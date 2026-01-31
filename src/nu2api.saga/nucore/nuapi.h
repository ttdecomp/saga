#pragma once

#include "gamelib/nuwind/nuwind.h"
#include "nu2api.saga/nucore/nutime.h"
#include "nu2api.saga/nucore/nuvideo.h"

struct nuapi_s {
    int field0_0x0;
    int video_mode;
    NUVIDEO_SWAPMODE video_swap_mode;
    int video_aspect;
    int screen_width;
    int screen_height;
    int video_is_pal;
    float fps;
    int nuframe_begin_cnt;
    char field12_0x24;
    char field13_0x25;
    char field14_0x26;
    char field15_0x27;
    NUTIME time;
    NUTIME time2;
    float frametime;
    char field19_0x3c;
    char field20_0x3d;
    char field21_0x3e;
    char field22_0x3f;
    float forced_frame_time;
    float max_fps;
    char field25_0x48;
    char field26_0x49;
    char field27_0x4a;
    char field28_0x4b;
    char field29_0x4c;
    char field30_0x4d;
    char field31_0x4e;
    char field32_0x4f;
    char field33_0x50;
    char field34_0x51;
    char field35_0x52;
    char field36_0x53;
    char field37_0x54;
    char field38_0x55;
    char field39_0x56;
    char field40_0x57;
    int padrec_buffersize;
    char field42_0x5c;
    char field43_0x5d;
    char field44_0x5e;
    char field45_0x5f;
    char field46_0x60;
    char field47_0x61;
    char field48_0x62;
    char field49_0x63;
    int field50_0x64;
    int field51_0x68;
    int pad_recording;
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
    float video_brightness;
};

typedef struct nuapi_s NUAPI;

extern NUAPI nuapi;

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
