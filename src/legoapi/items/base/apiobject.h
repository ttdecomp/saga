#pragma once

#include "decomp.h"
#include "nu2api/nucore/common.h"

// Spawn point data (OldPlrSPos).
typedef struct sockposition_s {
    f32 x;
    f32 y;
    f32 z;
    f32 heading;
} SOCKPOSITION;

typedef struct coinpacket_s {
    u32 coins;     // 0x00
    u32 field_0x4; // 0x04
    u16 lastcoin;  // 0x08
    u16 field_0xa; // 0x0a
    u32 field_0xc; // 0x0c
} COINPACKET;

typedef struct torpedopacket_s {
    u8 count;     // 0x00
    u8 field_0x1; // 0x01  (bit0 = in use)
    u8 pad[0x86]; // 0x02..0x88
} TORPEDOPACKET;

// Player/enemy bookkeeping block, base 0x2c0 within a GameObject.
typedef struct PAI_s {
    u8 pad0[0xd4];          // 0x2c0..0x394
    void *nearest_opponent; // 0x394
    u8 pad_d8[0x4];         // 0x398..0x39c
    u32 field_0xdc;         // 0x39c
    u32 field_0xe0;         // 0x3a0
    void *opponent;         // 0x3a4
    u8 pad_a8[0x4];         // 0x3a8..0x3ac
    u32 field_0xec;         // 0x3ac
    u32 field_0xf0;         // 0x3b0
    u8 pad1[0x448 - 0x3b4]; // 0x3b4..0x448
    f32 antinode_timer;     // 0x448
    u8 pad2[0x4a5 - 0x44c]; // 0x44c..0x4a5
    u8 field_0x1e5;         // 0x4a5
    u8 pad3[0x4b0 - 0x4a6]; // 0x4a6..0x4b0
} PAI;

typedef struct APIOBJECT_s {
    void *objptr; // 0x00
    undefined field_0x4[0x1dc];  // 0x04 .. 0x1e0
    f32 field_0x1e0;             // 0x1e0
    u32 field_0x1e4;             // 0x1e4  mission kill-mask lo
    u32 field_0x1e8;             // 0x1e8  mission kill-mask hi
    u32 field_0x1ec;             // 0x1ec  player mission-complete mask lo
    u32 field_0x1f0;             // 0x1f0  player mission-complete mask hi
    u32 field_0x1f4;             // 0x1f4
    u16 field_0x1f8;             // 0x1f8  flags (bit0 in-use, low-byte bit7 dying, 0x1000/0x2000)
    u8 field_0x1fa;              // 0x1fa
    undefined field_0x1fb[0x31]; // 0x1fb .. 0x22c
    f32 viewdistance;  // 0x22c
    f32 heardistance;  // 0x230
    f32 maxviewheight; // 0x234
    f32 minviewheight; // 0x238
    undefined field_0x23c[0x40];
    char field_0x27c;            // 0x27c  player/character slot (0xff = none)
    undefined field_0x27d[0xa];  // 0x27d .. 0x287
    u8 field_0x287;              // 0x287  owner/controller player index
    u8 field_0x288;              // 0x288
    u8 field_0x289;              // 0x289
    undefined field_0x28a[0x16];
    u32 field387_0x2a0; // 0x2a0
    u32 field388_0x2a4; // 0x2a4
} APIOBJECT;

typedef struct GameObject_s {
    APIOBJECT apiobj;             // 0x0000 .. 0x02a8
    u8 pad_2a8[0x18];             // 0x02a8 .. 0x02c0
    PAI ai;                       // 0x02c0 .. 0x04b0
    u8 pad_4b0[0x7ec - 0x4b0];    // 0x04b0 .. 0x07ec
    COINPACKET *coinpacket;       // 0x07ec
    void *gizforce_los_info;      // 0x07f0
    u8 pad_7f4[0xc98 - 0x7f4];    // 0x07f4 .. 0x0c98
    SOCKPOSITION *oldpos;         // 0x0c98
    u8 pad_c9c[0xcac - 0xc9c];    // 0x0c9c .. 0x0cac
    void *suit;                   // 0x0cac
    void *batarang;               // 0x0cb0
    TORPEDOPACKET *torpedo;       // 0x0cb4
    u8 pad_cb8[0xdec - 0xcb8];    // 0x0cb8 .. 0x0dec
    f32 field_0xdec;              // 0x0dec
    u8 pad_df0[0xe22 - 0xdf0];    // 0x0df0 .. 0x0e22
    u8 field_0xe22;               // 0x0e22
    u8 pad_e23[0xebc - 0xe23];    // 0x0e23 .. 0x0ebc
    u32 field_0xebc;              // 0x0ebc
    u32 field_0xec0;              // 0x0ec0
    u32 field_0xec4;              // 0x0ec4
    u32 field_0xec8;              // 0x0ec8
    u32 field_0xecc;              // 0x0ecc
    u32 field_0xed0;              // 0x0ed0
    u32 field_0xed4;              // 0x0ed4
    u32 field_0xed8;              // 0x0ed8
    u8 pad_edc[0xef8 - 0xedc];    // 0x0edc .. 0x0ef8
    u8 field_0xef8;               // 0x0ef8
    u8 field_0xef9;               // 0x0ef9
    u8 pad_efa[0xf00 - 0xefa];    // 0x0efa .. 0x0f00
    u8 field_0xf00;               // 0x0f00
    u8 pad_f01[0x1050 - 0xf01];   // 0x0f01 .. 0x1050
    u32 field_0x1050;             // 0x1050
    u8 pad_1054[0x106e - 0x1054]; // 0x1054 .. 0x106e
    u16 field_0x106e;             // 0x106e
    i16 id;                       // 0x1070
    u8 pad_1072[0x108a - 0x1072]; // 0x1072 .. 0x108a
    u8 hitpoints;                 // 0x108a
    u8 current_hp;                // 0x108b
    u8 pad_108c[0x108e - 0x108c]; // 0x108c .. 0x108e
    u8 field_0x108e;              // 0x108e
    u8 pad_108f[0x10b0 - 0x108f]; // 0x108f .. 0x10b0
    void *opponent;               // 0x10b0
    void *last_attacker;          // 0x10b4
    u8 pad_10b8[0x10e4 - 0x10b8]; // 0x10b8 .. 0x10e4 (tail)
    void ClearAddons();
    void ClearMechObjectInterface();
    void GetAddons(bool);
    void GetMechObjectInterface();
    void IsRunningTaskType(struct HashedKey const &);
    void KillTasks();
} GameObject;

typedef struct GameObject_s GameObject_s;