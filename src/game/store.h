#pragma once

#include <stddef.h>
#include <stdint.h>

struct storepack_s {
    char *name;
    uint8_t field1_0x4;
    uint8_t field2_0x5;
    uint8_t field3_0x6;
    uint8_t field4_0x7;
    uint8_t field5_0x8;
    uint8_t field6_0x9;
    uint8_t field7_0xa;
    uint8_t field8_0xb;
    uint8_t field9_0xc;
    uint8_t field10_0xd;
    uint8_t field11_0xe;
    uint8_t field12_0xf;
    uint8_t field13_0x10;
    uint8_t field14_0x11;
    uint8_t field15_0x12;
    uint8_t field16_0x13;
    uint8_t field17_0x14;
    uint8_t field18_0x15;
    uint8_t field19_0x16;
    uint8_t field20_0x17;
    uint8_t field21_0x18;
    uint8_t field22_0x19;
    uint8_t field23_0x1a;
    uint8_t field24_0x1b;
    uint8_t field25_0x1c;
    uint8_t field26_0x1d;
    uint8_t field27_0x1e;
    uint8_t field28_0x1f;
    short *id;
    uint8_t field30_0x24;
    uint8_t field31_0x25;
    uint8_t field32_0x26;
    uint8_t field33_0x27;
    uint8_t field34_0x28;
    uint8_t field35_0x29;
    uint8_t field36_0x2a;
    uint8_t field37_0x2b;
    uint8_t field38_0x2c;
    uint8_t field39_0x2d;
    uint8_t field40_0x2e;
    uint8_t field41_0x2f;
    uint8_t field42_0x30;
    uint8_t field43_0x31;
    uint8_t field44_0x32;
    uint8_t field45_0x33;
};
typedef struct storepack_s STOREPACK;

struct collectid_s {
    short id;
    uint8_t type;
    uint8_t field2_0x3;
    int field3_0x4;
    uint8_t can_buy;
    uint8_t field5_0x9;
    uint16_t field6_0xa;
    char cheat_code[16];
};
typedef struct collectid_s COLLECTID;

int32_t Store_FindPack(int32_t id, char *name);