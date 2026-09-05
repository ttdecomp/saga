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

#ifdef __cplusplus
extern "C" {
#endif
    i32 MenuGetSlotNum(void);
    i32 MenuCurrentID(void);
    i32 MenuInMemoryCardLoad(void);
    i32 MenuInMemoryCardWarning(void);
    void SetButtonScaleMode(i32 mode);
#ifdef __cplusplus
}
#endif
