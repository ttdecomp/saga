#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void initHintSys() {
}

i32 HINT_COMPLETE(i32 hint_id) {
    if (hint_id < 0) {
        return 0;
    }

    i32 word = hint_id >> 5;
    if (word > 2) {
        return 0;
    }
    if (MechInputTouchSystem::s_baseControlMode != 0) {
        word += 3;
    }
    return Game.hint_completion_bits[word] & (1U << (hint_id & 0x1f));
}

void Tilt_UpdateHint(HINT_s *) {
}

void CurrentHintAlpha() {
}

void Dodge_UpdateHint(HINT_s *) {
}

void SET_HINT_COMPLETE(i32 hint_id) {
    if (hint_id < 0) {
        return;
    }

    i32 word = hint_id >> 5;
    if (word > 2) {
        return;
    }
    if (MechInputTouchSystem::s_baseControlMode != 0) {
        word += 3;
    }
    Game.hint_completion_bits[word] |= 1U << (hint_id & 0x1f);
}

void CLEAR_HINT_COMPLETE(i32 hint_id) {
    if (hint_id < 0) {
        return;
    }

    i32 word = hint_id >> 5;
    if (word > 2) {
        return;
    }
    if (MechInputTouchSystem::s_baseControlMode != 0) {
        word += 3;
    }
    Game.hint_completion_bits[word] &= ~(1U << (hint_id & 0x1f));
}

void DragBomb_UpdateHint(HINT_s *) {
}

void GetHintFromUIButton() {
}

void initGameHintSys_LSW() {
}

void SmartBomb_UpdateHint(HINT_s *) {
}

void ShinyMetal_UpdateHint(HINT_s *) {
}

void CurrentHintButtonScale() {
}

void initGameHintSys_Batman() {
}

void IndyUnlocked_UpdateHint(HINT_s *) {
}

static __used__ void Sith_UpdateHint(HINT_s *) {
}

static __used__ void UnlockHubStuff_UpdateHint(HINT_s *) {
}
