#include <format>
#include <regex>
#include <string>

#include "decomp.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/world/level.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nudevicespecs.hpp"
#include "nu2api/nuplatform/nuplatform.h"
#include "legoapi/world/area.h"

char buf[0x1000000];

int test_load(int argc, char **argv) {
    VARIPTR buf_ptr = VARIPTR{.void_ptr = &buf};
    VARIPTR end_ptr = VARIPTR{.void_ptr = &buf[sizeof(buf)]};

    NUDATHDR *dat = NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &buf_ptr, 0);
    NuDatSet(dat);

    i32 area_count = 0;
    AREADATA_s *ADataList = Areas_ConfigureList("levels\\areas.txt", &buf_ptr, &end_ptr, 9999, &area_count);
    LOG_INFO("Loaded %d areas", area_count);
    for (i32 i = 0; i < area_count; i++) {
        AREADATA_s *area = &ADataList[i];
        LOG_INFO("Area %d: %s \\ %s", i, area->dir, area->file);
    }

    i32 level_count = 0;
    LEVELDATA *levels = Levels_ConfigureList("levels\\levels.txt", &buf_ptr, &end_ptr, 9999, &level_count, NULL);

    LOG_INFO("Loaded %d levels", level_count);
    for (i32 i = 0; i < level_count; i++) {
        LEVELDATA *level = &levels[i];
        LOG_INFO("Level %d: %s \\ %s", i, level->dir, level->name);
    }

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
