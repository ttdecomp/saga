#ifndef LEGOAPI_MENUS_CORE_GAMEHINT_H
#define LEGOAPI_MENUS_CORE_GAMEHINT_H

#include "decomp.h"

// Hint system (module legoapi/menus/core, gamehint.cpp).

HINT_s *Hint_FindHint(i32 hint_id);
i32 Hint_CurrentId();
i32 Hint_isComplete(HINT_s *hint);
i32 Hint_isComplete(i32 hint_id);
void Hint_SaveBits(i32 hint_id, i32 complete);
void Hint_SetComplete(HINT_s *hint);
void Hint_SetComplete(i32 hint_id);
i32 Hint_isAvailable(i32 hint_id);

#endif
