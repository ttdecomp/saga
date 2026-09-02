#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"

void GameCam_Reset(GAMECAMERA_s *camera);

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

__used__ static i32 Action_ResetTimer(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params,
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
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
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
    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    return 0;
}

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
    if (param_5 != 0) {
        GameCam_Reset(GameCam);
    }
    return 1;
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

__used__ static f32 Condition_Timer(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                    void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
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

__used__ static f32 Condition_Random(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                     void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
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

__used__ static f32 Condition_Message(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                      void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
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

__used__ static f32 Condition_Freeplay(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                       void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
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

__used__ static f32 Condition_BeenToLevel(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg,
                                          void *void_arg) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)arg;
    (void)void_arg;
    return 0.0f;
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

__used__ static void *Condition_MessageInit(AISYS *sys, char *arg, AISCRIPT *script) {
    (void)sys;
    (void)arg;
    (void)script;
    return NULL;
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
