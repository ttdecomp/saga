#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/gameobject.h"

extern "C" void AISysGetCharacterPathPos(AISYS *, GameObject_s *, AISCRIPTPROCESS *, i32, i32) {}

extern "C" AIGROUP *CreateAIGroup(AISYS *, u8, f32, f32, i32) {
    return NULL;
}

extern "C" void AddToAIGroup(AIGROUP *, AIGROUP *, GameObject_s *) {}

extern "C" void AIScriptProcess(AISYS *, GameObject_s *, AISCRIPTPROCESS *, AISCRIPTPROCESS *, f32) {}

GameObject_s *GetNamedGameObject(AISYS *, char *) {
    return NULL;
}

void ResetAICreature(GameObject_s *, AISYS *) {}
