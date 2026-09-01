#pragma once

#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nuvec.h"

struct AISCRIPTACTIONDEF_s;
struct AISCRIPTCONDITIONDEF_s;

typedef struct AIREFSCRIPT_s {
    NULISTLNK list_node;
    char *name;
    struct AISCRIPT_s *script;
    char *return_state_name;
    struct AISTATE_s *return_state;
    u32 check_global_scripts : 1;
    u32 check_level_scripts : 1;
    NULISTHDR conditions;
} AIREFSCRIPT;

typedef struct AISTATE_s {
    NULISTLNK list_node;
    NULISTHDR conditions;
    NULISTHDR actions;
    char *name;
    NULISTHDR ref_scripts;
} AISTATE;

typedef struct AIACTION_s {
    NULISTLNK list_node;
    char **params;
    i32 param_count;
    struct AISCRIPTACTIONDEF_s *def;
} AIACTION;

typedef struct AICONDITION_s {
    NULISTLNK list_node;
    f32 param_val;
    char type;
    i8 param_idx;
    u16 bool_and : 1;
    u16 keep_blocked : 1;
    u16 is_param_idx_valid : 1;
    u16 is_complex : 1;
    char *complex_arg;
    char *arg;
    void *void_arg;
    struct AISCRIPTCONDITIONDEF_s *def;
    char *next_state_name;
    AISTATE *next_state;
    struct AICONDITION_s *param_cond;
} AICONDITION;

enum AICONDITION_COMPARISON {
    AICONDITION_EQUAL = 0,
    AICONDITION_LESS_THAN = 1,
    AICONDITION_GREATER_THAN = 2,
    AICONDITION_LESS_THAN_OR_EQUAL = 3,
    AICONDITION_GREATER_THAN_OR_EQUAL = 4,
    AICONDITION_NOT_EQUAL = 5,
};

typedef struct AIACTIONMACRO_s {
    NULISTLNK list_node;
    char *name;
    NULISTHDR actions;
} AIACTIONMACRO;

typedef struct AICONDITIONMACRO_s {
    NULISTLNK list_node;
    char *name;
    NULISTHDR conditions;
} AICONDITIONMACRO;

typedef struct AISCRIPTPARAMS_s {
    char *name;
    f32 default_val;
} AISCRIPTPARAMS;

typedef struct AICONSTPARAMS_s {
    char name[32];
    f32 default_val;
} AICONSTPARAMS;

typedef struct AISCRIPT_s {
    NULISTLNK list_node;
    char *name;
    char *derived_from;
    NULISTHDR states;
    AISCRIPTPARAMS params[4];
    AISTATE *base_state;
    u32 is_level_script : 1;
    u32 is_derived : 1;
    u32 is_derived_from_level_script : 1;
    NULISTHDR ref_scripts;
    NULISTHDR condition_macros;
    NULISTHDR action_macros;
} AISCRIPT;

typedef struct AIPATHCNX_s {
    union {
        u32 traversal_flags[2];
        struct {
            u32 node_a;
            u32 node_b;
        };
    };
    union {
        u32 original_traversal_flags[2];
        struct {
            u32 previous_node_a;
            u32 previous_node_b;
        };
    };
    union {
        u8 node_indices[2];
        struct {
            u8 direction_a;
            u8 direction_b;
        };
    };
    union {
        i16 rotation;
        i16 flags;
    };
    union {
        u16 route_mask;
        i16 game_flags;
    };
    u8 open;
    u8 last_search_checksum;
    union {
        f32 distance;
        f32 width;
    };
    union {
        f32 horizontal_distance;
        f32 cost;
    };
    f32 max_horizontal_distance;
} AIPATHCNX;

typedef struct AIROUTE_s {
} AIROUTE;

typedef struct AIANTINODE_s AIANTINODE;

typedef struct AIPATHNODE_s {
    char *name;
    NUVEC position;
    f32 radius;
    f32 radius_squared;
    f32 min_height;
    f32 min_height_offset;
    f32 max_height;
    f32 max_height_offset;
    u8 connection_count;
    u8 flags;
    u8 has_special;
    u8 runtime_flags;
    i16 path_flags;
    u8 distance_cache_nodes[2];
    u8 special_type;
    u8 padding_0x31[3];
    AIPATHCNX **connections;
    f32 distance_cache[2];
    union {
        u8 special_handle[0xc];
        struct {
            void *special_scene;
            void *special;
            void *display_special;
        };
    };
    NUVEC special_position;
    i16 value_0x58;
    i16 value_0x5a;
} AIPATHNODE;

typedef struct AIPATHROUTE_s {
    char *name;
    u8 *node_routes;
    u8 *node_directions;
    u8 *characters;
    u8 route_count;
    u8 character_count;
    u8 padding_0x12[2];
    u8 **route_nodes;
    u32 character_mask[4];
} AIPATHROUTE;

typedef struct AIPATHSPECIALROUTE_s {
    u8 path_count;
    u8 padding_0x01[3];
    struct AIPATH_s **paths;
} AIPATHSPECIALROUTE;

typedef struct AIPATHNODELINK_s {
    u8 type;
    u8 padding_0x01;
    i16 node;
} AIPATHNODELINK;

typedef struct AIPATH_s {
    char name[0x10];
    u8 node_count;
    u8 flags;
    u16 connection_count;
    u8 route_count;
    u8 index;
    u8 padding_0x16[2];
    // Per-frame path bookkeeping. Dynamic special nodes are updated once per
    // bit, while characters mark the node volume they currently occupy.
    u8 updated_node_bits[0x40];
    u8 inside_node_bits[0x20];
    u8 search_checksum;
    u8 search_reset_node;
    u8 special_route_count;
    u8 padding_0x7b;
    AIPATHNODE *nodes;
    AIPATHCNX *connections;
    u8 **route_matrix;
    AIPATHROUTE *routes;
    AIPATHNODELINK *special_routes;
    NUVEC bounds_min;
    NUVEC bounds_max;
} AIPATH;

enum AIPATH_CONNECTION_FLAGS : u32 {
    // Bit 29 is connection metadata and does not request a character capability.
    AIPATH_CONNECTION_FLAG_NO_CAPABILITY_REQUIRED = 0x20000000u,
    AIPATH_CONNECTION_CAPABILITY_MASK = 0xdfffffffu,
    AIPATH_CONNECTION_SPECIAL_MASK = 0xd8000000u,
    // Recomputed when either endpoint is attached to a moving special.
    AIPATH_CONNECTION_FLAG_DYNAMIC_TOO_LONG = 0x08000000u,
    AIPATH_CONNECTION_FLAG_SPECIAL_UNAVAILABLE = 0x10000000u,
    // Endpoint flags with these high bits require the packet's route to be
    // selected again instead of accepting the connection directly.
    AIPATH_CONNECTION_FLAG_RESELECT_ROUTE = 0x98000000u,
};

typedef struct AIPATHSYS_s {
    u8 path_count;
    u8 padding_0x01;
    u16 special_route_count;
    AIPATH **paths;
    AIPATH *active_path;
    AIPATHSPECIALROUTE *special_routes;
} AIPATHSYS;

typedef struct AILOCATOR_s {
    char name[0x10];
    NUVEC position;
    i32 flags;
    AIPATH *path;
    AIPATHCNX *connection;
    u8 game_flags;
    u8 padding_0x29[7];
    f32 min_distance;
    f32 max_distance;
    i32 locator_flags;
} AILOCATOR;

typedef struct AILOCATORSET_s {
    char name[0x10];
    i8 locator_count;
    u8 padding_0x11[3];
    u8 *locator_entries;
    u8 *assigned;
} AILOCATORSET;

typedef struct AIAREA_s {
    char name[0x10];
    union {
        NUVEC position;
        struct {
            // Legacy loader aliases retained while the AI file parser is
            // reconstructed.  These three values are the area's origin.
            f32 min_x;
            f32 min_y;
            f32 min_z;
        };
    };
    union {
        struct {
            f32 half_width;
            f32 height;
            f32 half_depth;
        };
        struct {
            // Legacy loader aliases for the three local-space extents.
            f32 max_x;
            f32 max_y;
            f32 max_z;
        };
    };
    union {
        i16 rotation;
        i16 flags;
    };
    u8 runtime_flags;
    u8 game_flags;
    u8 padding_0x2c[8];
    struct AISYS_s *system;
    u8 padding_0x38[4];
} AIAREA;

enum AIAREA_RUNTIME_FLAGS : u8 {
    AIAREA_RUNTIME_PLAYER_PRESENT = 0x01,
    AIAREA_RUNTIME_OBJECT_STATE_CLEAR = 0x02,
    AIAREA_RUNTIME_OBJECT_STATE_SET = 0x04,
    AIAREA_RUNTIME_CHARACTER_SLOT_SEEN = 0x08,
};

typedef struct AISCRIPTPROCESSSTACK_s {
    f32 complex_params[4];
    u8 is_first_time_state;
    u8 force_complex_eval;
} AISCRIPTPROCESSSTACK;

typedef struct AISCRIPTPROCESS_s {
    AISCRIPT *base_script;
    AISCRIPT *script;

    AISTATE *state;
    NULISTLNK *action_node;
    AISTATE *next_state;
    f32 params[4];
    f32 script_timer;

    AISCRIPTPROCESSSTACK param_stack[2];

    u32 is_first_time_action : 1;
    u32 is_disabled : 1;
    u32 unknown_flag_4 : 1;

    AIREFSCRIPT *active_refs[4];
    i32 active_ref_count;

    u8 action_data_1;
    u8 action_data_2;
    u16 action_data_6;
    void *action_data_3;
    f32 action_data_4;
    f32 action_data_5;

    NUVEC action_pos;

    AIPATHINFO path_info;

    f32 action_timer;

    AIAREA *unknown_a0;
    AILOCATOR *unknown_a4;
    AILOCATORSET *unknown_a8;
    NUGSPLINE *unknown_ac;

    // Types uncertain.
    u8 unknown_b0;
    u16 unknown_b2;

    u8 interrupt_priority;
    u8 interrupt_id;

    u16 action_data_7;

    f32 interrupt_timer;
    AISTATE *interrupt_state;
    AISTATE *return_to_state;

    // Type uncertain.
    u32 unknown_c4;
} AISCRIPTPROCESS;

typedef struct AICREATURE_s {
    char name[0x10];

    char script_name[0x10];

    NUVEC pos;
    NUANG y_rot;

    AIPATHINFO path_info;

    i32 flags;

    u8 set;

    i16 type;

    u8 count;
    u8 count_across;

    u8 padding_0x52[6];

    u32 active_mask; // 0x58, one enabled bit per spawn entry

    f32 x_spacing;
    f32 z_spacing;

    f32 script_params[4];

    AIAREA *activate_area;
    AIAREA *area;

    AILOCATOR *locator;
    AILOCATOR *respawn_locator;

    u8 activation_difficulty;

    char min_respawn_count;
    char max_respawn_count;

    u8 activate_type;

    f32 min_respawn_time;
    f32 max_respawn_time;

    f32 start_stagger;

    f32 view_distance;
    f32 hear_distance;

    f32 max_view_height;
    f32 min_view_height;
} AICREATURE;

typedef struct AIROW_s {
    AIPATHINFO path_info;

    NUVEC pos;
    NUANG y_rot;

    AIPATHCNX *next_connection;
    u8 next_direction;

    u8 is_alive;

    char padding[2];

    u32 is_clockwise : 1;
    u32 is_turning : 1;
} AIROW;

typedef struct AIGROUP_s {
    APIOBJECT *leader;

    i16 rotation_speed;

    u8 row_count;
    u8 member_count;
    u8 count_across;

    APIOBJECT *members[16];

    i32 member_is_alive;

    u32 is_used : 1;
    u32 can_respawn : 1;
    u32 is_reversed : 1;
    u32 is_in_formation : 1;
    u32 is_row_turning : 1;

    AIROW rows[4];

    f32 radius;
    f32 x_spacing;
    f32 z_spacing;
    f32 max_speed;
} AIGROUP;

struct AIANTINODE_s {
    NUVEC position;
    f32 radius;
    f32 height;
    f32 width;
    f32 max_height;
    f32 min_height;
    union {
        u8 special_handle[0xc];
        struct {
            void *special_scene;
            void *special;
            void *display_special;
        };
    };
    NUVEC special_position;
    i32 flags;
    i32 rotation_offset;
    f32 base_radius;
    f32 base_height;
    u8 enabled;
    u8 game_flags;
    u8 type;
    u8 has_special;
    u8 special_type;
    u8 padding_0x4d[7];
};

enum AIPATHNODE_RUNTIME_FLAGS : u8 {
    AIPATHNODE_RUNTIME_POSITION_CHANGED = 0x02,
    AIPATHNODE_RUNTIME_SPECIAL_UNAVAILABLE = 0x04,
};

typedef struct AISYS_s {
    void *storage;
    VARIPTR storage_end;
    VARIPTR storage_cursor;
    i32 storage_size;

    AIPATHSYS *path_sys;

    i32 creature_count;
    AICREATURE *creatures;

    NULISTHDR scripts;

    i32 locator_count;
    AILOCATOR *locators;

    i32 locator_set_count;
    AILOCATORSET *locator_sets;

    i32 area_count;
    AIAREA *areas;

    AIGROUP groups[16];

    i32 antinode_count;
    AIANTINODE *antinodes;

    u8 next_area_check;
    u8 goody_idx;

    i16 has_done_reset : 1;
    i16 unknown_flag_2 : 1;
    i16 unknown_flag_4 : 1;

    // The AI system for game-specific logic.
    void *game_sys;

    APIOBJECT *player_1;
    APIOBJECT *player_2;

    NUGSCN *scene;
} AISYS;

DECOMP_ASSERT(sizeof(AICREATURE) == 0xa4, "AICREATURE size");
DECOMP_ASSERT(sizeof(AIAREA) == 0x3c, "AIAREA size");
DECOMP_ASSERT(offsetof(AIAREA, runtime_flags) == 0x2a, "AIAREA runtime flags offset");
DECOMP_ASSERT(sizeof(AILOCATOR) == 0x3c, "AILOCATOR size");
DECOMP_ASSERT(sizeof(AILOCATORSET) == 0x1c, "AILOCATORSET size");
DECOMP_ASSERT(sizeof(AIPATHCNX) == 0x24, "AIPATHCNX size");
DECOMP_ASSERT(sizeof(AIPATHNODE) == 0x5c, "AIPATHNODE size");
DECOMP_ASSERT(offsetof(AIPATHNODE, runtime_flags) == 0x2b, "AIPATHNODE runtime flags offset");
DECOMP_ASSERT(offsetof(AIPATHNODE, special_handle) == 0x40, "AIPATHNODE special handle offset");
DECOMP_ASSERT(sizeof(AIPATHROUTE) == 0x28, "AIPATHROUTE size");
DECOMP_ASSERT(sizeof(AIPATH) == 0xa8, "AIPATH size");
DECOMP_ASSERT(offsetof(AIPATH, updated_node_bits) == 0x18, "AIPATH updated-node bits offset");
DECOMP_ASSERT(offsetof(AIPATH, inside_node_bits) == 0x58, "AIPATH occupied-node bits offset");
DECOMP_ASSERT(sizeof(AIPATHSYS) == 0x10, "AIPATHSYS size");
DECOMP_ASSERT(sizeof(AIANTINODE) == 0x54, "AIANTINODE size");
DECOMP_ASSERT(offsetof(AIANTINODE, special_handle) == 0x20, "AIANTINODE special handle offset");
DECOMP_ASSERT(offsetof(AIANTINODE, rotation_offset) == 0x3c, "AIANTINODE rotation offset");
DECOMP_ASSERT(offsetof(AICREATURE, type) == 0x4e, "AICREATURE type offset");
DECOMP_ASSERT(offsetof(AICREATURE, count) == 0x50, "AICREATURE count offset");
DECOMP_ASSERT(offsetof(AICREATURE, active_mask) == 0x58, "AICREATURE active-mask offset");
DECOMP_ASSERT(offsetof(AICREATURE, area) == 0x78, "AICREATURE area offset");
DECOMP_ASSERT(offsetof(AICREATURE, start_stagger) == 0x90, "AICREATURE stagger offset");
DECOMP_ASSERT(sizeof(AIGROUP) == 0x134, "AIGROUP size");
DECOMP_ASSERT(offsetof(AIGROUP, count_across) == 0x8, "AIGROUP count-across offset");
DECOMP_ASSERT(offsetof(AIGROUP, x_spacing) == 0x128, "AIGROUP spacing offset");
DECOMP_ASSERT(sizeof(AISYS) == 0x1398, "AISYS size");
DECOMP_ASSERT(offsetof(AISYS, creature_count) == 0x14, "AISYS creature-count offset");
DECOMP_ASSERT(offsetof(AISYS, creatures) == 0x18, "AISYS creatures offset");
DECOMP_ASSERT(offsetof(AISYS, groups) == 0x3c, "AISYS groups offset");
DECOMP_ASSERT(offsetof(AISYS, scene) == 0x1394, "AISYS scene offset");

typedef struct AIPACKET_s AIPACKET;

typedef i32 AIACTIONFN(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
typedef f32 AICONDITIONFN(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
typedef void *AICONDITIONINITFN(AISYS *, char *, AISCRIPT *);

typedef struct AISCRIPTACTIONDEF_s {
    char *name;
    AIACTIONFN *eval_fn;
    char unknown;
    char is_game_action;
    i16 idx;
} AIACTIONDEF;

typedef struct AISCRIPTCONDITIONDEF_s {
    char *name;
    AICONDITIONFN *eval_fn;
    AICONDITIONINITFN *init_fn;
} AICONDITIONDEF;

// Shared registry indices let each callback-owning translation unit install
// its local callbacks without changing their linkage.  Keep these values in
// lockstep with the table definitions.
enum AISCRIPT_REGISTRY_INDEX {
    API_AI_ACTION_IDLE = 0,
    API_AI_ACTION_RESET_TIMER = 2,
    API_AI_ACTION_GO_TO_LOCATOR = 35,
    API_AI_ACTION_FOLLOW_PATH = 38,
    API_AI_CONDITION_TIMER = 2,
    API_AI_CONDITION_RANDOM = 3,

    LEGO_AI_ACTION_SET_DOOMED_ESCAPE_LOCATOR = 11,
    LEGO_AI_ACTION_SNAP_TO_SOCK_POSITION = 13,
    LEGO_AI_ACTION_CAN_SHOOT_OFF_SCREEN = 17,
    LEGO_AI_ACTION_SET_BOLTS_DONT_GET_DEFLECTED_BACK = 22,
    LEGO_AI_ACTION_CAN_SHOOT_OBSTRUCTIONS = 23,
    LEGO_AI_ACTION_CAN_HIT_FORCE_OBJECTS = 32,
    LEGO_AI_ACTION_PLAYER_SPEEDER_HACK = 34,
    LEGO_AI_ACTION_CHAR_CLIP_TO_BLOB_SHADOWS = 47,
    LEGO_AI_ACTION_DEFLECT_PLAYERS_PART = 55,
    LEGO_AI_ACTION_SET_AI_OVERRIDE_CONTROL = 59,
    LEGO_AI_ACTION_SET_LAST_SAFE_PATH_POS = 60,
    LEGO_AI_ACTION_DONT_SET_STOPPED_FLAG = 62,
    LEGO_AI_ACTION_PRESS_SPECIAL_BUTTON = 63,
    LEGO_AI_ACTION_PRESS_ACTION_BUTTON = 65,
    LEGO_AI_ACTION_DONT_AVOID_CHARACTER = 69,
    LEGO_AI_ACTION_SET_ZERO_ACCELERATION = 74,
    LEGO_AI_ACTION_CREATE_CREATURES = 78,

    LEGO_AI_CONDITION_SPECIAL_AT_START = 0,
    LEGO_AI_CONDITION_NUM_IN_SET_ALIVE = 1,
    LEGO_AI_CONDITION_BEEN_TO_LEVEL = 2,
    LEGO_AI_CONDITION_MESSAGE = 3,
    LEGO_AI_CONDITION_CUT_SCENE_FINISHED = 4,
    LEGO_AI_CONDITION_FREEPLAY = 5,
    LEGO_AI_CONDITION_IS_LOW_END_DEVICE = 7,
    LEGO_AI_CONDITION_RANDOM_MAP_CHARS_AVAILABLE = 8,
    LEGO_AI_CONDITION_CHARACTER_LOADED = 9,
};

typedef i32 GAMEPARAMTOFLOAT(AIPACKET *, AISCRIPTPROCESS *, char *, f32 *);
typedef i32 AICHARACTERTYPEID(char *name);
// Character lookups return -1 when no entry exists.  This must remain a
// signed integer callback: the level-character lookup is also installed as
// the default special-route lookup during game AI initialisation.
typedef i32 AISPECIALROUTECHARACTERTYPEID(char *name);
typedef f32 AICHARACTERDISTANCE(i32 character_type);
typedef void GAMEAILOAD(AISYS *system, i32 version, NUGSCN *scene, VARIPTR *buf, VARIPTR *buf_end);
typedef i32 AIACTIONPARSESPEED(char *name, u8 *speed);
typedef void SCRIPTPROCESSFIRSTTIMEACTION(AISYS *, AIPACKET *, AISCRIPTPROCESS *);
typedef u32 AIBIGJUMPTODESTINATION(APIOBJECT *object, NUVEC *destination);
typedef u32 AIRESPAWNONPATH(APIOBJECT *object);
typedef void AICLEARCREATURES(void);
typedef APIOBJECT *APIOBJECTFROMOBJID(u8 object_id);
typedef i32 AIFINDALTERNATIVESPECIALOBJECT(AISYS *system, struct nuhspecial_s *special);
typedef APIOBJECT *AIGETNAMEDAPIOBJECT(AISYS *system, char *name);
typedef NUVEC *AIGETCREATUREORIGIN(AISYS *system, AIPACKET *packet);

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 AiParseExpressionFailed;

    extern AICONDITIONDEF api_aiconditiondefs[];
    extern AICONDITIONDEF lego_aiconditiondefs[];

    extern AIACTIONDEF api_aiactiondefs[];
    extern AIACTIONDEF *game_aiactiondefs;
    extern AIACTIONDEF lego_aiactiondefs[];

    extern NULISTHDR global_aiscripts;

    extern i32 ai_usepackfile;

    extern GAMEPARAMTOFLOAT *GameParamToFloatFn;
    extern AICHARACTERTYPEID *GlobalCharacterTypeIDFn;
    extern AISPECIALROUTECHARACTERTYPEID *SpecialRouteCharacterTypeIDFn;
    extern AICHARACTERDISTANCE *GetViewRangeFn;
    extern AICHARACTERDISTANCE *GetHearDistanceFn;
    extern AICHARACTERDISTANCE *GetMaxViewHeightFn;
    extern AICHARACTERDISTANCE *GetMinViewHeightFn;
    extern GAMEAILOAD *GameAILoadFn;
    extern AIACTIONPARSESPEED *AIActionParseSpeedFn;
    extern SCRIPTPROCESSFIRSTTIMEACTION *ScriptProcessFirstTimeActionFn;
    extern AIBIGJUMPTODESTINATION *AIBigJumpToDestinationFn;
    extern AIRESPAWNONPATH *AIRespawnOnPathFn;
    extern AICLEARCREATURES *ClearAICreaturesFn;
    extern APIOBJECTFROMOBJID *APIOBJECTFromObjIDFn;
    extern AIFINDALTERNATIVESPECIALOBJECT *FindAlternativeSpecialObjectFn;
    extern AIGETNAMEDAPIOBJECT *GetNamedAPIObjectFn;
    extern AIGETCREATUREORIGIN *GetAICreatureOriginFn;
    extern char *AiLevelPathName;

    void InitFn_AIActionParseSpeed(AIACTIONPARSESPEED *function);
    void InitFn_AIBigJumpToDestination(AIBIGJUMPTODESTINATION *function);
    void InitFn_AIRespawnOnPath(AIRESPAWNONPATH *function);
    void InitFn_ScriptProcessFirstTimeAction(SCRIPTPROCESSFIRSTTIMEACTION *function);
    void InitFn_APIOBJECTFromObjIDFn(APIOBJECTFROMOBJID *function);
    void InitFn_ClearAICreatures(AICLEARCREATURES *function);
    void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *function);
    void InitFn_GameAILoad(GAMEAILOAD *function);
    void InitFn_GetAICreatureOrigin(AIGETCREATUREORIGIN *function);
    void InitFn_GetNamedAPIObject(AIGETNAMEDAPIOBJECT *function);

    void AIScriptLoadAll(char *path, VARIPTR *buf, VARIPTR *buf_end, AISYS *sys);
    void AIScriptLoadAllPakFile(void *pak, char *path, VARIPTR *buf, VARIPTR *buf_end, AISYS *sys);

    void AIScriptInitConditions(AISYS *sys);

    void AIScriptProcessorInit(AISYS *sys, AIPACKET *packet, AISCRIPTPROCESS *processor, AICREATURE *creature,
                               char *script_name, char *start_state_name, i32 can_use_default, AISCRIPT *script,
                               AISTATE *start_state);

    AISCRIPT *AIScriptFind(AISYS *sys, char *name, i32 can_use_default, i32 check_level_scripts,
                           i32 check_global_scripts);

    void AIScriptClearInterrupt(AISCRIPTPROCESS *processor, char *state_name);
    i32 AIScriptSetInterrupt(AISCRIPTPROCESS *processor, u8 priority, u8 id, char *state_name, f32 time);
    void AIScriptSetState(AISCRIPTPROCESS *processor, AISTATE *state);
    i32 AIScriptSetStateByName(AISCRIPTPROCESS *processor, char *name);
    i32 AIScriptSetBaseScriptStateByName(AISCRIPTPROCESS *processor, char *name);

    AISTATE *AIStateFind(char *name, AISCRIPT *script);

    void RegisterAIScriptActions(AIACTIONDEF *action_defs);
    void RegisterAIScriptConditions(AICONDITIONDEF *cond_defs);

    f32 AIParamToFloat(AISCRIPTPROCESS *processor, char *param);
    f32 AIParamToFloatEx(AIPACKET *packet, AISCRIPTPROCESS *processor, char *param);

    AIPATH *AISysFindPath(AISYS *system, char *name);
    AILOCATOR *AIPathFindLocator(AISYS *system, char *name);
    AILOCATORSET *AIPathFindLocatorSet(AISYS *system, char *name);
    void AILocatorSet_CheckLocatorsStillAssigned(AISYS *system, AILOCATORSET *locator_set);
    void AISysCharacterSetPath(AIPACKET *packet, AIPATH *path);
    void AISysCharacterSetPathCnx(AIPACKET *packet, NUVEC *position, AIPATHCNX *connection, i32 direction);
    i32 WithinConnection(AISYS *system, NUVEC *position, AIPATH *path, AIPATHCNX *connection, i32 checks,
                         AIPATHCNX *previous_connection, i32 route, i32 ground, AIPATHINFO *path_info, f32 radius,
                         i32 update_once);
    f32 AIPathNodeDistanceToPathNode(AIPATH *path, i32 start_node, i32 destination_node, i32 route,
                                     u32 excluded_route_mask);
    void AISysGetCharacterPathPos(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, i32 ground);
    void AISysUpdateCharacterPathPos(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, f32 elapsed);
    void AISysCharacterMovement(AISYS *system, AIPACKET *packet, APIOBJECT *object, i32 checks);
    void AISysProcessCharacter(AISYS *system, APIOBJECT *object, AIPACKET *packet, i32 checks, f32 elapsed,
                               i32 use_three_dimensions, i32 process_ai);
    void AISysProcess(AISYS *system, APIOBJECT *player_1, APIOBJECT *player_2);
    void AIMoveInstruction(AIPACKET *packet, NUVEC *destination, f32 stopping_distance, AIPATHINFO *path_info, i32 mode,
                           f32 movement_parameter);
    void AIScriptProcess(AISYS *system, APIOBJECT *object, AIPACKET *packet, AISCRIPTPROCESS *processor, f32 elapsed);
#ifdef __cplusplus
}
#endif

f32 AiParseExpression(char *expr);

void AIScriptOpenPakFileParse(AISCRIPT **script_ref, void *pak, char *filename, char *path, VARIPTR *buf,
                              VARIPTR *buf_end);
