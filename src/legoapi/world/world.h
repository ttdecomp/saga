#pragma once

#include "legoapi/gizmos/trigger/ai.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"

struct GIZFORCESYS_s;
struct GIZOBSTACLESYS_s;

struct AREADATA_s;
struct LEVEL_PROGRESS_s;
struct APIDEBRISSYS_s;
struct PARTDEBSYS_s;
struct GAMEANIMSYS_s;
struct SOCKSYS;
struct APIOBJECTSYS_s;
struct burnset_s;
struct rtlset;
struct CUTSYS;
struct GIZMOSYS_s;
struct GIZFLOW_s;
struct CHARPLATFORMSYS_s;
struct AISYS_s;
struct AIPATHCNXCONTROLSYS_s;
struct AIPATHCNXHELPERSYS_s;
struct AITRIGGERSETSYS_s;
struct CLIMBOBJECTSYS_s;
struct MechAutoJumpManager;
struct DOOR_s;
struct GIZOBSTACLESYS_s;
struct GIZTURRETSYS_s;
struct GRABBER_s;
struct PULSESYS_s;
struct LEVER_s;
struct SIGNAL_s;
struct GIZPANELSYS_s;
struct HATMACHINESYS_s;
struct TECHNO_s;
struct SECURITYDOOR_s;
struct GIZRANDOMSYS_s;
struct GIZSPECIALSYS_s;
struct GAMEANTINODESYS_s;
struct GIZBOMBGENSYS_s;
struct TRAFFICANIMSYS_s;
struct GIZTIMER_s;
struct TIMER_s;
struct LEVEL_OBJECT_RUNTIME_s;

// A portal-position spline stores camera/player points as position/target
// pairs (six floats per point).  MoveGameCamera uses portal_places[2] for
// the title-screen camera.
typedef struct portalpos_s {
    i16 count;
    i16 pad_02;
    f32 *weights;
    f32 *positions;
} PORTALPOS;

typedef struct MINIKIT {
    void *gscn;
    char filler[0x14];
    i32 field_0x18;
} MINIKIT;

// Layout matches the original WORLDINFO_s (0x51b0 = 20912 bytes).
// Field offsets verified against the original binary disassembly.
typedef struct WORLDINFO_s {
    char name[0x80]; // 0x000  level name / path scratch (Reset builds "levels\\..." here)

    char config_file[0x80]; // 0x080  level config path/name
    void *buffer_start;     // 0x100  start of this world's bump buffer (saved across Reset)

    VARIPTR giz_buffer;   // 0x104
    VARIPTR unknown_0108; // 0x108  end of the bump buffer
    i32 config_count;     // 0x10c  level config keyword count / gate

    i32 loaded; // 0x110  set to 1 at end of WorldInfo_Load

    char filler1[0x8]; // 0x114 .. 0x11c

    i32 level_idx;    // 0x11c
    i32 level_sub_id; // 0x120  from LEVELDATA.unknown_0af
    i32 area_sub_id;  // 0x124  from AREADATA.episode_index

    struct LEVELDATA_s *current_level; // 0x128
    AREADATA_s *area;                  // 0x12c
    LEVEL_PROGRESS_s *level_progress;  // 0x130
    APIDEBRISSYS_s *debris_sys;        // 0x134
    PARTDEBSYS_s *part_debris_sys;     // 0x138
    NUGSCN *current_gscn;              // 0x13c
    NUGSCN *scene;                     // 0x140

    f32 level_min[3];         // 0x144  CalculateWorldSize output
    f32 level_max[3];         // 0x150
    u8 progress_data[0x2800]; // 0x15c .. 0x295c  level progress store

    void *terrain; // 0x295c  LoadTerrainFile result

    GAMEANIMSYS_s *game_anim_sys;   // 0x2960
    NUGSCN *icons_gscn;             // 0x2964
    MINIKIT minikit;                // 0x2968
    void *minikit_pieces_buf;       // 0x2984
    struct SOCKSYS *sock_sys;       // 0x2988
    APIOBJECTSYS_s *api_object_sys; // 0x298c

    u8 room_visibility_flag; // 0x2990
    u8 rooms_visible[0x100]; // 0x2991 .. 0x2a91
    u8 *rooms_visible_ptr;   // 0x2a94

    i32 page_pp;                        // 0x2a98  edpp page handle
    i32 page_part;                      // 0x2a9c  edpart page handle
    i32 page_anim;                      // 0x2aa0  edanim page handle
    i32 page_grass;                     // 0x2aa4  edgra page handle
    i32 page_bridge;                    // 0x2aa8  edbri page handle
    burnset_s *burnset;                 // 0x2aac
    CUTSYS *cutscene_sys;               // 0x2ab0
    rtlset *rtl_set;                    // 0x2ab4  level real-time-light set
    i32 rtl_id;                         // 0x2ab8  rtlFindByUserId result
    void *light_dir;                    // 0x2abc  rtlGetDirection out-pointer
    LEVEL_OBJECT_RUNTIME_s *lev_objs;   // 0x2ac0  level-object array
    struct portalpos_s **portal_places; // 0x2ac4

    GIZMOSYS_s *gizmo_sys; // 0x2ac8
    GIZFLOW_s *giz_flow;   // 0x2acc

    char filler4[0x2adc - 0x2ad0]; // 0x2ad0 .. 0x2adc

    CHARPLATFORMSYS_s *char_platform_sys; // 0x2adc

    char filler5a[0x2ae4 - 0x2ae0]; // 0x2ae0 .. 0x2ae4
    i32 ai_loaded;                  // 0x2ae4
    AISYS_s *ai_sys;                // 0x2ae8
    i32 processor_count;
    LEVELSCRIPTPROCESS processors[32];

    AIPATHCNXCONTROLSYS_s *ai_path_cnx_control_sys;
    AIPATHCNXHELPERSYS_s *ai_path_cnx_helper_sys;
    AITRIGGERSETSYS_s *ai_trigger_set_sys;
    CLIMBOBJECTSYS_s *climb_object_sys;
    MechAutoJumpManager *mech_auto_jump_manager;

    char filler6a[0x469c - 0x4684];     // 0x4684 .. 0x469c
    DOOR_s *doors;                      // 0x469c
    i32 door_count;                     // 0x46a0
    DOOR_s *start_door;                 // 0x46a4
    GIZOBSTACLESYS_s *giz_obstacle_sys; // 0x46a8
    GIZTURRETSYS_s *giz_turret_sys;     // 0x46ac
    GIZFORCESYS_s *giz_force_sys;       // 0x46b0
    char filler7[0x46f0 - 0x46b4];      // 0x46b4 .. 0x46f0

    GRABBER_s *grabber; // 0x46f0

    char filler8[0x5038 - 0x46f4];
    void *faders;
    i32 fader_count;
    void *mini_trooper_packets;
    void *mini_trooper_teams;
    void *mini_trooper_storage;
    void *portal_doors;
    i32 portal_door_count;
    PULSESYS_s *pulses_sys;
    void *special_minikits;
    u8 reserved_505c[0x8];
    SIGNAL_s *signals;
    i32 signal_count;
    LEVER_s *levers;
    i32 nlevers;
    GIZPANELSYS_s *giz_panel_sys;
    HATMACHINESYS_s *hat_sys;
    TECHNO_s *techno_sys;
    i32 techno_count;
    void *grapples;
    i32 grapple_count;
    u8 reserved_508c[0x8];
    void *shards;
    i32 shard_count;
    void *attractos;
    i32 attracto_count;
    u8 reserved_50a4[0x8];
    void *ledges;
    i32 ledge_count;
    SECURITYDOOR_s *security;
    i32 security_count;
    void *pickup_sys;
    i32 blowup_type_count;
    i32 blowup_count;
    void *blowup_types;
    void *blowups;
    void *target_type;
    void *target;
    u8 reserved_50d8[0x14];
    void *matrices;
    u8 reserved_50f0[0xc];
    struct GIZTIMER_s *giz_timers;
    i32 giz_timers_count;
    void *torpedoes;
    GIZRANDOMSYS_s *giz_randoms;
    GIZSPECIALSYS_s *special_sys;
    GAMEANTINODESYS_s *game_antinode_sys;
    void *shadow_sys;
    GIZBOMBGENSYS_s *giz_bombgen_sys;
    i32 level_specific_allocated;
    union {
        void *level_specific_data;
        void *podrace;
    };
    void *customiser_parts[18];
    TRAFFICANIMSYS_s *trafficanim_sys; // 0x516c
    void *plugs;
    i32 progress_restored;
    u8 reserved_5178[0x38];
} WORLDINFO;

DECOMP_ASSERT(sizeof(WORLDINFO) == 0x51b0, "WORLDINFO size");
DECOMP_ASSERT(offsetof(WORLDINFO, pulses_sys) == 0x5054, "WORLDINFO pulse system offset");
DECOMP_ASSERT(offsetof(WORLDINFO, levers) == 0x506c, "WORLDINFO lever offset");
DECOMP_ASSERT(offsetof(WORLDINFO, giz_timers) == 0x50fc, "WORLDINFO timer offset");

extern void (*WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
extern void (*WorldInfo_InitLastFn)(WORLDINFO *);
extern TIMER_s LevelTimer;

#ifdef __cplusplus
extern "C" {
#endif
    extern WORLDINFO WorldInfo[];

    /// @brief Global pointer to the currently active world info
    extern WORLDINFO *WORLD;

    extern f32 g_BgLoadDelayHackTimer;

    extern i32 waiting_for_level;
    extern i32 level_already_loaded;
    extern i32 next_level;
#ifdef __cplusplus
}

void WorldInfo_InitOnce(void);

void WorldInfo_Init(WORLDINFO *world);

void WorldInfo_Load(WORLDINFO *world);
i32 WorldInfo_Reset(WORLDINFO *world, i32 level_idx);

void WorldInfo_Activate(void);
void WorldInfo_StreamLevel(BGPROCINFO *bg_info);

WORLDINFO *WorldInfo_CurrentlyActive(void);
WORLDINFO *WorldInfo_CurrentlyLoading(void);

i32 WorldInfo_OtherLevel(WORLDINFO *world);

void WorldInfo_Register(WORLDINFO *world);

void WorldInfo_ClearAllIfScreenFaded(void);

void WorldInfo_LoadObjectAnimFile(WORLDINFO *world);

void WorldInfo_DrawScene(WORLDINFO *world);

void WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param);
void WorldInfo_ReArrangeBuffers(i32 area1, i32 area2);

#endif
