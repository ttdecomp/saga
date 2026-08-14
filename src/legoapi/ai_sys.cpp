#include "legoapi/world_shared.h"
#include "decomp.h"
struct nuhspecial_s;
struct minitrooperteam_s;
struct nuvec_s;

extern "C" void *AISysLoad(void *buf, void *buf_end, i32 size, void *gscn, char *dir, char *name, char *param) {
    (void)buf;
    (void)buf_end;
    (void)size;
    (void)gscn;
    (void)dir;
    (void)name;
    (void)param;
    return NULL;
}
void *AIPathCnxControlSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AIPathCnxHelperSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AITriggerSetSysCreate(VARIPTR *buf, VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void GameAIScriptAddLevelSfx(WORLDINFO *world, NULISTHDR *scripts) {
    (void)world;
    (void)scripts;
}
void *CreateClimbObjectSys(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
extern "C" void *APIObjectSysInit(i32 size, void *buf, void *buf_end) {
    (void)size;
    (void)buf;
    (void)buf_end;
    return NULL;
}

static __used__ unsigned int AIRespawnOnPath(APIOBJECT_s*) { return {}; }

static __used__ int Collide2Objects(APIOBJECT_s*, APIOBJECT_s*) { return {}; }

static __used__ unsigned int CalculateIntersection(AISYS_s*, AIPACKET_s*, APIOBJECT_s*, AIPATHCNX_s*, AIPATHCNX_s*) { return {}; }

static __used__ unsigned int AIBigJumpToDestination(APIOBJECT_s*, nuvec_s*) { return {}; }

static __used__ unsigned int AISysCharacterTestPathCnx(AISYS_s*, APIOBJECT_s*, AIPACKET_s*, AIPATHCNX_s*, int, float*) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Circle(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Ellipse(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Rectangle(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

static __used__ void GameAILoad(AISYS_s *, int, nugscn_s *, variptr_u *, variptr_u *) {
}

static __used__ void FormationMove(AIGROUP_s *, int (*)(AIGROUP_s *, AIROW_s *, AIROW_s *, APIOBJECT_s *)) {
}

static __used__ APIOBJECT_s *GetNamedAPIObject(AISYS_s *, char *) {
    return nullptr;
}

static __used__ void *GetNextConnection(AIPACKET_s const *, int *) {
    return nullptr;
}

static __used__ nuvec_s *GetAICreatureOrigin(AISYS_s *, AIPACKET_s *) {
    return nullptr;
}

static __used__ int GameAIActionParseSpeed(char *, unsigned char *) {
    return 0;
}

static __used__ void GameFindAlternativeSpecialObject(AISYS_s *, nuhspecial_s *) {
}

static __used__ void GenerateTrooperTeamShape(minitrooperteam_s *, int) {
}

extern "C" {

    void AIAntinodeCreate(void) {
    }

    void AIAntinodeCreateSingleFrame(void) {
    }

    void AIAntinodeCullSingleFrame(void) {
    }

    void AIAntinodeDestroy(void) {
    }

    void AIAntinodeMove(void) {
    }

    void AIFormationFollow(void) {
    }

    void AILocatorSet_AssignFurthestLocator(void) {
    }

    void AILocatorSet_AssignNearestLocator(void) {
    }

    void AILocatorSet_AssignRandomLocator(void) {
    }

    void AILocatorSet_CheckLocatorsStillAssigned(void) {
    }

    void AIMoveInstruction(void) {
    }

    void AIPAthFindPathCnx(void) {
    }

    void AIPathFindLocator(void) {
    }

    void AIPathFindLocatorSet(void) {
    }

    void AIPathFindNode(void) {
    }

    void AIPathFindPathCnxFromIX(void) {
    }

    void AIPathNodeDistanceToPathNode(void) {
    }

    void AIPathNodeUpdatePos(void) {
    }

    void AISYSRebuildFromEditorData(void) {
    }

    void AIScriptForceParamReEval(void) {
    }

    void AIScriptNameFromIx(void) {
    }

    void AIScriptProcess(void) {
    }

    void AIScriptSetLevelPath(void) {
    }

    void AISetPathHeightTol(void) {
    }

    void AISysBufferAlloc(void) {
    }

    void AISysCharacterMovement(void) {
    }

    void AISysCharacterSetPath(void) {
    }

    void AISysCharacterSetPathCnx(void) {
    }

    void AISysCreatureAntinodeInteraction(void) {
    }

    void AISysCreatureInteraction2D(void) {
    }

    void AISysCreatureInteraction3D(void) {
    }

    void AISysFindArea(void) {
    }

    void AISysFindPath(void) {
    }

    void AISysFindRoute(void) {
    }

    void AISysGetCharacterPathPos(void) {
    }

    void AISysGetCharacterWaypoint(void) {
    }

    void AISysGetPathColour(void) {
    }

    void AISysGetPathColourCount(void) {
    }

    void AISysGetPathPos(void) {
    }

    void AISysGetPathPosEx(void) {
    }

    void AISysLoadEx(void) {
    }

    void AISysProcess(void) {
    }

    void AISysProcessCharacter(void) {
    }

    void AISysSetLevelPath(void) {
    }

    void AISysSetPathCylinderCheck(void) {
    }

    void AISysUpdateCharacterPathPos(void) {
    }

    void AddLocalAIMessage(void) {
    }

    void AddToAIGroup(void) {
    }

    void AiRndrLine3d(void) {
    }

    void AiRndrLine3dDbg(void) {
    }

    void AiSysOnlyUsePakFile(void) {
    }

    void AiSysSetStateDebugee(void) {
    }

    void AiSysUsePackFile(void) {
    }

    void CalculateLocatorDirection(void) {
    }

    void CheckAIMessage(void) {
    }

    void ClearAIMessageSys(void) {
    }

    void CreateAIGroup(void) {
    }

    void CreateAIMessageSys(void) {
    }

    void DestroyAIGroup(void) {
    }

    void FindAIDirectionedRandomPointOnNetwork2D(void) {
    }

    void FindLocalAIMessage(void) {
    }

    void FollowAPIObject(void) {
    }

    void GetAIMessage(void) {
    }

    void InitFn_AIActionParseSpeed(void) {
    }

    void InitFn_AIBigJumpToDestination(void) {
    }

    void InitFn_AIPathDeleted(void) {
    }

    void InitFn_AIPathNodeDeleted(void) {
    }

    void InitFn_AIPathNodeMoved(void) {
    }

    void InitFn_AIRespawnOnPath(void) {
    }

    void InitFn_APIOBJECTFromObjIDFn(void) {
    }

    void InitFn_ClearAICreatures(void) {
    }

    void InitFn_FindAlternativeSpecialObjectFn(void) {
    }

    void InitFn_GameAILoad(void) {
    }

    void InitFn_GameAISYSRebuildFromEditorData(void) {
    }

    void InitFn_GameAISave(void) {
    }

    void InitFn_GameParamToFloat(void) {
    }

    void InitFn_GetAICreatureOrigin(void) {
    }

    void InitFn_GetNamedAPIObject(void) {
    }

    void InitFn_ScriptProcessFirstTimeAction(void) {
    }

    void LEGO_AISysCreatureInteraction2D(void) {
    }

    void QueryAIMessage(void) {
    }

    void QueryLocalMessage(void) {
    }

    void RemoveAIMessage(void) {
    }

    void ResetAIMessageSys(void) {
    }

    void SetAIMessage(void) {
    }

    void SetScriptErrorLevel(void) {
    }

} // extern "C"
