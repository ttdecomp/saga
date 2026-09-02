#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"

#include <stdio.h>
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nurand.h"

extern i32 Hub_GetRandomCharType();

i32 Action_SetState(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_GoToOriginalPath(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_BigJumpToLocator(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_UseBigJumpToJump(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_CatchUpForbidden(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_SetAnimSpeedMul(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_ShootAtOpponent(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_SetInvulnerable(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_PressJumpButton(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_SetControlSystem(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
static i32 Action_FollowDirection(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);

enum LEVEL_PROGRESS_LAYOUT : isize {
    LEVEL_PROGRESS_STRIDE = 0x2e24,
    LEVEL_PROGRESS_COMPLETION_FLAGS_OFFSET = 0x2800,
    LEVEL_PROGRESS_STORY_COMPLETE = 1 << 0,
};

enum CREATE_CREATURE_LIMITS {
    CREATE_CREATURE_MAX_LOCATORS = 64,
    CREATE_CREATURE_MAX_MODELS = 10,
};

union AI_CONDITION_LEVEL_ARGUMENT {
    void *pointer;
    isize value;
};

static isize AIConditionArgumentValue(void *argument) {
    AI_CONDITION_LEVEL_ARGUMENT condition_argument = {};
    condition_argument.pointer = argument;
    return condition_argument.value;
}

static char *ActionParamValue(char *param, const char *name) {
    if (param == NULL) {
        return NULL;
    }

    const i32 length = NuStrLen(name);
    if (NuStrNICmp(param, name, length) != 0 || param[length] != '=') {
        return NULL;
    }
    return param + length + 1;
}

static void ActionCopyParam(char *destination, i32 capacity, const char *source) {
    i32 index = 0;
    while (index + 1 < capacity && source[index] != '\0') {
        destination[index] = source[index];
        ++index;
    }
    destination[index] = '\0';
}

// Game-specific AI actions and conditions (registered via
// RegisterAIScriptActions / RegisterAIScriptConditions). These are stubbed to
// satisfy the symbol baseline; the action/condition logic itself is not
// decompiled. Each stub matches the mangled symbol of the original binary.

__used__ static i32 Action_Idle(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_Kill(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_Launch(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                  i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddPart(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_BigJump(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanTurn(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_Explode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PlaySfx(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetBoss(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetHint(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetSide(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_4,
                                   i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_Activate(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddToSet(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_DontPush(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToNode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetLayer(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetParam(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_TakeOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_UseForce(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddDebris(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_BlockPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanAttack(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanDefend(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CnxHelper(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_DontAimAt(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_EatVictim(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ForcePush(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_NoShadows(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_NoTerrain(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetSpline(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_UseWeapon(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                     i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CancelHint(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_DeActivate(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_DontAttack(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_EnableSock(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FaceCamera(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FacePlayer(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FollowPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_count, i32 is_first_time, f32 elapsed) {
    (void)sys;

    if (packet == NULL || packet->owner == NULL || packet->owner->apiobj.objptr == NULL) {
        return 1;
    }
    if (packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 0;
    }

    f32 completion_time = 0.0f;
    if (is_first_time != 0) {
        packet->movement_target = NULL;

        f32 minimum_time = 0.0f;
        f32 maximum_time = 0.0f;
        for (i32 index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }

            char *value = NuStrIStr(params[index], "mintime");
            if (value != NULL) {
                minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            value = NuStrIStr(params[index], "maxtime");
            if (value != NULL) {
                maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
        }

        if (maximum_time > minimum_time) {
            processor->action_timer = NuRandFloat() * maximum_time + (1.0f - NuRandFloat()) * minimum_time;
        }
    }

    AIMoveInstruction(packet, NULL, 0.0f, NULL, AIPACKET_MOVEMENT_WANDER, packet->movement_instruction_parameter);

    if (processor->action_timer <= completion_time) {
        return 0;
    }

    processor->action_timer -= elapsed;
    return completion_time >= processor->action_timer;
}

__used__ static i32 Action_GoToOrigin(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GrabVictim(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_NoLosCheck(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ProbeDroid(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

static i32 Action_ResetTimer(AISYS *, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 param_count,
                             i32 is_first_time, f32) {
    if (is_first_time == 0) {
        return 1;
    }

    f32 minimum = 0.0f;
    f32 maximum = 0.0f;
    f32 exact = 0.0f;
    for (i32 param_index = 0; param_index < param_count; ++param_index) {
        char *value = NuStrIStr(params[param_index], "mintime=");
        if (value != NULL) {
            minimum = AIParamToFloatEx(packet, processor, value + 8);
            continue;
        }

        value = NuStrIStr(params[param_index], "maxtime=");
        if (value != NULL) {
            maximum = AIParamToFloatEx(packet, processor, value + 8);
            continue;
        }

        value = NuStrIStr(params[param_index], "time=");
        if (value != NULL) {
            exact = AIParamToFloatEx(packet, processor, value + 5);
        }
    }

    if (minimum != 0.0f || maximum != 0.0f) {
        const f32 maximum_random = NuRandFloat();
        const f32 minimum_random = NuRandFloat();
        processor->script_timer = maximum_random * maximum + (1.0f - minimum_random) * minimum;
    } else {
        processor->script_timer = exact;
    }
    return 1;
}

__used__ static i32 Action_SetLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetMessage(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SpinOnSpot(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_TakeDamage(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CameraShake(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CopyMessage(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CreateRider(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FaceLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FlatTerrain(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_count, i32 is_first_time, f32 elapsed) {
    enum GO_TO_LOCATOR_FLAGS : u8 {
        GO_TO_LOCATOR_ON_GROUND = 1 << 0,
        GO_TO_LOCATOR_XZ_RANGE_CHECK = 1 << 1,
        GO_TO_LOCATOR_FACE_OPPONENT = 1 << 2,
        GO_TO_LOCATOR_IGNORE_PATH = 1 << 3,
        GO_TO_LOCATOR_MUST_REACH_DESTINATION = 1 << 4,
    };

    if (packet == NULL || packet->owner == NULL || packet->owner->apiobj.objptr == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 1;
    }

    if (is_first_time == 0) {
        AILOCATOR *locator = static_cast<AILOCATOR *>(processor->action_data_3);
        if (locator == NULL) {
            return 1;
        }

        const i32 movement_mode = (processor->action_data_1 & GO_TO_LOCATOR_IGNORE_PATH) != 0
                                      ? AIPACKET_MOVEMENT_DIRECT
                                      : AIPACKET_MOVEMENT_TO_DESTINATION;
        AIMoveInstruction(packet, &locator->position, 0.0f, reinterpret_cast<AIPATHINFO *>(&locator->path),
                          movement_mode, packet->movement_instruction_parameter);

        if ((processor->action_data_1 & GO_TO_LOCATOR_FACE_OPPONENT) != 0 && packet->opponent != NULL) {
            packet->movement_look_target = &static_cast<APIOBJECT *>(packet->opponent)->position;
        }
        if ((processor->action_data_1 & GO_TO_LOCATOR_ON_GROUND) != 0 && packet->owner->apiobj.field_0x27d == 0) {
            return 0;
        }

        NUVEC distance_vector;
        const f32 distance_squared = (processor->action_data_1 & GO_TO_LOCATOR_XZ_RANGE_CHECK) != 0
                                         ? NuVecXZDistSqr(&packet->terrain_origin, &locator->position, &distance_vector)
                                         : NuVecDistSqr(&packet->terrain_origin, &locator->position, &distance_vector);
        const f32 reach_distance = packet->movement_instruction_parameter + 0.1f +
                                   elapsed * packet->owner->apiobj.horizontal_velocity_magnitude;
        if (reach_distance * reach_distance <= distance_squared) {
            if ((packet->field_0x1e6 & 0x40) != 0 &&
                (processor->action_data_1 & GO_TO_LOCATOR_MUST_REACH_DESTINATION) != 0 &&
                AIBigJumpToDestinationFn != NULL) {
                return AIBigJumpToDestinationFn(&packet->owner->apiobj, &locator->position) != 0;
            }
            return 0;
        }

        if ((processor->action_data_1 & GO_TO_LOCATOR_FACE_OPPONENT) == 0 || packet->opponent == NULL) {
            packet->movement_look_target = &processor->action_pos;
        }
        if (processor->action_timer <= 0.0f) {
            return 1;
        }
        processor->action_timer -= elapsed;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.0f;
        }
        return 0;
    }

    processor->action_data_1 = 0;
    processor->action_data_3 = processor->unknown_a4;

    i32 random_locator_count = 0;
    i32 use_personal_name = 0;
    i32 use_indexed_name = 0;
    f32 minimum_time = 0.0f;
    f32 maximum_time = 0.0f;
    char locator_name[64];

    for (i32 index = 0; index < param_count; ++index) {
        char *param = params[index];
        if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(param, &packet->goal_speed_mode) != 0) {
            continue;
        }

        if (NuStrIStr(param, "name") != NULL && ++index < param_count) {
            if (use_indexed_name != 0 && packet->owner->apiobj.field_0x27c != -1) {
                sprintf(locator_name, "%s_%d", params[index], packet->owner->apiobj.field_0x27c);
            } else if (use_personal_name != 0 && packet->owner->apiobj.character_data != NULL &&
                       packet->owner->apiobj.character_data->file != NULL) {
                sprintf(locator_name, "%s_%s", params[index], packet->owner->apiobj.character_data->file);
            } else if (random_locator_count != 0) {
                sprintf(locator_name, "%s_%d", params[index], NuRand(NULL) % random_locator_count);
            } else {
                sprintf(locator_name, params[index]);
            }

            processor->action_data_3 = AIPathFindLocator(sys, locator_name);
            if (NuStrIStr(params[index - 1], "teleport") != NULL) {
                AILOCATOR *locator = static_cast<AILOCATOR *>(processor->action_data_3);
                if (locator == NULL) {
                    return 1;
                }
                packet->owner->apiobj.position = locator->position;
                return 1;
            }
            continue;
        }

        if (NuStrIStr(param, "teleport") != NULL) {
            continue;
        }
        if (NuStrIStr(param, "personal") != NULL) {
            use_personal_name = 1;
            continue;
        }
        if (NuStrIStr(param, "indexed") != NULL) {
            use_indexed_name = 1;
            continue;
        }

        char *value = NuStrIStr(param, "random");
        if (value != NULL) {
            random_locator_count = AIParamToFloatEx(packet, processor, value + NuStrLen("random") + 1);
            continue;
        }
        value = NuStrIStr(param, "waittime");
        if (value != NULL) {
            processor->action_timer = AIParamToFloatEx(packet, processor, value + NuStrLen("waittime") + 1);
            continue;
        }
        value = NuStrIStr(param, "mintime");
        if (value != NULL) {
            minimum_time = AIParamToFloatEx(packet, processor, value + NuStrLen("mintime") + 1);
            continue;
        }
        value = NuStrIStr(param, "maxtime");
        if (value != NULL) {
            maximum_time = AIParamToFloatEx(packet, processor, value + NuStrLen("maxtime") + 1);
            continue;
        }
        value = NuStrIStr(param, "goalrange");
        if (value != NULL) {
            packet->movement_instruction_parameter =
                AIParamToFloatEx(packet, processor, value + NuStrLen("goalrange") + 1);
            continue;
        }

        if (NuStrICmp(param, "on_ground") == 0) {
            processor->action_data_1 |= GO_TO_LOCATOR_ON_GROUND;
        } else if (NuStrICmp(param, "xz_rangecheck") == 0) {
            processor->action_data_1 |= GO_TO_LOCATOR_XZ_RANGE_CHECK;
        } else if (NuStrICmp(param, "face_opponent") == 0) {
            processor->action_data_1 |= GO_TO_LOCATOR_FACE_OPPONENT;
        } else if (NuStrICmp(param, "ignore_path") == 0) {
            processor->action_data_1 |= GO_TO_LOCATOR_IGNORE_PATH;
        } else if (NuStrICmp(param, "must_reach_destination") == 0) {
            processor->action_data_1 |= GO_TO_LOCATOR_MUST_REACH_DESTINATION;
        } else {
            packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, param);
        }
    }

    if (processor->action_timer == 0.0f) {
        if (maximum_time > minimum_time) {
            processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
        } else {
            processor->action_timer = 0.01f;
        }
    }

    AILOCATOR *locator = static_cast<AILOCATOR *>(processor->action_data_3);
    if (locator == NULL) {
        return 1;
    }

    const i32 movement_mode = (processor->action_data_1 & GO_TO_LOCATOR_IGNORE_PATH) != 0
                                  ? AIPACKET_MOVEMENT_DIRECT
                                  : AIPACKET_MOVEMENT_TO_DESTINATION;
    AIMoveInstruction(packet, &locator->position, 0.0f, reinterpret_cast<AIPATHINFO *>(&locator->path), movement_mode,
                      packet->movement_instruction_parameter);

    processor->action_pos.x = 0.0f;
    processor->action_pos.y = 0.0f;
    processor->action_pos.z = 1.0f;
    NuVecRotateY(&processor->action_pos, &processor->action_pos, locator->flags);
    NuVecAdd(&processor->action_pos, &processor->action_pos, &locator->position);
    return 0;
}

__used__ static i32 Action_InitRowDist(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_NoIdleSpeed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_RequiresLOS(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_Respawnable(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetDontMove(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetRunSpeed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetTaggable(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                       i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ApplyGravity(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanBeCarried(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanOpenDoors(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanSeeBehind(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanUseWeapon(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CannotBeSeen(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CannotDropIn(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_EngageObject(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FaceOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToNewLevel(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)param_6;

    if (param_5 != 0 && param_4 > 0) {
        LEVELDATA *level = NULL;
        char *cutscene_name = NULL;

        i32 param_index = 0;
        do {
            char *value = NuStrIStr(params[param_index], "level=");
            if (value != NULL) {
                level = Level_FindByName(value + NuStrLen("level="), NULL);
            } else {
                value = NuStrIStr(params[param_index], "cutscene=");
                if (value != NULL) {
                    cutscene_name = value + NuStrLen("cutscene=");
                }
            }
            ++param_index;
        } while (param_4 != param_index);

        if (FreePlay == 0 && cutscene_name != NULL &&
            NewCutScene(NULL, WORLD->cutscene_sys, cutscene_name, 0) != NULL) {
            return 1;
        }

        if (level != NULL) {
            GoToNewLevel(level->idx);
        }
    }

    return 1;
}

__used__ static i32 Action_NotWithParty(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_RaceOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ResetContext(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetAnimation(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetForceBack(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetHitPoints(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetInterrupt(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetLevelPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetStateArea(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetWalkSpeed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SnapToOrigin(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_TagCharacter(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_TurnOnPickup(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                        i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddPartDebris(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CanPullLevers(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CircleLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CnxController(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CompleteLevel(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FaceCharacter(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FormationMove(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GizmoActivate(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToLevelPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ImmuneToBolts(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_KeepWeaponOut(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ReleaseVictim(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ResetToOrigin(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ReturnToState(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetHoverPhase(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetLocatorSet(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetMoveRadius(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ShadowTerrain(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SnapToLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SnapWeaponOut(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_TriggerBlowUp(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_UpdateSockPos(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_WalkBackwards(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                         i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddMiscPickups(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AlertCreatures(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AlwaysBackFlip(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AnimTimeRandom(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AttackOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_BreakFormation(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ClearInterrupt(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CycleCharacter(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_DontRaycastLOS(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_EngageOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FollowOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ForceLightning(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToNodeRandom(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_LetGoOfBalloon(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PlayGizSpecial(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PrefersPlayers(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PressTagButton(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetCanTakeOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetPathCnxFlag(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetScriptParam(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetScriptState(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SnapToPosition(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ThrowDetonator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                          i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddGameMsgCount(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CreateCreatures(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)param_6;
    if (param_5 == 0 || sys == NULL) {
        return 1;
    }

    AILOCATOR *locators[CREATE_CREATURE_MAX_LOCATORS];
    i32 locator_count = 0;
    i16 models[CREATE_CREATURE_MAX_MODELS];
    i32 model_count = 0;
    AILOCATORSET *locator_set = NULL;
    char script_name[64] = "default";
    char state_name[64] = "";
    f32 x_offset = 0.0f;
    f32 y_offset = 0.0f;
    f32 z_offset = 0.0f;
    i32 creature_set = 0;
    i32 set_on_surface = 1;

    for (i32 param_index = 0; param_index < param_4; ++param_index) {
        char *param = params[param_index];
        char *value = ActionParamValue(param, "locator_set");
        if (value != NULL) {
            locator_set = AIPathFindLocatorSet(sys, value);
            if (locator_set == NULL) {
                continue;
            }

            AILocatorSet_CheckLocatorsStillAssigned(sys, locator_set);
            for (i32 index = 0; index < locator_set->locator_count && locator_count < CREATE_CREATURE_MAX_LOCATORS;
                 ++index) {
                if (locator_set->assigned[index] != 0xff) {
                    continue;
                }

                const u8 locator_index = locator_set->locator_entries[index];
                if (locator_index < sys->locator_count) {
                    locators[locator_count++] = &sys->locators[locator_index];
                }
            }
            continue;
        }

        value = ActionParamValue(param, "locator");
        if (value != NULL) {
            for (i32 index = 0; index < sys->locator_count && locator_count < CREATE_CREATURE_MAX_LOCATORS; ++index) {
                if (NuStrICmp(sys->locators[index].name, value) == 0) {
                    locators[locator_count++] = &sys->locators[index];
                    break;
                }
            }
            continue;
        }

        value = ActionParamValue(param, "type");
        if (value != NULL && model_count < CREATE_CREATURE_MAX_MODELS) {
            i16 model = -1;
            if (NuStrICmp(value, "RandomMap") == 0) {
                model = Hub_GetRandomCharType();
            } else if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                if (WORLD->current_level == HUB_LDATA) {
                    i16 *hub_character = NULL;
                    if (NuStrICmp(value, "Barman") == 0) {
                        hub_character = &id_BARMAN;
                    } else if (NuStrICmp(value, "JABBA") == 0) {
                        hub_character = &id_JABBA;
                    } else if (NuStrICmp(value, "CANTINABAND") == 0) {
                        hub_character = &id_CANTINABAND;
                    }

                    if (hub_character != NULL && apicharsys->playermodelids[*hub_character] != -1) {
                        model = *hub_character;
                    }
                } else {
                    const i32 level_character = LevelCharacterTypeIDFn(value);
                    if (level_character != -1) {
                        model = LevelCharacterGlobalIDFn(static_cast<u8>(level_character));
                    }
                }
            }
            if (model != -1) {
                models[model_count++] = model;
            }
            continue;
        }

        value = ActionParamValue(param, "script");
        if (value != NULL) {
            ActionCopyParam(script_name, sizeof(script_name), value);
            continue;
        }
        value = ActionParamValue(param, "state");
        if (value != NULL) {
            ActionCopyParam(state_name, sizeof(state_name), value);
            continue;
        }
        value = ActionParamValue(param, "xoffset");
        if (value != NULL) {
            x_offset = AIParamToFloat(processor, value);
            continue;
        }
        value = ActionParamValue(param, "yoffset");
        if (value != NULL) {
            y_offset = AIParamToFloat(processor, value);
            continue;
        }
        value = ActionParamValue(param, "zoffset");
        if (value != NULL) {
            z_offset = AIParamToFloat(processor, value);
            continue;
        }
        value = ActionParamValue(param, "addtoset");
        if (value != NULL) {
            creature_set = NuStrICmp(value, "myset") == 0 && packet != NULL ? packet->creature_set : NuAToI(value);
            continue;
        }
        if (NuStrICmp(param, "dont_set_on_surface") == 0) {
            set_on_surface = 0;
        }
    }

    if (locator_count == 0 || model_count == 0) {
        return 1;
    }

    const i32 locator_choice = qrand() / (0xffff / locator_count + 1);
    const i32 model_choice = qrand() / (0xffff / model_count + 1);
    AILOCATOR *locator = locators[locator_choice];
    NUVEC position = locator->position;
    position.x += x_offset;
    position.y += y_offset;
    position.z += z_offset;

    GameObject_s *object = AddDynamicCreature(models[model_choice], &position, locator->flags, script_name,
                                              reinterpret_cast<AIPATHINFO *>(&locator->path), NULL, set_on_surface,
                                              NULL, NULL, 0, creature_set);
    if (object == NULL) {
        return 1;
    }

    if (state_name[0] != '\0') {
        AIScriptSetBaseScriptStateByName(reinterpret_cast<AISCRIPTPROCESS *>(&object->ai), state_name);
    }
    object->ai.locator = locator;
    object->ai.locator_set = locator_set;

    if (locator_set != NULL) {
        const i32 selected_index = static_cast<i32>(locator - sys->locators);
        for (i32 index = 0; index < locator_set->locator_count; ++index) {
            if (locator_set->locator_entries[index] == selected_index) {
                locator_set->assigned[index] = object->apiobj.field_0x289;
                break;
            }
        }
    }
    return 1;
}

extern "C" {
    // This target-order prefix keeps CreateCreatures at its shipped game-action
    // index. Later actions remain unregistered until their callbacks are needed.
    AIACTIONDEF lego_aiactiondefs[] = {
        {"Activate", Action_Activate, 1, 0, 0},
        {"DeActivate", Action_DeActivate, 1, 0, 0},
        {"GoToLevelPath", Action_GoToLevelPath, 0, 0, 0},
        {"SetPath", Action_SetPath, 0, 0, 0},
        {"GoToOriginalPath", Action_GoToOriginalPath, 0, 0, 0},
        {"SnapToLocator", Action_SnapToLocator, 1, 0, 0},
        {"SetLocator", Action_SetLocator, 1, 0, 0},
        {"SetLocatorSet", Action_SetLocatorSet, 0, 0, 0},
        {"SnapToOrigin", Action_SnapToOrigin, 1, 0, 0},
        {"BigJumpToLocator", Action_BigJumpToLocator, 0, 0, 0},
        {"BigJump", Action_BigJump, 0, 0, 0},
        {"SetDoomedEscapeLocator", NULL, 0, 0, 0},
        {"SnapToPosition", Action_SnapToPosition, 1, 0, 0},
        {"SnapToSockPosition", NULL, 1, 0, 0},
        {"SetAnimation", Action_SetAnimation, 0, 0, 0},
        {"AnimTimeRandom", Action_AnimTimeRandom, 0, 0, 0},
        {"CanOpenDoors", Action_CanOpenDoors, 0, 0, 0},
        {"CanShootOffScreen", NULL, 0, 0, 0},
        {"KeepWeaponOut", Action_KeepWeaponOut, 0, 0, 0},
        {"SnapWeaponOut", Action_SnapWeaponOut, 1, 0, 0},
        {"ResetContext", Action_ResetContext, 0, 0, 0},
        {"PrefersPlayers", Action_PrefersPlayers, 0, 0, 0},
        {"SetBoltsDontGetDeflectedBack", NULL, 0, 0, 0},
        {"CanShootObstructions", NULL, 0, 0, 0},
        {"UseBigJumpToJump", Action_UseBigJumpToJump, 0, 0, 0},
        {"SetTaggable", Action_SetTaggable, 1, 0, 0},
        {"CatchUpForbidden", Action_CatchUpForbidden, 0, 0, 0},
        {"CannotDropIn", Action_CannotDropIn, 1, 0, 0},
        {"CanAttack", Action_CanAttack, 0, 0, 0},
        {"NotWithParty", Action_NotWithParty, 1, 0, 0},
        {"TakeDamage", Action_TakeDamage, 0, 0, 0},
        {"TagCharacter", Action_TagCharacter, 1, 0, 0},
        {"CanHitForceObjects", NULL, 0, 0, 0},
        {"AlwaysBackFlip", Action_AlwaysBackFlip, 0, 0, 0},
        {"PlayerSpeederHack", NULL, 0, 0, 0},
        {"SetAnimSpeedMul", Action_SetAnimSpeedMul, 0, 0, 0},
        {"SetSide", Action_SetSide, 1, 0, 0},
        {"SetStateArea", Action_SetStateArea, 0, 0, 0},
        {"SetOpponent", Action_SetOpponent, 0, 0, 0},
        {"AttackOpponent", Action_AttackOpponent, 0, 0, 0},
        {"EngageOpponent", Action_EngageOpponent, 0, 0, 0},
        {"ShootAtOpponent", Action_ShootAtOpponent, 0, 0, 0},
        {"EngageObject", Action_EngageObject, 0, 0, 0},
        {"GrabVictim", Action_GrabVictim, 0, 0, 0},
        {"EatVictim", Action_EatVictim, 0, 0, 0},
        {"ReleaseVictim", Action_ReleaseVictim, 0, 0, 0},
        {"CanDefend", Action_CanDefend, 0, 0, 0},
        {"CharClipToBlobShadows", NULL, 0, 0, 0},
        {"DontAimAt", Action_DontAimAt, 0, 0, 0},
        {"CanUseWeapon", Action_CanUseWeapon, 0, 0, 0},
        {"SetBoss", Action_SetBoss, 0, 0, 0},
        {"UpdateSockPos", Action_UpdateSockPos, 0, 0, 0},
        {"UseForce", Action_UseForce, 0, 0, 0},
        {"TriggerBlowUp", Action_TriggerBlowUp, 0, 0, 0},
        {"ForcePush", Action_ForcePush, 0, 0, 0},
        {"DeflectPlayersPart", NULL, 0, 0, 0},
        {"Kill", Action_Kill, 1, 0, 0},
        {"Explode", Action_Explode, 0, 0, 0},
        {"SetScriptState", Action_SetScriptState, 0, 0, 0},
        {"SetAIOverrideControl", NULL, 0, 0, 0},
        {"SetLastSafePathPos", NULL, 0, 0, 0},
        {"SetDontMove", Action_SetDontMove, 0, 0, 0},
        {"DontSetStoppedFlag", NULL, 0, 0, 0},
        {"PressSpecialButton", NULL, 0, 0, 0},
        {"PressTagButton", Action_PressTagButton, 0, 0, 0},
        {"PressActionButton", NULL, 0, 0, 0},
        {"UseWeapon", Action_UseWeapon, 0, 0, 0},
        {"SetInvulnerable", Action_SetInvulnerable, 0, 0, 0},
        {"DontPush", Action_DontPush, 0, 0, 0},
        {"DontAvoidCharacter", NULL, 0, 0, 0},
        {"PressJumpButton", Action_PressJumpButton, 0, 0, 0},
        {"AddToSet", Action_AddToSet, 0, 0, 0},
        {"SetSpline", Action_SetSpline, 0, 0, 0},
        {"SetControlSystem", Action_SetControlSystem, 0, 0, 0},
        {"SetZeroAcceleration", NULL, 0, 0, 0},
        {"FollowDirection", Action_FollowDirection, 0, 0, 0},
        {"BreakFormation", Action_BreakFormation, 0, 0, 0},
        {"FormationMove", Action_FormationMove, 0, 0, 0},
        {"CreateCreatures", Action_CreateCreatures, 0, 0, 0},
        {NULL, NULL, 0, 0, 0},
    };
}

DECOMP_ASSERT(sizeof(lego_aiactiondefs) / sizeof(lego_aiactiondefs[0]) == LEGO_AI_ACTION_CREATE_CREATURES + 2,
              "CreateCreatures target action index");

__used__ static i32 Action_FollowCharacter(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_FollowDirection(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PlayGizObstacle(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_PressJumpButton(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ReleaseTakeOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ResetGameCamera(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetAnimSpeedMul(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetCurrentSpeed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetHearDistance(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetHintComplete(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetInvulnerable(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetLastAttacker(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetUseOneAtOnce(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetViewDistance(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_ShootAtOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_UseCurrentSpeed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                           i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_AddTorpedoPacket(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_BigJumpToLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CatchUpForbidden(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_CheckWallSplines(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_GoToOriginalPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_JudderGameCamera(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_MoveAwayFromNode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetControlSystem(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetGravityHeight(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetMaxViewHeight(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetMinViewHeight(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetObstacleToEnd(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetReturnToState(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_SetScaleOverride(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static i32 Action_UseBigJumpToJump(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
                                            i32 param_4, i32 param_5, f32 param_6) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

__used__ static f32 Condition_IAm(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg, void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_PSP(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg, void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IAmA(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Indy(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Side(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_XPos(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_YPos(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ZPos(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                   void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Debug(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                    void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_MySet(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                    void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Param(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                    void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

static f32 Condition_Timer(AISYS *, AISCRIPTPROCESS *processor, AIPACKET *, char *, void *) {
    return processor->script_timer;
}

__used__ static f32 Condition_Active(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                     void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GotGun(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                     void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_OnPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                     void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

static f32 Condition_Random(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *) {
    return NuRandFloat();
}

__used__ static f32 Condition_BeenHit(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Context(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_InSwamp(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IsAlive(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

static f32 Condition_Message(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *void_arg) {
    GIZAIMESSAGE_s *message = static_cast<GIZAIMESSAGE_s *>(void_arg);
    return message != NULL ? message->value : 0.0f;
}

__used__ static f32 Condition_MusicOn(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_RaceLap(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Blocking(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                       void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

static f32 Condition_Freeplay(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *) {
    return FreePlay != 0 ? 1.0f : 0.0f;
}

__used__ static f32 Condition_GlynTest(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                       void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_OnGround(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                       void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_OnObject(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                       void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Colliding(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GotVictim(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_HitPoints(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IAmABaddy(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IAmAGoody(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_InContext(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_InMiniCut(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IsVisible(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_LastLevel(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Player1Is(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_Player2Is(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_StuckTime(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_TakenOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                        void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BeingTowed(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_CategoryIs(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ForceAtEnd(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GotLocator(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_HoverPhase(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IAmPlayer2(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IsOnScreen(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IsSetAlive(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_MissionWon(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_NumBaddies(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ScreenWipe(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ShopActive(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_SpawnCount(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_UsingForce(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                         void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BeenAlerted(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BeenSpawned(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

static f32 Condition_BeenToLevel(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *void_arg) {
    const isize area_level = AIConditionArgumentValue(void_arg);
    if (area_level == -1) {
        return 0.0f;
    }
    const u8 *progress = static_cast<const u8 *>(LevelProgressData) + area_level * LEVEL_PROGRESS_STRIDE;
    return (progress[LEVEL_PROGRESS_COMPLETION_FLAGS_OFFSET] & LEVEL_PROGRESS_STORY_COMPLETE) != 0 ? 1.0f : 0.0f;
}

__used__ static f32 Condition_GotOpponent(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_HasTakeOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_IAmANeutral(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_InLevelNode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_InterruptID(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_MissionMode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_OpponentIsA(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_OriginRange(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_PathBlocked(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_PlayerRange(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ScriptParam(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_TimeOffPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_TurretAlive(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_AnimSpeedMul(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_AreaComplete(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BehindCamera(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_CanHearRadio(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ForceAtStart(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ForcePushing(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GizmoOutput0(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GizmoOutput1(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GizmoOutput2(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_GizmoOutput3(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_HintComplete(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_LocatorRange(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_PlayerInSock(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_PlayerOnPath(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                           void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BeenTakenOver(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                            void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_BlowupBlownup(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                            void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_ChallengeMode(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                            void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static f32 Condition_CheatProgress(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                            void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
}

__used__ static void *Condition_IAmInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_IAmAInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_SideInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_ForceInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_BlowupInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_XYZPosInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_BeenHitInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_IsAliveInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

static void *Condition_MessageInit(AISYS *system, char *arg, AISCRIPT *) {
    if (system == NULL || arg == NULL || gizaimessagesys == NULL) {
        return NULL;
    }
    return CheckGizAIMessage(gizaimessagesys, arg, NULL);
}

__used__ static void *Condition_ObstacleInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

__used__ static void *Condition_OnObjectInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
}

namespace {
    struct AISysRegistryCallbacks {
        AISysRegistryCallbacks() {
            api_aiactiondefs[API_AI_ACTION_IDLE].eval_fn = Action_Idle;
            api_aiactiondefs[API_AI_ACTION_RESET_TIMER].eval_fn = Action_ResetTimer;
            api_aiactiondefs[API_AI_ACTION_GO_TO_LOCATOR].eval_fn = Action_GoToLocator;
            api_aiactiondefs[API_AI_ACTION_FOLLOW_PATH].eval_fn = Action_FollowPath;
            api_aiconditiondefs[API_AI_CONDITION_TIMER].eval_fn = Condition_Timer;
            api_aiconditiondefs[API_AI_CONDITION_RANDOM].eval_fn = Condition_Random;

            lego_aiconditiondefs[LEGO_AI_CONDITION_BEEN_TO_LEVEL].eval_fn = Condition_BeenToLevel;
            lego_aiconditiondefs[LEGO_AI_CONDITION_MESSAGE].eval_fn = Condition_Message;
            lego_aiconditiondefs[LEGO_AI_CONDITION_MESSAGE].init_fn = Condition_MessageInit;
            lego_aiconditiondefs[LEGO_AI_CONDITION_FREEPLAY].eval_fn = Condition_Freeplay;
        }
    };

    AISysRegistryCallbacks aisys_registry_callbacks;
} // namespace
