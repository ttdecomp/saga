#pragma once

#include "decomp.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/numtx.h"

struct GameObject_s;
struct GizForceLOSState_s {
    u8 state[0x630];
};
struct MechObjectInterface;
struct GAMEOBJECTADDONS_s;
struct characterdata_s;
struct AIAREA_s;
struct AILOCATOR_s;
struct AIGROUP_s;
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
    u32 coins;              // 0x00
    f32 scale;              // 0x04
    u16 lastcoin;           // 0x08
    u8 active;              // 0x0a
    u8 field_0xb;           // 0x0b
    f32 double_score_timer; // 0x0c
} COINPACKET;
typedef COINPACKET COINPACKET_s;

typedef struct torpedopacket_s {
    u8 count;     // 0x00
    u8 field_0x1; // 0x01  (bit0 = in use)
    u8 pad[0x86]; // 0x02..0x88
} TORPEDOPACKET;

// Player/enemy bookkeeping block, base 0x2c0 within a GameObject.
typedef struct PAI_s {
    u8 pad0[0x360 - 0x2c0]; // 0x2c0..0x360
    AIAREA_s *area;         // 0x360 overall
    union {
        AILOCATOR_s *locator; // 0x364 overall
        void *field_0x364;
    };
    u8 pad0b[0x370 - 0x368];     // 0x368..0x370
    u8 creature_set;             // 0x370 overall
    u8 pad0c[0x390 - 0x371];     // 0x371..0x390
    GameObject_s *owner;         // 0x390
    void *nearest_opponent;      // 0x394
    f32 nearest_opponent_metric; // 0x398
    u32 field_0xdc;              // 0x39c
    u32 field_0xe0;              // 0x3a0
    void *opponent;              // 0x3a4
    u8 pad_a8[0x4];              // 0x3a8..0x3ac
    u32 field_0xec;              // 0x3ac
    u32 field_0xf0;              // 0x3b0
    u8 pad1[0x3e0 - 0x3b4];
    f32 mover_height; // 0x3e0 overall
    i16 field_0x124;  // 0x3e4 overall
    u8 pad1b[0x3f4 - 0x3e6];
    u8 field_0x134; // 0x3f4 overall
    u8 pad1c[3];    // 0x3f5..0x3f8
    u8 field_0x138; // 0x3f8 overall
    u8 field_0x139; // 0x3f9 overall
    u8 pad1c_end[0x400 - 0x3fa];
    AIGROUP_s *group; // 0x400 overall
    u8 group_row;     // 0x404 overall
    u8 group_column;  // 0x405 overall
    u8 group_member;  // 0x406 overall
    u8 pad_group;
    NUVEC terrain_origin; // 0x408 overall
    u8 reset_state[0x18]; // 0x414..0x42c overall
    u8 pad1d[0x448 - 0x42c];
    f32 antinode_timer; // 0x448
    u8 pad2[0x46c - 0x44c];
    AILOCATOR_s *respawn_locator; // 0x46c overall
    u8 pad2b[0x4a5 - 0x470];
    u8 field_0x1e5;         // 0x4a5
    u8 pad3[0x4b0 - 0x4a6]; // 0x4a6..0x4b0
} PAI;

typedef struct APIOBJECT_s {
    GameObject_s *objptr;              // 0x00
    PAI *ai;                           // 0x04
    ANIMPACKET_s anim_packet;          // 0x08
    CHARACTERMODEL_s *character_model; // 0x50
    characterdata_s *character_data;   // 0x54
    u16 facing_angle;                  // 0x58
    u16 movement_facing_angle;         // 0x5a
    NUVEC position;                    // 0x5c
    f32 field_0x68;                    // 0x68
    f32 field_0x6c;                    // 0x6c
    f32 field_0x70;                    // 0x70
    f32 previous_velocity_x;           // 0x74
    f32 previous_velocity_y;           // 0x78
    f32 previous_velocity_z;           // 0x7c
    f32 pos_x;                         // 0x80
    f32 pos_y;                         // 0x84
    f32 pos_z;                         // 0x88
    f32 start_position[3];             // 0x8c .. 0x98
    f32 initial_position[3];           // 0x98 .. 0xa4
    f32 scaled_radius;                 // 0xa4
    f32 field_0xa8;                    // 0xa8
    f32 collision_radius;              // 0xac
    f32 collision_height;              // 0xb0
    f32 scaled_height;                 // 0xb4
    NUMTX field_0xb8;                  // 0xb8
    NUMTX field_0xf8;                  // 0xf8
    NUMTX field_0x138;                 // 0x138
    NUVEC collision_min;               // 0x178
    NUVEC collision_max;               // 0x184
    union {
        NUVEC upper_position; // 0x190
        struct {
            f32 field_0x190;
            f32 field_0x194;
            f32 field_0x198;
        };
    };
    union {
        NUVEC lower_position; // 0x19c
        f32 field_0x19c[3];
    };
    NUVEC collision_origin;   // 0x1a8
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
    undefined field_0x208[0x214 - 0x208];
    f32 field_0x214;
    f32 field_0x218;                      // 0x218
    f32 field_0x21c;                      // 0x21c
    f32 field_0x220;                      // 0x220
    f32 velocity_magnitude;               // 0x224
    f32 horizontal_velocity_magnitude;    // 0x228
    f32 viewdistance;                     // 0x22c
    f32 heardistance;                     // 0x230
    f32 maxviewheight;                    // 0x234
    f32 minviewheight;                    // 0x238
    undefined field_0x23c[0x274 - 0x23c]; // 0x23c .. 0x274
    u16 pitch_angle;                      // 0x274
    u16 field_0x276;                      // 0x276
    u16 roll_angle;                       // 0x278
    undefined field_0x27a[0x27c - 0x27a];
    char field_0x27c;         // 0x27c  player/character slot (0xff = none)
    u8 field_0x27d;           // 0x27d  terrain/contact flags
    u8 field_0x27e;           // 0x27e  previous terrain/contact flags
    u8 field_0x27f;           // 0x27f
    u8 field_0x280;           // 0x280
    u8 field_0x281;           // 0x281
    undefined field_0x282[2]; // 0x282 .. 0x284
    u8 model_draw_result;     // 0x284
    u8 field_0x285;
    u8 field_0x286;
    u8 field_0x287; // 0x287  owner/controller player index
    u8 field_0x288; // 0x288
    u8 field_0x289; // 0x289
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
extern "C" void APIObjectVelocities(GameObject_s *object);

struct rtldata_s {
    union {
        u8 data[0x144];
        struct {
            u8 unknown_000[0x78];
            NUCOLOUR3 intensity[3]; // 0x078
            NUVEC direction[3];     // 0x09c
            NUVEC ambient;          // 0x0c0
            u8 unknown_0cc[0x78];
        };
    };
};

DECOMP_ASSERT(sizeof(rtldata_s) == 0x144, "rtldata_s size");
DECOMP_ASSERT(offsetof(rtldata_s, intensity) == 0x78, "rtldata_s intensity offset");
DECOMP_ASSERT(offsetof(rtldata_s, direction) == 0x9c, "rtldata_s direction offset");
DECOMP_ASSERT(offsetof(rtldata_s, ambient) == 0xc0, "rtldata_s ambient offset");

struct OBJECTLIGHTINGSTATE_s {
    NUVEC ambient;          // 0x00
    NUCOLOUR3 intensity[3]; // 0x0c
    NUVEC direction[3];     // 0x30
};

DECOMP_ASSERT(sizeof(OBJECTLIGHTINGSTATE_s) == 0x54, "OBJECTLIGHTINGSTATE_s size");

typedef struct GameObject_s {
    APIOBJECT apiobj; // 0x0000 .. 0x02a8
    u32 field_0x2a8;  // 0x02a8
    u32 field_0x2ac;  // 0x02ac
    u8 pad_2b0[0x10]; // 0x02b0 .. 0x02c0
    PAI ai;           // 0x02c0 .. 0x04b0
    u8 pad_4b0[0x4c8 - 0x4b0];
    rtldata_s light_data;                 // 0x04c8
    OBJECTLIGHTINGSTATE_s lighting_state; // 0x060c
    union {
        SOCKPOSITION sock_position; // 0x0660
        struct {
            u8 sock_location_flags; // 0x0660
            u8 field_0x661;         // 0x0661, signed socket index (0xff = none)
            i16 sock_segment;       // 0x0662
            u8 pad_664[0x686 - 0x664];
            u16 yrot; // 0x0686, aliases sock_position.midpoint_rotation.y
            u8 pad_688[0x68c - 0x688];
            f32 field_0x68c; // 0x068c, aliases sock_position.distance
            u8 pad_690[0x698 - 0x690];
        };
    };
    u8 pad_698[0x6b4 - 0x698];                  // 0x0698 .. 0x06b4
    u8 player_packet[0x7a5 - 0x6b4];            // 0x06b4, PLAYERPACKET_s begins here
    u8 field_0x7a5;                             // 0x07a5
    u8 pad_7a6[0x7c0 - 0x7a6];                  // 0x07a6 .. 0x07c0
    u8 mini_anim_packet[0x24];                  // 0x07c0 .. 0x07e4
    u8 pad_7e4[0x7ec - 0x7e4];                  // 0x07e4 .. 0x07ec
    COINPACKET *coinpacket;                     // 0x07ec
    GizForceLOSState_s *gizforce_los_info;      // 0x07f0
    NUMTX field_0x7f4;                          // 0x07f4
    u8 pad_834[0xc34 - 0x834];                  // 0x0834 .. 0x0c34
    u32 field_0xc34;                            // 0x0c34
    f32 field_0xc38;                            // 0x0c38
    u8 pad_c3c[0xc54 - 0xc3c];                  // 0x0c3c .. 0x0c54
    f32 field_0xc54;                            // 0x0c54
    u8 pad_c58[0xc94 - 0xc58];                  // 0x0c58 .. 0x0c94
    struct GAMEPAD_s *pad_gamepad;              // 0x0c94  (originally inside PLAYERPACKET_s)
    SOCKPOSITION *oldpos;                       // 0x0c98
    u8 pad_c9c[0xcac - 0xc9c];                  // 0x0c9c .. 0x0cac
    void *suit;                                 // 0x0cac
    void *batarang;                             // 0x0cb0
    TORPEDOPACKET *torpedo;                     // 0x0cb4
    u8 pad_cb8[0xcc0 - 0xcb8];                  // 0x0cb8 .. 0x0cc0
    GameObject_s *field_0xcc0;                  // 0x0cc0
    u8 pad_cc4[0xd6c - 0xcc4];                  // 0x0cc4 .. 0x0d6c
    f32 field_0xd6c;                            // 0x0d6c  surface/contact state
    u8 pad_d70[0xd78 - 0xd70];                  // 0x0d70 .. 0x0d78
    f32 field_0xd78;                            // 0x0d78
    u8 pad_d7c[0xd80 - 0xd7c];                  // 0x0d7c .. 0x0d80
    f32 field_0xd80;                            // 0x0d80
    u8 pad_d84[0xd8c - 0xd84];                  // 0x0d84 .. 0x0d8c
    f32 field_0xd8c;                            // 0x0d8c
    u8 pad_d90[0xda8 - 0xd90];                  // 0x0d90 .. 0x0da8
    f32 field_0xda8;                            // 0x0da8
    u8 pad_dac[0xdbc - 0xdac];                  // 0x0dac .. 0x0dbc
    f32 field_0xdbc;                            // 0x0dbc
    u8 pad_dc0[0xdc8 - 0xdc0];                  // 0x0dc0 .. 0x0dc8
    f32 field_0xdc8;                            // 0x0dc8
    u8 pad_dcc[0xde0 - 0xdcc];                  // 0x0dcc .. 0x0de0
    f32 field_0xde0;                            // 0x0de0
    u8 pad_de4[0xdec - 0xde4];                  // 0x0de4 .. 0x0dec
    f32 field_0xdec;                            // 0x0dec
    u8 pad_df0[0xdf8 - 0xdf0];                  // 0x0df0 .. 0x0df8
    NUVEC reset_velocity;                       // 0x0df8 .. 0x0e04
    u8 pad_e04[0xe1e - 0xe04];                  // 0x0e04 .. 0x0e1e
    i16 field_0xe1e;                            // 0x0e1e
    u8 field_0xe20;                             // 0x0e20
    u8 field_0xe21;                             // 0x0e21
    u8 field_0xe22;                             // 0x0e22
    u8 field_0xe23;                             // 0x0e23
    u8 field_0xe24;                             // 0x0e24
    u8 pad_e25[0xe31 - 0xe25];                  // 0x0e25 .. 0x0e31
    u8 field_0xe31;                             // 0x0e31
    u8 field_0xe32;                             // 0x0e32
    u8 pad_e33[0xe36 - 0xe33];                  // 0x0e33 .. 0x0e36
    u8 field_0xe36;                             // 0x0e36
    u8 field_0xe37;                             // 0x0e37
    u8 field_0xe38;                             // 0x0e38
    u8 pad_e39[0xe41 - 0xe39];                  // 0x0e39 .. 0x0e41
    u8 field_0xe41;                             // 0x0e41  current surface type
    u8 pad_e42[0xe50 - 0xe42];                  // 0x0e42 .. 0x0e50
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
    u8 pad_ee4[0xee8 - 0xee4];                  // 0x0ee4 .. 0x0ee8
    f32 field_0xee8;                            // 0x0ee8
    f32 field_0xeec;                            // 0x0eec
    u8 pad_ef0[0xef8 - 0xef0];                  // 0x0ef0 .. 0x0ef8
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
    u8 pad_f05[0xf1c - 0xf05];
    f32 field_0xf1c; // 0x0f1c
    u8 pad_f20[0xf30 - 0xf20];
    NUVEC surface_normal;   // 0x0f30
    NUVEC facing_direction; // 0x0f3c
    u8 pad_f48[0xffc - 0xf48];
    f32 field_0xffc;  // 0x0ffc
    f32 field_0x1000; // 0x1000
    f32 field_0x1004; // 0x1004
    f32 field_0x1008; // 0x1008
    u8 pad_100c[0x1010 - 0x100c];
    f32 collision_height_scale; // 0x1010
    u8 pad_1014[0x1018 - 0x1014];
    f32 field_0x1018;             // 0x1018
    f32 field_0x101c;             // 0x101c
    f32 field_0x1020;             // 0x1020
    u8 pad_1024[0x1050 - 0x1024]; // 0x1024 .. 0x1050
    u32 field_0x1050;             // 0x1050
    u32 field_0x1054;             // 0x1054
    u8 pad_1058[0x105c - 0x1058]; // 0x1058 .. 0x105c
    u16 field_0x105c;             // 0x105c terrain query flags
    u16 field_0x105e;             // 0x105e surface x rotation
    u16 field_0x1060;             // 0x1060 surface z rotation
    u16 field_0x1062;             // 0x1062 previous surface x rotation
    u16 field_0x1064;             // 0x1064 previous surface z rotation
    u8 pad_1066[0x1068 - 0x1066];
    u16 field_0x1068; // 0x1068 reflection x rotation
    u16 field_0x106a; // 0x106a reflection z rotation
    u8 pad_106c[0x106e - 0x106c];
    u16 field_0x106e; // 0x106e
    i16 id;           // 0x1070
    u8 pad_1072[0x1078 - 0x1072];
    i16 field_0x1078; // 0x1078 reflected/platform terrain id
    i16 field_0x107a; // 0x107a terrain id
    i16 field_0x107c; // 0x107c
    u8 pad_107e[0x1084 - 0x107e];
    u8 field_0x1084;                       // 0x1084
    u8 use_model_origin;                   // 0x1085
    u8 field_0x1086;                       // 0x1086
    u8 field_0x1087;                       // 0x1087
    u8 field_0x1088;                       // 0x1088
    u8 field_0x1089;                       // 0x1089
    u8 hitpoints;                          // 0x108a
    u8 current_hp;                         // 0x108b
    u8 pad_108c[0x108e - 0x108c];          // 0x108c .. 0x108e
    u8 field_0x108e;                       // 0x108e
    u8 pad_108f[0x10b0 - 0x108f];          // 0x108f .. 0x10b0
    void *opponent;                        // 0x10b0
    void *last_attacker;                   // 0x10b4
    void *field_0x10b8;                    // 0x10b8
    void (*move_override)(GameObject_s *); // 0x10bc
    u8 pad_10c0[0x10c8 - 0x10c0];
    f32 field_0x10c8;      // 0x10c8
    f32 field_0x10cc;      // 0x10cc
    f32 field_0x10d0;      // 0x10d0
    f32 vertical_velocity; // 0x10d4
    u8 pad_10d8[0x10e4 - 0x10d8];
    void ClearAddons();
    void ClearMechObjectInterface();
    void GetAddons(bool);
    void GetMechObjectInterface();
    void IsRunningTaskType(struct HashedKey const &);
    void KillTasks();
} GameObject;

DECOMP_ASSERT(sizeof(GameObject_s) == 0x10e4, "GameObject size");
DECOMP_ASSERT(offsetof(APIOBJECT, anim_packet) == 0x08, "APIOBJECT animation packet offset");
DECOMP_ASSERT(offsetof(APIOBJECT, facing_angle) == 0x58, "APIOBJECT facing angle offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0xb8) == 0xb8, "APIOBJECT primary matrix offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0x214) == 0x214, "APIOBJECT reset distance offset");
DECOMP_ASSERT(offsetof(APIOBJECT, pitch_angle) == 0x274, "APIOBJECT pitch angle offset");
DECOMP_ASSERT(offsetof(APIOBJECT, model_draw_result) == 0x284, "APIOBJECT model draw result offset");
DECOMP_ASSERT(offsetof(GameObject_s, sock_position) == 0x660, "GameObject socket position offset");
DECOMP_ASSERT(offsetof(GameObject_s, mini_anim_packet) == 0x7c0, "GameObject mini animation packet offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x7f4) == 0x7f4, "GameObject auxiliary matrix offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xcc0) == 0xcc0, "GameObject linked object offset");
DECOMP_ASSERT(offsetof(GameObject_s, reset_velocity) == 0xdf8, "GameObject reset velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xe1e) == 0xe1e, "GameObject force glow index offset");
DECOMP_ASSERT(offsetof(GameObject_s, facing_direction) == 0xf3c, "GameObject facing direction offset");
DECOMP_ASSERT(offsetof(GameObject_s, surface_normal) == 0xf30, "GameObject surface normal offset");
DECOMP_ASSERT(offsetof(GameObject_s, apiobj.collision_radius) == 0xac, "GameObject collision radius offset");
DECOMP_ASSERT(offsetof(GameObject_s, apiobj.collision_min) == 0x178, "GameObject collision bounds offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai.terrain_origin) == 0x408, "GameObject terrain origin offset");
DECOMP_ASSERT(offsetof(GameObject_s, collision_height_scale) == 0x1010, "GameObject collision scale offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x105c) == 0x105c, "GameObject terrain flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1084) == 0x1084, "GameObject surface reset flag offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1088) == 0x1088, "GameObject matrix selector offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x10b8) == 0x10b8, "GameObject snake data offset");
DECOMP_ASSERT(offsetof(GameObject_s, move_override) == 0x10bc, "GameObject movement override offset");
DECOMP_ASSERT(offsetof(GameObject_s, vertical_velocity) == 0x10d4, "GameObject vertical velocity offset");

typedef struct GameObject_s GameObject_s;
