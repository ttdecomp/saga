#include "nu2api.saga/nucore/nuapi.h"

#include <string.h>

NUAPI nuapi;

int nuapi_use_target_manager;
char *nuapi_target_manager_mac_address;

float nuapi_forced_frame_time;
float nuapi_max_fps;

static int NUAPI_PADREC_DEFAULT_BUFFERSIZE = 0x500000;

void NuAPIInit(void) {
    memset(&nuapi, 0, sizeof(NUAPI));

    nuapi.fps = 60.0f;
    nuapi.video_mode = NUVIDEOMODE_NTSC;
    nuapi.video_swap_mode = NUVIDEO_SWAPMODE_ROLLING;
    nuapi.field0_0x0 = 1;
    nuapi.video_aspect = 0;
    nuapi.disable_os_menu_freeze = 0;
    nuapi.forced_frame_time = nuapi_forced_frame_time;
    nuapi.max_fps = nuapi_max_fps;

    NuTimeGet(&nuapi.time);

    nuapi.pad_record.buf_size = NUAPI_PADREC_DEFAULT_BUFFERSIZE;
    nuapi.pad_record.end_record_buttons = 0;
    nuapi.pad_record.end_play_buttons = 0;

    NuWindInitialise(nuapi.wind);
}

void NuCommandLine(int argc, char **argv) {
}

void NuDisableOSMenuFreeze(void) {
    nuapi.disable_os_menu_freeze = 1;
}
