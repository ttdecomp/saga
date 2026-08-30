#pragma once

#include "decomp.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/numath/numtx.h"

struct GameObject_s;
struct MechObjectInterface;
struct GAMEOBJECTADDONS_s;
struct characterdata_s;
struct CHARACTERMODEL_s {
    i16 model_id;                         // 0x00
    u8 flags;                             // 0x02
    u8 field_0x3;                         // 0x03
    nuhgobj_s *hierarchy;                 // 0x04
    void **model_data_a;                  // 0x08
    void **model_data_b;                  // 0x0c
    void **model_data_c;                  // 0x10
    nuhgobjpoi_s *points_of_interest[16]; // 0x14
};

DECOMP_ASSERT(sizeof(CHARACTERMODEL_s) == 0x54, "CHARACTERMODEL_s size");

typedef struct COINPACKET_s {
    u32 coins;     // 0x00
    u32 field_0x4; // 0x04
    u16 lastcoin;  // 0x08
    u16 field_0xa; // 0x0a
    u32 field_0xc; // 0x0c
} COINPACKET;
typedef COINPACKET COINPACKET_s;

typedef struct torpedopacket_s {
    u8 count;     // 0x00
    u8 field_0x1; // 0x01  (bit0 = in use)
    u8 pad[0x86]; // 0x02..0x88
} TORPEDOPACKET;

// Player/enemy bookkeeping block, base 0x2c0 within a GameObject.
typedef struct PAI_s {
    u8 pad0[0x364 - 0x2c0];  // 0x2c0..0x364
    void *field_0x364;       // 0x364
    u8 pad0b[0x390 - 0x368]; // 0x368..0x390
    GameObject_s *owner;     // 0x390
    void *nearest_opponent;  // 0x394
    u8 pad_d8[0x4];          // 0x398..0x39c
    u32 field_0xdc;          // 0x39c
    u32 field_0xe0;          // 0x3a0
    void *opponent;          // 0x3a4
    u8 pad_a8[0x4];          // 0x3a8..0x3ac
    u32 field_0xec;          // 0x3ac
    u32 field_0xf0;          // 0x3b0
    u8 pad1[0x3e0 - 0x3b4];
    f32 mover_height; // 0x3e0 overall
    u8 pad1b[0x3f4 - 0x3e4];
    u8 field_0x134; // 0x3f4 overall
    u8 pad1c[0x448 - 0x3f5];
    f32 antinode_timer;     // 0x448
    u8 pad2[0x4a5 - 0x44c]; // 0x44c..0x4a5
    u8 field_0x1e5;         // 0x4a5
    u8 pad3[0x4b0 - 0x4a6]; // 0x4a6..0x4b0
} PAI;

typedef struct APIOBJECT_s {
    GameObject_s *objptr;              // 0x00
    PAI *ai;                           // 0x04
    ANIMPACKET_s anim_packet;          // 0x08
    CHARACTERMODEL_s *character_model; // 0x50
    characterdata_s *character_data;   // 0x54
    undefined field_0x58[0x4];         // 0x58 .. 0x5c
    NUVEC position;                    // 0x5c
    f32 field_0x68;                    // 0x68
    f32 field_0x6c;                    // 0x6c
    f32 field_0x70;                    // 0x70
    undefined field_0x74[0xc];         // 0x74 .. 0x80
    f32 pos_x;                         // 0x80
    f32 pos_y;                         // 0x84
    f32 pos_z;                         // 0x88
    f32 start_position[3];             // 0x8c .. 0x98
    f32 initial_position[3];           // 0x98 .. 0xa4
    undefined field_0xa4[0xa8 - 0xa4];
    f32 field_0xa8;                       // 0xa8
    undefined field_0xac[0xb8 - 0xac];    // 0xac .. 0xb8
    NUMTX field_0xb8;                     // 0xb8
    NUMTX field_0xf8;                     // 0xf8
    NUMTX field_0x138;                    // 0x138
    undefined field_0x178[0x190 - 0x178]; // 0x178 .. 0x190
    f32 field_0x190;                      // 0x190
    f32 field_0x194;                      // 0x194
    f32 field_0x198;                      // 0x198
    undefined field_0x19c[0x1b4 - 0x19c];
    f32 previous_position[3]; // 0x1b4 .. 0x1c0
    undefined field_0x1c0[0x1dc - 0x1c0];
    f32 field_0x1dc; // 0x1dc
    f32 field_0x1e0; // 0x1e0
    u32 field_0x1e4; // 0x1e4  mission kill-mask lo
    u32 field_0x1e8; // 0x1e8  mission kill-mask hi
    u32 field_0x1ec; // 0x1ec  player mission-complete mask lo
    u32 field_0x1f0; // 0x1f0  player mission-complete mask hi
    u32 field_0x1f4; // 0x1f4
    u16 field_0x1f8; // 0x1f8  flags (bit0 in-use, low-byte bit7 dying, 0x1000/0x2000)
    u8 field_0x1fa;  // 0x1fa
    u8 field_0x1fb;
    f32 field_0x1fc; // 0x1fc
    f32 field_0x200; // 0x200
    f32 field_0x204; // 0x204
    undefined field_0x208[0x218 - 0x208];
    f32 field_0x218;                      // 0x218
    f32 field_0x21c;                      // 0x21c
    f32 field_0x220;                      // 0x220
    undefined field_0x224[0x22c - 0x224]; // 0x224 .. 0x22c
    f32 viewdistance;                     // 0x22c
    f32 heardistance;                     // 0x230
    f32 maxviewheight;                    // 0x234
    f32 minviewheight;                    // 0x238
    undefined field_0x23c[0x3a];          // 0x23c .. 0x276
    u16 field_0x276;                      // 0x276
    undefined field_0x278[0x27c - 0x278];
    char field_0x27c;         // 0x27c  player/character slot (0xff = none)
    undefined field_0x27d[2]; // 0x27d .. 0x27f
    u8 field_0x27f;           // 0x27f
    u8 field_0x280;           // 0x280
    u8 field_0x281;           // 0x281
    undefined field_0x282[2]; // 0x282 .. 0x284
    u8 model_draw_result;     // 0x284
    undefined field_0x285[2]; // 0x285 .. 0x287
    u8 field_0x287;           // 0x287  owner/controller player index
    u8 field_0x288;           // 0x288
    u8 field_0x289;           // 0x289
    undefined field_0x28a[0x16];
    u32 field387_0x2a0; // 0x2a0
    u32 field388_0x2a4; // 0x2a4
} APIOBJECT;

struct APIOBJECTSYS_s {
    u32 object_size;
    APIOBJECT *objects;
    u8 state[0x214 - 8];
};

DECOMP_ASSERT(sizeof(APIOBJECTSYS_s) == 0x214, "APIOBJECTSYS size");

extern "C" APIOBJECT *APIObjectCreate(APIOBJECTSYS_s *system);
extern "C" void APIObjectDestroy(APIOBJECTSYS_s *system, APIOBJECT *object);
extern "C" void APIObjectDestroyAll(APIOBJECTSYS_s *system);
extern "C" void APIObjectSetUsed(APIOBJECT *object, u8 index, i32 used);

typedef struct GameObject_s {
    APIOBJECT apiobj; // 0x0000 .. 0x02a8
    u32 field_0x2a8;  // 0x02a8
    u32 field_0x2ac;  // 0x02ac
    u8 pad_2b0[0x10]; // 0x02b0 .. 0x02c0
    PAI ai;           // 0x02c0 .. 0x04b0
    u8 pad_4b0[0x4c8 - 0x4b0];
    u8 light_data[0x144]; // 0x04c8, embedded rtldata_s
    u8 pad_60c[0x661 - 0x60c];
    u8 field_0x661;                             // 0x0661
    u8 pad_662[0x686 - 0x662];                  // 0x0662 .. 0x0686
    u16 yrot;                                   // 0x0686
    u8 pad_688[0x68c - 0x688];                  // 0x0688 .. 0x068c
    f32 field_0x68c;                            // 0x068c
    u8 pad_690[0x6b4 - 0x690];                  // 0x0690 .. 0x06b4
    u8 player_packet[0x7a5 - 0x6b4];            // 0x06b4, PLAYERPACKET_s begins here
    u8 field_0x7a5;                             // 0x07a5
    u8 pad_7a6[0x7ec - 0x7a6];                  // 0x07a6 .. 0x07ec
    COINPACKET *coinpacket;                     // 0x07ec
    void *gizforce_los_info;                    // 0x07f0
    NUMTX field_0x7f4;                          // 0x07f4
    u8 pad_834[0xc34 - 0x834];                  // 0x0834 .. 0x0c34
    u32 field_0xc34;                            // 0x0c34
    u8 pad_c38[0xc94 - 0xc38];                  // 0x0c38 .. 0x0c94
    struct GAMEPAD_s *pad_gamepad;              // 0x0c94  (originally inside PLAYERPACKET_s)
    SOCKPOSITION *oldpos;                       // 0x0c98
    u8 pad_c9c[0xcac - 0xc9c];                  // 0x0c9c .. 0x0cac
    void *suit;                                 // 0x0cac
    void *batarang;                             // 0x0cb0
    TORPEDOPACKET *torpedo;                     // 0x0cb4
    u8 pad_cb8[0xcc0 - 0xcb8];                  // 0x0cb8 .. 0x0cc0
    GameObject_s *field_0xcc0;                  // 0x0cc0
    u8 pad_cc4[0xd80 - 0xcc4];                  // 0x0cc4 .. 0x0d80
    f32 field_0xd80;                            // 0x0d80
    u8 pad_d84[0xd8c - 0xd84];                  // 0x0d84 .. 0x0d8c
    f32 field_0xd8c;                            // 0x0d8c
    u8 pad_d90[0xdc8 - 0xd90];                  // 0x0d90 .. 0x0dc8
    f32 field_0xdc8;                            // 0x0dc8
    u8 pad_dcc[0xdec - 0xdcc];                  // 0x0dcc .. 0x0dec
    f32 field_0xdec;                            // 0x0dec
    u8 pad_df0[0xe1e - 0xdf0];                  // 0x0df0 .. 0x0e1e
    i16 field_0xe1e;                            // 0x0e1e
    u8 field_0xe20;                             // 0x0e20
    u8 field_0xe21;                             // 0x0e21
    u8 field_0xe22;                             // 0x0e22
    u8 field_0xe23;                             // 0x0e23
    u8 field_0xe24;                             // 0x0e24
    u8 pad_e25[0xe31 - 0xe25];                  // 0x0e25 .. 0x0e31
    u8 field_0xe31;                             // 0x0e31
    u8 field_0xe32;                             // 0x0e32
    u8 pad_e33[0xe37 - 0xe33];                  // 0x0e33 .. 0x0e37
    u8 field_0xe37;                             // 0x0e37
    u8 field_0xe38;                             // 0x0e38
    u8 pad_e39[0xe50 - 0xe39];                  // 0x0e39 .. 0x0e50
    MechObjectInterface *mech_object_interface; // 0x0e50
    GAMEOBJECTADDONS_s *addons;                 // 0x0e54
    u8 pad_e58[0xebc - 0xe58];                  // 0x0e58 .. 0x0ebc
    u32 field_0xebc;                            // 0x0ebc
    u32 field_0xec0;                            // 0x0ec0
    u32 field_0xec4;                            // 0x0ec4
    u32 field_0xec8;                            // 0x0ec8
    u32 field_0xecc;                            // 0x0ecc
    u32 field_0xed0;                            // 0x0ed0
    u32 field_0xed4;                            // 0x0ed4
    u32 field_0xed8;                            // 0x0ed8
    u8 pad_edc[0xee0 - 0xedc];                  // 0x0edc .. 0x0ee0
    f32 field_0xee0;                            // 0x0ee0
    u8 pad_ee4[0xef8 - 0xee4];                  // 0x0ee4 .. 0x0ef8
    u8 field_0xef8;                             // 0x0ef8
    u8 field_0xef9;                             // 0x0ef9
    u8 pad_efa[0xefc - 0xefa];                  // 0x0efa .. 0x0efc
    u8 field_0xefc;                             // 0x0efc
    u8 field_0xefd;                             // 0x0efd
    u8 field_0xefe;                             // 0x0efe
    u8 field_0xeff;                             // 0x0eff
    u8 field_0xf00;                             // 0x0f00
    u8 field_0xf01;                             // 0x0f01
    u8 pad_f02[0xf04 - 0xf02];
    u8 field_0xf04; // 0x0f04
    u8 pad_f05[0xffc - 0xf05];
    f32 field_0xffc;  // 0x0ffc
    f32 field_0x1000; // 0x1000
    f32 field_0x1004; // 0x1004
    f32 field_0x1008; // 0x1008
    u8 pad_100c[0x1018 - 0x100c];
    f32 field_0x1018; // 0x1018
    u8 pad_101c[0x1020 - 0x101c];
    f32 field_0x1020;             // 0x1020
    u8 pad_1024[0x1050 - 0x1024]; // 0x1024 .. 0x1050
    u32 field_0x1050;             // 0x1050
    u32 field_0x1054;             // 0x1054
    u8 pad_1058[0x106e - 0x1058]; // 0x1058 .. 0x106e
    u16 field_0x106e;             // 0x106e
    i16 id;                       // 0x1070
    u8 pad_1072[0x107c - 0x1072];
    i16 field_0x107c; // 0x107c
    u8 pad_107e[0x1086 - 0x107e];
    u8 field_0x1086;              // 0x1086
    u8 field_0x1087;              // 0x1087
    u8 field_0x1088;              // 0x1088
    u8 field_0x1089;              // 0x1089
    u8 hitpoints;                 // 0x108a
    u8 current_hp;                // 0x108b
    u8 pad_108c[0x108e - 0x108c]; // 0x108c .. 0x108e
    u8 field_0x108e;              // 0x108e
    u8 pad_108f[0x10b0 - 0x108f]; // 0x108f .. 0x10b0
    void *opponent;               // 0x10b0
    void *last_attacker;          // 0x10b4
    void *field_0x10b8;           // 0x10b8
    u8 pad_10bc[0x10c8 - 0x10bc];
    f32 field_0x10c8; // 0x10c8
    f32 field_0x10cc; // 0x10cc
    f32 field_0x10d0; // 0x10d0
    u8 pad_10d4[0x10e4 - 0x10d4];
    void ClearAddons();
    void ClearMechObjectInterface();
    void GetAddons(bool);
    void GetMechObjectInterface();
    void IsRunningTaskType(struct HashedKey const &);
    void KillTasks();
} GameObject;

DECOMP_ASSERT(sizeof(GameObject_s) == 0x10e4, "GameObject size");
DECOMP_ASSERT(offsetof(APIOBJECT, anim_packet) == 0x08, "APIOBJECT animation packet offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0xb8) == 0xb8, "APIOBJECT primary matrix offset");
DECOMP_ASSERT(offsetof(APIOBJECT, model_draw_result) == 0x284, "APIOBJECT model draw result offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x7f4) == 0x7f4, "GameObject auxiliary matrix offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xcc0) == 0xcc0, "GameObject linked object offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xe1e) == 0xe1e, "GameObject force glow index offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1088) == 0x1088, "GameObject matrix selector offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x10b8) == 0x10b8, "GameObject snake data offset");

typedef struct GameObject_s GameObject_s;
