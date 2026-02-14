#include "nu2api.saga/nucore/nuapi.h"

#include <stdarg.h>
#include <string.h>

#include "decomp.h"

#include "nu2api.saga/nu3d/numtl.h"
#include "nu2api.saga/nu3d/nuocclusion.h"
#include "nu2api.saga/nu3d/nuportal.h"
#include "nu2api.saga/nu3d/nuprim.h"
#include "nu2api.saga/nu3d/nuwater.h"
#include "nu2api.saga/nucore/nukeyboard.h"
#include "nu2api.saga/nucore/numouse.h"

NUAPI nuapi;

int nuapi_use_target_manager;
char *nuapi_target_manager_mac_address;

float nuapi_forced_frame_time;
i32 nuapi_max_fps = 60;

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

i32 NuInitHardware(VARIPTR *buf, VARIPTR *buf_end, int heap_size, ...) {
    i32 hostfs = 0;
    i32 streamsize = 0x200000;
    nupad_s *pad0 = NULL;
    nupad_s *pad1 = NULL;
    i32 videomode = 2;
    i32 resolution_x = 0;
    i32 resolution_y = 0;
    NUVIDEO_SWAPMODE swapmode = NUVIDEO_SWAPMODE_FIELDSYNC;
    i32 flags = 0;

    va_list args;
    va_start(args, heap_size);

    i32 setup_tok;
    do {
        setup_tok = va_arg(args, i32);

        LOG_DEBUG("NuInitHardware setup=%d", setup_tok);

        switch (setup_tok) {
            case NUAPI_SETUP_HOSTFS:
                hostfs = va_arg(args, i32);
                break;
            case NUAPI_SETUP_STREAMSIZE:
                streamsize = va_arg(args, i32);
                break;
            case NUAPI_SETUP_PAD0:
                pad0 = va_arg(args, NUPAD *);
                break;
            case NUAPI_SETUP_PAD1:
                pad1 = va_arg(args, NUPAD *);
                break;
            case NUAPI_SETUP_VIDEOMODE:
                videomode = va_arg(args, i32);
                break;
            case NUAPI_SETUP_RESOLUTION:
                resolution_x = va_arg(args, i32);
                resolution_y = va_arg(args, i32);
                break;
            case NUAPI_SETUP_SWAPMODE:
                swapmode = (NUVIDEO_SWAPMODE)va_arg(args, i32);
                break;
            case NUAPI_SETUP_0x46:
                if (va_arg(args, i32) != 0) {
                    flags |= 0x4;
                }
                break;
            case NUAPI_SETUP_0x47:
                if (va_arg(args, i32) != 0) {
                    flags |= 0x8;
                }
                break;
            case NUAPI_SETUP_0x49:
                if (va_arg(args, i32) != 0) {
                    flags |= 0x20;
                }
                break;
            case NUAPI_SETUP_0x4b:
                if (va_arg(args, i32) != 0) {
                    flags |= 0x80;
                }
                break;
            default:
                if (NuInitHardwareParseArgsPS(setup_tok, va_arg(args, char **)) == 0) {
                    switch (setup_tok) {
                        case NUAPI_SETUP_CDDVDMODE:
                            break;
                        case NUAPI_SETUP_GLASSRPLANE:
                            break;
                    }
                }
                break;
        }
    } while (setup_tok != NUAPI_SETUP_END);

    va_end(args);

    NuInitHardwarePS(buf, buf_end, heap_size);
    NuMtlInitEx(buf, 512);

    return 0;
}

void NuFrameBegin(void) {
    nuapi.nuframe_begin_cnt++;
    NuPortalInit();
    NuPrimReset();
    NuWaterReset();
    NuMouseRead();
    NuKeyboardRead();
    NuOcclusionManagerBeginFrame();
}
