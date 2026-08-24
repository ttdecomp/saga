#ifndef LEGOAPI_CUTSCENES_CUTSCENES_H
#define LEGOAPI_CUTSCENES_CUTSCENES_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"

// Cutscene API (module legoapi/cutscenes, cutscenes.cpp).

CUTINFO *CutScene_Find(CUTSYS *cutsys, char *name);
void CutScene_SnapToEnd(CUTINFO *cut);
void CutScene_StoppedFn_LSW(CUTINFO *cut);
bool CutScene_PlayingOrRequested(CUTINFO *cut);

#endif
