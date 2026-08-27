#include "decomp.h"

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/startup/main.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/light/shadow.h"
#include "legogame/startup.h"
#include "legogame/target.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nusound/nusound.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/gui/apimenu.h"
#include "legoapi/legoapi_types.h"

__attribute__((weak)) i32 main(i32 argc, char **argv) {
    UNIMPLEMENTED();
    return {};
}

extern "C" i32 NuMain(i32 argc, char **argv) {
    NuCommandLine(argc, argv);
    WorldInfo_InitOnce();
    InitOnce(argc, argv);
    TriggerExtraDataLoad();
    MenuInitialise(GameMenuInfo, 34, LANGUAGECOUNT, DrawSaveSlots, 1, 0);
    MenuReset();
    edGraInitTerrainSwapProtection();
    NuHGobjReversibleCharacters(1);
    NuHGobjForceShadowsOnCharacters(1);
    GameAISysInit();
    APITransparentInit();
    NuSound3SetLoopHoldTime(0.5f);
    InitShadowLights();
    GamePads_Init();
    MechSystems::Get()->Init();
    bgProcInit();
    FinishLoop(2);
    Text_InitLanguageList(reinterpret_cast<LANGUAGEDATA *>(Text_LanguageList));
    Text_SetLanguage(-1);
    StartPerm();
    LoadPerm();
    EndPerm();

    WORLDINFO *saved_world = WORLD;
    if (GAMEDEMO != 0) {
        WORLD = nullptr;
        PLAYERCOUNT = 0;
        // Demo playback was Trailer[0] via Movie_Play in the original.
        // Keep host-safe: just tick the frame loop once.
        FinishLoop(2);
    }
    WORLD = saved_world;
    if (openlevels != 0) {
        Areas_OpenAll(0);
    }

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
