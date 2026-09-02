#ifndef LEGOAPI_TYPES_H
#define LEGOAPI_TYPES_H
#pragma once

#include "decomp_assert.h"
#include "nu2api/nucore/fixed_width.h"
#include "decomp_assert.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nu3d/nuspecial.h"

#include "legoapi/items/base/apiobject.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "legoapi/characters/core/CharacterObjectInterface.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/gizmo/base/GizBuildItObjectInterface.h"
#include "legoapi/gizmo/base/GizForceObjectInterface.h"
#include "legoapi/gizmo/base/GizLeverObjectInterface.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "legoapi/gizmo/base/GizPanelObjectInterface.h"
#include "legoapi/gizmo/base/GizTurretObjectInterface.h"
#include "legoapi/gizmo/base/HatMachineObjectInterface.h"
#include "legoapi/gizmo/base/TeleportObjectInterface.h"
#include "legoapi/render/core/SwipeDecalRenderer.h"

struct ADDGAMEMSG;
struct ADDPART_s;
struct AIANTINODE_s;
struct AIAREA_s;
struct AIGROUP_s;
struct AILOCATOR_s;
struct AIPACKET_s;
struct AIPATHCNXCONTROLLER_s;
struct AIPATHCNXCONTROLSYS_s;
struct AIPATHCNXHELPERSYS_s;
struct AIPATHCNXHELPER_s;
struct AIPATHCNX_s;
struct AIPATHINFO_s;
struct AIPATHNODE_s;
struct AIPATH_s;
struct AISCRIPTPROCESS_s;
struct AISCRIPT_s;
struct AISYS_s;
struct AITRIGGERSETSYS_s;
struct AITRIGGERSET_s;
struct ANIMLIST_s;
struct ANIMPACKET_s;
struct ANIMREDIRECT;
struct APICHARACTERMODELLIST_s;
struct APIOBJECT_s;
struct AREADATA_s;
struct AREASAVE_s;
struct ATTRACTO_s;
struct BATARANG_s;
struct BOLTSYS;
struct BOLT_s;
struct BUILDIT_FIND_ENUM;
struct BaseEditor;
struct BaseThing;
struct CABLE_s;
struct CHARACTERDATA_s;
struct CHARACTERMODEL_s;
struct CHARCATEGORY;
struct CHARFIXUP;
struct CHARPIVOT;
struct CHARPLATFORMSYS_s;
struct CHARVARIANT;
struct CHEAT;
struct CLIMBOBJECTSYS_s;
struct COLLECTION_s;
struct CUSTOMISER;
struct CUSTOMISESAVE_s;
struct CUSTOMPIECE;
struct CUTINFO;
struct CUTSCENESYS;
struct CUTSYS;
struct CantPickupBombTimerAddon;
struct CharacterObjectInterface;
struct ClassEditor;
struct ClassItem;
struct ClassObject;
struct ClassObjectList;
struct ClickToPressStartGestureTracker;
struct CursorTool;
struct DATAPTR;
struct DETONATOR_s;
struct DOOR_s;
struct EDCREATURE_s;
struct EPISODEDATA;
struct EXPLOSION;
struct EXTRAMODEL;
struct EdClass;
struct EdControl;
struct EdFileInputStream;
struct EdFileOutputStream;
struct EdInputContext;
struct EdRef;
struct EdStream;
struct EdTool;
struct FADEINFO_s;
struct FADETYPE;
struct FLOWBOX_s;
struct FS_FILEENTRYHDR;
struct Fade;
struct FadeBase;
struct FadeStill;
struct FadeStillWipe;
struct FadeSystem;
struct FadeWipe;
struct GAMEANIMOBJPOOL_s;
struct GAMEANIMOBJ_s;
struct GAMEANIMSET_s;
struct GAMEANIMSYS_s;
struct GAMEANTINODEDATA_s;
struct GAMEANTINODESYS_s;
struct GAMEANTINODE_s;
struct GAMEAUDIO;
struct GAMECAMERA_s;
struct GAMECHARACTERDATA_s;
struct GAMEMESSAGE_s;
struct GAMEPAD_s;
struct GIZACTIONDEFN_s;
struct GIZAIMESSAGESYS_s;
struct GIZAIMESSAGE_s;
struct AIMESSAGESYS_s;
struct AIMESSAGE_s;
struct GIZBOMBGENSYS_s;
struct GIZBUILDIT_s;
struct GIZFLOWPROGRESS_s;
struct GIZFLOW_s;
struct GIZFORCESYS_s;
struct GIZFORCE_s;
struct GIZMOBLOWUPTYPE_s;
struct GIZMOBLOWUP_s;
struct GIZMOPICKUPSYS_s;
struct GIZMOPICKUP_s;
struct GIZMOSYS_s;
struct GIZMO_s;
struct GIZOBSTACLESYS_s;
struct GIZOBSTACLE_s;
struct GIZPANEL_s;
struct GIZSPECIAL_s;
struct GIZSPINNER_s;
struct GIZTURRETSYS_s;
struct GIZTURRET_s;
struct GRABBER_s;
struct GRAPPLE_s;
struct GameObject_s;
struct GameThingManager;
struct GizBlowupObjectInterface;
struct GizBuildItObjectInterface;
struct GizForceObjectInterface;
struct GizLeverObjectInterface;
struct GizObstacleObjectInterface;
struct GizPanelObjectInterface;
struct GizTurretObjectInterface;
struct HATMACHINE_s;
struct HINT_s;
struct HOTHBATTLE_MELEE_s;
struct HashRedirect;
struct HashedKey;
struct HatMachineObjectInterface;
struct HudRadarPulse;
struct LANGUAGEDATA;
struct LEVELDATADISPLAY;
struct LEVELDATA_s;
struct LEVELOBJECT;
struct LEVELSCRIPTPROCESS_s;
struct LEVELSPLINE;
struct LEVEL_PROGRESS_s;
struct LEVER_s;
struct LevelEditor;
struct LoadedUniqueShaderRecord;
struct MENU_s;
struct MISSIONSAVE_s;
struct MISSIONSYS_s;
struct MechAddon;
struct MechInputTouchMainController;
struct MechInputTouchVirtualConsoleController;
struct MechObjectInterface;
struct MechTouchUIElement;
struct MemoryBuffer;
struct MemoryManager;
struct MoveToMarker;
struct NARROWSOCKEXCEPTION;
struct NUGCUTCHAR_s;
struct NUGCUTSCENE_s;
struct NetAddress;
struct NetMessage;
struct NetPeer;
struct NuBloomParameters;
struct NuShaderObject;
struct NuShaderUsageMask_s;
struct NuVec2;
struct OPTIONSSAVE_s;
struct OccluderSet;
struct OcclusionManager;
struct PARTDEBSYS_s;
struct PART_s;
struct PLATSKININFO;
struct PLAYERITEMTYPE_s;
struct PLAYERITEM_s;
struct PLAYERPACKET_s;
struct PLUGSYS_s;
struct PLUG_s;
struct PULSESYS_s;
struct PartHeader;
struct PartObjectInterface;
struct Placeable;
struct PlaceableHelper;
struct PlaceableInterface;
struct PlaceableNameControl;
struct PropertyMenu;
struct PropertyMenuList;
struct PropertyTool;
struct REGISTERSTATUSPACKET_s;
struct RGBA;
struct RUMBLEPACKET;
struct SCENEPROGRESS_s;
struct SHADERSEMANTIC_enum;
struct SHARD_s;
struct SOCKPOSITION_s;
struct SOCKSYS;
struct SPLINEPOS_s;
struct STATUSPACKET_s;
struct STATUS_STAGE_s;
struct SUIT_s;
struct SUPERCOUNTER;
struct SUPERCOUNTERPICKUP;
struct SceneInstance;
struct SceneObject;
struct SceneObjectHelper;
struct ShaderManagerOpenGL;
struct ShaderMtlDescFilter;
struct ShaderObjectKey;
struct SoundTable;
struct SpecialObject;
struct SwipeDecalRenderer;
struct TECHNO_s {
    char name[0x10];
    NUVEC position;
    NUVEC operator_position;
    f32 movement;
    f32 scale;
    u16 yaw;
    u16 terrain_pitch;
    u16 terrain_roll;
    u8 control_flags;
    u8 target_type;
    u32 target_flags;
    u8 state_flags;
    char target_name[0x10];
    char loaded_name[0x1f];
    void *target;
};
DECOMP_ASSERT(sizeof(TECHNO_s) == 0x70, "TECHNO_s size");
struct TELEPORT_s;
struct TERRPICKUPSET;
struct TERRSET;
struct TEXTCRAWL_s;
struct TEXTENTRY;
struct TMClient;
struct TORPEDOPACKET_s;
struct TRAFFICANIMSYS_s;
struct TTNetwork;
struct TUBE_s;
struct TeleportObjectInterface;
struct ThingLevelData;
struct ThingManager;
struct ThingProcessData;
struct ThingRemoveData;
struct ThingRenderData;
struct ThingResetData;
struct TouchHolder;
struct VirtualControlButton;
struct VirtualControlButtonMover;
struct VirtualControlDPad;
struct VirtualControlDPad_LockButton;
struct VirtualStackAllocator;
struct VuMtx;
struct VuVec;
struct WORLDINFO_s;
struct __sFILE;
struct _vum_s;
struct _vuv_s;
struct bgprocinfo_s;
struct bitrate_manager_state;
struct codebook;
struct debinftype;
struct debkeydatatype_s;
struct debris_chunk_control_s;
struct nunativedebrisdata_s;
struct drft_lookup;
struct edcam_s;
struct eduiitem_s;
struct eduimenu_s;
struct envelope_lookup;
struct flightspline_s;
struct instNUGCUTLOOKAT_s;
struct instNUGCUTSCENE_s;
struct mdct_lookup;
struct minisnowtrooper_s;
struct minitrooperteam_s;
struct nuanimbuff_s;
struct nucolour3_s;
struct nudisplaylistitem_s;
struct nudisplayscene_s;
struct nufile_device_s;
struct nufpar_s;
struct nufpcomjmp_s;
struct nuglobalrndrstate_s;
struct nugraph_s;
struct nugscn_s;
struct nugspline_s;
struct nuhspecial_s;
struct nuinstanim_s;
struct numtl_s;
struct numtx_s;
struct nunativedebrisdata_s;
struct nuoctreenode_s;
struct nupad_s;
struct nushadermtldesc_s;
struct nusound_filename_info_s;
struct nutex_s;
struct nutexmanager_s;
struct nuvec_s;
struct particlechunkrendertype_s;
struct pushblock_s;
struct ripple_node_s;
struct ripple_set_s;
struct rtldata_s;
struct rtlset;
struct shopitem_s;
struct specialsfx_s;
struct speedup_s;
struct starfighter_s;
struct terrsitu_s;
struct tertype;
struct uv1deb;
struct uv1debdata;
struct vucharidx_s;

struct ADDGAMEMSG {};
struct ADDPART_s {};
struct AIANTINODE_s;
struct AIAREA_s;
struct AIGROUP_s;
struct AILOCATOR_s;
struct AIPACKET_s;
struct AIPATHCNXCONTROLLER_s {
    NULISTLNK links;
    u8 pad_0x08[0xa8 - 0x08];
};
DECOMP_ASSERT(sizeof(AIPATHCNXCONTROLLER_s) == 0xa8, "AIPATHCNXCONTROLLER_s size");
struct AIPATHCNXCONTROLSYS_s {
    i32 controller_count;
    AIPATHCNXCONTROLLER_s *controllers;
    NULISTHDR available_controllers;
    i32 field_0x10;
    i32 field_0x14;
};
DECOMP_ASSERT(sizeof(AIPATHCNXCONTROLSYS_s) == 0x18, "AIPATHCNXCONTROLSYS_s size");
struct AIPATHCNXHELPER_s {
    u8 data[0x10];
};
DECOMP_ASSERT(sizeof(AIPATHCNXHELPER_s) == 0x10, "AIPATHCNXHELPER_s size");
struct AIPATHCNXHELPERSYS_s {
    i16 field_0x00;
    i16 helper_count;
    AIPATHCNXHELPER_s *helpers;
};
DECOMP_ASSERT(sizeof(AIPATHCNXHELPERSYS_s) == 0x8, "AIPATHCNXHELPERSYS_s size");
struct AIPATHINFO_s;
struct AIPATHNODE_s;
struct AISCRIPTPROCESS_s;
struct AISCRIPT_s;
struct AITRIGGERSET_s {
    u8 pad_0x000[0x204];
    i8 trigger_indices[8];
    u8 pad_0x20c[8];
};
DECOMP_ASSERT(sizeof(AITRIGGERSET_s) == 0x214, "AITRIGGERSET_s size");
struct AITRIGGERSETSYS_s {
    AITRIGGERSET_s sets[32];
    i8 field_0x4280[64];
    i8 field_0x42c0[64];
    i32 field_0x4300;
};
DECOMP_ASSERT(sizeof(AITRIGGERSETSYS_s) == 0x4304, "AITRIGGERSETSYS_s size");
struct ANIMREDIRECT {};
struct AREADATA_s;
struct AREASAVE_s {
    u8 complete;
    u8 area_complete;
    u8 story_buildup_complete;
    u8 freeplay_buildup_complete;
    u8 minikit_count;
    u8 field_0x5[3];
    f32 challenge_trial_time;
};
DECOMP_ASSERT(sizeof(AREASAVE_s) == 0xc, "AREASAVE_s size");
DECOMP_ASSERT(offsetof(AREASAVE_s, challenge_trial_time) == 0x8, "AREASAVE challenge time offset");
struct ATTRACTO_s;
struct BATARANG_s {
    u8 pad_0x00[0x7c];
    u8 active; // 0x7c
    u8 field_0x7d;
    u8 pad_0x7e[0x84 - 0x7e];
    i32 target_id; // 0x84
    u8 pad_0x88[0xac - 0x88];
    u16 cooldown; // 0xac
    u8 pad_0xae[0xb4 - 0xae];
};
DECOMP_ASSERT(sizeof(BATARANG_s) == 0xb4, "BATARANG_s size");
struct BOLTSYS {};
struct BOLT_s {
    u8 pad_0x00[0xf0];
    u8 flags; // 0xf0
    u8 pad_0xf1[0x100 - 0xf1];
    u8 active; // 0x100
    u8 pad_0x101[0x138 - 0x101];
};
DECOMP_ASSERT(sizeof(BOLT_s) == 0x138, "BOLT_s size");
struct BUILDIT_FIND_ENUM {};
struct CABLE_s {};
struct CHARACTERDATA_s {};
struct CHARCATEGORY {};
struct CHARFIXUP;
struct CHARPIVOT {};
struct CHARPLATFORM_s;
struct CHARVARIANT {};
struct CHEAT;
struct CLIMBOBJECTSYS_s {};
struct CUSTOMISER {
    u8 pad_0x00[0x178];
    ANIMPACKET_s animation_packets[2]; // 0x178
    i32 model_texture_ids[18];         // 0x208
    u8 pad_0x250[0xa6c - 0x250];
    u8 animation_active[2];  // 0xa6c
    u8 animation_state[2];   // 0xa6e
    i32 animation_values[2]; // 0xa70
};
struct __attribute__((packed)) CUSTOMISESAVE_s {
    i16 pieces[9];              // 0x00
    u8 field_0x12[2];           // 0x12
    char primary_name[0x20];    // 0x14
    u8 primary_name_unlocked;   // 0x34
    u8 field_0x35[0x17];        // 0x35
    char secondary_name[0x20];  // 0x4c
    u8 secondary_name_unlocked; // 0x6c
    u8 field_0x6d[2];           // 0x6d
};
DECOMP_ASSERT(sizeof(CUSTOMISESAVE_s) == 0x6f, "CUSTOMISESAVE_s size");
DECOMP_ASSERT(offsetof(CUSTOMISESAVE_s, primary_name) == 0x14, "CUSTOMISESAVE primary name offset");
DECOMP_ASSERT(offsetof(CUSTOMISESAVE_s, primary_name_unlocked) == 0x34, "CUSTOMISESAVE primary flag offset");
DECOMP_ASSERT(offsetof(CUSTOMISESAVE_s, secondary_name) == 0x4c, "CUSTOMISESAVE secondary name offset");
DECOMP_ASSERT(offsetof(CUSTOMISESAVE_s, secondary_name_unlocked) == 0x6c, "CUSTOMISESAVE secondary flag offset");
struct CUSTOMPIECE {};
struct CUTINFO {
    void *scene;
    void *instance;
    char name[0x40];
    void *state_entries;
    u8 state_count;
    u8 pad_4d[3];
    u32 flags;
    f32 previous_frame;
    i32 field_58;
    f32 frames_per_second;
    f32 field_60;
    u8 pad_64[0x198 - 0x64];
};
struct CUTSCENESYS {
    i16 blaster_object_0;
    i16 blaster_object_1;
    i16 field_04;
    i16 field_06;
};
struct CUTSYS {
    CUTINFO **cuts;
    i32 count;
    u32 *character_bits;
};
struct ClassItem {};
struct DETONATOR_s {
    NUVEC position;            // 0x00
    NUVEC detonation_position; // 0x0c
    NUVEC screen_position;     // 0x18
    GameObject_s *owner;       // 0x24
    u8 active;                 // 0x28
    u8 pad_29;
    i16 rotation_x; // 0x2a
    i16 rotation_y; // 0x2c
    u16 rotation_z; // 0x2e
    f32 timer;      // 0x30
    void *antinode; // 0x34
};
DECOMP_ASSERT(sizeof(DETONATOR_s) == 0x38, "DETONATOR_s size");
struct EDCREATURE_s {};
struct EPISODEDATA;
struct EXPLOSION {
    i32 state;
    i32 field_0x4;
    GameObject_s *object;
    NUVEC position;
    f32 field_0x18;
    f32 elapsed;
    f32 duration;
    i32 field_0x24;
    u16 field_0x28;
    u16 rotation;
    u16 type;
    u16 hit_flags;
    u16 field_0x30;
    u8 active;
    u8 field_0x33;
};
DECOMP_ASSERT(sizeof(EXPLOSION) == 0x34, "EXPLOSION size");
struct EXTRAMODEL {
    i16 *model_list;
    void *field_04;
};
DECOMP_ASSERT(sizeof(EXTRAMODEL) == 8, "EXTRAMODEL size");
struct EdClass;
struct EdControl;
struct EdFileInputStream;
struct EdFileOutputStream;
struct EdInputContext;
struct EdRef;
struct EdStream;
struct EdTool {};
struct FADEINFO_s {
    // FadeSystem passes its own storage to the individual fade objects as a
    // FADEINFO_s*.  The original object is 0x2c bytes; keep these fields
    // named instead of accessing the storage through byte offsets.
    u32 direction;
    f32 fade;
    f32 rate;
    i32 busy;
    i32 stage;
    FadeBase *fades[4];
    i32 pending_type;
    i32 field_28;
};
struct FADETYPE {
    i32 type;
};
struct FLOWBOXGIZMODATA_s {
    i32 gizmo_count;
    u8 reserved_04[0x04];
    GIZMO_s ***gizmos;
};
DECOMP_ASSERT(sizeof(FLOWBOXGIZMODATA_s) == 0xc, "FLOWBOXGIZMODATA_s size");
struct FLOWBOX_s {
    u8 parent_count;
    u8 loop_parent_count;
    u8 child_count;
    u8 type;
    u32 runtime_id;
    u8 last_process_frame;
    u8 loop_checksum;
    u16 state_flags;
    FLOWBOXGIZMODATA_s *data;
    FLOWBOX_s **parents;
    FLOWBOX_s **children;
    u8 *output_indices;
    char *name;
};
DECOMP_ASSERT(sizeof(FLOWBOX_s) == 0x20, "FLOWBOX_s size");
struct FS_FILEENTRYHDR {};
struct FadeBase {
    virtual ~FadeBase() = default;
    virtual void Init(FADEINFO_s *) = 0;
    virtual void InitFade() = 0;
    virtual void UpdateFade() = 0;
    virtual void DrawFade() = 0;
    virtual i32 GetFadeType() const = 0;
    FADEINFO_s *info;
};
struct GAMEANIMOBJPOOL_s {};
struct GAMEANIMSYS_s {};
struct GAMEANTINODEDATA_s {};
struct GAMEANTINODESYS_s {};
struct GAMEANTINODE_s {};
struct GAMEAUDIO {
    void (*action_music_fn)(void);
    void (*reset_music_fn)(void);
    const char *sfx_names[0x55];
    i16 sfx_ids[0x55];
};

DECOMP_ASSERT(offsetof(GAMEAUDIO, sfx_names) == 0x8, "GAMEAUDIO sfx_names offset");
DECOMP_ASSERT(offsetof(GAMEAUDIO, sfx_ids) == 0x15c, "GAMEAUDIO sfx_ids offset");
// Camera state (GameCam). The original global points directly at a static
// 0x230-byte block; matrix and mode offsets are verified against
// MoveGameCamera and InitGameBeforeConfig.
struct GAMECAMERA_s {
    SOCKPOSITION sock_position; // 0x000; camera begins with its rail position state
    NUMTX mtx;                  // 0x038
    NUMTX target_mtx;           // 0x078
    NUMTX render_mtx;           // 0x0b8
    NUVEC shaken_right;         // 0x0f8
    NUVEC shaken_up;            // 0x104
    NUVEC dir;                  // 0x110
    NUVEC pos;                  // 0x11c
    NUVEC target;               // 0x128
    u8 pad_134[0x1b8 - 0x134];
    f32 field_0x1b8;
    f32 field_0x1bc;
    f32 field_0x1c0;
    u8 pad_1c4[4];
    f32 field_0x1c8;
    u8 pad_1cc[4];
    f32 field_0x1d0;
    f32 field_0x1d4;
    f32 field_0x1d8;
    f32 field_0x1dc;
    f32 position_seek;
    f32 angle_seek;
    f32 field_0x1e8;
    f32 field_0x1ec;
    f32 field_0x1f0;
    f32 field_0x1f4;
    u8 pad_1f8[4];
    u16 input_yaw; // 0x1fc, added to camera-relative controller angles
    u8 pad_1fe[0x204 - 0x1fe];
    f32 field_0x204;
    f32 field_0x208;
    f32 field_0x20c;
    f32 field_0x210;
    f32 field_0x214;
    f32 field_0x218;
    u8 pad_21c[0x22a - 0x21c];
    u8 reset_blend;          // 0x22a
    u8 blend_mode;           // 0x22b
    i8 mode;                 // 0x22c
    i8 previous_mode;        // 0x22d
    i8 previous_camera_mode; // 0x22e
    u8 pad_22f;
};
DECOMP_ASSERT(sizeof(GAMECAMERA_s) == 0x230, "GAMECAMERA_s ABI");
struct GAMECHARACTERDATA_s {};

// game_cutscenes wrapper (pointer at 0x1c).
// game_cutscenes (0x28 bytes, held by value).
struct GAMECUTSCENES_s {
    char pad_0x00[0x1c];
    void *cutscene; // 0x1c active cutscene
    char pad_0x20[0x28 - 0x20];
};

struct GAMEMESSAGE_s {
    char pad_0x00[0xe6];
    u16 icon;   // 0xe6
    u32 color1; // 0xe8
    u32 color2; // 0xec
    u32 color3; // 0xf0
    char pad_0xf4[0xf7 - 0xf4];
    u8 alpha;  // 0xf7
    u8 active; // 0xf8
    u8 pad_0xf9[0x114 - 0xf9];
};
DECOMP_ASSERT(sizeof(GAMEMESSAGE_s) == 0x114, "GAMEMESSAGE_s size");
// Rumble state packet embedded in GAMEPAD_s (20 bytes; floats driven by
// NuSound3UpdateRumble / UpdateRumble).
struct RUMBLEPACKET {
    undefined field_0x00[4];
    f32 rumble_amount; // 0x04
    undefined field_0x08[4];
    f32 rumble_time; // 0x0c
    u8 active;       // 0x10
    u8 pad_0x11[3];
};
DECOMP_ASSERT(sizeof(RUMBLEPACKET) == 0x14, "RUMBLEPACKET ABI");
struct GAMEPAD_s {
    nupad_s *pad; // 0x00  the bound input pad (fields 0x00..0x1f)
    union {
        u32 buttons_held;
        u32 unknown_04;
    };
    union {
        u32 buttons_pressed;
        u32 buttons_down_08;
    };
    union {
        u32 buttons_released;
        u32 unknown_0c;
    };
    union {
        u32 left_directions;
        u32 unknown_10;
    };
    union {
        u32 previous_left_directions;
        u32 unknown_14;
    };
    union {
        u32 right_directions;
        u32 unknown_18;
    };
    union {
        u32 previous_right_directions;
        u32 unknown_1c;
    };
    u32 unknown_20;
    union {
        struct {
            u8 input_state;
            u8 input_mode;
            u16 input_angle;
        };
        u32 unknown_24;
    };
    union {
        f32 input_magnitude;
        f32 unknown_28;
    };
    f32 input_direction_x; // 0x2c
    f32 input_direction_z; // 0x30
    f32 peak_input_magnitude;
    f32 previous_input_magnitude;
    f32 waggle_magnitude;
    RUMBLEPACKET rumble_packet; // 0x40
    void *operator_data;        // 0x54
    u16 previous_input_angle;
    u8 allocated_5a; // allocation and input-state flags
    char pad_5b[0x60 - 0x5b];
};
DECOMP_ASSERT(sizeof(GAMEPAD_s) == 0x60, "GAMEPAD_s ABI");
DECOMP_ASSERT(offsetof(GAMEPAD_s, input_angle) == 0x26, "GAMEPAD input angle offset");
DECOMP_ASSERT(offsetof(GAMEPAD_s, input_magnitude) == 0x28, "GAMEPAD input magnitude offset");
struct GIZACTIONDEFN_s {};

// AI messages use the same pooled-list arrangement as Giz AI messages.  Their
// payload is 0x24 bytes after the list links; no payload fields are consumed
// by the message-system reset path.
struct AIMESSAGESYS_s {
    i32 count;
    AIMESSAGE_s *messages;
    NULISTHDR free_list;
    NULISTHDR active_list;
};
struct AIMESSAGE_s {
    NULISTLNK links;
    u8 payload[0x24];
};
DECOMP_ASSERT(sizeof(AIMESSAGE_s) == 0x2c, "AIMESSAGE_s size");
DECOMP_ASSERT(sizeof(AIMESSAGESYS_s) == 0x18, "AIMESSAGESYS_s size");

// The AI message system: a fixed pool of 0x38-byte messages; the free list
// and the active list live in the header (ResetGizAIMessageSys fills the
// free list from the pool, CheckGizAIMessage moves nodes free -> active).
struct GIZAIMESSAGESYS_s {
    i32 count;                // 0x00 number of pooled messages
    GIZAIMESSAGE_s *messages; // 0x04 pool base (each 0x38 bytes)
    NULISTHDR free_list;      // 0x08
    NULISTHDR active_list;    // 0x10
};
struct GIZAIMESSAGE_s {
    NULISTLNK links;     // 0x00
    char name[0x20];     // 0x08
    float value;         // 0x28
    i8 output_values[8]; // 0x2c
    i8 num_outputs;      // 0x34
    u8 field_0x35;
    u8 flags; // 0x36
    u8 field_0x37;
};
struct GIZBOMBGENSYS_s {};
struct GIZFLOWPROGRESS_s {};
struct GIZFLOW_s {
    GIZMOSYS_s *gizmo_sys;
    i32 flow_box_count;
    void *flow_boxes;
    u8 flags;
    u8 pointers_valid;
    u8 reserved_0e[2];
};
DECOMP_ASSERT(sizeof(GIZFLOW_s) == 0x10, "GIZFLOW_s size");
struct GIZFORCESYS_s {};
struct GIZMOBLOWUPTYPE_s;
struct GIZMOPICKUPTYPE_s;
struct GIZMOPICKUPSYSDESCRIPTOR_s {
    GIZMOPICKUPTYPE_s *pickup_types;
    u8 *coin_table;
    u8 descriptor_state[3];
    i8 gizmo_type_id;
    i32 descriptor_flags;
};
DECOMP_ASSERT(sizeof(GIZMOPICKUPSYSDESCRIPTOR_s) == 0x10, "GIZMOPICKUPSYS descriptor size");
struct GIZMOPICKUPSYSRUNTIME_s {
    GIZMOPICKUP_s *pickups;
    GIZMOPICKUP_s *active_pickups;
    i32 pickup_count;
    i32 field_0c;
};
DECOMP_ASSERT(sizeof(GIZMOPICKUPSYSRUNTIME_s) == 0x10, "GIZMOPICKUPSYS runtime size");
struct GIZMOPICKUPSYS_s {
    union {
        GIZMOPICKUPSYSDESCRIPTOR_s descriptor;
        GIZMOPICKUPSYSRUNTIME_s runtime;
    };
};
DECOMP_ASSERT(sizeof(GIZMOPICKUPSYS_s) == 0x10, "GIZMOPICKUPSYS size");
struct GIZMOPICKUP_s {
    char name[8];
    NUVEC position;
    u8 field_14;
    u8 flags;
    u8 field_16;
    u8 runtime_flags;
    u8 type_id;
    u8 field_19_to_23[0x0b];
    u8 collected;
    u8 field_25_to_2b[7];
};
DECOMP_ASSERT(sizeof(GIZMOPICKUP_s) == 0x2c, "GIZMOPICKUP size");
struct GIZMOSYS_s;
struct GIZMO_s;
struct GIZOBSTACLESYS_s {};
struct GIZSPECIAL_s;
struct GIZSPINNER_s {};
struct GIZTURRETSYS_s {};
struct GRABBER_s {};
struct GRAPPLE_s;
struct HINT_s {
    i16 control_mode_ids[2]; // 0x00
    u8 flags;                // 0x04
    u8 pad_0x05[0x1c - 0x05];
    u8 completion_flags[4]; // 0x1c
    i32 field_0x20;
};
DECOMP_ASSERT(sizeof(HINT_s) == 0x24, "HINT_s size");
struct HINTSYS_s {
    u8 pad_0x00[0x04];
    HINT_s *hints; // 0x04
    u8 pad_0x08[0x0d - 0x08];
    u8 state; // 0x0d
    u8 pad_0x0e[0x18 - 0x0e];
    i32 current_hint; // 0x18
    i32 field_0x1c;
};
DECOMP_ASSERT(sizeof(HINTSYS_s) == 0x20, "HINTSYS_s size");
struct HINTUIBUTTON_s {
    u8 pad_0x00[0x40];
    f32 *field_0x40;
    f32 field_0x44;
    f32 field_0x48;
    f32 field_0x4c;
    f32 field_0x50;
    f32 field_0x54;
    u8 pad_0x58[0x78 - 0x58];
    void *field_0x78;
    i32 field_0x7c;
    u8 pad_0x80[0x84 - 0x80];
    f32 *field_0x84;
    f32 field_0x88;
    f32 field_0x8c;
    f32 field_0x90;
    f32 field_0x94;
    f32 field_0x98;
    u8 pad_0x9c[0xa0 - 0x9c];
    u8 field_0xa0;
};
struct HOTHBATTLE_MELEE_s {};
struct HashRedirect;
struct HashedKey {};
struct LANGUAGEDATA {
    i32 language;   // 0x00
    i32 unknown_04; // 0x04 (Game_LanguageList entries: {1,0},{2,0},{4,0},{5,0},{3,0},{8,0},{-1,0})
};
struct LEVELDATADISPLAY;
struct LEVELDATA_s;
struct LEVELOBJECT;
struct LEVELSCRIPTPROCESS_s;
struct LEVELSPLINE {
    struct nugscn_s **scene;
    const char *name;
    u16 min_points;
    u16 max_points;
    i16 level;
    i16 area;
};

DECOMP_ASSERT(sizeof(LEVELSPLINE) == 0x10, "LEVELSPLINE ABI");

struct LoadedUniqueShaderRecord;
struct MENU_s;
struct MISSIONSAVE_s;
struct MemoryBuffer;
struct NARROWSOCKEXCEPTION {};
struct NUGCUTCHAR_s;
struct NUGCUTSCENE_s;
struct NetAddress;
struct NetMessage;
struct NetPeer;
struct NuBloomParameters {};
struct NuShaderObject {};
struct NuShaderUsageMask_s {};
struct OPTIONSSAVE_s;
struct PARTDEBSYS_s {};
struct PLATSKININFO {};
struct CHARSHADOW_s {
    NUVEC position;
    u16 angle;
    u16 flags;
    f32 scale;
};
DECOMP_ASSERT(sizeof(CHARSHADOW_s) == 0x14, "CHARSHADOW_s ABI");
struct PLAYERITEMTYPE_s {};
struct PLAYERITEM_s {};
struct PLAYERPACKET_s {
    CHARSHADOW_s char_shadows[5];
    u8 reserved_064[0x654 - 0x64];
    i32 force_glow_mode;
    i32 force_glow_state;
    u8 reserved_65c[0x70];
    f32 force_glow_x;
    f32 force_glow_y;
    f32 force_glow_intensity;
    u8 reserved_6d8[0x4];
    f32 force_glow_z;
};
struct PLUGSYS_s {};
struct PLUG_s;
struct PULSE_s;
struct PULSESYS_s {
    PULSE_s *pulses;
    u16 pulse_count;
    u16 pad_0x06;
};
DECOMP_ASSERT(offsetof(PULSESYS_s, pulse_count) == 0x4, "PULSESYS pulse count offset");
struct PartHeader;
struct PropertyMenuList {};
struct REGISTERSTATUSPACKET_s {};
struct RGBA {};
struct SCENEPROGRESS_s {};
struct SHADERSEMANTIC_enum {};
struct SHARD_s;
struct SOCKPOSITION_s;
struct SPLINEPOS_s {};
// Status / achievements screen packet (332 bytes; fields used by NuMain:
// model ids at 0x9c/0x9e, per-player bytes at 0xa4/0xa5, flags at 0xb1/0xb2).
struct STATUSPACKET_s {
    struct STATUSPACKET_LSW_s *lsw_packet; // 0x00
    i32 field_0x04;
    i32 field_0x08;
    void (*reset_callback)(STATUSPACKET_s *); // 0x0c
    i32 field_0x10;
    undefined field_0x14[0x68 - 0x14];
    f32 field_0x68;
    undefined field_0x6c[0x9c - 0x6c];
    u16 player0_model; // 0x9c
    u16 player1_model; // 0x9e
    undefined field_0xa0[0xa4 - 0xa0];
    u8 player0_active; // 0xa4
    u8 player1_active; // 0xa5
    undefined field_0xa6[0xb1 - 0xa6];
    u8 mode_flags;   // 0xb1
    u8 status_flags; // 0xb2
    undefined field_0xb3[0x14c - 0xb3];
};
struct STATUSPACKET_LSW_s {
    i32 field_0x00;
};
struct STATUS_STAGE_s {
    u8 pad_0x00[0x12];
    u8 field_0x12;
    u8 pad_0x13;
    i32 field_0x14;
    i32 field_0x18;
    f32 field_0x1c;
};
DECOMP_ASSERT(sizeof(STATUS_STAGE_s) == 0x20, "STATUS_STAGE_s size");
struct SUIT_s {
    char *base_character_name;
    char *suit_character_name;
    i16 *text_id;
    u8 store_flag;
    u8 initially_available;
    u8 group;
    char letter;
    u32 character_flags;
    i16 character_id;
    u8 index;
    u8 field_0x17;
};
DECOMP_ASSERT(sizeof(SUIT_s) == 0x18, "SUIT_s size");
DECOMP_ASSERT(offsetof(SUIT_s, group) == 0xe, "SUIT_s group offset");
struct SUPERCOUNTER {
    char pad_0x00[0x1e2];
    u16 reset_value; // 0x1e2
    char pad_0x1e4[0x1e7 - 0x1e4];
    u8 processed_flags; // 0x1e7
};
DECOMP_ASSERT(sizeof(SUPERCOUNTER) == 0x1e8, "SUPERCOUNTER size");
struct SUPERCOUNTERPICKUP {};
struct ShaderObjectKey;
struct SoundTable {};
struct TECHNO_s;
struct TERRPICKUPSET {};
struct TERRSET {};
struct TEXTCRAWL_s {
    i16 *heading_text;
    i16 *vehicle_bonus_text;
    i16 *character_bonus_text;
    i32 paragraph_count;
};
DECOMP_ASSERT(sizeof(TEXTCRAWL_s) == 0x10, "TEXTCRAWL_s size");
struct TEXTENTRY {
    i16 *text_id;
    i16 value;
    i16 pad;
};
struct TORPEDOPACKET_s {};
struct TRAFFICANIMSYS_s {};
struct TUBE_s;
struct ThingLevelData {};
struct ThingRemoveData {};
struct VirtualStackAllocator;
struct VuMtx;
struct _vum_s {};
struct _vuv_s;
struct bgprocinfo_s;
struct bitrate_manager_state {};
struct codebook {};
struct debris_colour_key_s {
    f32 time;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};
struct debris_float_key_s {
    f32 time;
    f32 value;
};
struct debinftype {
    char name[16];               // 0x000
    u8 category;                 // 0x010
    u8 page;                     // 0x011
    u8 cutscene_only;            // 0x012
    u8 disabled;                 // 0x013
    i16 max_particles;           // 0x014
    i16 frequency;               // 0x016
    f32 emission_period;         // 0x018
    f32 emission_period_random;  // 0x01c
    f32 emission_pause;          // 0x020
    f32 emission_pause_random;   // 0x024
    f32 start_offset_random;     // 0x028
    u8 generator_type;           // 0x02c
    u8 momentum_adjustment_type; // 0x02d
    u8 particle_type;            // 0x02e
    u8 status;                   // 0x02f
    u8 fields_030[8];            // 0x030
    f32 clip_extent;             // 0x038
    u8 fields_03c[8];
    f32 field_044;
    f32 field_048;
    f32 field_04c;
    f32 field_050;
    f32 field_054;
    f32 field_058;
    f32 field_05c;
    f32 field_060;
    NUVEC emitter_velocity; // 0x064
    u8 fields_070[0x30];
    f32 field_0a0;
    f32 particle_lifetime; // 0x0a4
    i16 field_0a8;
    u8 field_0aa;
    u8 field_0ab;
    f32 field_0ac;
    f32 field_0b0;
    f32 field_0b4;
    f32 field_0b8;
    f32 field_0bc;
    debris_colour_key_s colour_keys[8]; // 0x0c0
    debris_float_key_s alpha_keys[8];   // 0x100
    f32 field_140;
    f32 field_144;
    f32 field_148;
    f32 field_14c;
    debris_float_key_s width_keys[8];  // 0x150
    debris_float_key_s height_keys[8]; // 0x190
    u8 fields_1d0[8];
    debris_float_key_s rotation_keys[8]; // 0x1d8
    u8 fields_218[0x80];
    f32 texture_u0;          // 0x298
    f32 texture_v0;          // 0x29c
    f32 texture_u1;          // 0x2a0
    f32 texture_v1;          // 0x2a4
    PartHeader *native_data; // 0x2a8 (target)
    f32 last_render_time;    // 0x2ac (target)
    u8 fields_2b0[0x40];     // 0x2b0 (target)
    u8 process_spheres;      // 0x2f0 (target)
    u8 time_group;           // 0x2f1
    u8 field_2f2;
    u8 use_explicit_clip_box; // 0x2f3
    f32 thinning;             // 0x2f4
    u8 fields_2f8[0xd8];      // 0x2f8
    i16 particle_keys[8];     // 0x3d0
    i32 sound_data[12];       // 0x3e0
    u8 trail_count;           // 0x410
    u8 radial_segments;       // 0x411
    u8 camera_facing;         // 0x412
    u8 field_413;
    f32 trail_time;            // 0x414
    f32 scale_in_time;         // 0x418
    f32 radial_floor;          // 0x41c
    f32 scale;                 // 0x420
    i32 unscaled_effect_index; // 0x424
};
struct debscale_s {
    i32 unscaled_effect_index;
    i32 scaled_effect_index;
    f32 scale;
};
DECOMP_ASSERT(sizeof(debinftype) == 0x428, "debinftype size");

typedef uv1deb *(*DEBRISGENERATOR)(debkeydatatype_s *, debinftype *, f32);
typedef void (*DEBRISMOMENTUMADJUSTER)(debkeydatatype_s *, debinftype *, uv1deb *);

struct debris_process_sphere_s {
    f32 time;
    u8 fields_004[0x18];
};

// One interpolated particle frame in the original debris effect table.
// The table is embedded in PartHeader at 0x4c and contains 64 entries.
struct debris_particle_frame_s {
    NUVEC position;       // 0x00
    NUVEC texture_offset; // 0x0c
    NUVEC extent;         // 0x18
    u32 colour;           // 0x24
};
DECOMP_ASSERT(sizeof(debris_particle_frame_s) == 0x28, "debris particle frame size");

struct PartHeader {
    u8 fields_000[0x10];
    f32 gravity;          // 0x10
    f32 last_render_time; // 0x14
    u8 fields_018[0x10];
    f32 texture_u0; // 0x28
    f32 texture_v0; // 0x2c
    f32 texture_u1; // 0x30
    f32 texture_v1; // 0x34
    u8 fields_038[0x14];
    debris_particle_frame_s frames[64]; // 0x4c
};
DECOMP_ASSERT(offsetof(PartHeader, gravity) == 0x10, "PartHeader gravity offset");
DECOMP_ASSERT(offsetof(PartHeader, last_render_time) == 0x14, "PartHeader render time offset");
DECOMP_ASSERT(offsetof(PartHeader, texture_u0) == 0x28, "PartHeader texture coordinate offset");
DECOMP_ASSERT(offsetof(PartHeader, frames) == 0x4c, "PartHeader frame table offset");
DECOMP_ASSERT(sizeof(PartHeader) == 0xa4c, "PartHeader size");

struct dma_particle_s {
    NUVEC position;
    f32 start_time;
    NUVEC momentum;
    f32 inverse_lifetime;
};

struct dma_particle_chunk_s {
    u8 command;
    u8 fields_001[3];
    dma_particle_chunk_s *next;
    u8 fields_008[0x18]; // particle records start at 0x20
    dma_particle_s particles[32];
    u8 end_command;
};
DECOMP_ASSERT(sizeof(dma_particle_s) == 0x20, "dma_particle_s size");
DECOMP_ASSERT(offsetof(dma_particle_s, position) == 0x00, "dma particle position offset");
DECOMP_ASSERT(offsetof(dma_particle_s, start_time) == 0x0c, "dma particle start time offset");
DECOMP_ASSERT(offsetof(dma_particle_s, momentum) == 0x10, "dma particle momentum offset");
DECOMP_ASSERT(offsetof(dma_particle_s, inverse_lifetime) == 0x1c, "dma particle lifetime offset");
DECOMP_ASSERT(offsetof(dma_particle_chunk_s, next) == 0x04, "dma particle next offset");
DECOMP_ASSERT(offsetof(dma_particle_chunk_s, particles) == 0x20, "dma particle data offset");
DECOMP_ASSERT(offsetof(dma_particle_chunk_s, end_command) == 0x420, "dma particle terminator offset");
DECOMP_ASSERT(sizeof(dma_particle_chunk_s) == 0x424, "dma_particle_chunk_s size");

struct debkeydatatype_s {
    u8 fields_000[0x40];
    NUMTX effect_orientation;                  // 0x040
    NUMTX emitter_orientation;                 // 0x080
    NUMTX particle_orientation;                // 0x0c0
    dma_particle_chunk_s *particle_chunks[32]; // 0x100 (target)
    i16 allocated_chunk_count;                 // 0x180 (target)
    i16 effect_index;                          // 0x182
    u8 field_184;
    u8 allocation_locked;       // 0x185
    i16 particle_count;         // 0x186
    i16 controlled_chunk_count; // 0x188
    i16 field_18a;
    i16 previous_allocated_chunk_count; // 0x18c
    i16 previous_particle_count;        // 0x18e
    NUVEC position;                     // 0x190
    NUVEC emission_position;            // 0x19c
    NUVEC momentum;                     // 0x1a8
    u8 fields_1b4[0x0c];
    DEBRISGENERATOR generator;                // 0x1c0 (target)
    DEBRISMOMENTUMADJUSTER momentum_adjuster; // 0x1c4 (target)
    debkeydatatype_s *previous;               // 0x1c8 (target)
    debkeydatatype_s *next;                   // 0x1cc (target)
    i16 emitter_rotation_x;                   // 0x1d0 (target)
    i16 emitter_rotation_y;                   // 0x1d2
    i32 field_1d4;
    i16 field_1d8;
    u8 field_1da;
    u8 field_1db;
    f32 previous_emission_time; // 0x1dc
    f32 emission_time;          // 0x1e0
    f32 field_1e4;
    u8 fields_1e8[0x18];
    debris_process_sphere_s process_spheres[7]; // 0x200
    u8 fields_2c4[4];
    i16 field_2c8;
    u8 fields_2ca[2];
    i32 field_2cc;
    i32 trigger_first;
    i32 trigger_second;
    i32 trigger_third;
    i16 reflection_x;
    i16 reflection_y;
    i32 reflection_mode;
    f32 reflection_scale;
    i16 collision_timers[4];
    u8 fields_2f0[2];
    i16 field_2f2;
    u8 field_2f4;
    u8 process_collision_sound;
    u8 field_2f6;
    u8 field_2f7;
    u8 timed_flags;
    u8 field_2f9;
    u8 field_2fa;
    u8 field_2fb;
    nugscn_s *gscene;      // 0x2fc (target)
    f32 orientation_dirty; // 0x300 (target)
    u8 fields_304[0x18];
    f32 cutoff_distance;
    f32 last_update_time;
    f32 emission_epoch;
    i16 render_priority;
    i16 allocation_index;
    i32 field_32c;
};
DECOMP_ASSERT(sizeof(debkeydatatype_s) == 0x330, "debkeydatatype_s size");
struct debris_chunk_control_s {
    dma_particle_chunk_s *particle_chunk;
    f32 expiry_time;
    i32 active;
    debkeydatatype_s *owner;
    debris_chunk_control_s *next;
    i32 particle_index;
    f32 collision_time;
    i16 effect_index;
    i16 field_01e;
    i16 rotation_y;
    i16 field_022;
    f32 collision_plane;
    f32 restitution;
};
DECOMP_ASSERT(sizeof(debris_chunk_control_s) == 0x2c, "debris_chunk_control_s size");
struct drft_lookup {};
struct edcam_s {};
struct eduiitem_s;
struct eduimenu_s;
struct envelope_lookup {};
struct flightspline_s {};
struct instNUGCUTLOOKAT_s {};
struct instNUGCUTSCENE_s;
struct mdct_lookup {};
struct minisnowtrooper_s {};
struct minitrooperteam_s {};
struct nuanimbuff_s;
struct nucolour3_s;
struct nudisplaylistitem_s;
struct nudisplayscene_s;
struct nufile_device_s;
struct nufpar_s;
struct nufpcomjmp_s;
struct nuglobalrndrstate_s;
struct nugraph_s {};
struct nugscn_s;
struct nugspline_s;
struct SPECIALMINIKIT_s {
    nuhspecial_s special;
    void *inst_anim;
    void *anim_data;
    f32 end_frame;
    char special_name[16];
    char pickup_name[8];
    GIZMO_s *special_gizmo;
    GIZMO_s *pickup_gizmo;
    u8 flags;
};
DECOMP_ASSERT(sizeof(SPECIALMINIKIT_s) == 0x3c, "SPECIALMINIKIT size");
struct SPECIALMINIKITSYS_s {
    SPECIALMINIKIT_s *items;
    i32 count;
};
DECOMP_ASSERT(sizeof(SPECIALMINIKITSYS_s) == 0x8, "SPECIALMINIKITSYS size");
struct CHARPLATFORM_s {
    nuhspecial_s special;
    i16 object_id;
    i16 platform_id;
    GameObject_s *object;
};
DECOMP_ASSERT(sizeof(CHARPLATFORM_s) == 0x14, "CHARPLATFORM_s size");
struct CHARPLATFORMSYS_s {
    i32 field_0x00;
    i32 platform_count;
    CHARPLATFORM_s platforms[1];
};
DECOMP_ASSERT(offsetof(CHARPLATFORMSYS_s, platforms) == 0x8, "CHARPLATFORMSYS platforms offset");
struct GIZMOBLOWUPTYPE_s {
    u8 reserved_00[0x30];
    nuhspecial_s special;
    u8 reserved_3c[0x100 - 0x3c];
};
DECOMP_ASSERT(sizeof(GIZMOBLOWUPTYPE_s) == 0x100, "GIZMOBLOWUPTYPE_s size");
struct FADER_s {
    nuhspecial_s special;
    u8 pad_0x0c[0xc];
};
DECOMP_ASSERT(sizeof(FADER_s) == 0x18, "FADER_s size");
struct PULSE_s {
    nuhspecial_s special;
    f32 on_time;
    f32 off_time;
    f32 timer;
    f32 start_wait;
    u16 active;
    u16 disabled;
    char gizmo_name[16];
    GIZMO_s *gizmo;
};
DECOMP_ASSERT(sizeof(PULSE_s) == 0x34, "PULSE size");
struct GAMEANIMOBJ_s {
    GAMEANIMOBJ_s *next;
    nuhspecial_s special;
    struct GAMEANIMINSTANCE_s *instance_animation;
    void *animation_data;
    f32 start_frame;
    f32 end_frame;
    u32 reserved;
    u32 flags;
    void *custom_data;
};
DECOMP_ASSERT(sizeof(GAMEANIMOBJ_s) == 0x2c, "GAMEANIMOBJ_s size");
DECOMP_ASSERT(offsetof(GAMEANIMOBJ_s, special) == 0x4, "GAMEANIMOBJ_s special offset");
struct GAMEANIMSET_s {
    GAMEANIMSET_s *previous;
    GAMEANIMSET_s *next;
    u8 object_count;
    u8 animated_object_count;
    u8 flags;
    u8 pad_0x0b;
    i32 state;
    GAMEANIMOBJPOOL_s *pool;
    GAMEANIMSYS_s *system;
    GAMEANIMOBJ_s *objects;
};
DECOMP_ASSERT(sizeof(GAMEANIMSET_s) == 0x1c, "GAMEANIMSET_s size");
DECOMP_ASSERT(offsetof(GAMEANIMSET_s, objects) == 0x18, "GAMEANIMSET_s objects offset");
struct GAMEANIMINSTANCE_s {
    u8 pad_0x00[0x40];
    f32 playback_scale;
    u8 pad_0x44[0x4c - 0x44];
    f32 start_frame;
    u8 flags;
};
DECOMP_ASSERT(offsetof(GAMEANIMINSTANCE_s, playback_scale) == 0x40, "GAMEANIMINSTANCE_s scale offset");
DECOMP_ASSERT(offsetof(GAMEANIMINSTANCE_s, start_frame) == 0x4c, "GAMEANIMINSTANCE_s frame offset");
DECOMP_ASSERT(offsetof(GAMEANIMINSTANCE_s, flags) == 0x50, "GAMEANIMINSTANCE_s flags offset");
struct MINIANIMPACKET_s {
    i32 current_animation;
    i32 animation_mode;
    u8 pad_0x08[0x14 - 0x08];
    i32 previous_animation_mode;
    u8 reset_state;
    u8 field_0x19;
    u8 pad_0x1a[0x1e - 0x1a];
    i16 current_animation_id;
    i16 previous_animation_id;
    i16 requested_animation_id;
};
DECOMP_ASSERT(sizeof(MINIANIMPACKET_s) == 0x24, "MINIANIMPACKET_s size");
struct MINICAMCOMMAND_s {
    i32 state_words[7];
};
DECOMP_ASSERT(sizeof(MINICAMCOMMAND_s) == 0x1c, "MINICAMCOMMAND_s size");
struct MINICAM_s {
    MINICAMCOMMAND_s commands[32];
    u8 command_count;
    u8 current_command;
    u8 flags[2];
    i32 command_state[27];
    f32 delta_time;
    i32 field_0x3f4;
    i32 field_0x3f8;
};
DECOMP_ASSERT(sizeof(MINICAM_s) == 0x3fc, "MINICAM_s size");
DECOMP_ASSERT(offsetof(MINICAM_s, command_count) == 0x380, "MINICAM command count offset");
DECOMP_ASSERT(offsetof(MINICAM_s, current_command) == 0x381, "MINICAM command index offset");
DECOMP_ASSERT(offsetof(MINICAM_s, delta_time) == 0x3f0, "MINICAM delta time offset");
struct nuinstanim_s {};
struct numtl_s;
struct numtx_s;
struct nunativedebrisdata_s {
    u8 vertex_buffer_index; // 0x00
    u8 fields_001[3];
    i32 use_system_memory_vb; // 0x04
    i32 first_vertex;         // 0x08
    i32 vertex_count;         // 0x0c
    numtl_s *material;        // 0x10
};
struct debris_vertex_s {
    NUVEC position;
    u32 colour;
    f32 u;
    f32 v;
};
DECOMP_ASSERT(sizeof(debris_vertex_s) == 0x18, "debris vertex size");
DECOMP_ASSERT(offsetof(nunativedebrisdata_s, use_system_memory_vb) == 0x04, "native debris system-memory flag offset");
DECOMP_ASSERT(offsetof(nunativedebrisdata_s, first_vertex) == 0x08, "native debris first vertex offset");
DECOMP_ASSERT(offsetof(nunativedebrisdata_s, vertex_count) == 0x0c, "native debris vertex count offset");
DECOMP_ASSERT(offsetof(nunativedebrisdata_s, material) == 0x10, "native debris material offset");
DECOMP_ASSERT(sizeof(nunativedebrisdata_s) == 0x14, "native debris packet size");
struct nuoctreenode_s {};
struct nupad_s;
struct nushadermtldesc_s;
struct nusound_filename_info_s;
struct nutex_s;
struct particlechunkrendertype_s {
    dma_particle_chunk_s *particle_chunk;
    debinftype *effect;
    debkeydatatype_s *key;
    NUMTX effect_orientation;
    NUVEC position;
    particlechunkrendertype_s *previous;
    particlechunkrendertype_s *next;
    i16 render_priority;
    u8 fields_062[2];
};
DECOMP_ASSERT(sizeof(particlechunkrendertype_s) == 0x64, "particlechunkrendertype_s size");
struct edpp_particle_s {
    u8 reserved_00[0x10];
    i32 instance_id;
    u8 reserved_14[0x58 - 0x14];
};
DECOMP_ASSERT(sizeof(edpp_particle_s) == 0x58, "edpp_particle_s size");
struct NUDISPLAYSPECIAL_s {
    u8 pad_0x00[0xc0];
    i32 instance_ix; // 0xc0
};
DECOMP_ASSERT(offsetof(NUDISPLAYSPECIAL_s, instance_ix) == 0xc0, "display special instance index offset");
struct pushblock_s {
    f32 vertical_penetration;
    f32 support_height;
    u8 reserved_08[0x14];
    pushblock_s *block_below;
    u32 field_20;
    nuhspecial_s special;
    u8 reserved_30[0x28];
    NUVEC *position;
    u8 reserved_5c[0x54];
    NUVEC bounds_min;
    NUVEC bounds_max;
    u8 state_flags[4];
    u16 field_cc;
    u16 field_ce;
};
DECOMP_ASSERT(offsetof(pushblock_s, block_below) == 0x1c, "push block below offset");
DECOMP_ASSERT(offsetof(pushblock_s, special) == 0x24, "push block special offset");
DECOMP_ASSERT(offsetof(pushblock_s, position) == 0x58, "push block position offset");
DECOMP_ASSERT(offsetof(pushblock_s, bounds_min) == 0xb0, "push block minimum bounds offset");
DECOMP_ASSERT(offsetof(pushblock_s, bounds_max) == 0xbc, "push block maximum bounds offset");
DECOMP_ASSERT(sizeof(pushblock_s) == 0xd0, "push block size");
struct ripple_node_s {
    u8 pad_0x00[0x78];
    ripple_node_s *next;
    ripple_node_s *previous;
};
DECOMP_ASSERT(sizeof(ripple_node_s) == 0x80, "ripple_node_s size");
struct ripple_set_s {
    union {
        u32 reset_state;
        struct {
            u16 count;
            u16 free_count;
        };
    };
    ripple_node_s *nodes;
    ripple_node_s *current;
    void *field_0x0c;
    void *field_0x10;
};
DECOMP_ASSERT(sizeof(ripple_set_s) == 0x14, "ripple_set_s size");
struct rtlset {};
struct shopitem_s {};
struct specialsfx_s {};
struct speedup_s {};
struct starfighter_s {};
struct terrsitu_s {};
struct tertype {
    f32 flMovement_scale;
    u32 dwFlags;
    u32 field_0x08;
};
DECOMP_ASSERT(sizeof(tertype) == 0xc, "tertype size");
struct uv1deb {};
struct uv1debdata;
struct BaseEditor {
    void Initialise(variptr_u &, variptr_u &, i32);
    void ReadBuffer(void **, void *, i32);
    void WriteBeginBlock(i32, i32);
    void WriteEndBlock(i32);
    void WriteMetaData(i32, i32, i32, i32);
};
#include "legoapi/items/objects/basething.h"
struct CantPickupBombTimerAddon {
    CantPickupBombTimerAddon(MechObjectInterface &, float);
    void OnProcess(MechAddon::ProcessStage, float);
    virtual ~CantPickupBombTimerAddon();
};

struct ClassEditor {
    ClassEditor();
    void AddMenuItems(eduimenu_s *);
    void ClearLevel(i32);
    void CreateObject();
    void CreateObject(ClassObject &);
    void CreateObject(EdClass *);
    void CreateObject(i32);
    void DestroySelectedObjects();
    void DestroySelectedObjectsNow();
    void DrawObjectSphere(ClassObject &, i32);
    void Editable(void *, EdClass *, i32);
    void Enter();
    void Exit();
    void FindNearestObject(VuVec &, ClassObject &, ClassObject &, i32);
    void FindNearestObject(VuVec &, ClassObject &, i32);
    void FindNearestObject(VuVec &, VuVec &, ClassObject &, ClassObject &, i32);
    void FindNearestObject(VuVec &, VuVec &, ClassObject &, i32);
    void Flush();
    void FocusSelected();
    void Initialise(variptr_u &, variptr_u &, i32);
    void InitialiseObject(ClassObject &);
    void IsSelectedClass(EdClass *);
    void IsSelectedObject(ClassObject &);
    void IsSelectedObject(void *, EdRef *);
    void IsUniqueName(char *);
    void MakeUniqueName(char const *, char *, i32);
    void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void PostSaveInitialisation();
    void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void PreSaveInitialisation();
    void Process(EdInputContext &);
    void ReadBlock(DATAPTR *);
    void RegisterTool(EdTool &);
    void Render();
    void SelectLED(i32);
    void SelectObject(ClassObject &, i32);
    void Serialise(EdStream &);
    void SetMode(i32);
    void SetViewMenuHilight(eduimenu_s *);
    void SnapPoint(VuVec &);
    void UpdateClassFilter(EdInputContext &);
    void UpdateLists(MemoryBuffer *, MemoryBuffer *);
    void UpdateSelectedObjects(EdInputContext &);
    void UpdateSnapRay(VuVec &);
    void ViewSelected();
    void WriteBlock(i32);
    void cbDestroyMenu(eduimenu_s *, eduimenu_s *);
    void cbDestroyObject(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassDeleteObject(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassExportMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassFileMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassImportMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassModeMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassNewMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassNewObject(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassRemoveDuplicates(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSelectClassMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSelectObject(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSelectObjectMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSetMode(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSetPinned(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSetSnap(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSetView(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassSnapMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassToolsMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdClassViewMenu(eduimenu_s *, eduiitem_s *, u32);
    void cbEdCopySelectedObject(EdInputContext &);
    void cbEdCreateClassNewObject(i32);
    void cbEdFilterLED(eduimenu_s *, eduiitem_s *, u32);
    void cbEdLevelDeselectAll(eduimenu_s *, eduiitem_s *, u32);
    void cbEdLevelSelectAll(eduimenu_s *, eduiitem_s *, u32);
    void cbEdPadSetManipulatorMode(eduimenu_s *, eduiitem_s *, u32);
    void cbFileSelected(eduimenu_s *, eduiitem_s *, u32);
};
struct ClassObject {
    void GetName(char *, i32);
    void Set(char *);
};
struct ClassObjectList {
    void GetAveragePosition(VuVec &);
    void GetAveragePosition(VuVec &, float &);
    void IsInList(ClassObject);
    void IsInList(EdClass *);
    void IsInList(void *, EdRef *);
};
struct ClickToPressStartGestureTracker {
    void OnClick(GameObject_s &, TouchHolder &);
};
struct CursorTool {
    void Initialise(variptr_u &, variptr_u &, i32);
    void Process(EdInputContext &);
    void Render();
};
struct Fade : FadeBase {
    i32 GetFadeType() const override;
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeStill : FadeBase {
    i32 GetFadeType() const override;
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeStillWipe : FadeBase {
    i32 GetFadeType() const override;
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeSystem : FADEINFO_s {
    i32 AddFade(FadeBase *);
    void Draw();
    void Init();
    i32 SetFade(FADETYPE const &, u32);
    void SetStage(char);
    void Update();
};
struct FadeWipe : FadeBase {
    i32 GetFadeType() const override;
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct GIZBUILDIT_s {
    char name[0x10];

    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZFORCE_s {
    char name[0x10];
    char pad_0x10[0x30];
    void *field_0x40; // 0x40
    float field_0x44; // 0x44  force strength threshold / start value
    char pad_0x48[0x6c - 0x48];
    float strength_0x6c;
    char pad_0x70[0xac - 0x70];
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZMOBLOWUP_s {
    u8 reserved_00[0x17];
    u8 state_flags;
    u8 reserved_18[0x50 - 0x18];
    char field_0x50[0x4f];      // 0x50 .. 0x9f
    u8 field_0x9f;              // 0x9f  state/flags byte
    i32 field_0xa0;             // 0xa0
    char field_0xa4[0x8];       // 0xa4 .. 0xac
    void *field_0xac;           // 0xac
    float field_0xb0;           // 0xb0
    undefined field_0xb4[0x46]; // 0xb4 .. 0xf9
    char name[0x10];            // 0xfa
    undefined field_0x10a[0x16];
    void *field_0x120; // 0x120
    u8 field_0x124;    // 0x124
    undefined field_0x125[3];
    float field_0x128; // 0x128
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZOBSTACLE_s {
    char name[0x10];
    NUVEC position;
    NUVEC target_position;
    NUVEC average_position;
    GAMEANIMSET_s *animation_set;
    void *controller;
    f32 trigger_delay;
    f32 trigger_timer;
    f32 trigger_distance;
    f32 playback_speed;
    f32 forward_speed;
    f32 reverse_speed;
    f32 proximity_radius;
    f32 animation_radius;
    NUVEC collision_extents;
    u32 config_flags;
    u32 exclude_flags;
    NUVEC pickup_offset;
    f32 pickup_spread;
    u16 pickup_pitch;
    u16 pickup_yaw;
    u16 trigger_yaw;
    i16 room_id;
    i16 blowup_type;
    i16 score;
    i16 forward_sfx;
    i16 reverse_sfx;
    i8 control_mode;
    i8 control_type;
    i8 control_variant;
    u8 pad_0x93[5];
    u8 state_flags;
    u8 reverse_flags;
    u8 pad_0x9a[2];
    i32 proximity;
    u8 behavior_flags;
    union {
        u8 field_0xa1;
        u8 field_a1_0xa1;
    };
    u8 pad_0xa2[2];
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
DECOMP_ASSERT(sizeof(GIZOBSTACLE_s) == 0xa4, "GIZOBSTACLE_s size");

// Object referenced by GIZFORCE_s::field_0x40 (flags at 0x24).
struct GIZFORCEOBJ_s {
    char pad_0x00[0x24];
    u32 flags; // 0x24
};

// Path connection flags (LevPathCnx entries).
struct PATHCNXDATA_s {
    u32 flags0; // 0x00
    u32 flags4; // 0x04
};

// Cutscene scene data (byte flag at 0x89).
struct CUTSCENEDATA_s {
    char pad_0x00[0x88];
    u8 field_0x88; // 0x88  flags byte
    u8 flags;      // 0x89
};

// Player sub-object (through APIOBJECT::field_0x54).
struct PLAYERSUBOBJ_s {
    char pad_0x00[0x24];
    void *field_0x24; // 0x24
};

// Second-level player sub-object (float at 0x1c).
struct PLAYERSUBOBJ2_s {
    char pad_0x00[0x1c];
    float value; // 0x1c
};

// Character entry inside apicharsys character-data array (stride 0x54).
struct PODCHARENTRY_s {
    char pad_0x00[0xc];
    void *model;                // 0x0c
    char pad_0x10[0x54 - 0x10]; // pad to the 0x54-byte stride
};

// Model data referenced by PODCHARENTRY_s::model.
struct PODMODELDATA_s {
    char pad_0x00[0x4];
    float *value; // 0x04 pointer to the pod model's speed value
};
struct GIZPANEL_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();

    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 yaw;
    i16 special_index;
    u8 panel_variant;
    u8 panel_type;
    u8 reserved_62[6];
    u16 flags;
    u8 reserved_6a[2];
    NUVEC player_position;
    NUVEC target_offset;
    u16 tracking_pitch;
    u16 tracking_yaw;
    u16 animation_angle;
    u16 terrain_pitch;
    u16 terrain_roll;
    i16 platform_id;
    f32 tracking_timer;
    f32 scale;
    GizPanelObjectInterface *mech_interface;
};
DECOMP_ASSERT(sizeof(GIZPANEL_s) == 0x9c, "GIZPANEL_s size");
struct GIZTURRET_s {
    char reserved_00[0x8];
    char name[0x10];
    char reserved_18[0x12c];
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GameThingManager; // defined after ThingManager (derives from it)
struct HATMACHINE_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();

    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 yaw;
    u8 configured_hat;
    u8 current_hat;
    u8 animation_state;
    char model_letter;
    u8 model_special_index;
    union {
        u8 flags;
        struct {
            u8 state_bit0 : 1;
            u8 state_bit1 : 1;
            u8 progress_state1 : 1;
            u8 progress_state0 : 1;
            u8 reserved_state_bits : 4;
        };
    };
    NUVEC player_position;
    NUVEC target_offset;
    u16 terrain_pitch;
    u16 terrain_roll;
    i16 platform_id;
    u16 reserved_82;
    f32 hat_refresh_timer;
    f32 animation_duration;
    f32 animation_time;
    f32 scale;
    f32 flash_timer;
    f32 render_animation_time;
    f32 blink_timer;
    u32 reserved_a0;
    HatMachineObjectInterface *mech_interface;
};
DECOMP_ASSERT(sizeof(HATMACHINE_s) == 0xa8, "HATMACHINE_s size");
struct HudRadarPulse {
    HudRadarPulse(VuVec const &);
    void IsFinished();
    void Process(float);
    void Render();
};
struct LEVER_s {
    u8 reserved_00[0x5c];
    char name[0x10];

    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct LevelEditor : BaseThing {
    u8 unknown_10[0x290];
    i32 reset_pending;

    void AddInfoText(char *);
    void AddScene(char *, nugscn_s *, i32);
    void AddText(char *);
    void BeginMultiLoad(variptr_u *, variptr_u *);
    void ClearLevel(i32);
    void CloseMenu();
    void CreateEditorList(eduimenu_s *, eduiitem_s *);
    void CreateMenu();
    void Display(ThingRenderData *);
    void DrawInfoText(char **, i32, i32, i32, i32, i32, i32, i32);
    void EndMultiLoad(variptr_u *, variptr_u *);
    void Enter();
    void Exit();
    void FindSceneId(char *);
    void Flush();
    void GetEdScene(i32);
    void GetScene(char *);
    void GetScene(i32);
    void Initalise(variptr_u &, variptr_u &, i32);
    void IsActiveScene(nugscn_s *);
    void IsEditable(i32);
    LevelEditor();
    void Load(char *, variptr_u *, variptr_u *, i32);
    void LoadState(variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *, variptr_u *);
    void ProcessEvenWhenPaused(ThingProcessData *);
    void ReadStream(EdFileInputStream &);
    void RegisterEditor(BaseEditor &);
    void Reset();
    void Save();
    void SaveState(i32, variptr_u *, variptr_u *);
    void SaveState(variptr_u *, variptr_u *);
    void SetNextMenu(eduimenu_s *);
    void SetPadText(i32, char *);
    void SetSaveFilename(char *);
    void WriteStream(EdFileOutputStream &);
};
struct MemoryManager {
    void *AllocPool(u32, i32);
    void FreePool(void *, u32);
};
// One mine entry (0x1c bytes, MINESYS_s::mines[64] starting at 0x0c).
struct MINEENTRY_s {
    u32 active;       // 0x00
    NUVEC pos;        // 0x04
    float grow;       // 0x10 grows by FRAMETIME each update
    float grow_alpha; // 0x14 clamped copy of grow (max 1.0); used as the draw scale
    u16 rotx;         // 0x18 draw pre-rotation about X
    u16 roty;         // 0x1a draw pre-rotation about Y
};

// Mine system (minesys global, 0x748 bytes). Layout from the original
// pod-race code: PodRaceInit fills the header + radius/areas/debris ids,
// UpdatePodRaceMines iterates mines[] and uses the timers.
struct MINESYS_s {
    nuhspecial_s mine_special; // 0x00
    MINEENTRY_s mines[64];     // 0x0c .. 0x70c
    float mine_radius;         // 0x70c NuSpecialGetOriginRadius(mine special)
    float spawn_timer;         // 0x710 seeded from camera zoom / 1000000000.0f sentinel
    void *nomine_areas[10];    // 0x714 AISysFindArea("nomine_N")
    i16 nomine_count;          // 0x73c
    i16 mine_debris;           // 0x73e FindGameDebris("MINE_POP")
    i32 mine_part;             // 0x740 PARTLookupType("POD_MINE_PART")
    float update_timer;        // 0x744
};

struct MoveToMarker {
    void BlowUp();
    void FadeOut();
    MoveToMarker(MechObjectInterface &);
    void Process(float);
    void Render();
};
struct OccluderSet {
    void AddOccluder(nuvec_s const *, nuvec_s const *, nuvec_s const *, nuvec_s const *);
    void Clear();
    void Init(u32, variptr_u *, variptr_u);
    void IsOccludedOBB(nuvec_s const *, nuvec_s const *, numtx_s const *);
    void IsOccludedSphere(nuvec_s const *, float);
    OccluderSet();
    void OnCameraSet();
    void PrepareForQueries(numtx_s const *, numtx_s const *);
    void RenderOccluders(bool) const;
    void SortByDepth(void const *, void const *);
    ~OccluderSet();
};
struct OcclusionManager {
    void AddOccluder(nuvec_s const *, float);
    void AddOccluder(nuvec_s const *, nuvec_s const *, numtx_s const *);
    void AddOccluder(nuvec_s const *, nuvec_s const *, nuvec_s const *, nuvec_s const *);
    void BeginFrame();
    void EndFrame();
    void Init(u32, variptr_u *, variptr_u);
    void IsOccludedOBB(nuvec_s const *, nuvec_s const *, numtx_s const *);
    void IsOccludedSphere(nuvec_s const *, float);
    OcclusionManager();
    void OnCameraSet();
    void RenderStats() const;
    void RenderZPass() const;
    void SetEnabled(bool);
    ~OcclusionManager();
};
struct PART_s {
    u8 data[0x224];
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
DECOMP_ASSERT(sizeof(PART_s) == 0x224, "PART_s ABI");
struct PartObjectInterface {
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    PartObjectInterface(PART_s &);
    virtual ~PartObjectInterface();
};
struct Placeable {
    void Reset();
    void GetCurrentPosition() const;
    void GetInitialPosition() const;
    void SetCurrentPosition(VuVec const *);
    void SetInitialPosition(VuVec const *);
};
struct PlaceableHelper {
    void Find(char *);
    void Find(char *, Placeable **, i32);
    void FindObject(char *);
    void GetNextObject(void *);
    void GetNextObject(void *, i32 (*)(void *));
    void Initialise();
    void IsEditorObject(ClassObject &);
    PlaceableHelper();
    void RegisterObjectType(char *, PlaceableInterface *);
};
struct PlaceableInterface {
    void DebugOutputObjects();
    void Find(char *);
    void Find(char *, Placeable **, i32);
};
struct PlaceableNameControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    PlaceableNameControl();
    void Process(EdInputContext &);
    void Render();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectObject(eduimenu_s *, eduiitem_s *, u32);
};
// Pod race net packet (podrace_netpacket).
struct PODRACENETPACKET_s {
    char pad_0x00[0xc];
    float countdown; // 0x0c
};

// One AI spline slot inside the pod sprint state. Type name from the original
// local symbol _ZL22PodSprint_InitAISplineP11WORLDINFO_sP20PODSPRINT_AISPLINE_sPc.
struct PODSPRINT_AISPLINE_s {
    struct nugspline_s *spline; // 0x00
    i16 vals[6];                // 0x04
};

// Pod sprint state (podsprint, 0x94 bytes).
struct PODSPRINT_s {
    struct nugspline_s *finish_line; // 0x00
    struct nugspline_s *halfway;     // 0x04
    PODSPRINT_AISPLINE_s ai[6];      // 0x08
    GIZAIMESSAGE_s *lap_msg;         // 0x68
    GIZAIMESSAGE_s *max_speed_msg;   // 0x6c
    GIZAIMESSAGE_s *min_speed_msg;   // 0x70
    GIZAIMESSAGE_s *speed_step_msg;  // 0x74
    void *field_0x78;                // 0x78 pacemaker/boulders object (dword write in the original)
    void *boulders;                  // 0x7c  AIArea from AISysFindArea("Boulders")
    float field_0x80;                // 0x80 pacemaker alpha
    float speed;                     // 0x84 sebulba speed (integer part drives the lap lights)
    float field_0x88;                // 0x88 second speed channel (sent as net->speed2)
    char pad_0x8c[0x8e - 0x8c];
    u8 ai_state; // 0x8e
    u8 ai_index; // 0x8f signed; index into PODSPRINT_AISPLINE_s::vals
    char pad_0x90[0x92 - 0x90];
    u8 flags; // 0x92
    char pad_0x93[0x94 - 0x93];
};

// Pod-sprint network packet (podsprint_netpacket).
struct PODSPRINTNETPACKET_s {
    i16 ai_state; // 0x00
    char pad_0x02[0x6 - 0x2];
    i16 speed;  // 0x06
    i16 speed2; // 0x08
};

struct PropertyMenu {
    void AddObject(ClassObject &);
    void ClearObjecs();
    void ContainsObject(ClassObject &);
    void ContainsObject(void *);
    void Destroy();
    void SelectAttr(i32);
};
struct PropertyTool {
    void AddPropertyMenuItems(eduimenu_s *, EdClass *, void *, eduiitem_s *);
    void AutoLocateMenu(PropertyMenu *);
    void BringToFront(PropertyMenu *);
    void CreatePropertyMenu(ClassObject &);
    void FindItemMenu(PropertyMenu *, ClassItem *);
    void GetActiveMenu(PropertyMenu *);
    void GetClassName(EdRef *, char *);
    void GetNextActiveMenu();
    void GetNextDefaultActiveMenu(eduimenu_s *);
    void GetTypeName(EdRef *, char *);
    void HasActiveMenu();
    void Initialise(variptr_u &, variptr_u &, i32);
    void Process(EdInputContext &);
    void ProcessControls(EdInputContext &);
    void ProcessMenu(EdInputContext &);
    PropertyTool();
    void RefreshMenuControls(PropertyMenu *);
    void Render();
    void RenderMenu(PropertyMenu *);
    void RetrievePropertyMenu(ClassObject *, PropertyMenuList *);
    void SelectAttr(i32);
    void SetDefaultActiveMenu(PropertyMenu *);
    void SetMenuControl(eduimenu_s *, EdControl *);
    void ToggleActiveMenu();
    void ediGetMenuStartMetrics();
    void ediMenuRetrieveMetrics(eduimenu_s *);
    void ediMenuStoreMetrics(eduimenu_s *);
};
// Retake-G network packet (retakeg_netpacket).
struct RETAKEGNETPACKET_s {
    i16 guard_a; // 0x00
    i16 guard_b; // 0x02
};

// Pod-race sniper entry (PodRace_snipers[5], 0x40 bytes each). The first two
// vectors overlay consecutive spline points; the position is copied from the
// sniper's spline via a 0x18-byte copy.
struct SNIPER_s {
    NUVEC prev;       // 0x00 first spline point (unused)
    NUVEC pos;        // 0x0c actual sniper position
    float fire_timer; // 0x18
    float state;      // 0x1c
}; // 0x20 bytes: the original strides this array by 0x20

struct SceneInstance {
    void GetCurrentPosition() const;
    void GetCurrentTransform() const;
    void GetInitialPosition() const;
    void GetInitialTransform() const;
    void GetVisibility() const;
    void Render(VuMtx const *) const;
    SceneInstance();
    void SetCurrentPosition(VuVec const *);
    void SetCurrentTransform(VuMtx const *);
    void SetInitialPosition(VuVec const *);
    void SetInitialTransform(VuMtx const *);
    void SetVisibility(i32);
};
struct SceneObject {
    void Clone(i32) const;
    SceneObject();
};
struct SceneObjectHelper {
    void AddMenuItems(eduimenu_s *);
    void ClearLevel(i32);
    void CreateObject(void *, i32, i32);
    void DestroyObject(void *, i32);
    void Flush();
    void GetNextObject(void *);
    void GetNumObjects();
    void Initialise();
    void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void SetSceneFilter(char *);
    void SubRender();
    void UpdateLists(MemoryBuffer *, MemoryBuffer *);
    void cbEdSceneObjectShowHiddenSolid(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSceneObjectShowHiddenWire(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSceneObjectShowOwnedObjects(eduimenu_s *, eduiitem_s *, u32);
};
struct ShaderManagerOpenGL {
    ShaderManagerOpenGL(VirtualStackAllocator &);
    void adaptShaderMaterialForShaderVersion(nushadermtldesc_s *);
    void createShader(ShaderMtlDescFilter &, bool, NuShaderObject *, i32);
    void createShader(ShaderObjectKey const &, NuShaderObject *, i32);
    void setElementfv(SHADERSEMANTIC_enum, i32, float const *);
    void setElementsfv(SHADERSEMANTIC_enum, i32, i32, float const *);
    void setElementsfv_transpose(SHADERSEMANTIC_enum, i32, i32, float const *);
    void setfv(SHADERSEMANTIC_enum, float const *);
    virtual ~ShaderManagerOpenGL();
};
struct ShaderMtlDescFilter {
    void getVertexFlags() const;
    void hasDiffuseMap(i32) const;
    void hasLayer(i32) const;
    void internalInit(nushadermtldesc_s const *, numtl_s const *, i32, i32);
};
struct SpecialObject {
    void Exists() const;
    void GetCollision() const;
    void GetCurrentPosition() const;
    void GetCurrentTransform() const;
    void GetInitialPosition() const;
    void GetInitialTransform() const;
    void GetMtl(i32) const;
    void GetName() const;
    void GetNumMtls() const;
    void GetRadius() const;
    void GetVisibility() const;
    void Render(VuMtx const *) const;
    void SetCollision(i32);
    void SetCurrentPosition(VuVec const *);
    void SetCurrentTransform(VuMtx const *);
    void SetInitialPosition(VuVec const *);
    void SetInitialTransform(VuMtx const *);
    void SetVisibility(i32);
    SpecialObject();
};
struct TELEPORT_s {
    u8 reserved_00[0x4e];
    u8 enabled;
    u8 active;
    u8 reserved_50[0x24];
    u16 field_74;
    u16 field_76;
    u16 field_78;
    u16 field_7a;
    u8 reserved_7c[0x84];
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
DECOMP_ASSERT(sizeof(TELEPORT_s) == 0x100, "TELEPORT_s size");
struct TMClient {
    struct TM_MOUSE_AXIS {};
    void AllocHandle();
    void Connect();
    void FClose(i32);
    void FOpen(char const *, char const *);
    void FRead(void *, u32, u32, i32);
    void FSeek(i32, i64, i32);
    void FTell(i32);
    void FWrite(void const *, u32, u32, i32);
    void FlushKeyBuffer();
    void GetKey(i32 *);
    void GetMouseAxis(TMClient::TM_MOUSE_AXIS);
    void GetMouseButtons();
    void SendTTY(char const *, i32);
    void TestKey(i32);
};
struct TTNetwork {
    void Broadcast(NetMessage, unsigned char);
    void ClearMyHostAddress();
    void Display(ThingRenderData *);
    void GetMyAddress() const;
    void GetMyHostAddress() const;
    void Initialise();
    void ProcessEvenWhenPaused(ThingProcessData *);
    void ReliableBroadcast(NetMessage, unsigned char);
    void ReliableSend(NetMessage, unsigned char, NetPeer &, char const *, u32);
    void Resume();
    void Send(NetMessage, unsigned char, NetPeer &);
    void SetMyHostAddress(NetAddress const &);
    void Shutdown();
    void Suspend();
    TTNetwork();
    void Update();
    virtual ~TTNetwork();
};
// All ThingManager methods are virtual in the original: its vtable order is
// D2, D0, AddThing, AddThingAfterThis, RemoveTemporaryThings,
// RemoveDependanciesThings, ResetThings, EnterLevelThings, ExitLevelThings,
// ProcessThings (vptr+0x24), RenderThings (vptr+0x28), DisplayThings,
// EffectsThings. NuMain dispatches ProcessThings/RenderThings through the
// vtable, so the declarations must carry the same slots.
struct ThingManager {
    virtual ~ThingManager();
    virtual void AddThing(BaseThing *);
    virtual void AddThingAfterThis(BaseThing *);
    virtual void RemoveTemporaryThings();
    virtual void RemoveDependanciesThings(ThingRemoveData *);
    virtual void ResetThings(ThingResetData *);
    virtual void EnterLevelThings(ThingLevelData *);
    virtual void ExitLevelThings(ThingLevelData *);
    virtual void ProcessThings(ThingProcessData *);
    virtual void RenderThings(ThingRenderData *);
    virtual void DisplayThings(ThingRenderData *);
    virtual void EffectsThings(ThingRenderData *);
    void EnableActions(i32, i32, i32);
    ThingManager(i32);
    void cbEdTimingSelect(eduimenu_s *, eduiitem_s *, u32);
    void cbEdTrackCancel(eduimenu_s *, eduimenu_s *);
    void edTimingEnter();
    void edTimingInit();
    void edTimingProc(float, nupad_s *);
    void edTimingRender();

    // data (object is 0x24 bytes; the ctor carves `things` from theMemoryManager)
    BaseThing **things; // 0x04
    i32 max_things;     // 0x08
    i32 count;          // 0x0c
    u32 field_0x10;     // 0x10 high-water cursor (written by the ctor / AllocPool)
    i32 field_0x14;     // 0x14 AddThingAfterThis reservation, folded in by the next AddThing
    void *timebar;      // 0x18 NuTimeBarCreateSet handle (profiling, stubbed)
};
// GameThingManager shares the base vtable entries (only the dtors differ) and
// registers itself in theGameThings (ctor @0x4e8b00 / D1 dtor @0x4e8a80).
struct GameThingManager : ThingManager {
    void AddLevelOnlyThings();
    void AddOnceOnlyThings();
    GameThingManager(i32);
    virtual ~GameThingManager();
};
struct VirtualControlButton {
    void Process(float);
    void Render();
    VirtualControlButton(NuVec2 const &, float, MechInputTouchMainController::eButtonTypes);
};
struct VirtualControlButtonMover {
    void Process(float);
    VirtualControlButtonMover(MechInputTouchVirtualConsoleController &);
};
struct VirtualControlDPad {
    void Process(float);
    void Render();
    VirtualControlDPad(NuVec2 const &, float, MechInputTouchVirtualConsoleController &);
    virtual ~VirtualControlDPad();
};
struct VirtualControlDPad_LockButton {
    void Process(float);
    void Render();
    VirtualControlDPad_LockButton(VirtualControlDPad &);
};

#endif // LEGOAPI_TYPES_H
