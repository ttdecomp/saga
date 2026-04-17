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

#ifndef HOST_BUILD
int main(int argc, char **argv) {
    UNIMPLEMENTED();
}
#endif

extern "C" i32 NuMain(i32 argc, char **argv) {
    InitOnce(argc, argv);
    TriggerExtraDataLoad();

    StartPerm();
    LoadPerm();
    EndPerm();

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
