#pragma once

#include <stddef.h>

#include "decomp.h"
#include "nu2api/numath/nuvec.h"

struct storepack_s {
    char *name;
    u8 field1_0x4;
    u8 field2_0x5;
    u8 field3_0x6;
    u8 field4_0x7;
    u8 field5_0x8;
    u8 field6_0x9;
    u8 field7_0xa;
    u8 field8_0xb;
    u8 field9_0xc;
    u8 field10_0xd;
    u8 field11_0xe;
    u8 field12_0xf;
    u8 field13_0x10;
    u8 field14_0x11;
    u8 field15_0x12;
    u8 field16_0x13;
    char *custodian_locator_set; // 0x14
    u8 field21_0x18;
    u8 field22_0x19;
    u8 field23_0x1a;
    u8 field24_0x1b;
    u8 field25_0x1c;
    u8 field26_0x1d;
    u8 field27_0x1e;
    u8 field28_0x1f;
    i16 *id;                  // 0x20
    NUVEC custodian_position; // 0x24
    u16 custodian_angle;      // 0x30
    u8 field44_0x32;          // 0x32, camera socket or 0xff
    u8 field45_0x33;
};
DECOMP_ASSERT(sizeof(storepack_s) == 0x34, "STOREPACK size");
typedef struct storepack_s STOREPACK;
extern STOREPACK StorePack[11];

enum STORE_PACK_INDEX {
    STORE_PACK_EPISODE_I = 0,
    STORE_PACK_EPISODE_II = 1,
    STORE_PACK_EPISODE_III = 2,
    STORE_PACK_EPISODE_IV = 3,
    STORE_PACK_EPISODE_V = 4,
    STORE_PACK_BONUS_AREA = 5,
    STORE_PACK_OPEN_ALL_AREAS = 8,
};

typedef struct COLLECTID {
    i16 id;
    u8 type;
    u8 field2_0x3;
    i32 field3_0x4;
    u8 can_buy;
    u8 field5_0x9;
    u16 field6_0xa;
    char cheat_code[16];
} COLLECTID;

i32 Store_FindPack(i32 id, char *name);
bool Store_IsPackUnlocked(i32 pack);
