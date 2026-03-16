#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nucore.hpp"
#include "decomp.h"
#include "globals.h"
#include <string.h>
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/android/nurenderthread.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nucore/numemory.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "gamelib/nuwind/nuwind.h"



i32 nurndr_pixel_width;
i32 nurndr_pixel_height;
i32 unknown_timer;



void NuRndrInitEx(i32 stream_buffer_size, VARIPTR *buffer) {
    NuRndrStreamInit(stream_buffer_size, buffer);
}

u32 rndrstream_nbuffers = 2;
i32 nurndr_maxstreamsize;
VARIPTR rndrstream_end;
VARIPTR rndrstream_free;
static VARIPTR rndrstream[NURNDR_STREAM_MAX_BUFFERS];

static i32 rndrstream_buffid;

void NuRndrStreamInit(i32 stream_buffer_size, VARIPTR *buffer) {
    i32 i;

    if (buffer == NULL) {
        for (i = 0; i < rndrstream_nbuffers; i++) {
            rndrstream[i].void_ptr = NU_ALLOC(stream_buffer_size, 4, 1, "", NUMEMORY_CATEGORY_NONE);
            memset(rndrstream[i].void_ptr, 0, stream_buffer_size);
        }
    } else {
        for (i = 0; i < rndrstream_nbuffers; i++) {
            buffer->addr = ALIGN(buffer->addr, 0x80);
            rndrstream[i].void_ptr = buffer->void_ptr;

            memset(buffer->void_ptr, 0, stream_buffer_size);

            buffer->void_ptr = (void *)(buffer->addr + stream_buffer_size);
        }
    }

    rndrstream_buffid = 0;
    nurndr_maxstreamsize = stream_buffer_size;
    rndrstream_end.addr = rndrstream[0].addr + stream_buffer_size;
    rndrstream_free.addr = ALIGN(rndrstream[0].addr, 16);
}



static int g_isBlockedInSwapScreen;




void NuRndrClear(int a1, int a2, float a3)
{
    if (NuIOS_IsLowEndDevice() && g_BackgroundUsedFogColour) {
        a2 = g_BackgroundColour;
    }

    currentScene->field_10 = a3;           // ASM: movss [esi+0x10], xmm0
    currentScene->flags |= a1;             // ASM: or [esi+0x8], eax
    currentScene->background_color = a2;   // ASM: mov [esi+0xc], edi

    NuVpGetPosition2(&currentScene->vp_pos_x, &currentScene->vp_pos_y);
    NuVpGetSize2(&currentScene->vp_size_w, &currentScene->vp_size_h);
}

int NuRndrBeginScene() {
    currentScene->field_4C = 0;
    currentScene->field_00 = -1; // [0]
    currentScene->flags = 0;    // [2]
    currentScene->field_04 = 0; // [1]
    currentScene->field_24 = 0; // [9]
    currentScene->field_28 = 0; // [10]
    currentScene->field_58 = 0;
    currentScene->field_38 = 0;  // [14]
    currentScene->field_3C = 0;  // [15]
    currentScene->field_40 = -1; // [16]
    currentScene->field_48 = 0; // [18]
    currentScene->field_AC = 0;
    currentScene->field_C4 = 0; // [49]
    currentScene->field_E4 = 0;
    currentScene->field_E0 = 0; // [56]
    currentScene->field_178 = 0; // [94]
    currentScene->field_174 = 0; // [93]
    currentScene->field_188 = 0; // [98]
    currentScene->field_214 = 0; // [133]
    
    return 1;
}

int NuRndrSwapScreen() {
  NuApplicationState *ApplicationState; // eax

  NuRenderThreadLock();
  //rndr_blend_shape_deformer_wt_cnt = 16128;
  //rndr_blend_shape_deformer_wt_ptrs_cnt = 2048;
  //NuRenderThreadPrepareRender();
  //NuShaderManagerBindShader(0);
  //NuDebrisRendererFlushBuffers();
  //NuDisplayListSwapBuffersEndFrame();
  //NuRndrSwapStreamBuffers();
  //NuDisplayListSwapBuffersBeginFrame();
  //NuDisplayListCheckBuffer();
  //NuDisplayListResetBuffer();
  //NuRenderThreadUnlock();
  NuIOS_WakeRenderThread(); //NuRenderThreadStartRender();
  while ( 1 )
  {
    ApplicationState = NuCore::GetApplicationState();
    if ( ApplicationState->GetStatus() != NuApplicationStatus::NUSTATUS_ERROR )
      break;
    g_isBlockedInSwapScreen = 1;
    NuThreadSleep(1);
  }
  g_isBlockedInSwapScreen = 0;
  return 1;
}


i32 NuHasError(void)

{
  return bHaveErr;
}

i32 NuFrameEnd(void) {
  i32 lVar1;
  NUTIME local_30;
  NUTIME local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  float local_18;
  int local_14;
  int local_10;
  
  local_10 = 0;
  local_14 = NuHasError();
  if (nuapi.max_fps != 0) {
    local_18 = 1.0 / (float)nuapi.max_fps;
    NuTimeGet(&local_28);
    NuTimeSub(&local_30,&local_28,&nuapi.time);
    while( true ) {
      lVar1 = NuTimeSeconds(&local_30);
      if (local_18 <= (float)lVar1) break;
      NuTimeGet(&local_28);
      NuTimeSub(&local_30,&local_28,&nuapi.time);
    }
    lVar1 = NuTimeSeconds(&local_30);
    nuapi.frametime = (float)lVar1;
  }
  NuMtlAnimate(nuapi.frametime);
  //NuTexAnimProcess(nuapi.frametime);
  NuWindAnimate(nuapi.wind,nuapi.frametime);
  //NuOcclusionManagerEndFrame();
  //NuPadRecordEndFrame();
  //NuTimeBarSetRender(0xffffffff);
  //NuPad_Interface_Render();
  local_10 = 1;
 // if (preRenderFlashingHack != (code *)0x0) {
  //  (*preRenderFlashingHack)();
  //}
  //NuRndrSwapScreenEx(0xffffffff,nuapi_endframe_callbackfn);
  NuTimeGet(&local_28);
  NuTimeSub(&local_30,&local_28,&nuapi.time);
  lVar1 = NuTimeSeconds(&local_30);
  nuapi.frametime = (float)lVar1;
  nuapi.time = local_28;
  unknown_timer = local_24;
  local_1c = 0x3dcccccd;
  local_20 = 0x41200000;
  if (0.1 < nuapi.frametime) {
    nuapi.frametime = 0.1;
  }
  //NuTimeGet(&DAT_006cda70);
  if (local_10 != 0) {
  //  bgSuspendMain(min_delay);
  }
  //if (postRenderFlashingHack != (code *)0x0) {
  //  (*postRenderFlashingHack)();
  //}
  NuPadUpdatePads();
 // DAT_006cda7c = DAT_006cda7c + 1;
  //_DAT_006cda60 = _DAT_006cda60 + -1;
  //if ((NuFrameEnd::ShowingError == 0) && (local_14 != 0)) {
   // NuFrameEnd::ShowingError = 1;
    //NuErrorSleep();
   // NuClearError();
   // NuFrameEnd::ShowingError = 0;
  //}
 // DAT_006cdac8 = 0;
  return nuapi.frametime;
}




