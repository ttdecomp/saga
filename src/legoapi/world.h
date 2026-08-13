#pragma once

#include "legoapi/gizmos/ai.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/common.h"

struct AREADATA_s;
struct LEVEL_PROGRESS_s;
struct APIDEBRISSYS_s;
struct PARTDEBSYS_s;
struct GAMEANIMSYS_s;
struct SOCKSYS_s;
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
struct GAMEANTINODESYS_s;
struct GIZBOMBGENSYS_s;
struct TRAFFICANIMSYS_s;
struct GIZTIMER_s;
struct portalpos_s;
struct spawnsys_s;

typedef struct MINIKIT {
    void *gscn;
    char filler[0x14];
    i32 field_0x18;
} MINIKIT;

// Layout matches the original WORLDINFO_s (0x51b0 = 20912 bytes).
typedef struct WORLDINFO_s {
    char filler0[0x80];

    char config_file[0x84];

    VARIPTR giz_buffer;
    VARIPTR unknown_0108;
    i32 unknown_010c;

    i32 unknown_0110;

    char filler1[0x8];

    i32 unknown_011c;
    i32 unknown_0120;
    i32 unknown_0124;

    struct LEVELDATA_s *current_level;
    AREADATA_s *area;
    LEVEL_PROGRESS_s *level_progress;
    APIDEBRISSYS_s *field278_0x134;
    PARTDEBSYS_s *part_debris_sys;
    NUGSCN *current_gscn;
    NUGSCN *scene;

    char unknown_0140[0x2960 - 0x144];

    GAMEANIMSYS_s *game_anim_sys;
    NUGSCN *icons_gscn;
    MINIKIT minikit;
    struct SOCKSYS_s *sock_sys;
    APIOBJECTSYS_s *api_object_sys;

    char filler2[0x2a98 - 0x2990];

    i32 unknown_0x2a98;
    i32 unknown_0x2a9c;
    i32 unknown_0x2aa0;
    i32 unknown_0x2aa4;
    i32 unknown_0x2aa8;
    burnset_s *field_0x2aac;
    CUTSYS *cutscene_sys;
    rtlset *lights;
    i32 unknown_0x2ab8;

    char filler3a[0x2ac4 - 0x2abc];
    struct portalpos_s **portal_places;

    GIZMOSYS_s *gizmo_sys;
    GIZFLOW_s *giz_flow;

    char filler4[0x2adc - 0x2ad0];

    CHARPLATFORMSYS_s *char_platform_sys;

    char filler5[0x2ae8 - 0x2ae0];

    AISYS_s *ai_sys;
    i32 processor_count;
    LEVELSCRIPTPROCESS processors[32];

    AIPATHCNXCONTROLSYS_s *ai_path_cnx_control_sys;
    AIPATHCNXHELPERSYS_s *ai_path_cnx_helper_sys;
    AITRIGGERSETSYS_s *ai_trigger_set_sys;
    CLIMBOBJECTSYS_s *climb_object_sys;
    MechAutoJumpManager *mech_auto_jump_manager;

    char filler6a[0x46a4 - 0x4684];
    struct spawnsys_s *spawn_sys;
    char filler6b[0x46ac - 0x46a8];

    GIZTURRETSYS_s *giz_turret_sys;

    char filler7[0x46f0 - 0x46b0];

    GRABBER_s *grabber;

    char filler8[0x5054 - 0x46f4];

    PULSESYS_s *pulses_sys;

    char filler9[0x505c - 0x5058];

    LEVER_s *levers;
    i32 nlevers;

    char filler10[0x50cc - 0x5064];

    struct GIZTIMER_s *giz_timers;
    i32 giz_timers_count;

    char filler11[0x50d8 - 0x50d4];

    void *giz_randoms;

    char filler12[0x50e0 - 0x50dc];

    GAMEANTINODESYS_s *game_antinode_sys;

    char filler13[0x50e8 - 0x50e4];

    GIZBOMBGENSYS_s *giz_bombgen_sys;

    char filler14[0x516c - 0x50ec];

    TRAFFICANIMSYS_s *trafficanim_sys;

    char filler15[0x51b0 - 0x5170];
} WORLDINFO;

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
