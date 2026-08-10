#pragma once

#include "nu2api/numath/nuvec.h"

typedef struct GAMEANIMSET_s GAMEANIMSET;

void GameAnimSet_EvaluateState(GAMEANIMSET *anim_set);
i32 GameAnimSet_JumpToStart(GAMEANIMSET *anim_set);
void GameAnimSet_GetCentreAndRadius(GAMEANIMSET *anim_set, NUVEC *centre, f32 *radius, i32 unknown_1, i32 unknown_2,
                                    i32 unknown_3);
