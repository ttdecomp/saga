#include "legoapi/legoapi_types.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "globals.h"

u8 LSW_HintConditions[4];
HINTSYS_s hintsys = {};
HINTUIBUTTON_s *hintUIButton = NULL;
f32 AlphaBlendTime = 1.0f;

void CLEAR_HINT_COMPLETE(i32 hint_id);
i32 HINT_COMPLETE(i32 hint_id);
void SET_HINT_COMPLETE(i32 hint_id);
void Hint_SaveGameState(HINT_s *hint);
void initHintSys();

void Hint_Reset() {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL) {
        return;
    }

    while (hint->control_mode_ids[0] != -1) {
        hint->field_0x20 = 0;
        hint++;
    }

    hintsys.state = 2;
    hintsys.current_hint = 0;

    HINTUIBUTTON_s *button = hintUIButton;
    if (button->field_0x78 != NULL) {
        f32 alpha_blend_time = AlphaBlendTime;
        button->field_0x7c = 0;
        f32 field_0x84 = *button->field_0x84;
        f32 *field_0x40 = button->field_0x40;
        button->field_0x8c = -1.5f;
        button->field_0x90 = 0.0f;
        button->field_0x98 = 0.0f;
        button->field_0x88 = field_0x84;
        button->field_0x78 = NULL;
        button->field_0x94 = alpha_blend_time;
        f32 field_0x40_value = *field_0x40;
        button->field_0xa0 = 0;
        button->field_0x48 = 0.0f;
        button->field_0x4c = 0.0f;
        button->field_0x54 = 0.0f;
        button->field_0x44 = field_0x40_value;
        button->field_0x50 = alpha_blend_time;
    }

    hintsys.field_0x1c = 0;
}

void Hint_Process(float) {
}

void Hint_SetHint(HINT_s *, i32, i32) {
}

HINT_s *Hint_FindHint(i32 hint_id) {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL) {
        return NULL;
    }

    while (hint->control_mode_ids[0] != -1) {
        if (hint->control_mode_ids[0] == hint_id || hint->control_mode_ids[1] == hint_id) {
            return hint;
        }
        ++hint;
    }
    return NULL;
}

void Hint_SaveBits(i32 hint_id, i32 complete) {
    if (complete != 0) {
        SET_HINT_COMPLETE(hint_id);
        return;
    }
    CLEAR_HINT_COMPLETE(hint_id);
}

i32 Hint_CurrentId() {
    HINT_s *hint = hintsys.active_hint;
    if (hint == NULL) {
        return -1;
    }
    return hint->control_mode_ids[MechInputTouchSystem::s_baseControlMode];
}

void Hint_ResetHint(i32 hint_id, i32 reset_completed) {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL || hint->control_mode_ids[0] == -1) {
        return;
    }

    i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    while (hint->control_mode_ids[control_mode] != hint_id) {
        hint++;
        if (hint->control_mode_ids[0] == -1) {
            return;
        }
    }

    if (reset_completed == 1) {
        hint->completion_flags[control_mode] = 0;
    }
    hintsys.current_hint = 0;
}

i32 Hint_isComplete(HINT_s *hint) {
    return hint->completion_flags[MechInputTouchSystem::s_baseControlMode];
}

i32 Hint_isComplete(i32 hint_id) {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL) {
        return 0;
    }

    hint = Hint_FindHint(hint_id);
    if (hint == NULL) {
        return 0;
    }
    return hint->completion_flags[MechInputTouchSystem::s_baseControlMode];
}

void Hint_SetComplete(HINT_s *hint) {
    if (hint == NULL) {
        return;
    }

    const i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    if (hint->completion_flags[control_mode] == 0) {
        hint->completion_flags[control_mode] = 1;
        Hint_SaveGameState(hint);
    }
}

void Hint_SetComplete(i32 hint_id) {
    if (hint_id == -1) {
        return;
    }

    HINT_s *hint = Hint_FindHint(hint_id);
    if (hint == NULL || (hint->flags & 0x40) == 0) {
        return;
    }

    const i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    if (hint->completion_flags[control_mode] == 0) {
        hint->completion_flags[control_mode] = 1;
        Hint_SaveGameState(hint);
    }
}

i32 Hint_isAvailable(i32 hint_id) {
    HINT_s *active_hint = hintsys.active_hint;
    if (active_hint != NULL && active_hint->control_mode_ids[MechInputTouchSystem::s_baseControlMode] == hint_id) {
        return 0;
    }
    if (hint_id < 0 || TTab[hint_id] == NULL) {
        return 0;
    }

    HINT_s *hint = Hint_FindHint(hint_id);
    if (hint == NULL || hint->completion_flags[MechInputTouchSystem::s_baseControlMode] != 0) {
        return 0;
    }
    return !(hint->field_0x20 > 0.0f);
}

void Hint_CancelCurrent() {
}

void Hint_ExpandButtons(char *, char *) {
}

void Hint_SaveGameState(HINT_s *hint) {
    HINT_s *candidate = hintsys.hints;
    if (candidate == NULL) {
        return;
    }
    if (hintsys.save_bits == NULL) {
        return;
    }
    if (candidate->control_mode_ids[0] == -1) {
        return;
    }

    const u16 hint_id = static_cast<u16>(hint->control_mode_ids[0]);
    i32 index = 0;
    u16 candidate_id = static_cast<u16>(candidate->control_mode_ids[0]);
    if (candidate_id == hint_id) {
        goto found;
    }
    do {
        ++candidate;
        ++index;
        candidate_id = static_cast<u16>(candidate->control_mode_ids[0]);
        if (candidate_id == 0xffff) {
            return;
        }
    } while (candidate_id != hint_id);

found:
    Hint_SaveBits(index, 1);
}

void Hint_SetHintFromId(i32, i32, i32) {
}

void Hint_LoadAllGameState() {
    HINTSYS_s *system = &hintsys;
    HINT_s *hint = system->hints;
    if (hint == NULL || system->save_bits == NULL || hint->control_mode_ids[0] == -1 || system->save_bit_count <= 0) {
        return;
    }

    i32 index = 0;
    const i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    do {
        if (HINT_COMPLETE(index) != 0) {
            hint->completion_flags[control_mode] = 1;
        } else {
            hint->completion_flags[control_mode] = 0;
        }
        ++hint;
        if (hint->control_mode_ids[0] == -1) {
            return;
        }
        ++index;
    } while ((index >> 5) < system->save_bit_count);
}

void Hint_SaveAllGameState() {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL) {
        return;
    }
    u32 *save_bits = hintsys.save_bits;
    if (save_bits == NULL || hint->control_mode_ids[0] == -1) {
        return;
    }

    i32 index = 0;
    const i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    do {
        if (hint->completion_flags[control_mode] != 0) {
            Hint_SaveBits(index, 1);
        } else {
            Hint_SaveBits(index, 0);
        }
        ++hint;
        ++index;
    } while (hint->control_mode_ids[0] != -1);
}

void Hint_ClearHintsAndDoneFlags() {
    HINT_s *hint = hintsys.hints;
    if (hint == NULL || hint->control_mode_ids[0] == -1) {
        return;
    }

    i32 control_mode = MechInputTouchSystem::s_baseControlMode;
    do {
        hint->completion_flags[control_mode] = 0;
        hint->flags &= ~0x42;
        hint++;
    } while (hint->control_mode_ids[0] != -1);
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

void RegisterWithHintSys(void (*update_fn)(HINT_s *, i32), HINT_s *hints, u32 *save_bits, i32 save_bit_count) {
    hintsys.update_fn = update_fn;
    hintsys.hints = hints;
    hintsys.save_bits = save_bits;
    hintsys.save_bit_count = save_bit_count;
    initHintSys();
}
