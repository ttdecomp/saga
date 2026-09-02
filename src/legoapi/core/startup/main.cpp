#include "legoapi/core/startup/main.h"
#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/android/NuThread_android.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nu3d/nutex.h"

#include <stdarg.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" {
    extern i32 FinishLoop_On;
    i32 NuRndrBeginScene(i32 flags);
    void NuRndrClear(i32 flags, i32 colour, f32 depth);
    void NuRndrEndScene(void);
    f32 NuFrameEnd(void);
    void edGraEnableTerrainSwap(void);
    void edGraDisableTerrainSwap(void);
}

void BackDrop_Draw(f32 alpha, i32 flags);
void NewGame(void);

void FinishLoop(i32 frames) {
    if (!FinishLoop_On) {
        return;
    }

    if (finishloop_backdroponly != 0) {
        pNuCam->mtx = numtx_identity;
        NuCameraSet(pNuCam);

        do {
            NuFrameBegin();
            NuRndrBeginScene(-1);
            NuRndrClear(0xb00, 0, 1.0f);
            BackDrop_Draw(1.0f, 1);
            NuRndrEndScene();
            edGraEnableTerrainSwap();
            NuFrameEnd();
            edGraDisableTerrainSwap();
        } while (--frames > 0);
        finishloop_backdroponly = 0;
        return;
    }

    if (FadeSys.pending_type == FADE_TYPE_NONE) {
        do {
            NuFrameBegin();
            NuRndrBeginScene(-1);
            NuRndrClear(0xb00, 0, 1.0f);
            NuRndrEndScene();
            edGraEnableTerrainSwap();
            NuFrameEnd();
            edGraDisableTerrainSwap();
        } while (--frames > 0);
    } else {
        do {
            NuFrameBegin();
            FadeSys.Draw();
            edGraEnableTerrainSwap();
            NuFrameEnd();
            edGraDisableTerrainSwap();
        } while (--frames > 0);
    }
}

void bgSuspendMain(i32) {
}

extern NULSTHDR *procinfo_pool;
extern i32 g_bgCritSec;
extern NuThreadSemaphore events[2];

BGPROCINFO *bgPostRequestV(bgprocdofn *do_fn, bgprocackfn *ack_fn, i32 first_type, ...) {
    NuThreadCriticalSectionBegin(g_bgCritSec);

    BGPROCINFO *info = reinterpret_cast<BGPROCINFO *>(NuLstAllocTail(procinfo_pool));
    if (info != NULL) {
        info->do_fn = do_fn;
        info->ack_fn = ack_fn;
        info->work_started = false;
        info->unknown_flag_2 = false;
        info->vars = reinterpret_cast<BGVAR *>(info->data);
        info->var_count = 0;

        BGVAR *var = info->vars;
        char *string_end = reinterpret_cast<char *>(&info->vars);
        BGPROCARGTYPE type = static_cast<BGPROCARGTYPE>(first_type);
        va_list args;
        va_start(args, first_type);

        while (type != BGPROC_ARG_END) {
            var->type = type;
            switch (type) {
                case BGPROC_ARG_I32:
                    var->value.i32_value = va_arg(args, i32);
                    break;
                case BGPROC_ARG_FLOAT:
                    // C varargs promote float to double.
                    var->value.float_value = static_cast<f32>(va_arg(args, f64));
                    break;
                case BGPROC_ARG_STRING: {
                    char *string = va_arg(args, char *);
                    string_end -= NuStrLen(string) + 1;
                    var->value.string_value = string_end;
                    NuStrCpy(string_end, string);
                    break;
                }
                case BGPROC_ARG_U32:
                    var->value.u32_value = va_arg(args, u32);
                    break;
                case BGPROC_ARG_POINTER:
                    var->value.pointer_value = va_arg(args, void *);
                    break;
                case BGPROC_ARG_BOOL:
                    var->value.i32_value = va_arg(args, i32);
                    break;
                case BGPROC_ARG_NONE:
                case BGPROC_ARG_END:
                    break;
            }

            ++info->var_count;
            ++var;
            type = static_cast<BGPROCARGTYPE>(va_arg(args, i32));
        }
        va_end(args);
    }

    NuThreadCriticalSectionEnd(g_bgCritSec);
    events[0].Signal();
    return info;
}

void EndOfDemo(i32) {
    NewLData = TITLES_LDATA;
    new_level_from_menu = 1;
    NewGame();
}

static __used__ void ThreadMain(u64 thread_data_value) {
    NULEGACYTHREADDATA *thread_data = reinterpret_cast<NULEGACYTHREADDATA *>(static_cast<usize>(thread_data_value));
    pthread_setspecific(g_currentThreadSpecificKey, thread_data);
    thread_data->thread_fn(thread_data->fn_arg);
    pthread_exit(NULL);
}
