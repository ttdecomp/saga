#include "legoapi/gameobject.h"
#include "legoapi/gizmos/gizforce.h"
#include "legoapi/gizmos/gizobstacles.h"

struct GAMEANIMSET_s;
struct WORLDINFO_s;

struct GIZMOBLOWUP_s {
    MechObjectInterface *GetMechObjectInterface();
};

MechObjectInterface *GIZMOBLOWUP_s::GetMechObjectInterface() {
    return NULL;
}

i32 GizForce_Complete(GIZFORCE *) {
    return 0;
}

i32 GizForce_StoodOnForce(GIZFORCE *, GameObject_s *) {
    return 0;
}

void GameAnimSet_SetVisibility(GAMEANIMSET_s *, i32) {}
void GameAnimSet_JumpToStart(GAMEANIMSET_s *) {}
void GameAnimSet_EvaluateState(GAMEANIMSET_s *) {}
void GizObstacle_PlayBackwards(GIZOBSTACLE *) {}

void CollectPowerUp(GameObject_s *, NUVEC *, u16, i32) {}
void GizmoPickup_CollectCoin(WORLDINFO_s *, NUVEC *, i32, i32, GameObject_s *, i32) {}
void CollectMinikit(NUVEC *, char *, i32) {}
void CollectHitPoint(GameObject_s *, NUVEC *, i32) {}

void Hint_SetComplete(i32) {}

void Shards_HandleLostObj(WORLDINFO_s *, GameObject_s *) {}
