#include "legoapi/legoapi_types.h"

static f32 Tag_TransferTimer0 = 0.0f;
static f32 Tag_TransferTimer1 = 0.0f;

static const f32 Tag_TransferResetTimer = 0.5f;

void Tag_SetMode(i32) {
}

void TagCharacter(GameObject_s *, GameObject_s *, i32) {
}

void Tag_UpdateHint(HINT_s *) {
}

void Tag_NewTransfer(GameObject_s *, GameObject_s *) {
}

void Tag_DrawIcon_LSW(GameObject_s *) {
}

void Tag_ResetTransfers() {
    Tag_TransferTimer0 = Tag_TransferResetTimer;
    Tag_TransferTimer1 = Tag_TransferResetTimer;
}

void Tag_DrawIcon_Batman(GameObject_s *) {
}

void Tag_UpdateTransfers(i32, i32, i32) {
}

void TagCode(GameObject_s *, GameObject_s *, i32, i32, i32) {
}

void Tag_Check(GameObject_s *) {
}
