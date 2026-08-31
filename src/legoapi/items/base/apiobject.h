#pragma once

#include "decomp.h"
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/numtx.h"

struct GameObject_s;
struct MechObjectInterface;
struct GAMEOBJECTADDONS_s;
struct GAMEANIMOBJ_s;
struct GIZFORCE_s;
struct characterdata_s;
struct AIAREA_s;
struct AILOCATOR_s;
struct AIGROUP_s;
struct AIPATHNODE_s;
struct AISCRIPTPROCESS_s;

enum APIOBJECT_FLAGS {
    APIOBJECT_FLAG_IN_USE = 0x0001,
    APIOBJECT_FLAG_PLAYER_ACTIVE = 0x0080,
    // Set for every live character object, including AI creatures.  Player
    // ownership is represented separately by Player[]/field_0x287.
    APIOBJECT_FLAG_CHARACTER = 0x1000,
    APIOBJECT_FLAG_PLAYER_CHARACTER = APIOBJECT_FLAG_CHARACTER,
    APIOBJECT_FLAG_RESPAWN_ENABLED = 0x2000,
    APIOBJECT_FLAG_AI_PLAYER_MASK = 0x0180,
};

enum APIOBJECT_HIGH_FLAGS : u8 {
    APIOBJECT_HIGH_FLAG_CHARACTER = APIOBJECT_FLAG_CHARACTER >> 8,
    APIOBJECT_HIGH_FLAG_PLAYER_CHARACTER = APIOBJECT_HIGH_FLAG_CHARACTER,
    APIOBJECT_HIGH_FLAG_RESPAWN_ENABLED = APIOBJECT_FLAG_RESPAWN_ENABLED >> 8,
};

enum AI_RESPAWN_FLAGS : u8 {
    AI_RESPAWN_FLAG_DISABLED = 0x01,
};

enum APIOBJECT_MOTION_FLAGS {
    APIOBJECT_MOTION_FLAG_AI_CONTROLLED = 0x0400,
};

enum APIOBJECT_STATE_FLAGS : u32 {
    // Doors_Check tests byte +0x1f6 bit 2, which is bit 18 of the runtime
    // state word at +0x1f4.  It is not part of the u16 object flags at +0x1f8.
    APIOBJECT_STATE_FLAG_IGNORE_DOORS = 0x00040000,
};

enum GAMEOBJECT_F03_FLAGS : u8 {
    // Allows the object to satisfy an obstacle's terrain/platform trigger
    // even when APIOBJECT::field_0x27d is clear.
    GAMEOBJECT_F03_FLAG_OBSTACLE_TERRAIN_VALID = 0x20,
};

enum GAMEOBJECT_E22_FLAGS : u8 {
    GAMEOBJECT_E22_FLAG_WEAPON_ANIMATION = 0x01,
    GAMEOBJECT_E22_FLAG_INPUT_ANGLE_VALID = 0x20,
};

enum GAMEOBJECT_E23_FLAGS : u8 {
    GAMEOBJECT_E23_FLAG_FORCE_WEAPON_IDLE = 0x01,
};

enum GAMEOBJECT_E20_FLAGS : u8 {
    GAMEOBJECT_E20_FLAG_MOVEMENT_DISABLED = 0x20,
};

enum GAMEOBJECT_MOVEMENT_FLAGS : u8 {
    GAMEOBJECT_MOVEMENT_FLAG_FACE_REVERSED = 0x04,
    GAMEOBJECT_MOVEMENT_FLAG_BACKWARDS = 0x40,
    GAMEOBJECT_MOVEMENT_FLAG_REVERSE_VELOCITY = 0x80,
};
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

typedef struct TORPEDOPACKET_s {
    u8 count;     // 0x00
    u8 field_0x1; // 0x01  (bit0 = in use)
    u8 pad[0x86]; // 0x02..0x88
} TORPEDOPACKET;

// Player/enemy bookkeeping block, base 0x2c0 within a GameObject.
typedef struct AIPACKET_s {
    union {
        // The packet begins with its primary script processor.  Keeping a raw
        // view avoids a circular include while preserving the processor ABI.
        u8 script_process[0xc8];
        struct {
            u8 pad0[0xa0];
            AIAREA_s *area; // 0xa0
            union {
                AILOCATOR_s *locator; // 0xa4
                void *field_0x364;
            };
            u8 pad0b[0xb0 - 0xa8];
            u8 creature_set; // 0xb0
            u8 pad0c[0xc8 - 0xb1];
        };
    };
    void *field_0xc8;
    AISCRIPTPROCESS_s *alternate_script_process;
    GameObject_s *owner;         // 0xd0
    void *nearest_opponent;      // 0xd4
    f32 nearest_opponent_metric; // 0xd8
    u32 field_0xdc;
    u32 field_0xe0;
    void *opponent; // 0xe4
    u8 pad_e8[0xec - 0xe8];
    u32 field_0xec;
    u32 field_0xf0;
    u8 pad_f4[0x104 - 0xf4];
    union {
        NUVEC movement_destination; // 0x104
        NUVEC reset_position;
    };
    union {
        f32 movement_stopping_distance; // 0x110
        u32 field_0x110;
    };
    NUVEC movement_position; // 0x114
    f32 mover_height;        // 0x120
    union {
        i16 inside_path_node; // 0x3e4 overall (-1 when not inside a node)
        i16 field_0x124;
    };
    u8 pad1b[0x3ec - 0x3e6];
    u64 character_type_mask;  // 0x3ec overall
    u8 field_0x134;           // 0x3f4 overall: source creature index
    u8 path_connection_state; // 0x3f5 overall
    u16 available_routes;     // 0x3f6 overall
    union {
        u8 current_route; // 0x3f8 overall
        u8 field_0x138;
    };
    union {
        u8 next_route; // 0x3f9 overall
        u8 field_0x139;
    };
    u8 reset_mode;      // 0x3fa overall: AI reset/activation state
    u8 goal_speed_mode; // 0x3fb overall: walk/run/tiptoe speed selector
    u8 pad1c_end[0x400 - 0x3fc];
    AIGROUP_s *group; // 0x400 overall
    u8 group_row;     // 0x404 overall
    u8 group_column;  // 0x405 overall
    u8 group_member;  // 0x406 overall
    u8 pad_group;
    NUVEC terrain_origin;         // 0x148
    AIPATHINFO path_info;         // 0x154
    NUVEC last_path_position;     // 0x16c
    AIPATHNODE_s *goal_path_node; // 0x178
    u8 pad_17c[0x180 - 0x17c];
    void *movement_target; // 0x180
    u8 pad_184[0x188 - 0x184];
    f32 antinode_timer; // 0x188
    u8 pad_18c[0x1ac - 0x18c];
    AILOCATOR_s *respawn_locator;   // 0x1ac
    NUVEC fallback_destination;     // 0x1b0
    f32 fallback_stopping_distance; // 0x1bc
    f32 movement_parameter;         // 0x1c0
    u8 pad_1c4[0x1c8 - 0x1c4];
    AIPATHINFO fallback_path_info; // 0x1c8
    u32 frame_flags;               // 0x1e0
    union {
        u8 movement_flags; // 0x1e4
        u8 field_0x1e4;
    };
    u8 field_0x1e5;
    u8 field_0x1e6;
    u8 field_0x1e7;
    u8 navigation_flags; // 0x1e8
    u8 pad_1e9[0x1ec - 0x1e9];
    f32 movement_target_radius; // 0x1ec
    u32 capabilities;           // 0x1f0
    u32 frame_state;            // 0x1f4
    u32 respawn_count;          // 0x1f8
    f32 spawn_delay;            // 0x1fc
    u32 field_0x200;
    f32 time_off_path; // 0x204
} AIPACKET;

typedef AIPACKET PAI;

enum AIPACKET_NAVIGATION_FLAGS : u8 {
    AIPACKET_NAVIGATION_FLAG_TRANSIENT = 0x01,
    // Search every loaded path instead of only the active level path.
    AIPACKET_NAVIGATION_FLAG_SEARCH_ALL_PATHS = 0x02,
};

enum AIPACKET_MOVEMENT_MODE : u8 {
    AIPACKET_MOVEMENT_NONE = 0,
    AIPACKET_MOVEMENT_TO_DESTINATION = 1,
    AIPACKET_MOVEMENT_RETREAT = 2,
    AIPACKET_MOVEMENT_CIRCLE = 3,
    AIPACKET_MOVEMENT_WANDER = 4,
    AIPACKET_MOVEMENT_AVOIDING_CAMERA = 6,
    AIPACKET_MOVEMENT_DIRECT = 7,
    AIPACKET_MOVEMENT_MODE_MASK = 7,
    AIPACKET_MOVEMENT_OPTION_TRANSIENT = 0x08,
};

enum AIPACKET_RUNTIME_FLAGS : u8 {
    AIPACKET_RUNTIME_INITIALISED = 0x01,
    AIPACKET_RUNTIME_SPECIAL_MOVE = 0x08,
};

enum AIPACKET_MOVEMENT_SOURCE_FLAGS : u8 {
    AIPACKET_MOVEMENT_SOURCE_MASK = 0x0c,
    AIPACKET_MOVEMENT_SOURCE_CREATURE = 0x04,
    AIPACKET_MOVEMENT_SOURCE_LOCATOR = 0x08,
    AIPACKET_MOVEMENT_SOURCE_ACTIVE = 0x10,
    AIPACKET_MOVEMENT_FRAME_RESET = 0x20,
    AIPACKET_MOVEMENT_FORCE_PATH_REFRESH = 0x02,
    AIPACKET_MOVEMENT_SPECIAL_HANDLED = 0x80,
};

typedef struct APIOBJECT_s {
    GameObject_s *objptr;              // 0x00
    PAI *ai;                           // 0x04
    ANIMPACKET_s anim_packet;          // 0x08
    CHARACTERMODEL_s *character_model; // 0x50
    characterdata_s *character_data;   // 0x54
    u16 facing_angle;                  // 0x58
    u16 movement_facing_angle;         // 0x5a
    NUVEC position;                    // 0x5c
    NUVEC velocity;                    // 0x68
    NUVEC previous_velocity;           // 0x74
    f32 pos_x;                         // 0x80
    f32 pos_y;                         // 0x84
    f32 pos_z;                         // 0x88
    NUVEC start_position;              // 0x8c
    NUVEC initial_position;            // 0x98
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
    NUVEC field_0x1c0;        // 0x1c0  alternate camera position used by vehicle type 0x2b
    NUVEC respawn_position;   // 0x1cc
    f32 respawn_timer;        // 0x1d8
    f32 field_0x1dc;          // 0x1dc
    f32 field_0x1e0;          // 0x1e0
    u32 field_0x1e4;          // 0x1e4  mission kill-mask lo
    u32 field_0x1e8;          // 0x1e8  mission kill-mask hi
    u32 field_0x1ec;          // 0x1ec  player mission-complete mask lo
    u32 field_0x1f0;          // 0x1f0  player mission-complete mask hi
    u32 field_0x1f4;          // 0x1f4
    union {
        u16 field_0x1f8; // 0x1f8  complete object flags
        struct {
            u8 flags_low;
            u8 flags_high;
        };
    };
    u8 field_0x1fa; // 0x1fa
    u8 field_0x1fb;
    f32 field_0x1fc; // 0x1fc
    f32 field_0x200; // 0x200
    f32 field_0x204; // 0x204
    undefined field_0x208[0x214 - 0x208];
    f32 field_0x214;
    f32 field_0x218;                      // 0x218
    f32 water_height;                     // 0x21c
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
    i16 supporting_platform_id;           // 0x27a (-1 = no supporting character platform)
    char field_0x27c;                     // 0x27c  player/character slot (0xff = none)
    u8 field_0x27d;                       // 0x27d  terrain/contact flags
    u8 field_0x27e;                       // 0x27e  previous terrain/contact flags
    u8 field_0x27f;                       // 0x27f
    u8 field_0x280;                       // 0x280
    u8 field_0x281;                       // 0x281
    u8 is_underwater;                     // 0x282
    u8 intersects_water;                  // 0x283
    u8 model_draw_result;                 // 0x284
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
    union {
        PAI ai; // 0x02c0 .. 0x04c8
        struct {
            u8 ai_packet_prefix[0x1f8];
            u32 ai_respawn_count; // 0x04b8
            f32 ai_spawn_delay;   // 0x04bc
            u8 ai_packet_suffix[8];
        };
    };
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
    u8 pad_698[0x6b0 - 0x698];       // 0x0698 .. 0x06b0
    u8 field_0x6b0;                  // 0x06b0 terrain/contact state
    u8 pad_6b1[0x6b4 - 0x6b1];       // 0x06b1 .. 0x06b4
    u8 player_packet[0x780 - 0x6b4]; // 0x06b4, PLAYERPACKET_s begins here
    void *field_0x780;               // 0x0780
    u8 pad_784[0x788 - 0x784];       // 0x0784 .. 0x0788
    void *field_0x788;               // 0x0788
    u8 pad_78c[0x79a - 0x78c];       // 0x078c .. 0x079a
    i16 context_animation;           // 0x079a, action-owned animation index
    u8 pad_79c[0x7a4 - 0x79c];       // 0x079c .. 0x07a4
    u8 build_button_taps;            // 0x07a4, capped Build-It acceleration input
    union {
        u8 field_0x7a5;
        i8 build_context; // 0x07a5, -1 when no Build-It owns the player
    };
    u8 pad_7a6[0x7a8 - 0x7a6]; // 0x07a6 .. 0x07a8
    u8 action_movement_state;  // 0x07a8
    u8 pad_7a9[0x7ac - 0x7a9]; // 0x07a9 .. 0x07ac
    u8 context_flags;          // 0x07ac
    i8 context_variant_flags;  // 0x07ad
    u8 pad_7ae[0x7c0 - 0x7ae]; // 0x07ae .. 0x07c0
    union {
        u8 mini_anim_packet[0x24]; // 0x07c0 .. 0x07e4
        MINIANIMPACKET_s mini_animation;
    };
    u8 pad_7e4[0x7ec - 0x7e4];                  // 0x07e4 .. 0x07ec
    COINPACKET *coinpacket;                     // 0x07ec
    void *gizforce_los_info;                    // 0x07f0
    NUMTX field_0x7f4;                          // 0x07f4
    u8 pad_834[0xc34 - 0x834];                  // 0x0834 .. 0x0c34
    u32 field_0xc34;                            // 0x0c34
    f32 field_0xc38;                            // 0x0c38
    u8 pad_c3c[0xc54 - 0xc3c];                  // 0x0c3c .. 0x0c54
    f32 field_0xc54;                            // 0x0c54
    u8 pad_c58[0xc94 - 0xc58];                  // 0x0c58 .. 0x0c94
    struct GAMEPAD_s *pad_gamepad;              // 0x0c94  (originally inside PLAYERPACKET_s)
    SOCKPOSITION *oldpos;                       // 0x0c98
    u8 pad_c9c[0xca8 - 0xc9c];                  // 0x0c9c .. 0x0ca8
    void *field_0xca8;                          // 0x0ca8
    void *suit;                                 // 0x0cac
    void *batarang;                             // 0x0cb0
    TORPEDOPACKET *torpedo;                     // 0x0cb4
    u8 pad_cb8[0xcc0 - 0xcb8];                  // 0x0cb8 .. 0x0cc0
    GameObject_s *field_0xcc0;                  // 0x0cc0
    u8 pad_cc4[0xd14 - 0xcc4];                  // 0x0cc4 .. 0x0d14
    u32 field_0xd14;                            // 0x0d14
    u8 pad_d18[0xd40 - 0xd18];                  // 0x0d18 .. 0x0d40
    f32 delayed_turn_timer;                     // 0x0d40
    u8 pad_d44[0xd6c - 0xd44];                  // 0x0d44 .. 0x0d6c
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
    u8 pad_e04[0xe08 - 0xe04];                  // 0x0e04 .. 0x0e08
    u16 delayed_turn_target_angle;              // 0x0e08
    u8 pad_e0a[0xe0c - 0xe0a];                  // 0x0e0a .. 0x0e0c
    u16 current_input_angle;                    // 0x0e0c
    u8 pad_e0e[0xe18 - 0xe0e];                  // 0x0e0e .. 0x0e18
    i16 movement_lean_angle;                    // 0x0e18
    i16 secondary_lean_angle;                   // 0x0e1a
    i16 tertiary_lean_angle;                    // 0x0e1c
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
    u8 pad_e58[0xeb4 - 0xe58];                  // 0x0e58 .. 0x0eb4
    u32 field_0xeb4;                            // 0x0eb4, cleared on hub room changes
    u8 pad_eb8[0xebc - 0xeb8];                  // 0x0eb8 .. 0x0ebc
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
    u8 field_0xf02;
    u8 field_0xf03; // GAMEOBJECT_F03_FLAGS
    u8 field_0xf04; // 0x0f04
    u8 pad_f05[0xf1c - 0xf05];
    f32 field_0xf1c; // 0x0f1c
    u8 pad_f20[0xf24 - 0xf20];
    NUVEC target_velocity;  // 0x0f24
    NUVEC surface_normal;   // 0x0f30
    NUVEC facing_direction; // 0x0f3c
    u8 pad_f48[0xffc - 0xf48];
    union {
        f32 field_0xffc;
        f32 character_bottom; // 0x0ffc, unscaled lower character bound
    };
    union {
        f32 field_0x1000;
        f32 character_top; // 0x1000, unscaled upper character bound
    };
    f32 field_0x1004; // 0x1004
    f32 field_0x1008; // 0x1008
    u8 pad_100c[0x1010 - 0x100c];
    f32 collision_height_scale; // 0x1010
    u8 pad_1014[0x1018 - 0x1014];
    f32 field_0x1018;             // 0x1018
    f32 field_0x101c;             // 0x101c
    f32 field_0x1020;             // 0x1020
    u8 pad_1024[0x104c - 0x1024]; // 0x1024 .. 0x104c
    void *field_0x104c;           // 0x104c
    u32 field_0x1050;             // 0x1050
    u32 field_0x1054;             // 0x1054
    u8 pad_1058[0x105c - 0x1058]; // 0x1058 .. 0x105c
    u16 field_0x105c;             // 0x105c terrain query flags
    u16 field_0x105e;             // 0x105e surface x rotation
    u16 field_0x1060;             // 0x1060 surface z rotation
    u16 field_0x1062;             // 0x1062 previous surface x rotation
    u16 field_0x1064;             // 0x1064 previous surface z rotation
    u8 pad_1066[0x1068 - 0x1066];
    u16 field_0x1068;            // 0x1068 reflection x rotation
    u16 field_0x106a;            // 0x106a reflection z rotation
    u16 previous_movement_angle; // 0x106c
    u16 field_0x106e;            // 0x106e
    i16 id;                      // 0x1070
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
    u8 pad_108f[0x1092 - 0x108f];          // 0x108f .. 0x1092
    u8 field_0x1092;                       // 0x1092
    u8 field_0x1093;                       // 0x1093
    u8 pad_1094[0x109c - 0x1094];          // 0x1094 .. 0x109c
    u32 field_0x109c;                      // 0x109c
    u8 pad_10a0[0x10b0 - 0x10a0];          // 0x10a0 .. 0x10b0
    void *opponent;                        // 0x10b0
    void *last_attacker;                   // 0x10b4
    void *field_0x10b8;                    // 0x10b8
    void (*move_override)(GameObject_s *); // 0x10bc
    u8 pad_10c0[0x10c4 - 0x10c0];
    f32 ai_elapsed_time;                   // 0x10c4, accumulated until the next AI update
    f32 field_0x10c8;                      // 0x10c8
    f32 field_0x10cc;                      // 0x10cc
    f32 field_0x10d0;                      // 0x10d0
    f32 vertical_velocity;                 // 0x10d4
    GIZFORCE_s *gizforce_target;           // 0x10d8
    GAMEANIMOBJ_s *gizforce_target_object; // 0x10dc
    u8 pad_10e0[0x10e4 - 0x10e0];
    void ClearAddons();
    void ClearMechObjectInterface();
    void GetAddons(bool);
    void GetMechObjectInterface();
    void IsRunningTaskType(struct HashedKey const &);
    void KillTasks();
} GameObject;

DECOMP_ASSERT(sizeof(GameObject_s) == 0x10e4, "GameObject size");
DECOMP_ASSERT(sizeof(AIPACKET) == 0x208, "AIPACKET size");
DECOMP_ASSERT(offsetof(AIPACKET, alternate_script_process) == 0xcc, "AIPACKET alternate script processor offset");
DECOMP_ASSERT(offsetof(AIPACKET, movement_destination) == 0x104, "AIPACKET destination offset");
DECOMP_ASSERT(offsetof(AIPACKET, movement_position) == 0x114, "AIPACKET movement position offset");
DECOMP_ASSERT(offsetof(AIPACKET, path_info) == 0x154, "AIPACKET path-info offset");
DECOMP_ASSERT(offsetof(AIPACKET, last_path_position) == 0x16c, "AIPACKET last path position offset");
DECOMP_ASSERT(offsetof(AIPACKET, goal_path_node) == 0x178, "AIPACKET goal-node offset");
DECOMP_ASSERT(offsetof(AIPACKET, navigation_flags) == 0x1e8, "AIPACKET navigation flags offset");
DECOMP_ASSERT(offsetof(AIPACKET, movement_target_radius) == 0x1ec, "AIPACKET target-radius offset");
DECOMP_ASSERT(offsetof(AIPACKET, capabilities) == 0x1f0, "AIPACKET capabilities offset");
DECOMP_ASSERT(offsetof(AIPACKET, fallback_path_info) == 0x1c8, "AIPACKET fallback path-info offset");
DECOMP_ASSERT(offsetof(AIPACKET, time_off_path) == 0x204, "AIPACKET off-path timer offset");
DECOMP_ASSERT(offsetof(APIOBJECT, anim_packet) == 0x08, "APIOBJECT animation packet offset");
DECOMP_ASSERT(offsetof(APIOBJECT, facing_angle) == 0x58, "APIOBJECT facing angle offset");
DECOMP_ASSERT(offsetof(APIOBJECT, velocity) == 0x68, "APIOBJECT velocity offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_velocity) == 0x74, "APIOBJECT previous velocity offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0x1c0) == 0x1c0, "APIOBJECT alternate camera position offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0xb8) == 0xb8, "APIOBJECT primary matrix offset");
DECOMP_ASSERT(offsetof(APIOBJECT, field_0x214) == 0x214, "APIOBJECT reset distance offset");
DECOMP_ASSERT(offsetof(APIOBJECT, water_height) == 0x21c, "APIOBJECT water height offset");
DECOMP_ASSERT(offsetof(APIOBJECT, pitch_angle) == 0x274, "APIOBJECT pitch angle offset");
DECOMP_ASSERT(offsetof(APIOBJECT, supporting_platform_id) == 0x27a, "APIOBJECT supporting platform id offset");
DECOMP_ASSERT(offsetof(APIOBJECT, model_draw_result) == 0x284, "APIOBJECT model draw result offset");
DECOMP_ASSERT(offsetof(GameObject_s, sock_position) == 0x660, "GameObject socket position offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x6b0) == 0x6b0, "GameObject terrain contact state offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x780) == 0x780, "GameObject field_0x780 offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x788) == 0x788, "GameObject field_0x788 offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_animation) == 0x79a, "GameObject context animation offset");
DECOMP_ASSERT(offsetof(GameObject_s, build_button_taps) == 0x7a4, "GameObject Build-It tap count offset");
DECOMP_ASSERT(offsetof(GameObject_s, build_context) == 0x7a5, "GameObject Build-It context offset");
DECOMP_ASSERT(offsetof(GameObject_s, action_movement_state) == 0x7a8, "GameObject action movement state offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_flags) == 0x7ac, "GameObject context flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_variant_flags) == 0x7ad, "GameObject context variant flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, mini_anim_packet) == 0x7c0, "GameObject mini animation packet offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x7f4) == 0x7f4, "GameObject auxiliary matrix offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xcc0) == 0xcc0, "GameObject linked object offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xca8) == 0xca8, "GameObject field_0xca8 offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xd14) == 0xd14, "GameObject input state offset");
DECOMP_ASSERT(offsetof(GameObject_s, reset_velocity) == 0xdf8, "GameObject reset velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, delayed_turn_timer) == 0xd40, "GameObject delayed turn timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, delayed_turn_target_angle) == 0xe08, "GameObject delayed turn target offset");
DECOMP_ASSERT(offsetof(GameObject_s, current_input_angle) == 0xe0c, "GameObject input angle offset");
DECOMP_ASSERT(offsetof(GameObject_s, movement_lean_angle) == 0xe18, "GameObject movement lean offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xe1e) == 0xe1e, "GameObject force glow index offset");
DECOMP_ASSERT(offsetof(GameObject_s, facing_direction) == 0xf3c, "GameObject facing direction offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xf03) == 0xf03, "GameObject obstacle terrain flag offset");
DECOMP_ASSERT(offsetof(GameObject_s, surface_normal) == 0xf30, "GameObject surface normal offset");
DECOMP_ASSERT(offsetof(GameObject_s, target_velocity) == 0xf24, "GameObject target velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, character_bottom) == 0xffc, "GameObject lower bound offset");
DECOMP_ASSERT(offsetof(GameObject_s, character_top) == 0x1000, "GameObject upper bound offset");
DECOMP_ASSERT(offsetof(GameObject_s, previous_movement_angle) == 0x106c, "GameObject previous movement angle offset");
DECOMP_ASSERT(offsetof(GameObject_s, apiobj.collision_radius) == 0xac, "GameObject collision radius offset");
DECOMP_ASSERT(offsetof(GameObject_s, apiobj.collision_min) == 0x178, "GameObject collision bounds offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai.terrain_origin) == 0x408, "GameObject terrain origin offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai.path_info) == 0x414, "GameObject AI path-info offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai.reset_position) == 0x3c4, "GameObject AI reset-position offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_respawn_count) == 0x4b8, "GameObject AI respawn-count offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_spawn_delay) == 0x4bc, "GameObject AI spawn-delay offset");
DECOMP_ASSERT(offsetof(GameObject_s, collision_height_scale) == 0x1010, "GameObject collision scale offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x104c) == 0x104c, "GameObject field_0x104c offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x105c) == 0x105c, "GameObject terrain flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1084) == 0x1084, "GameObject surface reset flag offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1088) == 0x1088, "GameObject matrix selector offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x10b8) == 0x10b8, "GameObject snake data offset");
DECOMP_ASSERT(offsetof(GameObject_s, move_override) == 0x10bc, "GameObject movement override offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_elapsed_time) == 0x10c4, "GameObject AI elapsed-time offset");
DECOMP_ASSERT(offsetof(GameObject_s, vertical_velocity) == 0x10d4, "GameObject vertical velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, gizforce_target) == 0x10d8, "GameObject Force target offset");
DECOMP_ASSERT(offsetof(GameObject_s, gizforce_target_object) == 0x10dc, "GameObject Force object target offset");

typedef struct GameObject_s GameObject_s;
