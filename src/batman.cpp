#include <stdint.h>

#include "decomp.h"

#include "game/level.h"
#include "globals.h"
#include "init/init.hpp"
#include "nu2api.saga/numusic/numusic.h"
#include "nu2api.saga/nuplatform/nuplatform.h"

extern "C" int32_t NuMain(int32_t argc, char **argv) {
    InitOnce(argc, argv);
    TriggerExtraDataLoad();

    StartPerm();
    LoadPerm();
    EndPerm();

    LOG_INFO("COMPLETIONPOINTS=%d", COMPLETIONPOINTS);
    for (int32_t i = 0; i < 3; i++) {
        float completion = (float)saveload_slotcode[i] * 100.0f / COMPLETIONPOINTS;
        LOG_INFO("slot %d used=%d completion=%f%% (%.1f%%)", i, saveload_slotused[i], completion, completion);
    }

    LEVELDATA *level = Level_FindByName("titles", NULL);
    LOG_INFO("titles level: %p", level);

    int32_t result = GamePlayMusic(level, 0, &Game.options_save);
    if (result != 0) {
        LOG_WARN("GamePlayMusic returned %d", result);
    }

    return 0;
}

int main(int argc, char **argv) {
    NuPlatform::Create();
    NuPlatform::ms_instance->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    NuMain(argc, argv);

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
