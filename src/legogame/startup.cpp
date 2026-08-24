#include "legogame/startup.h"

#include "globals.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/world/area.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/world/levels/episode.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"
#include "legogame/game.h"

static void LoadPermData(BGPROCINFO *proc) {
    VARIPTR legalTex;
    legalTex.addr = superbuffer_end.addr + -0x400000;

    i32 legal_tid = NuTexRead("stuff\\legal\\LEGAL_ENGLISH", &legalTex, &superbuffer_end);

    MusicInfo = ConfigureMusic("audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
    RegisterMusic(MusicInfo);
    InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");

    CDataList =
        ConfigureCharacterList("chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 340, &CHARCOUNT, 288, &GCDataList);
    for (i32 i = 0; i < CHARCOUNT; i++) {
        LOG_INFO("Character %d: %s / %s", i, CDataList[i].dir, CDataList[i].file);
    }
    LOG_INFO("Loaded %d characters", CHARCOUNT);

    LDataList = Levels_ConfigureList("levels\\levels.txt", &permbuffer_ptr, &permbuffer_end, 365, &LEVELCOUNT,
                                     Level_SetDefaults);
    FixUpLevels(&LevFixUp);

    for (i32 i = 0; i < LEVELCOUNT; i++) {
        LOG_INFO("Level %d: %s / %s", i, LDataList[i].dir, LDataList[i].name);
    }
    LOG_INFO("Loaded %d levels", LEVELCOUNT);

    ADataList = Areas_ConfigureList("levels\\areas.txt", &permbuffer_ptr, &permbuffer_end, 72, &AREACOUNT);
    // FixUpAreas();

    for (i32 i = 0; i < AREACOUNT; i++) {
        LOG_INFO("Area %d: %s / %s", i, ADataList[i].dir, ADataList[i].file);
    }
    LOG_INFO("Loaded %d areas", AREACOUNT);

    EDataList = Episodes_ConfigureList("levels\\episodes.txt", &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

    for (i32 i = 0; i < EPISODECOUNT; i++) {
        LOG_INFO("Episode %d: %hu areas", i, EDataList[i].area_count);
    }
    LOG_INFO("Loaded %d episodes", EPISODECOUNT);

    Cheats_Init(Cheat);

    InitGameAfterConfig();
}

void StartPerm(void) {
    permbuffer_ptr = permbuffer_base;
    permbuffer_end = superbuffer_end;
}

void LoadPerm(void) {
    LoadPermData(NULL);
}

void EndPerm(void) {
}
