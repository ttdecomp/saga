#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nugcutscene.h"

#include "globals.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"

#include <string.h>

struct CUTSCENEPLAYERCLIP_s {
    i16 level_id;
    u8 pad_0x02[0x44 - 0x02];
};
DECOMP_ASSERT(sizeof(CUTSCENEPLAYERCLIP_s) == 0x44, "CUTSCENEPLAYERCLIP ABI");

struct CUTSCENEPLAYER_s {
    CUTSCENEPLAYERCLIP_s *clips;
    i32 active;
    u16 clip_count;
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

i32 CutScenePlayer_CanStart(i32 clip_id) {
    if (CutScenePlayer == NULL || clip_id < 0 || clip_id >= CutScenePlayer->clip_count) {
        return 0;
    }

    i32 area_id = LDataList[CutScenePlayer->clips[clip_id].level_id].area_index;
    if (area_id == -1) {
        return 0;
    }

    AREADATA_s *area = &ADataList[area_id];
    if ((area->flags & 2) == 0) {
        if (Game_AreaSave != NULL && Game_AreaSave[area_id].area_complete != 0) {
            return 1;
        }
        return 0;
    }

    i32 episode_id = area->episode_index;
    if (episode_id == -1) {
        return 0;
    }
    if (Episode_IsComplete(&EDataList[episode_id], NULL) != 0) {
        return 2;
    }
    return 0;
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
