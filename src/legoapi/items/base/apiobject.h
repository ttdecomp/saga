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
struct GizForceLOSState_s {
    u8 state[0x630];
};
struct MechObjectInterface;
struct GAMEOBJECTADDONS_s;
struct GAMEANIMOBJ_s;
struct GIZFORCE_s;
struct CABLE_s;
struct characterdata_s;
struct CHARACTERANIM_s;
struct AIAREA_s;
struct AILOCATOR_s;
struct AILOCATORSET_s;
struct AIGROUP_s;
struct AIPATHNODE_s;
struct AISCRIPTPROCESS_s;

enum CHARACTER_CONTEXT : i8 {
    CHARACTER_CONTEXT_JUMP = 0,
    CHARACTER_CONTEXT_LAND_JUMP = 1,
    CHARACTER_CONTEXT_LAND_JUMP_2 = 2,
    CHARACTER_CONTEXT_LAND_FLIP = 3,
    CHARACTER_CONTEXT_LAND_COMBO_JUMP = 4,
    CHARACTER_CONTEXT_COMBO = 5,
    CHARACTER_CONTEXT_FORCE = 0x08,
    CHARACTER_CONTEXT_FORCE_THROW = 0x12,
    CHARACTER_CONTEXT_FORCE_PUSH = 0x1b,
    CHARACTER_CONTEXT_FORCE_DEFLECT = 0x1d,
    CHARACTER_CONTEXT_DROP_IN = 0x23,
    CHARACTER_CONTEXT_DROP_OUT = 0x24,
    CHARACTER_CONTEXT_DOOMED = 0x2b,
    CHARACTER_CONTEXT_BUILD_IT = 0x2d,
    CHARACTER_CONTEXT_LINKED_OBJECT = 0x3b,
    CHARACTER_CONTEXT_NONE = -1,
};

enum APIOBJECT_FLAGS {
    APIOBJECT_FLAG_IN_USE = 0x0001,
    APIOBJECT_FLAG_GROUNDED = 0x0004,
    APIOBJECT_FLAG_SHADOW_AT_OBJECT_HEIGHT = 0x0010,
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

enum APIOBJECT_TERRAIN_CONTACT_FLAGS : u8 {
    APIOBJECT_TERRAIN_CONTACT_FLOOR = 0x01,
    APIOBJECT_TERRAIN_CONTACT_NEAR_FLOOR = 0x02,
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

enum GAMEOBJECT_E24_FLAGS : u8 {
    // The render evaluation has refreshed joint_matrices for model-origin
    // collision and attachment queries.
    GAMEOBJECT_E24_FLAG_JOINT_MATRICES_UPDATED = 0x08,
};

enum GAMEOBJECT_E20_FLAGS : u8 {
    GAMEOBJECT_E20_FLAG_MOVEMENT_DISABLED = 0x20,
    GAMEOBJECT_E20_FLAG_COMBO_MOVEMENT = 0x40,
};

enum GAMEOBJECT_ACTION_FLAGS : u16 {
    GAMEOBJECT_ACTION_FLAG_FORCE_PUSH_WEAPON_IDLE_MASK = 0x0580,
};

enum GAMEOBJECT_EF8_FLAGS : u8 {
    GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT = 0x10,
};

enum GAMEOBJECT_CONTEXT_FLAGS : u8 {
    GAMEOBJECT_CONTEXT_FLAGS_COMBO_START_RETAIN_MASK = 0xa3,
};

enum WEAPON_SCALE_STATE : u8 {
    WEAPON_SCALE_IDLE = 0,
    WEAPON_SCALE_EXTENDING = 1,
    WEAPON_SCALE_RETRACTING = 2,
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

struct CHARACTER_SHADOW_s {
    NUVEC position;
    u16 x_rotation;
    u16 z_rotation;
    f32 opacity;
};

DECOMP_ASSERT(sizeof(CHARACTER_SHADOW_s) == 0x14, "CHARACTER_SHADOW_s ABI");

typedef struct COINPACKET_s {
    u32 coins;              // 0x00
    f32 scale;              // 0x04
    u16 lastcoin;           // 0x08
    u8 active;              // 0x0a
    u8 field_0xb;           // 0x0b
    f32 double_score_timer; // 0x0c
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
            AILOCATORSET_s *locator_set; // 0xa8
            u8 pad0b[0xb0 - 0xac];
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
    AIGROUP_s *group;                   // 0x400 overall
    u8 group_row;                       // 0x404 overall
    u8 group_column;                    // 0x405 overall
    u8 group_member;                    // 0x406 overall
    u8 movement_target_direction;       // 0x147
    NUVEC terrain_origin;               // 0x148
    AIPATHINFO path_info;               // 0x154
    NUVEC last_path_position;           // 0x16c
    AIPATHNODE_s *goal_path_node;       // 0x178
    f32 movement_instruction_parameter; // 0x17c
    void *field_0x180;
    AIPATHCNX_s *movement_target;                // 0x184
    f32 antinode_timer;                          // 0x188
    AIPATHCNX_s *intersection_connection;        // 0x18c
    AIPATHCNX_s *intersection_target_connection; // 0x190
    NUVEC right_diversion;                       // 0x194
    NUVEC left_diversion;                        // 0x1a0
    AILOCATOR_s *respawn_locator;                // 0x1ac
    NUVEC fallback_destination;                  // 0x1b0
    f32 fallback_stopping_distance;              // 0x1bc
    f32 movement_parameter;                      // 0x1c0
    u8 pad_1c4[0x1c8 - 0x1c4];
    AIPATHINFO fallback_path_info; // 0x1c8
    union {
        u32 frame_flags; // 0x1e0
        NUVEC *movement_look_target;
    };
    union {
        u8 movement_flags; // 0x1e4
        u8 field_0x1e4;
    };
    u8 field_0x1e5;
    union {
        u8 runtime_flags;
        u8 field_0x1e6;
    };
    union {
        u8 movement_event_flags;
        u8 field_0x1e7;
    };
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
    AIPACKET_MOVEMENT_DIVERSION_RIGHT = 0x20,
    AIPACKET_MOVEMENT_DIVERSION_LEFT = 0x40,
};

enum AIPACKET_RUNTIME_FLAGS : u8 {
    AIPACKET_RUNTIME_INITIALISED = 0x01,
    AIPACKET_RUNTIME_SPECIAL_MOVE = 0x08,
    AIPACKET_RUNTIME_ROUTE_SELECTED = 0x10,
    AIPACKET_RUNTIME_PATH_BLOCKED = 0x20,
    AIPACKET_RUNTIME_USING_PATH_WAYPOINT = 0x40,
};

enum AIPACKET_MOVEMENT_SOURCE_FLAGS : u8 {
    AIPACKET_MOVEMENT_SOURCE_MASK = 0x0c,
    AIPACKET_MOVEMENT_SOURCE_CREATURE = 0x04,
    AIPACKET_MOVEMENT_SOURCE_LOCATOR = 0x08,
    AIPACKET_MOVEMENT_SOURCE_ACTIVE = 0x10,
    AIPACKET_PATH_CONNECTION_CHANGED = 0x20,
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
    union {
        struct {
            f32 pos_x;
            f32 pos_y;
            f32 pos_z;
        };
        NUVEC collision_position; // 0x80
    };
    NUVEC start_position;   // 0x8c
    NUVEC initial_position; // 0x98
    f32 scaled_radius;      // 0xa4
    f32 field_0xa8;         // 0xa8
    f32 collision_radius;   // 0xac
    f32 collision_height;   // 0xb0
    f32 scaled_height;      // 0xb4
    NUMTX field_0xb8;       // 0xb8
    NUMTX field_0xf8;       // 0xf8
    NUMTX field_0x138;      // 0x138
    NUVEC collision_min;    // 0x178
    NUVEC collision_max;    // 0x184
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
    union {
        struct {
            f32 field_0x1fc;
            f32 field_0x200;
            f32 field_0x204;
        };
        NUVEC movement_direction; // 0x1fc
    };
    undefined field_0x208[0x214 - 0x208];
    f32 field_0x214;
    f32 field_0x218;                                  // 0x218
    f32 water_height;                                 // 0x21c
    f32 field_0x220;                                  // 0x220
    f32 velocity_magnitude;                           // 0x224
    f32 horizontal_velocity_magnitude;                // 0x228
    f32 viewdistance;                                 // 0x22c
    f32 heardistance;                                 // 0x230
    f32 maxviewheight;                                // 0x234
    f32 minviewheight;                                // 0x238
    undefined field_0x23c[0x240 - 0x23c];             // 0x23c .. 0x240
    NUVEC previous_animation_root;                    // 0x240
    NUVEC previous_blend_target_root;                 // 0x24c
    f32 previous_animation_root_time;                 // 0x258
    f32 previous_blend_target_root_time;              // 0x25c
    CHARACTERANIM_s *previous_animation_root_info;    // 0x260
    CHARACTERANIM_s *previous_blend_target_root_info; // 0x264
    NUVEC animation_root_delta;                       // 0x268
    u16 pitch_angle;                                  // 0x274
    u16 field_0x276;                                  // 0x276
    u16 roll_angle;                                   // 0x278
    i16 supporting_platform_id;                       // 0x27a (-1 = no supporting character platform)
    char field_0x27c;                                 // 0x27c  player/character slot (0xff = none)
    u8 field_0x27d;                                   // 0x27d  terrain/contact flags
    u8 field_0x27e;                                   // 0x27e  previous terrain/contact flags
    u8 field_0x27f;                                   // 0x27f
    u8 field_0x280;                                   // 0x280
    u8 field_0x281;                                   // 0x281
    u8 is_underwater;                                 // 0x282
    u8 intersects_water;                              // 0x283
    u8 model_draw_result;                             // 0x284
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
    union {
        struct {
            u32 field_0x2a8;
            u32 field_0x2ac;
        };
        u64 ai_area_mask; // one bit per AIAREA occupied by this object
    };
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
    NUVEC contact_position;    // 0x0698
    NUVEC contact_normal;      // 0x06a4
    u8 field_0x6b0;            // 0x06b0 terrain/contact state
    u8 pad_6b1[0x6b4 - 0x6b1]; // 0x06b1 .. 0x06b4
    union {
        u8 player_packet[0x780 - 0x6b4]; // 0x06b4, PLAYERPACKET_s begins here
        CHARACTER_SHADOW_s character_shadows[5];
        struct {
            u8 player_packet_prefix[0x768 - 0x6b4];
            f32 field_0x768; // 0x0768
            union {
                f32 context_animation_timer; // 0x076c
                f32 drop_transition_time;
            };
            f32 field_0x770; // 0x0770
            union {
                f32 airborne_action_duration; // 0x0774
                f32 drop_transition_duration;
            };
            f32 airborne_action_timer; // 0x0778
            f32 jump_start_height;     // 0x077c
        };
    };
    void *field_0x780;            // 0x0780
    u8 pad_784[0x788 - 0x784];    // 0x0784 .. 0x0788
    void *field_0x788;            // 0x0788
    u8 pad_78c[0x79a - 0x78c];    // 0x078c .. 0x079a
    i16 context_animation;        // 0x079a, action-owned animation index
    i16 queued_context_animation; // 0x079c, base action used by combo branches
    u8 combo_branch;              // 0x079e, selected offset from the base combo action
    u8 pad_79f[0x7a0 - 0x79f];    // 0x079f .. 0x07a0
    u8 combo_input_latched;       // 0x07a0
    u8 pad_7a1[0x7a2 - 0x7a1];    // 0x07a1 .. 0x07a2
    u8 combo_stage;               // 0x07a2
    u8 field_0x7a3;               // 0x07a3, affects camera-look stick selection
    union {
        struct {
            u8 build_button_taps; // 0x07a4, capped Build-It acceleration input
            union {
                u8 field_0x7a5;
                u8 character_id_0x7a5; // character selector used by ObjLookingWithLeftStick
                i8 character_context;  // 0x07a5, current action owner (-1 when unowned)
                i8 build_context;      // Build-It alias retained for its existing callers
            };
            u8 pad_7a6[0x7a8 - 0x7a6]; // 0x07a6 .. 0x07a8
        };
        u32 movement_context_state; // 0x07a4, packed action context and variant state
    };
    u8 action_movement_state;  // 0x07a8
    u8 jump_sequence;          // 0x07a9, 1 for the first jump and 2 for the second
    u8 pad_7aa[0x7ac - 0x7aa]; // 0x07aa .. 0x07ac
    u8 context_flags;          // 0x07ac
    i8 context_variant_flags;  // 0x07ad
    u8 jump_flags;             // 0x07ae
    u8 pad_7af[0x7c0 - 0x7af]; // 0x07af .. 0x07c0
    union {
        u8 mini_anim_packet[0x24]; // 0x07c0 .. 0x07e4
        MINIANIMPACKET_s mini_animation;
    };
    u8 pad_7e4[0x7ec - 0x7e4];             // 0x07e4 .. 0x07ec
    COINPACKET *coinpacket;                // 0x07ec
    GizForceLOSState_s *gizforce_los_info; // 0x07f0
    union {
        NUMTX joint_matrices[16]; // 0x07f4 .. 0x0bf4
        struct {
            NUMTX field_0x7f4;
            NUMTX remaining_joint_matrices[15];
        };
    };
    u8 pad_bf4[0xc34 - 0xbf4];       // 0x0bf4 .. 0x0c34
    u32 field_0xc34;                 // 0x0c34
    f32 field_0xc38;                 // 0x0c38
    u8 pad_c3c[0xc54 - 0xc3c];       // 0x0c3c .. 0x0c54
    f32 field_0xc54;                 // 0x0c54
    u8 pad_c58[0xc94 - 0xc58];       // 0x0c58 .. 0x0c94
    struct GAMEPAD_s *pad_gamepad;   // 0x0c94  (originally inside PLAYERPACKET_s)
    SOCKPOSITION *oldpos;            // 0x0c98
    u8 pad_c9c[0xca8 - 0xc9c];       // 0x0c9c .. 0x0ca8
    void *field_0xca8;               // 0x0ca8
    void *suit;                      // 0x0cac
    void *batarang;                  // 0x0cb0
    TORPEDOPACKET *torpedo;          // 0x0cb4
    u8 pad_cb8[0xcc0 - 0xcb8];       // 0x0cb8 .. 0x0cc0
    GameObject_s *field_0xcc0;       // 0x0cc0
    u8 pad_cc4[0xd10 - 0xcc4];       // 0x0cc4 .. 0x0d10
    f32 airborne_reset_timer;        // 0x0d10
    u32 field_0xd14;                 // 0x0d14
    f32 ground_contact_grace_timer;  // 0x0d18, keeps airborne animation briefly after contact
    f32 jump_reentry_timer;          // 0x0d1c
    f32 airborne_input_timer;        // 0x0d20
    f32 field_0xd24;                 // 0x0d24
    u8 pad_d28[0xd30 - 0xd28];       // 0x0d28 .. 0x0d30
    f32 weapon_scale;                // 0x0d30, 0.0 retracted .. 1.0 extended
    f32 weapon_scale_rate;           // 0x0d34
    u8 pad_d38[0xd3c - 0xd38];       // 0x0d38 .. 0x0d3c
    f32 weapon_out_timer;            // 0x0d3c
    f32 delayed_turn_timer;          // 0x0d40
    f32 combo_input_timer;           // 0x0d44
    u8 pad_d48[0xd5c - 0xd48];       // 0x0d48 .. 0x0d5c
    i32 pause_input_state;           // 0x0d5c, cleared when entering pause
    u8 pad_d60[0xd64 - 0xd60];       // 0x0d60 .. 0x0d64
    f32 jump_variant_timer;          // 0x0d64
    f32 jump_chain_timer;            // 0x0d68
    f32 field_0xd6c;                 // 0x0d6c  surface/contact state
    u8 pad_d70[0xd78 - 0xd70];       // 0x0d70 .. 0x0d78
    f32 field_0xd78;                 // 0x0d78
    f32 terrain_origin_floor_offset; // 0x0d7c
    f32 field_0xd80;                 // 0x0d80
    u8 pad_d84[0xd8c - 0xd84];       // 0x0d84 .. 0x0d8c
    f32 field_0xd8c;                 // 0x0d8c
    u8 pad_d90[0xda0 - 0xd90];       // 0x0d90 .. 0x0da0
    f32 nearby_floor_distance;       // 0x0da0, sentinel when no floor is nearby
    f32 input_toggle_hold_time;      // 0x0da4, reset when entering pause
    f32 field_0xda8;                 // 0x0da8
    f32 fall_animation_timer;        // 0x0dac
    u8 pad_db0[0xdbc - 0xdb0];       // 0x0db0 .. 0x0dbc
    f32 field_0xdbc;                 // 0x0dbc
    u8 pad_dc0[0xdc8 - 0xdc0];       // 0x0dc0 .. 0x0dc8
    f32 field_0xdc8;                 // 0x0dc8
    u8 pad_dcc[0xde0 - 0xdcc];       // 0x0dcc .. 0x0de0
    f32 field_0xde0;                 // 0x0de0
    u8 pad_de4[0xdec - 0xde4];       // 0x0de4 .. 0x0dec
    f32 field_0xdec;                 // 0x0dec
    u8 pad_df0[0xdf8 - 0xdf0];       // 0x0df0 .. 0x0df8
    NUVEC reset_velocity;            // 0x0df8 .. 0x0e04
    u8 pad_e04[0xe08 - 0xe04];       // 0x0e04 .. 0x0e08
    u16 delayed_turn_target_angle;   // 0x0e08
    u8 pad_e0a[0xe0c - 0xe0a];       // 0x0e0a .. 0x0e0c
    u16 current_input_angle;         // 0x0e0c
    u8 pad_e0e[0xe18 - 0xe0e];       // 0x0e0e .. 0x0e18
    i16 movement_lean_angle;         // 0x0e18
    i16 secondary_lean_angle;        // 0x0e1a
    i16 tertiary_lean_angle;         // 0x0e1c
    i16 field_0xe1e;                 // 0x0e1e
    union {
        struct {
            u8 field_0xe20;
            u8 field_0xe21;
        };
        u16 action_flags; // 0x0e20
    };
    u8 field_0xe22;            // 0x0e22
    u8 field_0xe23;            // 0x0e23
    u8 field_0xe24;            // 0x0e24
    u8 movement_runtime_flags; // 0x0e25
    u8 pad_e26[0xe31 - 0xe26]; // 0x0e26 .. 0x0e31
    u8 field_0xe31;            // 0x0e31
    union {
        u8 field_0xe32;
        WEAPON_SCALE_STATE weapon_scale_state; // 0x0e32
    };
    u8 pad_e33[0xe36 - 0xe33];                  // 0x0e33 .. 0x0e36
    u8 field_0xe36;                             // 0x0e36
    u8 field_0xe37;                             // 0x0e37
    u8 field_0xe38;                             // 0x0e38
    u8 pad_e39[0xe40 - 0xe39];                  // 0x0e39 .. 0x0e40
    u8 combo_alternate;                         // 0x0e40, alternates the opening saber action
    u8 field_0xe41;                             // 0x0e41  current surface type
    u8 pad_e42[0xe50 - 0xe42];                  // 0x0e42 .. 0x0e50
    MechObjectInterface *mech_object_interface; // 0x0e50
    GAMEOBJECTADDONS_s *addons;                 // 0x0e54
    u8 pad_e58[0xe70 - 0xe58];                  // 0x0e58 .. 0x0e70
    nugspline_s *movement_spline;               // 0x0e70
    u8 pad_e74[0xeb4 - 0xe74];                  // 0x0e74 .. 0x0eb4
    u32 field_0xeb4;                            // 0x0eb4, cleared on hub room changes
    NUVEC *context_target_position;             // 0x0eb8
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
    u8 pad_ef0[0xef4 - 0xef0];                  // 0x0ef0 .. 0x0ef4
    i32 pause_context_state;                    // 0x0ef4, cleared when entering pause
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
    union {
        u8 field_0xf04;
        u8 jump_input_flags; // 0x0f04, buffered airborne action inputs
    };
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
    f32 collision_y_scale; // 0x1010
    u8 pad_1014[0x1018 - 0x1014];
    f32 field_0x1018; // 0x1018
    f32 field_0x101c; // 0x101c
    f32 field_0x1020; // 0x1020
    u8 pad_1024[4];
    f32 ai_update_distance;       // 0x1028, distance used to select the staggered AI cadence
    f32 shadow_opacity;           // 0x102c
    f32 shadow_radius;            // 0x1030
    u8 pad_1034[0x104c - 0x1034]; // 0x1034 .. 0x104c
    CABLE_s *cable;               // 0x104c
    u32 field_0x1050;             // 0x1050
    u32 field_0x1054;             // 0x1054
    u8 pad_1058[0x105c - 0x1058]; // 0x1058 .. 0x105c
    u16 field_0x105c;             // 0x105c terrain query flags
    u16 field_0x105e;             // 0x105e surface x rotation
    u16 field_0x1060;             // 0x1060 surface z rotation
    u16 field_0x1062;             // 0x1062 previous surface x rotation
    u16 field_0x1064;             // 0x1064 previous surface z rotation
    u16 shadow_joint_mask;        // 0x1066
    u16 field_0x1068;             // 0x1068 reflection x rotation
    u16 field_0x106a;             // 0x106a reflection z rotation
    u16 previous_movement_angle;  // 0x106c
    u16 field_0x106e;             // 0x106e
    i16 id;                       // 0x1070
    u8 pad_1072[0x1076 - 0x1072];
    i16 room_id;      // 0x1076, portal room containing the character
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
DECOMP_ASSERT(offsetof(AIPACKET, movement_target_direction) == 0x147, "AIPACKET target direction offset");
DECOMP_ASSERT(offsetof(AIPACKET, movement_target) == 0x184, "AIPACKET movement target offset");
DECOMP_ASSERT(offsetof(AIPACKET, intersection_connection) == 0x18c, "AIPACKET intersection connection offset");
DECOMP_ASSERT(offsetof(AIPACKET, intersection_target_connection) == 0x190, "AIPACKET intersection target offset");
DECOMP_ASSERT(offsetof(AIPACKET, right_diversion) == 0x194, "AIPACKET right diversion offset");
DECOMP_ASSERT(offsetof(AIPACKET, left_diversion) == 0x1a0, "AIPACKET left diversion offset");
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
DECOMP_ASSERT(offsetof(APIOBJECT, previous_animation_root) == 0x240, "APIOBJECT previous animation root offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_animation_root_time) == 0x258,
              "APIOBJECT previous animation root time offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_blend_target_root) == 0x24c, "APIOBJECT previous blend-target root offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_blend_target_root_time) == 0x25c,
              "APIOBJECT previous blend-target root time offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_animation_root_info) == 0x260,
              "APIOBJECT previous animation root-info offset");
DECOMP_ASSERT(offsetof(APIOBJECT, previous_blend_target_root_info) == 0x264,
              "APIOBJECT previous blend-target root-info offset");
DECOMP_ASSERT(offsetof(APIOBJECT, animation_root_delta) == 0x268, "APIOBJECT animation root delta offset");
DECOMP_ASSERT(offsetof(APIOBJECT, movement_direction) == 0x1fc, "APIOBJECT movement direction offset");
DECOMP_ASSERT(offsetof(APIOBJECT, collision_position) == 0x80, "APIOBJECT collision position offset");
DECOMP_ASSERT(offsetof(APIOBJECT, pitch_angle) == 0x274, "APIOBJECT pitch angle offset");
DECOMP_ASSERT(offsetof(APIOBJECT, supporting_platform_id) == 0x27a, "APIOBJECT supporting platform id offset");
DECOMP_ASSERT(offsetof(APIOBJECT, model_draw_result) == 0x284, "APIOBJECT model draw result offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_area_mask) == 0x2a8, "GameObject AI area mask offset");
DECOMP_ASSERT(offsetof(GameObject_s, sock_position) == 0x660, "GameObject socket position offset");
DECOMP_ASSERT(offsetof(GameObject_s, contact_position) == 0x698, "GameObject contact position offset");
DECOMP_ASSERT(offsetof(GameObject_s, contact_normal) == 0x6a4, "GameObject contact normal offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x6b0) == 0x6b0, "GameObject terrain contact state offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x780) == 0x780, "GameObject field_0x780 offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x788) == 0x788, "GameObject field_0x788 offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_animation) == 0x79a, "GameObject context animation offset");
DECOMP_ASSERT(offsetof(GameObject_s, queued_context_animation) == 0x79c, "GameObject queued context animation offset");
DECOMP_ASSERT(offsetof(GameObject_s, combo_branch) == 0x79e, "GameObject combo branch offset");
DECOMP_ASSERT(offsetof(GameObject_s, combo_input_latched) == 0x7a0, "GameObject combo input latch offset");
DECOMP_ASSERT(offsetof(GameObject_s, combo_stage) == 0x7a2, "GameObject combo stage offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x7a3) == 0x7a3, "GameObject camera-look selector offset");
DECOMP_ASSERT(offsetof(GameObject_s, build_button_taps) == 0x7a4, "GameObject Build-It tap count offset");
DECOMP_ASSERT(offsetof(GameObject_s, movement_context_state) == 0x7a4, "GameObject packed movement context offset");
DECOMP_ASSERT(offsetof(GameObject_s, build_context) == 0x7a5, "GameObject Build-It context offset");
DECOMP_ASSERT(offsetof(GameObject_s, character_context) == 0x7a5, "GameObject character context offset");
DECOMP_ASSERT(offsetof(GameObject_s, action_movement_state) == 0x7a8, "GameObject action movement state offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_flags) == 0x7ac, "GameObject context flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_variant_flags) == 0x7ad, "GameObject context variant flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, mini_anim_packet) == 0x7c0, "GameObject mini animation packet offset");
DECOMP_ASSERT(offsetof(GameObject_s, joint_matrices) == 0x7f4, "GameObject joint-matrix offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xcc0) == 0xcc0, "GameObject linked object offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xca8) == 0xca8, "GameObject field_0xca8 offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xd14) == 0xd14, "GameObject input state offset");
DECOMP_ASSERT(offsetof(GameObject_s, ground_contact_grace_timer) == 0xd18,
              "GameObject ground-contact grace timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, weapon_scale) == 0xd30, "GameObject weapon scale offset");
DECOMP_ASSERT(offsetof(GameObject_s, weapon_scale_rate) == 0xd34, "GameObject weapon scale rate offset");
DECOMP_ASSERT(offsetof(GameObject_s, weapon_out_timer) == 0xd3c, "GameObject weapon timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, combo_input_timer) == 0xd44, "GameObject combo input timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, reset_velocity) == 0xdf8, "GameObject reset velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, delayed_turn_timer) == 0xd40, "GameObject delayed turn timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, pause_input_state) == 0xd5c, "GameObject pause input state offset");
DECOMP_ASSERT(offsetof(GameObject_s, input_toggle_hold_time) == 0xda4, "GameObject toggle hold time offset");
DECOMP_ASSERT(offsetof(GameObject_s, nearby_floor_distance) == 0xda0, "GameObject nearby-floor offset");
DECOMP_ASSERT(offsetof(GameObject_s, fall_animation_timer) == 0xdac, "GameObject fall animation timer offset");
DECOMP_ASSERT(offsetof(GameObject_s, pause_context_state) == 0xef4, "GameObject pause context state offset");
DECOMP_ASSERT(offsetof(GameObject_s, delayed_turn_target_angle) == 0xe08, "GameObject delayed turn target offset");
DECOMP_ASSERT(offsetof(GameObject_s, current_input_angle) == 0xe0c, "GameObject input angle offset");
DECOMP_ASSERT(offsetof(GameObject_s, movement_lean_angle) == 0xe18, "GameObject movement lean offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xe1e) == 0xe1e, "GameObject force glow index offset");
DECOMP_ASSERT(offsetof(GameObject_s, weapon_scale_state) == 0xe32, "GameObject weapon scale state offset");
DECOMP_ASSERT(offsetof(GameObject_s, combo_alternate) == 0xe40, "GameObject alternate combo offset");
DECOMP_ASSERT(offsetof(GameObject_s, context_target_position) == 0xeb8, "GameObject context target offset");
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
DECOMP_ASSERT(offsetof(GameObject_s, collision_y_scale) == 0x1010, "GameObject collision Y scale offset");
DECOMP_ASSERT(offsetof(GameObject_s, cable) == 0x104c, "GameObject cable offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x105c) == 0x105c, "GameObject terrain flags offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1084) == 0x1084, "GameObject surface reset flag offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x1088) == 0x1088, "GameObject matrix selector offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0x10b8) == 0x10b8, "GameObject snake data offset");
DECOMP_ASSERT(offsetof(GameObject_s, move_override) == 0x10bc, "GameObject movement override offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_elapsed_time) == 0x10c4, "GameObject AI elapsed-time offset");
DECOMP_ASSERT(offsetof(GameObject_s, ai_update_distance) == 0x1028, "GameObject AI update-distance offset");
DECOMP_ASSERT(offsetof(GameObject_s, character_shadows) == 0x6b4, "GameObject character-shadow offset");
DECOMP_ASSERT(offsetof(GameObject_s, shadow_opacity) == 0x102c, "GameObject shadow-opacity offset");
DECOMP_ASSERT(offsetof(GameObject_s, shadow_radius) == 0x1030, "GameObject shadow-radius offset");
DECOMP_ASSERT(offsetof(GameObject_s, shadow_joint_mask) == 0x1066, "GameObject shadow-joint-mask offset");
DECOMP_ASSERT(offsetof(GameObject_s, vertical_velocity) == 0x10d4, "GameObject vertical velocity offset");
DECOMP_ASSERT(offsetof(GameObject_s, field_0xd24) == 0xd24, "GameObject model-origin state offset");
DECOMP_ASSERT(offsetof(GameObject_s, terrain_origin_floor_offset) == 0xd7c, "GameObject terrain-origin floor offset");
DECOMP_ASSERT(offsetof(GameObject_s, gizforce_target) == 0x10d8, "GameObject Force target offset");
DECOMP_ASSERT(offsetof(GameObject_s, gizforce_target_object) == 0x10dc, "GameObject Force object target offset");

typedef struct GameObject_s GameObject_s;
