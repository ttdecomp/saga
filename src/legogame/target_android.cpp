#include "legogame/target.h"

#include <stdarg.h>

#include "globals.h"
#include "legogame/game.h"
#include "nu2api.saga/nu3d/nucamera.h"
#include "nu2api.saga/nuandroid/ios_graphics.h"
#include "nu2api.saga/nucore/nuapi.h"
#include "nu2api.saga/nucore/numemory.h"
#include "nu2api.saga/nucore/nuvideo.h"

extern "C" {
    void *DVD = NULL;

    struct nupad_s;
    nupad_s *Game_NuPad_Store[2];

    nupad_s **Game_NuPad;
};

void NuPhoneOSRegisterEventCallback(i32 param_1, void (*param_2)()) {
}

void DummyCallback() {
}

void InitOnce(i32 argc, char **param_2) {
    NuPhoneOSRegisterEventCallback(1, /* TouchCallback */ DummyCallback);
    NuPhoneOSRegisterEventCallback(3, /* SystemPauseCallback */ DummyCallback);
    NuPhoneOSRegisterEventCallback(6, /* SystemDidBecomeActiveCallback */ DummyCallback);

    if (NuIOS_IsLowEndDevice()) {
        SUPERBUFFERSIZE -= 0x38370;
    }

    i32 size = SUPERBUFFERSIZE;

    permbuffer_base.void_ptr = NU_ALLOC(size, 4, 1, "", NUMEMORY_CATEGORY_NONE);
    superbuffer_end.void_ptr = (void *)(SUPERBUFFERSIZE + (usize)permbuffer_base.void_ptr);
    original_permbuffer_base.void_ptr = permbuffer_base.void_ptr;
    InitGameBeforeConfig();

    Game_NuPad = &Game_NuPad_Store[0];

#define SETUP(cmd, ...) cmd, ##__VA_ARGS__

    NuInitHardware(&permbuffer_base, &superbuffer_end, 0,                      //
                   SETUP(NUAPI_SETUP_HOSTFS, 0),                               //
                   SETUP(NUAPI_SETUP_SWAPMODE, NUVIDEO_SWAPMODE_ASYNC),        //
                   SETUP(NUAPI_SETUP_STREAMSIZE, 0x20000),                     //
                   SETUP(NUAPI_SETUP_VIDEOMODE, (PAL == 0) ? 0xdeadbeef : 8),  //
                   SETUP(NUAPI_SETUP_RESOLUTION, 512, (PAL == 0) ? 224 : 256), //
                   SETUP(NUAPI_SETUP_GLASSRPLANE, 1),                          //
                   SETUP(NUAPI_SETUP_CDDVDMODE, &DVD),                         //
                   // (-(uint)(NOSOUND == 0) & 0xffffffee) + 0x1c, 0, 0x640, 0, 0,
                   SETUP(NUAPI_SETUP_PAD0, &Game_NuPad_Store[0]), //
                   SETUP(NUAPI_SETUP_PAD1, &Game_NuPad_Store[1]), //
                   SETUP(NUAPI_SETUP_0x46, 1),                    //
                   SETUP(NUAPI_SETUP_0x47, 1),                    //
                   SETUP(NUAPI_SETUP_0x49, 1),                    //
                   SETUP(NUAPI_SETUP_0x4b, 1),                    //
                   NUAPI_SETUP_END                                //
    );

    pNuCam = NuCameraCreate();
    // Game.optionsSave._11_1_ = NuIOS_IsWidescreen();
    // WidescreenCode((uint)(byte)Game.optionsSave._11_1_);
    // InitPanel((uint)(byte)Game.optionsSave._11_1_);

    // app_tbgameset = NuTimeBarCreateSet(0);
    // app_tbplayerset = NuTimeBarCreateSet(0);
    // app_tbaiset = NuTimeBarCreateSet(0);
    // app_tbdrawset = NuTimeBarCreateSet(0);
}
