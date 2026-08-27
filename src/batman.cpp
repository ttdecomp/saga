#include "decomp.h"

#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/menus/screens/movies.h"
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

extern "C" {
    void NuRndrBeginScene();
    void NuRndrEndScene();
    void edGraEnableTerrainSwap();
    void edGraDisableTerrainSwap();
    void NuFrameBegin();
    float NuFrameEnd();
    void DrawMenu(int);
}
void BackDrop_Update(float);
void BackDrop_Draw(float, int);
void UpdateGameMenu(GAMEPAD_s *, int);
void SetBackgroundMusic(int);
int GamePlayMusic(LEVELDATA *, int, OPTIONSSAVE *);
extern GAMEPAD_s GamePad[4];
extern LEVELDATA *TITLES_LDATA;
extern float FRAMETIME;
extern float DEFAULTFRAMETIME;
char *Trailer[3];
extern VARIPTR characterbuffer_ptr;
extern VARIPTR characterbuffer_end;
extern "C" __attribute__((weak)) void LSW_SetIndy(i32);
extern "C" __attribute__((weak)) void GameAISysSetGame();
extern "C" __attribute__((weak)) void GizmoSysSetGame();
extern "C" __attribute__((weak)) void Mission_Clear(void *);
extern "C" __attribute__((weak)) void ResetCharacterBuffer(i32);

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
    MechSystems::Get()->Init(0);
    bgProcInit();
    FinishLoop(2);
    Text_InitLanguageList(reinterpret_cast<LANGUAGEDATA *>(Text_LanguageList));
    Text_SetLanguage(-1);
    StartPerm();
    LoadPerm();
    EndPerm();

    WORLDINFO *saved_world = WORLD;
    if (GAMEDEMO != 0) {
        WORLD = NULL;
        PLAYERCOUNT = 0;
        Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                   (float)Game.options_save.field4_0x4 / 10.0f);
        FinishLoop(2);
    }

    WORLD = saved_world;
    if (openlevels != 0) {
        Areas_OpenAll(0);
    }

    if (LSW_SetIndy)
        LSW_SetIndy(openlevels);
    if (GameAISysSetGame)
        GameAISysSetGame();
    if (GizmoSysSetGame)
        GizmoSysSetGame();

    // Original also does Mission_Clear / ResetCharacterBuffer
    if (Mission_Clear)
        Mission_Clear(nullptr);
    if (ResetCharacterBuffer)
        ResetCharacterBuffer(0);

    if (TITLES_LDATA != nullptr) {
        GamePlayMusic(TITLES_LDATA, 0, reinterpret_cast<OPTIONSSAVE *>(&Game.options_save));
    } else {
        SetBackgroundMusic(1);
    }
    DrawMenu(0);
    UpdateGameMenu(&GamePad[0], 0);

    while (true) {
        NuFrameBegin();
        UpdateGameMenu(&GamePad[0], 0);
        BackDrop_Update(FRAMETIME);
        NuRndrBeginScene();
        BackDrop_Draw(1.0f, 0);
        DrawMenu(0);
        NuRndrEndScene();
        edGraEnableTerrainSwap();
        float dt = NuFrameEnd();
        edGraDisableTerrainSwap();
        if (dt < DEFAULTFRAMETIME || dt > DEFAULTFRAMETIME * 3.0f) {
            dt = DEFAULTFRAMETIME;
        }
        (void)dt;
    }

    return 0;
}

char uberShader2[] = {
#include <uberShader2.array>
};
