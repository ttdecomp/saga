#pragma once

#include "gamelib/nuwind/nuwind.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nucore/nuvideo.h"

typedef struct nuapi_s {
    i32 language;
    i32 video_mode;
    NUVIDEO_SWAPMODE video_swap_mode;
    i32 video_aspect;
    i32 screen_width;
    i32 screen_height;
    i32 video_is_pal;
    f32 fps;
    i32 nuframe_begin_cnt;
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
    i32 max_fps;

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
    i32 reversible_characters;
    i32 loading_hgobj;
    i32 force_shadows_on_characters;
    i32 disable_os_menu_freeze;
    f32 video_brightness;
} NUAPI;

extern NUAPI nuapi;

enum {
    NUAPI_SETUP_END = 0x00,
    NUAPI_SETUP_HOSTFS = 0x04,
    NUAPI_SETUP_CDDVDMODE = 0x05,
    NUAPI_SETUP_STREAMSIZE = 0x08,
    NUAPI_SETUP_AUDIO = 0x0a,
    NUAPI_SETUP_PAD0 = 0x0e,
    NUAPI_SETUP_PAD1 = 0x0f,
    NUAPI_SETUP_VIDEOMODE = 0x12,
    NUAPI_SETUP_GLASSRPLANE = 0x15,
    NUAPI_SETUP_AUDIO_DISABLED = 0x1c,
    NUAPI_SETUP_RESOLUTION = 0x21,
    NUAPI_SETUP_SWAPMODE = 0x22,
    NUAPI_SETUP_0x46 = 0x46,
    NUAPI_SETUP_0x47 = 0x47,
    NUAPI_SETUP_0x49 = 0x49,
    NUAPI_SETUP_0x4b = 0x4b,
};

extern i32 nuapi_use_target_manager;
extern char *nuapi_target_manager_mac_address;

#ifdef __cplusplus
void NuAPIInit(void);

extern "C" {
#endif
    i32 NuInitHardware(VARIPTR *buf, VARIPTR *buf_end, i32 heap_size, ...);

    void NuCommandLine(i32 *argc, char ***argv);
    void NuDisableOSMenuFreeze(void);

    void NudxFw_D3DBeginCriticalSection(void);
    void NudxFw_D3DEndCriticalSection(void);
    void NuFrameBegin(void);
    i32 NuLanguageGet(void);
    void NuLanguageSet(i32 language);
#ifdef __cplusplus
}
#endif

i32 NuInitHardwarePS(VARIPTR *buf, VARIPTR *buf_end, i32 heap_size);

i32 NuInitHardwareParseArgsPS(i32 setup_tok, char **args);
