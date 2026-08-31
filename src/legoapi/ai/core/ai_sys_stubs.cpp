#include "legoapi/world/world_shared.h"
#include "decomp.h"

#include <string.h>

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

    void *AIPAthFindPathCnx(AISYS_s *, i32, char *, void *, void *) {
        return NULL;
    }

    void *AIPathFindLocator(AISYS_s *, char *) {
        return NULL;
    }

    void AIPathFindLocatorSet(void) {
    }

    void *AIPathFindNode(AISYS_s *, char *, i32) {
        return NULL;
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

    // libTTapp.so 0x3e5fd0: carve a zeroed, 16-byte-aligned block from the
    // permbuffer cursor. Returns NULL when the cursor or the end pointer is
    // missing or the buffer has less room than requested.
    void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *buf_end, u32 size) {
        void *block = NULL;
        if (cursor != NULL && buf_end != NULL) {
            const u32 offset = (u32)cursor->addr;
            if (buf_end->addr > (usize)(offset + size)) {
                block = (void *)(usize)((offset + 0xf) & ~0xfu);
                cursor->addr = (usize)block + size;
                memset(block, 0, size);
            }
        }
        return block;
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

    void *AISysFindArea(void *, char *) {
        return NULL;
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
