#include "decomp.h"

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/level.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nucore/common.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"

#ifdef HOST_BUILD
#include "host.hpp"
#endif

extern "C" i32 NuMain(i32 argc, char **argv) {
    InitOnce(argc, argv);
    TriggerExtraDataLoad();

    StartPerm();
    LoadPerm();
    EndPerm();

    LOG_INFO("COMPLETIONPOINTS=%d", COMPLETIONPOINTS);
    for (i32 i = 0; i < 3; i++) {
        f32 completion = (f32)saveload_slotcode[i] * 100.0f / COMPLETIONPOINTS;
        LOG_INFO("slot %d used=%d completion=%f%% (%.1f%%)", i, saveload_slotused[i], completion, completion);
    }

    LEVELDATA *level = Level_FindByName("titles", NULL);
    LOG_INFO("titles level: %p", level);

    i32 result = GamePlayMusic(level, 0, &Game.options_save);
    if (result != 0) {
        LOG_WARN("GamePlayMusic returned %d", result);
    }

    while (true) {
        NuThreadSleep(1);
    }

    return 0;
}

int main(int argc, char **argv) {
    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    if (!NuScreen::Exists()) {
        NuScreen::Create();
    }

#ifdef HOST_BUILD
    host_init();
#endif

    g_renderDevice.Initialize();

    g_isLowestEndDevice = 1;
    g_isLowEndDevice = 1;
    g_isMidRangeDevice = 0;
    i32 specs = NuDeviceSpecs::ms_instance->specs;
    if (specs == 1) {
        g_isLowestEndDevice = 0;
        g_isLowEndDevice = 0;
        g_isMidRangeDevice = 1;
        g_lowEndLevelBehaviour = 0;
    } else if (specs < 1 || 3 < specs) {
        g_lowEndLevelBehaviour = 1;
    } else {
        g_isLowestEndDevice = 0;
        g_isLowEndDevice = 0;
        g_lowEndLevelBehaviour = 0;
    }

    NuMain(argc, argv);

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
