#include "decomp.h"
#include "legoapi_types.h"

// Static game message and hint helpers. Stubbed to satisfy the symbol baseline.

static __used__ void EndRedBrickMessage(GAMEMESSAGE_s *) {
}

static __used__ int GameMsg_GetExtraObj(GAMEMESSAGE_s *) {
    return 0;
}

static __used__ void GameMsg_EndDelay_Game(GAMEMESSAGE_s *) {
}

static __used__ void GameMsg_Draw_MiniKitDetector(GAMEMESSAGE_s *, nuvec_s *, float) {
}

static __used__ void Hint_AlphaTarget() {
}

static __used__ bool HoldTag_UpdateHint(HINT_s *) {
    return false;
}

static __used__ bool HatMachine_UpdateHint(HINT_s *) {
    return false;
}
