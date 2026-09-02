#include "legoapi/legoapi_types.h"

#include "globals.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"

#include <string.h>

struct CUTSCENEPLAYER_s {
    void *clips;
    i32 active;
};

CUTSCENEPLAYER_s *CutScenePlayer = NULL;

void CutScenePlayer_Reset() {
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
    memset(&game_cutscenes, 0, sizeof(game_cutscenes));

    game_cutscenes.podrace_pod_explode = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_podexplode");
    game_cutscenes.podrace_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_outoftime");
    game_cutscenes.bonus_gunship_cavalry_explode =
        CutScene_Find(WORLD->cutscene_sys, "ep2_bonus_gunshipcavalry_explode");
    game_cutscenes.droid_factory_conveyor = CutScene_Find(WORLD->cutscene_sys, "ep2_droidfactory_conveyor");
    game_cutscenes.podrace_avalanche = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_avalanche");
    game_cutscenes.dogfight_die = CutScene_Find(WORLD->cutscene_sys, "ep3_dogfight_die");
    game_cutscenes.podrace_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_sebulba");
    game_cutscenes.cutscene = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_avalanche");
    game_cutscenes.podsprint_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_outoftime");
    game_cutscenes.podsprint_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_sebulba");
}

void FindSceneStateObj(nugscn_s *, SCENEPROGRESS_s *, nuhspecial_s *) {
}

void instGetLookAtLocatorInfo(instNUGCUTSCENE_s *, instNUGCUTLOOKAT_s *) {
}

void instNuGCutGetNextRigidInfo(instNUGCUTSCENE_s *, float, i32, numtx_s *, nuhspecial_s *) {
}

void instNuGCutSceneSwapBuffers(instNUGCUTSCENE_s *, i32) {
}

void instNuGCutSceneResetCamLock(instNUGCUTSCENE_s *) {
}

void instNuGCutSceneEndFirstFrame(instNUGCUTSCENE_s *) {
}

void instNuGCutSceneEndButNotSystems(instNUGCUTSCENE_s *) {
}

void instNuGCutContainsInstancedRigids(instNUGCUTSCENE_s *) {
}
