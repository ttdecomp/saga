

#include "decomp.h"

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/level.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nucore/common.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nuplatform/nuplatform.h"

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

    NuMain(argc, argv);

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
