#pragma once

#include <stddef.h>

#include "decomp.h"

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
    u8 field17_0x14;
    u8 field18_0x15;
    u8 field19_0x16;
    u8 field20_0x17;
    u8 field21_0x18;
    u8 field22_0x19;
    u8 field23_0x1a;
    u8 field24_0x1b;
    u8 field25_0x1c;
    u8 field26_0x1d;
    u8 field27_0x1e;
    u8 field28_0x1f;
    i16 *id;
    u8 field30_0x24;
    u8 field31_0x25;
    u8 field32_0x26;
    u8 field33_0x27;
    u8 field34_0x28;
    u8 field35_0x29;
    u8 field36_0x2a;
    u8 field37_0x2b;
    u8 field38_0x2c;
    u8 field39_0x2d;
    u8 field40_0x2e;
    u8 field41_0x2f;
    u8 field42_0x30;
    u8 field43_0x31;
    u8 field44_0x32;
    u8 field45_0x33;
};
typedef struct storepack_s STOREPACK;

typedef struct COLLECTID {
    i16 id;
    u8 type;
    u8 field2_0x3;
    int field3_0x4;
    u8 can_buy;
    u8 field5_0x9;
    u16 field6_0xa;
    char cheat_code[16];
} COLLECTID;

i32 Store_FindPack(i32 id, char *name);
