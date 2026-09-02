#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nugcutscene.h"

struct CUTSCENEPLAYER_s {
    void *clips;
    i32 active;
};

CUTSCENEPLAYER_s *CutScenePlayer = NULL;

void CutScenePlayer_Reset() {
    if (CutScenePlayer != NULL) {
        CutScenePlayer->active = 0;
    }
}

void CutScenePlayer_Start(i32, i32) {
}

i32 CutScenePlayer_Active() {
    return CutScenePlayer != NULL ? CutScenePlayer->active : 0;
}

void CutScenePlayer_GetText(i32, char *, char *, i32) {
}

void CutScenePlayer_CanStart(i32) {
}

void CutScenePlayer_DrawGrid(COLLECTION_s *, i16 *, float, float, i32, float) {
}

void CutScenePlayer_Configure(char *, variptr_u *, variptr_u *, i16 *, i16 *, i16 *, i16 *, i16 *) {
}

void CutScenePlayer_SetObjects(CUTINFO *) {
}

void CutScenePlayer_CountEpisodeClips(i32, i32, i16 *) {
}

void FindGameCutScenes() {
}

void FindSceneStateObj(nugscn_s *, SCENEPROGRESS_s *, nuhspecial_s *) {
}

void instGetLookAtLocatorInfo(instNUGCUTSCENE_s *, instNUGCUTLOOKAT_s *) {
}

void instNuGCutGetNextRigidInfo(instNUGCUTSCENE_s *, float, i32, numtx_s *, nuhspecial_s *) {
}

void instNuGCutSceneSwapBuffers(instNUGCUTSCENE_s *, i32) {
}

void instNuGCutSceneResetCamLock(instNUGCUTSCENE_s *instance) {
    if (instance != NULL && instance->camera_instance != NULL && instance->camera_instance->lock_state >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void instNuGCutSceneEndFirstFrame(instNUGCUTSCENE_s *) {
}

void instNuGCutSceneEndButNotSystems(instNUGCUTSCENE_s *) {
}

void instNuGCutContainsInstancedRigids(instNUGCUTSCENE_s *) {
}
