#include <format>
#include <regex>
#include <string>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/level.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"

char buf[0x1000000];

int main(int argc, char **argv) {
    VARIPTR buf_ptr = (VARIPTR)&buf;
    NUDATHDR *dat = NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &buf_ptr, 0);
    NuDatSet(dat);

    char badwords[0x10000];
    i32 size = NuFileLoadBuffer("stuff\\text\\badwords.txt", &badwords, sizeof(badwords));
    LOG_DEBUG("Loaded badwords.txt, size=%d", size);

    std::string badwords_str(badwords, size);
    badwords_str = std::regex_replace(badwords_str, std::regex("\\r\\n(.)"), ",$1");

    LOG_INFO("%s", badwords_str.c_str());

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

    while (true) {
        NuThreadSleep(1);
    }

    return 0;
}
