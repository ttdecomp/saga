#ifndef LEGOAPI_AI_GAME_LSW_HUB_AI_H
#define LEGOAPI_AI_GAME_LSW_HUB_AI_H

#include "decomp.h"

// Pointer/isize pun shared by Condition_InHubArea (lsw_hub_ai.cpp) and
// Condition_InHubAreaInit (lsw_hub_ai_stubs.cpp). Defined once here so the
// duplicate-definition lint stays green.
union AI_HUB_AREA_ARGUMENT {
    void *pointer;
    isize value;
};

#endif
