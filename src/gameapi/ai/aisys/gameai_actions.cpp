#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"

extern "C" i32 instNuGCutSceneIsFinished(instNUGCUTSCENE_s *cutscene);
extern i32 Hub_GetRandomCharType();
extern u8 hub_custodians_finished_loading;

enum AI_CREATURE_SET : isize {
    AI_CREATURE_SET_CURRENT = -1,
    AI_CREATURE_SET_NONE = 0,
    AI_CREATURE_SET_FIRST = 1,
    AI_CREATURE_SET_LAST = 16,
};

union AI_CONDITION_SET_ARGUMENT {
    void *pointer;
    isize value;
};

static isize AIConditionArgumentValue(void *argument) {
    AI_CONDITION_SET_ARGUMENT condition_argument = {};
    condition_argument.pointer = argument;
    return condition_argument.value;
}

static __used__ f32 Condition_CurrentHintId(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EmptyTakeOver(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ForceComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ForceFinished(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_GotLocatorSet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_HintAvailable(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_HitPointsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_InContextInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_InTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_IsVisibleInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_LastLevelInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_LocatorRangeY(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static f32 Condition_NumInSetAlive(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *packet, char *, void *void_arg) {
    isize creature_set = AIConditionArgumentValue(void_arg);
    if (creature_set == AI_CREATURE_SET_CURRENT) {
        creature_set = packet->creature_set;
    }
    if (creature_set == AI_CREATURE_SET_NONE) {
        return 0.0f;
    }
    return static_cast<f32>(aicreature_sets_alive[creature_set - AI_CREATURE_SET_FIRST]);
}

static __used__ f32 Condition_ObstacleAtEnd(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OnSpeederBike(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentBelow(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_Player2Active(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_TakeOverRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_TakenOverInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_YawToOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ i32 Action_CanShootOffScreen(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DrawBossHitPoints(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_IgnoreShoveSystem(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_IgnoreWallSplines(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_NotifyStateChange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_OverrideAnimation(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_PlayerSpeederHack(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_PressActionButton(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetFullPathSearch(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetRespawnLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetShootOpponents(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetTakeOverTarget(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetTechnoComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ void *Condition_CategoryIsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_CharacterRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_CutSceneExists(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerIs(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_FinishedSpline(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ForceBeingUsed(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static void *Condition_GizSpecialInit(AISYS_s *, char *arg, AISCRIPT_s *) {
    if (WORLD == NULL) {
        return NULL;
    }
    return GizmoFindByName(WORLD->gizmo_sys, gizspecial_gizmotype_id, arg);
}

static __used__ f32 Condition_GotOpponentLOS(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_GotTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_IsLowEndDevice(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return g_lowEndLevelBehaviour != 0 ? 1.0f : 0.0f;
}

static __used__ void *Condition_IsOnScreenInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static void *Condition_IsSetAliveInit(AISYS_s *, char *arg, AISCRIPT_s *) {
    isize creature_set = AI_CREATURE_SET_NONE;
    if (arg != NULL) {
        if (NuStrICmp(arg, "myset") == 0) {
            creature_set = AI_CREATURE_SET_CURRENT;
        } else {
            const i32 parsed_set = NuAToI(arg);
            if (parsed_set >= AI_CREATURE_SET_FIRST && parsed_set <= AI_CREATURE_SET_LAST) {
                creature_set = parsed_set;
            }
        }
    }
    return reinterpret_cast<void *>(creature_set);
}

static __used__ f32 Condition_LevelNodeRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_LocatorRangeXZ(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_NumBaddiesInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OffScreenTimer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentOnPath(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerOnGround(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerOnObject(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerToOrigin(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_RigidAnimFrame(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_UsingForceInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ i32 Action_AddScriptProcessor(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CanHitForceObjects(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CanTriggerObstacle(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CannotBeForcedBack(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DeflectPlayersPart(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DisableNarrowSocks(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DontAvoidCharacter(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DontSetStoppedFlag(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_GizmoSetVisibility(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_IgnoreSlideTerrain(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_MoveAwayFromPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_PressSpecialButton(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SelectRandomSpline(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetAttackersAtOnce(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetAttackersPerRow(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetCircleDirection(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetIgnoreAntinodes(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetLastSafePathPos(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetShieldHitPoints(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SnapToSockPosition(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_UseTimeBasedUpdate(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static void *Condition_BeenToLevelInit(AISYS_s *system, char *arg, AISCRIPT_s *) {
    if (system == NULL || arg == NULL || WORLD == NULL || WORLD->area == NULL) {
        return reinterpret_cast<void *>(static_cast<isize>(-1));
    }
    for (i32 area_level = 0; area_level < WORLD->area->level_count; ++area_level) {
        const i32 level = WORLD->area->levels[area_level];
        if (NuStrICmp(arg, LDataList[level].name) == 0) {
            return reinterpret_cast<void *>(static_cast<isize>(area_level));
        }
    }
    return reinterpret_cast<void *>(static_cast<isize>(-1));
}

static __used__ f32 Condition_BigJumpComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_BuildItComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_CharacterExists(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static f32 Condition_CharacterLoaded(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *character_argument) {
    const i32 character = static_cast<i32>(AIConditionArgumentValue(character_argument));
    return APICharacterLoaded(character) != NULL ? 1.0f : 0.0f;
}

static __used__ f32 Condition_CutScenePlaying(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_CutSceneStarted(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_DropBackInTimer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_FlowBoxComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_GizmoOutputInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_GizmoVisibility(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_GotLocatorInSet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_InLevelNodeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_LocatorOnScreen(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_NumForceObjects(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static f32 Condition_ObstacleAtStart(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *void_arg) {
    GIZMO_s *special = static_cast<GIZMO_s *>(void_arg);
    if (special == NULL) {
        return 0.0f;
    }
    return GizmoGetOutput(WORLD->gizmo_sys, special, 1, 1) == 0 ? 1.0f : 0.0f;
}

static __used__ f32 Condition_OnForcePlatform(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentContext(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OpponentIsAInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_PartyUnderCover(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerTakenOver(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerToLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PrefersBrawling(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_ScriptParamInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_TurretAliveInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ i32 Action_CanHelpWithTriggers(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_ClearTakeOverTarget(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_ImmuneToKillTerrain(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_LaunchGuidedMissile(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_MoveAwayFromPlayer2(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_RetreatFromOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetMaxMovementRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetZeroAcceleration(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SplineFollowTerrain(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ void *Condition_AreaCompleteInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_CurrentLocatorIs(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static f32 Condition_CutSceneFinished(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *void_arg) {
    CUTINFO *cutscene = static_cast<CUTINFO *>(void_arg);
    if (cutscene == NULL || cutscene->instance == NULL) {
        return 0.0f;
    }
    return instNuGCutSceneIsFinished(static_cast<instNUGCUTSCENE_s *>(cutscene->instance)) != 0 ? 1.0f : 0.0f;
}

static __used__ void *Condition_ForcePushingInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_HelpWithTriggers(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_IAmAGoodieBaddie(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_LocatorRangeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OnDynamicGrapple(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentToOrigin(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerCategoryIs(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_PlayerInSockInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_PlayerUsingForce(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_RespawnLocatorIs(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ i32 Action_AwkwardShapeOverride(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CanShootObstructions(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_DontUseShadowTerrain(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_MoveAwayFromOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_PartyCanBeUnderCover(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetAIOverrideControl(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetAtOnceRowDistance(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ f32 Condition_AIOverrideControl(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_AnimationFinished(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_BeenTakenOverInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_CanFightLikeAJedi(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_ForceCompleteInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_HintAvailableInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_InTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_LastAttackerRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_MaulShouldRunAway(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_NearestPartyRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OnSpeederBikeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OpponentIsAThreat(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentToLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerInLevelNode(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ i32 Action_AlwaysTriggerObstacle(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CanCollideWithObjects(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static i32 Action_CharClipToBlobShadows(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_CreateSplineCreatures(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_IgnoreLastSafePathPos(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ i32 Action_SetFormationCommander(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ f32 Condition_BaddyInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_CharacterRangeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_CutSceneExistsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_EitherPlayerIsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_ForceStackComplete(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_GoodyInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_HeadTurnRestricted(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_IAmAPartyCharacter(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_LevelNodeRangeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_NearestPlayerRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_NetworkGameOnGoing(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ObstacleLockedOpen(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ObstacleLockedShut(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OffScreenTimerInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OpponentOnSamePath(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentToLocatorY(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PickupBeenTurnedOn(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_RigidAnimFrameInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_UnderPlayerControl(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ i32 Action_CanMoveWhenDeactivated(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ i32 Action_IgnoreTurnAroundSpline(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ i32 Action_LinkTurretToController(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ i32 Action_PathConnectionObstacle(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ i32 Action_RegisterTakeOverObject(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ i32 Action_SetDoomedEscapeLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                  f32) {
    return 0;
}

static __used__ f32 Condition_AreaContainsBaddies(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_AreaContainsGoodies(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_BuildItCompleteInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_CharacterExistsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static void *Condition_CharacterLoadedInit(AISYS_s *, char *argument, AISCRIPT_s *) {
    if (argument == NULL) {
        return NULL;
    }
    return reinterpret_cast<void *>(static_cast<isize>(CharIDFromName(argument)));
}

static __used__ f32 Condition_CharacterTypeExists(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_CutScenePlayingInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_CutSceneStartedInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_FlowBoxCompleteInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_GizmoVisibilityInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_GotLocatorInSetInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_LocatorOnScreenInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_NearestPartyXZRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_NumForceObjectsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_OnForcePlatformInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OpponentToLocatorXZ(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PartyContainsDroids(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_PlayerToLocatorInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ i32 Action_PathConnectionMaxLength(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                   f32) {
    return 0;
}

static __used__ i32 Action_SetDefaultMovementRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                   f32) {
    return 0;
}

static __used__ void *Condition_CurrentLocatorIsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static void *Condition_CutSceneFinishedInit(AISYS_s *, char *arg, AISCRIPT_s *) {
    if (WORLD == NULL) {
        return NULL;
    }
    return CutScene_Find(WORLD->cutscene_sys, arg);
}

static __used__ f32 Condition_EitherPlayerOnObject(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_NearestOpponentRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_NearestPlayerXZRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OnDynamicGrappleInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OnSameObjectAsPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentPathPosRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_Player2InTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerDeflectingPart(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_RespawnLocatorIsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_ShouldAttackOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_SockDistanceToPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ i32 Action_MoveAwayFromLastAttacker(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                    f32) {
    return 0;
}

static __used__ i32 Action_RemoveThrownForceObjects(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                    f32) {
    return 0;
}

static __used__ void *Condition_AIOverrideControlInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_AnimationFinishedInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_CollidingWithOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerTakenOver(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_OpponentInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OpponentToLocatorInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_OpponentToPlayerRange(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_PlayerInLevelNodeInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_PlayerOnForcePlatform(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_SockXDistanceToPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_BaddyInTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_CannotReachDestination(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerUsingForce(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerUsingPanel(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_GoodyInTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_NearestPlayerToLocator(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ObstacleOpenedByPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_PickupBeenTurnedOnInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_SockDistanceToOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_UnderPlayerControlInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ i32 Action_RetreatFromNearestOpponent(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32,
                                                      f32) {
    return 0;
}

static __used__ void *Condition_AreaContainsBaddiesInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_AreaContainsGoodiesInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_AreaContainsPartyMember(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_CharacterTypeExistsInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_EitherPlayerInLevelNode(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_PlayerDistanceAlongSock(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_PlayerInTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_RandomMapCharsAvailable(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    if (hub_custodians_finished_loading == 0) {
        return 0.0f;
    }
    return Hub_GetRandomCharType() != -1 ? 1.0f : 0.0f;
}

static __used__ f32 Condition_EitherPlayerPullingLever(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static i32 Action_SetBoltsDontGetDeflectedBack(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, f32) {
    return 0;
}

static __used__ f32 Condition_BoltsDontGetDeflectedBack(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerWearingHelmet(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_ForceStackCompleteInOrder(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_OpponentInTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_EitherPlayerLocatorRangeXZ(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerPushingSpinner(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ f32 Condition_LastAttackerIsActivePlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
    return 0;
}

static __used__ void *Condition_ObstacleOpenedByPlayerInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_AngleAboutMyLocatorToPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ void *Condition_AreaContainsPartyMemberInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_EitherPlayerInMyTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerOnForcePlatform(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ f32 Condition_EitherPlayerUsingHatMachine(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ f32 Condition_NumBaddiesThatCanSeePlayers(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ f32 Condition_TakeOverTargetInTriggerArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                          void *) {
    return 0;
}

static __used__ f32 Condition_ObstacleOpenedByEitherPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                           void *) {
    return 0;
}

static __used__ void *Condition_EitherPlayerPushingSpinnerInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ void *Condition_AngleAboutMyLocatorToPlayerInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_FurthestPlayerDistanceAlongSock(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                              void *) {
    return 0;
}

static __used__ void *Condition_TakeOverTargetInTriggerAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
    return nullptr;
}

static __used__ f32 Condition_InSameTriggerAreaAsNearestPlayer(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *,
                                                               void *) {
    return 0;
}

namespace {
    struct GameAIRegistryCallbacks {
        GameAIRegistryCallbacks() {
            lego_aiactiondefs[LEGO_AI_ACTION_SET_DOOMED_ESCAPE_LOCATOR].eval_fn = Action_SetDoomedEscapeLocator;
            lego_aiactiondefs[LEGO_AI_ACTION_SNAP_TO_SOCK_POSITION].eval_fn = Action_SnapToSockPosition;
            lego_aiactiondefs[LEGO_AI_ACTION_CAN_SHOOT_OFF_SCREEN].eval_fn = Action_CanShootOffScreen;
            lego_aiactiondefs[LEGO_AI_ACTION_SET_BOLTS_DONT_GET_DEFLECTED_BACK].eval_fn =
                Action_SetBoltsDontGetDeflectedBack;
            lego_aiactiondefs[LEGO_AI_ACTION_CAN_SHOOT_OBSTRUCTIONS].eval_fn = Action_CanShootObstructions;
            lego_aiactiondefs[LEGO_AI_ACTION_CAN_HIT_FORCE_OBJECTS].eval_fn = Action_CanHitForceObjects;
            lego_aiactiondefs[LEGO_AI_ACTION_PLAYER_SPEEDER_HACK].eval_fn = Action_PlayerSpeederHack;
            lego_aiactiondefs[LEGO_AI_ACTION_CHAR_CLIP_TO_BLOB_SHADOWS].eval_fn = Action_CharClipToBlobShadows;
            lego_aiactiondefs[LEGO_AI_ACTION_DEFLECT_PLAYERS_PART].eval_fn = Action_DeflectPlayersPart;
            lego_aiactiondefs[LEGO_AI_ACTION_SET_AI_OVERRIDE_CONTROL].eval_fn = Action_SetAIOverrideControl;
            lego_aiactiondefs[LEGO_AI_ACTION_SET_LAST_SAFE_PATH_POS].eval_fn = Action_SetLastSafePathPos;
            lego_aiactiondefs[LEGO_AI_ACTION_DONT_SET_STOPPED_FLAG].eval_fn = Action_DontSetStoppedFlag;
            lego_aiactiondefs[LEGO_AI_ACTION_PRESS_SPECIAL_BUTTON].eval_fn = Action_PressSpecialButton;
            lego_aiactiondefs[LEGO_AI_ACTION_PRESS_ACTION_BUTTON].eval_fn = Action_PressActionButton;
            lego_aiactiondefs[LEGO_AI_ACTION_DONT_AVOID_CHARACTER].eval_fn = Action_DontAvoidCharacter;
            lego_aiactiondefs[LEGO_AI_ACTION_SET_ZERO_ACCELERATION].eval_fn = Action_SetZeroAcceleration;

            lego_aiconditiondefs[LEGO_AI_CONDITION_SPECIAL_AT_START].eval_fn = Condition_ObstacleAtStart;
            lego_aiconditiondefs[LEGO_AI_CONDITION_SPECIAL_AT_START].init_fn = Condition_GizSpecialInit;
            lego_aiconditiondefs[LEGO_AI_CONDITION_NUM_IN_SET_ALIVE].eval_fn = Condition_NumInSetAlive;
            lego_aiconditiondefs[LEGO_AI_CONDITION_NUM_IN_SET_ALIVE].init_fn = Condition_IsSetAliveInit;
            lego_aiconditiondefs[LEGO_AI_CONDITION_BEEN_TO_LEVEL].init_fn = Condition_BeenToLevelInit;
            lego_aiconditiondefs[LEGO_AI_CONDITION_CUT_SCENE_FINISHED].eval_fn = Condition_CutSceneFinished;
            lego_aiconditiondefs[LEGO_AI_CONDITION_CUT_SCENE_FINISHED].init_fn = Condition_CutSceneFinishedInit;
            lego_aiconditiondefs[LEGO_AI_CONDITION_IS_LOW_END_DEVICE].eval_fn = Condition_IsLowEndDevice;
            lego_aiconditiondefs[LEGO_AI_CONDITION_RANDOM_MAP_CHARS_AVAILABLE].eval_fn =
                Condition_RandomMapCharsAvailable;
            lego_aiconditiondefs[LEGO_AI_CONDITION_CHARACTER_LOADED].eval_fn = Condition_CharacterLoaded;
            lego_aiconditiondefs[LEGO_AI_CONDITION_CHARACTER_LOADED].init_fn = Condition_CharacterLoadedInit;
        }
    };

    GameAIRegistryCallbacks game_ai_registry_callbacks;
} // namespace
