#ifndef LEGOAPI_CUTSCENES_CUTSCENES_H
#define LEGOAPI_CUTSCENES_CUTSCENES_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"

// Cutscene API (module legoapi/cutscenes, cutscenes.cpp).

CUTINFO *CutScene_Find(CUTSYS *cutsys, char *name);
i32 CutScene_HasPlayed(CUTINFO *cut);
void CutScene_SnapToEnd(CUTINFO *cut);
void CutScene_StoppedFn_LSW(CUTINFO *cut);
i32 CutScene_PlayingOrRequested(CUTINFO *cut);
i32 CutScene_IsSkippable(CUTINFO *cut);

#endif
