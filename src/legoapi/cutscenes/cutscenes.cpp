#include "legoapi/legoapi_types.h"
#include "globals.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nugcutscene.h"

extern i32 ACTIVECUTCOUNT;
extern "C" i32 CUTDRAWWORLD;
extern "C" i32 Paused;
CUTSCENESYS *CutSceneSys;

extern "C" {
    void instNuGCutScenePause(instNUGCUTSCENE_s *, u8);
    void instNuGCutSceneReset(instNUGCUTSCENE_s *);
    void instNuGCutSceneStart(instNUGCUTSCENE_s *);
    void NuGCutSceneSysRender(f32);
    void NuGCutSceneSysUpdate(i32, i32, f32);
    void NuGCutSceneSysInit(NUGCUTLOCATORFNENTRY_s *);
    extern NUGCUTLOCATORFNENTRY_s cutscene_locatorfns[];
}

void SetLevelLights(void *, f32);

CUTINFO *CutScene_Find(CUTSYS *, char *) {
    return NULL;
}

void CutScenes_End() {
}

void CutScenes_Draw(WORLDINFO_s *world) {
    if (world->cutscene_sys != NULL && ACTIVECUTCOUNT > 0) {
        SetLevelLights(world->rtl_set, 1.0f);
        NuGCutSceneSysRender(static_cast<f32>(Paused));
    }
}

void CutScenes_Stop(CUTSYS *) {
}

void CutScenes_Reset(WORLDINFO_s *world) {
    ACTIVECUTCOUNT = 0;
    CUTSTOPGAME = 0;
    CUTDRAWWORLD = 0;
    if (world == NULL || world->cutscene_sys == NULL) {
        return;
    }

    CUTSYS *system = world->cutscene_sys;
    for (i32 i = 0; i < system->count; ++i) {
        CUTINFO *cut = system->cuts[i];
        if (cut == NULL || cut->instance == NULL) {
            continue;
        }
        instNUGCUTSCENE_s *instance = reinterpret_cast<instNUGCUTSCENE_s *>(cut->instance);
        instNuGCutSceneReset(instance);
        if (reset_restart != 0 && (cut->flags & 0x1000) != 0) {
            if ((cut->flags & 0x200) != 0) {
                instance->flags_88 |= 8;
            } else {
                instance->flags_88 &= ~8U;
            }
            instNuGCutSceneStart(instance);
            reset_restart = 0;
            break;
        }
    }
}

void CutScenes_Start(WORLDINFO_s *) {
}

void CutScenes_Update(WORLDINFO_s *world, i32 paused) {
    ACTIVECUTCOUNT = 0;
    CUTSTOPGAME = 0;
    CUTDRAWWORLD = 0;
    if (world == NULL || world->cutscene_sys == NULL) {
        return;
    }
    CUTSYS *system = world->cutscene_sys;
    for (i32 i = 0; i < system->count; ++i) {
        CUTINFO *cut = system->cuts[i];
        if (cut == NULL || cut->instance == NULL) {
            continue;
        }
        instNUGCUTSCENE_s *instance = reinterpret_cast<instNUGCUTSCENE_s *>(cut->instance);
        if ((instance->flags_88 & 2) == 0) {
            continue;
        }
        instance->rate = cut->frames_per_second * FRAMETIME;
        instNuGCutScenePause(instance, 0);
        ++ACTIVECUTCOUNT;
        if ((cut->flags & 1) != 0) {
            CUTSTOPGAME = 1;
        }
        if ((cut->flags & 2) != 0) {
            CUTDRAWWORLD = 1;
        }
    }
    NuGCutSceneSysUpdate(paused, 0, 1.0f);
}

void CutScene_FindInst(CUTSYS *, char *) {
}

void CutScenes_Destroy(CUTSYS *) {
}

void CutScene_HasPlayed(CUTINFO *) {
}

void CutScene_SnapToEnd(CUTINFO *) {
}

void CutScene_StartAudio() {
}

bool CutScene_IsSkippable(CUTINFO *) {
    return false;
}

void CutScene_StartFn_LSW(CUTINFO *) {
}

void CutScenes_InitSystem(CUTSCENESYS *system) {
    CutSceneSys = system;
    NuGCutSceneSysInit(cutscene_locatorfns);
}

void CutScene_DrawSubtitles() {
}

void CutScene_StoppedFn_LSW(CUTINFO *) {
}

void CutScenes_BGLoadManager() {
}

void CutScenes_ConfigureList(char *, variptr_u *, variptr_u) {
}

void CutScene_PreUpdateFn_LSW(CUTINFO *) {
}

void CutScene_PostUpdateFn_LSW() {
}

bool CutScene_PlayingOrRequested(CUTINFO *) {
    return false;
}

void CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *, NUGCUTCHAR_s *) {
}

void ResetScene(nugscn_s *, SCENEPROGRESS_s *) {
}

i32 NewCutScene(CUTINFO *, CUTSYS *, char *, i32) {
    return 0;
}

void Exit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void Fade_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void RelocateCutScene(NUGCUTSCENE_s *, variptr_u *) {
}

void GoldBrick_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void GoldBrick_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void GoldBrick_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void LevelComplete_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void LevelComplete_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void LevelComplete_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

static __used__ void Titles_Draw(WORLDINFO_s *) {
}

static __used__ void Titles_Init(WORLDINFO_s *) {
}

static __used__ void Titles_Update(WORLDINFO_s *) {
}
