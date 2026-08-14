#include "legoapi_types.h"

void Hint_Reset() {
}

void Hint_Process(float) {
}

void Hint_SetHint(HINT_s *, i32, i32) {
}

void Hint_FindHint(i32) {
}

void Hint_SaveBits(i32, i32) {
}

void Hint_CurrentId() {
}

void Hint_ResetHint(i32, i32) {
}

void Hint_isComplete(HINT_s *) {
}

void Hint_isComplete(i32) {
}

void Hint_SetComplete(HINT_s *) {
}

void Hint_SetComplete(i32) {
}

void Hint_isAvailable(i32) {
}

void Hint_CancelCurrent() {
}

void Hint_ExpandButtons(char *, char *) {
}

void Hint_SaveGameState(HINT_s *) {
}

void Hint_SetHintFromId(i32, i32, i32) {
}

void Hint_LoadAllGameState() {
}

void Hint_SaveAllGameState() {
}

void Hint_ClearHintsAndDoneFlags() {
}

void Hint_Draw(i32) {
}

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

void Push_UpdateHints(HINT_s *) {
}

void Percent_UpdateHint(HINT_s *) {
}

void RegisterWithHintSys(void (*)(HINT_s *, i32), HINT_s *, u32 *, i32) {
}
