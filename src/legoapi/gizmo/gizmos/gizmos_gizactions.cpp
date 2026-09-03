#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"

void Action_Circle(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_Sebulba(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

i32 Action_SetState(AISYS_s *, AISCRIPTPROCESS_s *processor, AIPACKET_s *, char **params, i32 param_count,
                    i32 is_first_time, float) {
    if (is_first_time == 0 || param_count == 0) {
        return 0;
    }

    processor->next_state = AIStateFind(params[0], processor->script);
    processor->unknown_flag_4 = 0;
    for (i32 param_index = 1; param_index < param_count; ++param_index) {
        if (NuStrICmp(params[param_index], "KeepBlockedMessages") == 0) {
            processor->unknown_flag_4 = 1;
        }
    }
    return 0;
}

void Action_UsePanel(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_CameraCut(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_CreatePod(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_PullLever(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_UseTechno(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_NewSebulba(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_SetLapTime(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_MoveForward(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_CirclePlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_EndCameraCut(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_FollowPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_PlayCutScene(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_SetVisibility(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_UseTriggerSet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_BoulderSection(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_CircleOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_ReleaseLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_DynamicCameraCut(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_GameFollowPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_HelpWithTriggers(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_MushroomCollapse(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_GetLocatorFromSet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_AssignLocatorInSet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

void Action_SpeederBeingChased(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {
}

// Static GIZFLOW/FLOWBOX action callbacks (GizAction*/GizActions*). Moved from
// gizactions_stubs.cpp to satisfy the symbol baseline.

static __used__ void GizAction_SetAIState(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_HitBlowup(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_PlayForce(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_PlayRadio(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_EnableSock(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_ActivateChar(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_SetAIMessage(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_PlaySpecial(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_ActivateGizmo(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_SetVisibility(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_TurnOnFlowBox(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_ActivateBelt(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_GoToNewLevel(GIZFLOW_s *, FLOWBOX_s *, char **params, int param_count) {
    if (param_count <= 0) {
        return;
    }

    char *cutscene_name = NULL;
    LEVELDATA *level = NULL;

    for (i32 param_index = 0; param_index < param_count; ++param_index) {
        char *value = NuStrIStr(params[param_index], "level=");
        if (value != NULL) {
            level = Level_FindByName(value + NuStrLen("level="), NULL);
        } else {
            value = NuStrIStr(params[param_index], "cutscene=");
            if (value != NULL) {
                cutscene_name = value + NuStrLen("cutscene=");
            }
        }
    }

    if (FreePlay == 0 && cutscene_name != NULL && NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL) {
        return;
    }

    if (level != NULL && netclient == 0) {
        GoToNewLevel(level->idx);
    }
}

static __used__ void GizActions_PlayCutscene(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_PlayObstacle(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_ActivateEffect(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_CompleteLevel(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_GoThroughDoor(GIZFLOW_s *, FLOWBOX_s *, char **params, int param_count) {
    char *door_name = NULL;

    for (i32 param_index = 0; param_index < param_count; ++param_index) {
        char *value = NuStrIStr(params[param_index], "Name");
        if (value != NULL) {
            door_name = value + NuStrLen("Name=");
        }
    }

    if (door_name != NULL) {
        DOOR_s *door = Door_FindByName(WORLD, door_name);
        if (door != NULL) {
            Door_GoThrough(WORLD, door, 1);
        }
    }
}

static __used__ void GizAction_ChangeTechnoTgt(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_ActivatePartEffect(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_SetGizmoVisibility(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizAction_SetPickupVisibility(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

static __used__ void GizActions_ChangeObstTriggerType(GIZFLOW_s *, FLOWBOX_s *, char **, int) {
}

GIZACTIONDEFN_s game_gizactiondefs[] = {
    {"SetVisibility", GizAction_SetVisibility},
    {"SetGizmoVisibility", GizAction_SetGizmoVisibility},
    {"SetPickupVisibility", GizAction_SetPickupVisibility},
    {"ActivateGizmo", GizAction_ActivateGizmo},
    {"ActivateChar", GizAction_ActivateChar},
    {"TurnOnFlowBox", GizAction_TurnOnFlowBox},
    {"ActivateEffect", GizAction_ActivateEffect},
    {"ActivatePartEffect", GizAction_ActivatePartEffect},
    {"ChangeTechnoTarget", GizAction_ChangeTechnoTgt},
    {"SetAIMessage", GizAction_SetAIMessage},
    {"SetAIState", GizAction_SetAIState},
    {"CompleteLevel", GizActions_CompleteLevel},
    {"GoToNewLevel", GizActions_GoToNewLevel},
    {"GoThroughDoor", GizActions_GoThroughDoor},
    {"PlayObstacle", GizActions_PlayObstacle},
    {"PlaySpecial", GizActions_PlaySpecial},
    {"PlayForce", GizActions_PlayForce},
    {"ChangeObstTriggerType", GizActions_ChangeObstTriggerType},
    {"PlayRadio", GizActions_PlayRadio},
    {"PlayCutscene", GizActions_PlayCutscene},
    {"HitBlowup", GizActions_HitBlowup},
    {"ActivateBelt", GizActions_ActivateBelt},
    {"EnableSock", GizActions_EnableSock},
    {NULL, NULL},
};
