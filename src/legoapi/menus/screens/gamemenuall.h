#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/common.h"

struct MENUPACKET_s {
    i16 player_model[2];
    u8 active_player[2];
    u8 reserved_0[2];
    u8 reserved_1[2];
};
DECOMP_ASSERT(sizeof(MENUPACKET_s) == 0x0a, "MENUPACKET_s ABI");

extern MENUPACKET_s MenuPacket;

void MakeMenuPacket();
