#include <stdint.h>

#include "globals.h"
#include "init/init.hpp"

extern "C" int32_t NuMain(int32_t argc, char **argv) {
    InitOnce(argc, argv);
    TriggerExtraDataLoad();

    StartPerm();
    LoadPerm();
    EndPerm();

    LOG("COMPLETIONPOINTS=%d", COMPLETIONPOINTS);
    for (int32_t i = 0; i < 3; i++) {
        LOG("slot %d used=%d completion=%f%%", i, saveload_slotused[i],
            (float)saveload_slotcode[i] * 100.0f / COMPLETIONPOINTS);
    }

    return 0;
}

int main(int argc, char **argv) {
    NuMain(argc, argv);

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};