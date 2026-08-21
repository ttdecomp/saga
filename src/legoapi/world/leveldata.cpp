#include "legoapi/world/level_shared.h"

#include <string.h>

void ResetLevel(WORLDINFO_s *, char *, i32) {
}

void ClearLevData() {
    LevFlag[0] = 0;
    LevFlag[1] = 0;
    LevFlag[2] = 0;
    LevFlag[3] = 0;
    memset(LevHSpecial, 0, sizeof(LevHSpecial));
    LevSfxFlag[0] = 0;
    LevSfxFlag[1] = 0;
    LevSfxFlag[2] = 0;
    LevSfxFlag[3] = 0;
    memset(dynamic_antinodes, 0, 0x1500);
    memset(LevInstAnim, 0, sizeof(LevInstAnim));
    LevArea[0] = 0;
    LevArea[1] = 0;
    LevArea[2] = 0;
    LevArea[3] = 0;
    memset(LevPathNodes, 0, sizeof(LevPathNodes));
    memset(LevPathCnx, 0, sizeof(LevPathCnx));
    memset(LevGameObject, 0, sizeof(LevGameObject));
    memset(LevGamePart, 0, sizeof(LevGamePart));
    memset(LevAIMessage, 0, sizeof(LevAIMessage));
    LevelLocator = 0;
    memset(LevGizObst, 0, sizeof(LevGizObst));
    LevBlowUp[0] = 0;
    LevBlowUp[1] = 0;
    LevBlowUp[2] = 0;
    LevBlowUp[3] = 0;
    LevBlowUp[4] = 0;
    memset(LevGizmo, 0, sizeof(LevGizmo));
    LevSfxId[0] = 0;
    LevSfxId[1] = 0;
    LevSfxId[2] = 0;
    LevSfxId[3] = 0;
    memset(LevelCodeSpline, 0, sizeof(LevelCodeSpline));
    LevGizForce[0] = 0;
    LevGizForce[1] = 0;
    LevGizForce[2] = 0;
    LevGizForce[3] = 0;
    LevAIPathNode[0] = 0;
    LevAIPathNode[1] = 0;
    LevAIPathNode[2] = 0;
    LevAIPathNode[3] = 0;
    LevBoltIgnorePlatIds[0] = -1;
    LevBoltIgnorePlatIds[1] = -1;
    LevPlatID[0] = -1;
    LevPlatID[1] = -1;
    LevPathCnxDir = 0;
    LevDeaths = 0;
    LevLock[0] = 0;
    LevLock[1] = 0;
    LevLock[2] = 0;
    LevLock[3] = 0;
    LevSafePlatID[0] = -1;
    LevSafePlatID[1] = -1;
}

void *SetLevelHack(i32 size) {
    memset(LevelHackData, 0, 0x80);
    memset(OldLevelHackData, 0, 0x80);
    LevelHackSize = size;
    LevelHackSendTimer = 0;
    return LevelHackData;
}
