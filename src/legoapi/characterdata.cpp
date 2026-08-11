#include "nu2api/nucore/common.h"

static u8 CharacterInfoStorage[0x1000];

u8 *CInfo = CharacterInfoStorage;

i16 LEGOACT_SHOOTRIGHT = -1;
i16 LEGOACT_SHOOTLEFT = -1;
i16 LEGOACT_SHOOTBACK = -1;
i16 LEGOACT_DEACTIVATED = -1;
i32 LEGOCONTEXT_HOLD = -1;
i32 LEGOCONTEXT_BLOCK = -1;
f32 DEACTIVATEDTIME;
i32 i_temp_xrot;
u16 temp_xrot;
i16 temp_yrot;
