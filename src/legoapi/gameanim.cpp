#include "decomp.h"
#include "legoapi_types.h"

void Animate_POD(GameObject_s *) {
}

void Animate_ATAT(GameObject_s *) {
}

void Animate_JEDI(GameObject_s *) {
}

void AnimatePlayer(GameObject_s *) {
}

void Animate_BEAST(GameObject_s *) {
}

void Animate_BARMAN(GameObject_s *) {
}

void Animate_CANNON(GameObject_s *) {
}

void Animate_WALKER(GameObject_s *) {
}

void Animate_WEIRDO(GameObject_s *) {
}

void Animate_CRITTER(GameObject_s *) {
}

void Animate_DEFAULT(GameObject_s *) {
}

void Animate_VEHICLE(GameObject_s *) {
}

void Animate_DROIDEKA(GameObject_s *) {
}

void Animate_PROTOCOL(GameObject_s *) {
}

void GameAnimSet_Draw(GAMEANIMSET_s &) {
}

void GameAnimSet_Play(GAMEANIMSET_s *, float, i32) {
}

void GameAnimSet_Stop(GAMEANIMSET_s *) {
}

void Animate_ASTROMECH(GameObject_s *) {
}

void Animate_CHARACTER(GameObject_s *) {
}

void Animate_GEONOSIAN(GameObject_s *) {
}

void GameAnimSet_Reset(GAMEANIMSET_s *) {
}

void Animate_HOVERDROID(GameObject_s *) {
}

void GameAnimSet_Create(variptr_u *, variptr_u *, GAMEANIMOBJPOOL_s *, GAMEANIMSYS_s *) {
}

void Animate_BATTLEDROID(GameObject_s *) {
}

void Animate_SPEEDERBIKE(GameObject_s *) {
}

void GameAnimSet_Playing(GAMEANIMSET_s *) {
}

void GameAnimSet_EvalAnim(GAMEANIMSET_s *) {
}

void GameAnimSet_AddObject(GAMEANIMSET_s *, nuhspecial_s *, float, float, i32) {
}

void GameAnimSet_JumpToEnd(GAMEANIMSET_s *) {
}

void GameAnimSet_SetOffset(GAMEANIMSET_s *, nuvec_s *) {
}

void GameAnimSet_GetAnimPos(GAMEANIMOBJ_s *) {
}

void GameAnimSet_SetAnimPos(GAMEANIMOBJ_s *, float) {
}

void GameAnimSet_SetTFactor(GAMEANIMSET_s *, float) {
}

void Animate_REPUBLICGUNSHIP(GameObject_s *) {
}

void GameAnimSet_JumpToStart(GAMEANIMSET_s *) {
}

void Animate_SUPERBATTLEDROID(GameObject_s *) {
}

void GameAnimSet_RemoveObject(GAMEANIMSET_s *, GAMEANIMOBJ_s *) {
}

void GameAnimSet_ScaleFParam1(GAMEANIMSET_s *, float) {
}

void GameAnimSet_SetRepeating(GAMEANIMSET_s *, i32) {
}

void GameAnimSet_EvaluateState(GAMEANIMSET_s *) {
}

void GameAnimSet_GetAveragePos(GAMEANIMSET_s *, nuvec_s *, i32, i32, i32) {
}

void GameAnimSet_GetVisibility(GAMEANIMSET_s *) {
}

void GameAnimSet_JumpToAnimPos(GAMEANIMSET_s *, float) {
}

void GameAnimSet_RemoveSpecial(GAMEANIMSET_s *, nuhspecial_s *) {
}

void GameAnimSet_SetVisibility(GAMEANIMSET_s *, i32) {
}

void GameAnimSet_DrawReflection(GAMEANIMSET_s *, i32, float, numtx_s *) {
}

void GameAnimSet_AddObjectByName(GAMEANIMSET_s *, nugscn_s *, char *, float, float, i32, GIZMOSYS_s *, char *, char *) {
}

void GameAnimSet_AddToSystemList(GAMEANIMSET_s *) {
}

void GameAnimSet_AutoSetReflectY(GAMEANIMSET_s *, nuvec_s *, numtx_s *) {
}

void GameAnimSet_GetCurrentFrame(GAMEANIMSET_s *) {
}

void GameAnimSet_CreateObjectPool(variptr_u *, variptr_u *, i32, i32) {
}

void GameAnimSet_IsAnimationReset(GAMEANIMSET_s *) {
}

void GameAnimSet_RemoveAllObjects(GAMEANIMSET_s *) {
}

void GameAnimSet_GetCentreAndRadius(GAMEANIMSET_s *, nuvec_s *, float *, i32, i32, i32) {
}

void GameAnimSet_GetCompletionRatio(GAMEANIMSET_s *) {
}

void GameAnimSet_RemoveFromSystemList(GAMEANIMSET_s *) {
}

static __used__ i32 LoadAnim(char *, i32, variptr_u *, variptr_u) {
    return 0;
}
static __used__ i32 LoadAnimFromPAK(char *, i32, char *, i32) {
    return 0;
}
static __used__ void NormalizeAnimPath(char *) {
}

extern "C" {

    void ANI_Ani3ExtractAllNodeCurves(void) {
    }

    void ANI_FixUpAddrs(void) {
    }

    void ANI_SimpleAni3PlayerV4Joint(void) {
    }

    void ANI_SimpleAni3PlayerV4Joint_Blend(void) {
    }

    void ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(void) {
    }

    void ANI_SimpleAni3PlayerV4Joint_EulerQuat(void) {
    }

    void AddAnimEffects(void) {
    }

    void AnimBlendingFromTo(void) {
    }

    void AnimDuration(void) {
    }

    void AnimEndFrame(void) {
    }

    void AnimListFrame(void) {
    }

    void AnimListFrameArray(void) {
    }

    void AnimList_NoLoad(void) {
    }

    void AnimList_RequestAnimGroupForCreatures(void) {
    }

    void AnimList_RequestAnimGroups(void) {
    }

    void AnimMiscFlags(void) {
    }

    void AnimPacket_FullToMini(void) {
    }

    void AnimPacket_MiniToFull(void) {
    }

    void AnimPlaying(void) {
    }

    void AnimSpeed(void) {
    }

    void AnimStopFrame(void) {
    }

    void AnimsAvailableToBothCharacters(void) {
    }

    void BlendRootFn(void) {
    }

    void BlendTimeBetweenAnims(void) {
    }

    void CurrentAnim(void) {
    }

    void EvalModelAnim(void) {
    }

    void FindAnimIX(void) {
    }

    void GetAnimBlendMode(void) {
    }

    void GetAnimTimeRandom(void) {
    }

    void GetInstAnimEndFrame(void) {
    }

    void ResetAnimPacket(void) {
    }

    void ResetMiniAnimPacket(void) {
    }

    void RootFn(void) {
    }

    void RootFnY(void) {
    }

    void SetActionInfo(void) {
    }

    void SetAnimBlendMode(void) {
    }

    void SetAnimTimeRandom(void) {
    }

    void SetProceduralAnimationFn(void) {
    }

    void StateAnimEvaluate(void) {
    }

    void StateAnimEvaluate2(void) {
    }

    void StateAnimFixPtrs(void) {
    }

    void UpdateAnimPacket(void) {
    }

    void UpdateMiniAnimPacket(void) {
    }

} // extern "C"

void SetAnimFrame(nuhspecial_s *, float) {
}

void GetDefaultIdle(GameObject_s *) {
}

void GetAnimDirection(nuinstanim_s *) {
}

void FindTexAnimFromMtl(nugscn_s *, numtl_s *) {
}

void InitTexAnimScripts(char **) {
}

void GizmoFileReadGameAnimSet(GAMEANIMSET_s *, void *, void (*)(GAMEANIMOBJ_s *, unsigned char), unsigned char, char *,
                              char *) {
}

void ANI_SimpleAni3PlayerV4Joint_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, i32, i32) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ani3_animheader_s *, float, nuanimbuff_s *, float, i32, i32, nuvec_s *) {
}

void EvalAnim(nuhspecial_s *, float, numtx_s *, i32) {
}

void EvalAnim2(nuhspecial_s *, float) {
}

void *GameAnimSys_Create(variptr_u *buf, variptr_u *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}

void *GameAntnode_CreateSys(WORLDINFO_s *world, variptr_u *buf, variptr_u *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
