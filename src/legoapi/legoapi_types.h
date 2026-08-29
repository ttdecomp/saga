#ifndef LEGOAPI_TYPES_H
#define LEGOAPI_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#include "legoapi/items/base/apiobject.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "legoapi/characters/core/CharacterObjectInterface.h"
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
struct TECHNO_s;
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
struct ani3_animheader_s;
struct ani3_scalemin_s;
struct bgprocinfo_s;
struct bitrate_manager_state;
struct codebook;
struct debinftype;
struct debkeydatatype_s;
struct debris_chunk_control_s;
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
struct AIPATHCNXCONTROLLER_s {};
struct AIPATHCNXCONTROLSYS_s {};
struct AIPATHCNXHELPERSYS_s {};
struct AIPATHCNXHELPER_s {};
struct AIPATHINFO_s;
struct AIPATHNODE_s;
struct AISCRIPTPROCESS_s;
struct AISCRIPT_s;
struct AITRIGGERSETSYS_s {};
struct AITRIGGERSET_s {};
struct ANIMLIST_s {};
struct ANIMPACKET_s {
    char pad_0x00[0x10];
    float time;  // 0x10
    float time2; // 0x14
    char pad_0x18[0x3a - 0x18];
    u16 field_0x3a; // 0x3a
    char pad_0x3c[0x42 - 0x3c];
    u16 frame;      // 0x42
    f32 field_0x44; // 0x44
};
struct ANIMREDIRECT {};
struct AREADATA_s;
struct AREASAVE_s {};
struct ATTRACTO_s;
struct BATARANG_s {};
struct BOLTSYS {};
struct BOLT_s;
struct BUILDIT_FIND_ENUM {};
struct CABLE_s {};
struct CHARACTERDATA_s {};
struct CHARACTERMODEL_s {};
struct CHARCATEGORY {};
struct CHARFIXUP;
struct CHARPIVOT {};
struct CHARPLATFORMSYS_s {};
struct CHARVARIANT {};
struct CHEAT;
struct CLIMBOBJECTSYS_s {};
struct CUSTOMISER {};
struct CUSTOMISESAVE_s {};
struct CUSTOMPIECE {};
struct CUTINFO {
    char pad_0x00[0x4];
    void *scene; // 0x04
};
struct CUTSCENESYS {};
struct CUTSYS {};
struct ClassItem {};
struct DETONATOR_s {};
struct EDCREATURE_s {};
struct EPISODEDATA;
struct EXPLOSION {};
struct EXTRAMODEL {};
struct EdClass;
struct EdControl;
struct EdFileInputStream;
struct EdFileOutputStream;
struct EdInputContext;
struct EdRef;
struct EdStream;
struct EdTool {};
struct FADEINFO_s {};
struct FADETYPE {};
struct FLOWBOX_s {};
struct FS_FILEENTRYHDR {};
struct FadeBase {};
struct GAMEANIMOBJPOOL_s {};
struct GAMEANIMOBJ_s {};
struct GAMEANIMSET_s {};
struct GAMEANIMSYS_s {};
struct GAMEANTINODEDATA_s {};
struct GAMEANTINODESYS_s {};
struct GAMEANTINODE_s {};
struct GAMEAUDIO {};
// Camera state (GameCam). The original global points directly at a static
// 0x230-byte block; matrix and mode offsets are verified against
// MoveGameCamera and InitGameBeforeConfig.
struct GAMECAMERA_s {
    u8 pad_00;
    u8 field_0x01;
    u8 pad_02[0x2a];
    f32 zoom; // 0x02c
    u8 pad_030[0x08];
    NUMTX mtx;          // 0x038
    NUMTX target_mtx;   // 0x078
    NUMTX render_mtx;   // 0x0b8
    NUVEC shaken_right; // 0x0f8
    NUVEC shaken_up;    // 0x104
    NUVEC dir;          // 0x110
    NUVEC pos;          // 0x11c
    NUVEC target;       // 0x128
    u8 pad_134[0x22a - 0x134];
    u8 reset_blend;          // 0x22a
    u8 blend_mode;           // 0x22b
    i8 mode;                 // 0x22c
    i8 previous_mode;        // 0x22d
    i8 previous_camera_mode; // 0x22e
    u8 pad_22f;
};
static_assert(sizeof(GAMECAMERA_s) == 0x230, "GAMECAMERA_s ABI");
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
    u8 alpha; // 0xf7
};
// Rumble state packet embedded in GAMEPAD_s (20 bytes; floats driven by
// NuSound3UpdateRumble / UpdateRumble).
struct RUMBLEPACKET {
    undefined field_0x00[4];
    f32 rumble_amount; // 0x04
    undefined field_0x08[4];
    f32 rumble_time; // 0x0c
    undefined field_0x10[4];
};
struct GAMEPAD_s {
    nupad_s *pad; // 0x00  the bound input pad (fields 0x00..0x1f)
    u32 unknown_04;
    u32 buttons_down_08; // bitmask of pressed buttons (GAMEPAD_* masks)
    u32 unknown_0c;
    u32 unknown_10;
    u32 unknown_14;
    u32 unknown_18;
    u32 unknown_1c;
    u32 unknown_20;
    u32 unknown_24;
    f32 unknown_28;
    char pad_2c[0x40 - 0x2c];
    RUMBLEPACKET rumble_packet; // 0x40
    char pad_54[0x5a - 0x54];
    u8 allocated_5a; // set by GamePad_Allocate()
    char pad_5b[0x60 - 0x5b];
};
struct GIZACTIONDEFN_s {};

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
    NULISTLNK links; // 0x00
    char name[0x20]; // 0x08
    float value;     // 0x28
    byte mode;       // 0x2c
    byte mode_args;  // 0x2d
    undefined field_0x2e[0x34 - 0x2e];
    i32 flag;  // 0x34
    u16 flags; // 0x36
};
struct GIZBOMBGENSYS_s {};
struct GIZFLOWPROGRESS_s {};
struct GIZFLOW_s {};
struct GIZFORCESYS_s {};
struct GIZMOBLOWUPTYPE_s {};
struct GIZMOPICKUPSYS_s {};
struct GIZMOPICKUP_s;
struct GIZMOSYS_s;
struct GIZMO_s;
struct GIZOBSTACLESYS_s {};
struct GIZSPECIAL_s;
struct GIZSPINNER_s {};
struct GIZTURRETSYS_s {};
struct GRABBER_s {};
struct GRAPPLE_s;
struct HINT_s {};
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
#if !defined(HOST_BUILD) && !defined(__x86_64__)
static_assert(sizeof(LEVELSPLINE) == 0x10, "LEVELSPLINE ABI");
#endif
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
struct PLAYERITEMTYPE_s {};
struct PLAYERITEM_s {};
struct PLAYERPACKET_s {};
struct PLUGSYS_s {};
struct PLUG_s;
struct PULSESYS_s {};
struct PartHeader;
struct PropertyMenuList {};
struct REGISTERSTATUSPACKET_s {};
struct RGBA {};
struct SCENEPROGRESS_s {};
struct SHADERSEMANTIC_enum {};
struct SHARD_s;
struct SOCKPOSITION_s {};
struct SOCKSYS {};
struct SPLINEPOS_s {};
// Status / achievements screen packet (332 bytes; fields used by NuMain:
// model ids at 0x9c/0x9e, per-player bytes at 0xa4/0xa5, flags at 0xb1/0xb2).
struct STATUSPACKET_s {
    undefined field_0x00[0x9c];
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
struct STATUS_STAGE_s {};
struct SUIT_s {};
struct SUPERCOUNTER {};
struct SUPERCOUNTERPICKUP {};
struct ShaderObjectKey;
struct SoundTable {};
struct TECHNO_s;
struct TERRPICKUPSET {};
struct TERRSET {};
struct TEXTCRAWL_s {
    u8 pad[32]; // NOLINT(readability-identifier-naming)
};
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
struct ani3_animheader_s;
struct ani3_scalemin_s {};
struct bgprocinfo_s;
struct bitrate_manager_state {};
struct codebook {};
struct debinftype {};
struct debkeydatatype_s {};
struct debris_chunk_control_s {};
struct drft_lookup {};
struct edcam_s {};
struct eduiitem_s;
struct eduimenu_s;
struct envelope_lookup {};
struct flightspline_s {};
struct instNUGCUTLOOKAT_s {};
struct instNUGCUTSCENE_s {};
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
struct nuhspecial_s {
    nugscn_s *scene;       // 0x00
    void *special;         // 0x04
    void *display_special; // 0x08
};
#if !defined(HOST_BUILD) && !defined(__x86_64__)
static_assert(sizeof(nuhspecial_s) == 0xc, "nuhspecial_s size");
#endif
struct nuinstanim_s {};
struct numtl_s;
struct numtx_s;
struct nunativedebrisdata_s {};
struct nuoctreenode_s {};
struct nupad_s;
struct nushadermtldesc_s;
struct nusound_filename_info_s;
struct nutex_s;
struct nutexmanager_s {};
struct particlechunkrendertype_s {};
struct pushblock_s {};
struct ripple_node_s {};
struct ripple_set_s {};
struct rtldata_s {};
struct rtlset {};
struct shopitem_s {};
struct specialsfx_s {};
struct speedup_s {};
struct starfighter_s {};
struct terrsitu_s {};
struct tertype {};
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
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeStill : FadeBase {
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeStillWipe : FadeBase {
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct FadeSystem {
    char pad_0x00[0x4];
    float fade; // 0x04  current fade amount
    char pad_0x08[0xc - 0x08];
    i32 busy; // 0x0c  non-zero while a fade transition runs
    char pad_0x10[0x24 - 0x10];
    i32 pending_type; // 0x24  fade type queued by SetFade / stage marker
    void AddFade(FadeBase *);
    void Draw();
    void Init();
    void SetFade(FADETYPE const &, u32);
    void SetStage(char);
    void Update();
};
struct FadeWipe : FadeBase {
    void DrawFade();
    void Init(FADEINFO_s *);
    void InitFade();
    void UpdateFade();
};
struct GIZBUILDIT_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZFORCE_s {
    char pad_0x00[0x40];
    void *field_0x40; // 0x40
    float field_0x44; // 0x44  force strength threshold / start value
    char pad_0x48[0x6c - 0x48];
    float strength_0x6c;
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZMOBLOWUP_s {
    undefined field0_0x0[0x50];
    char field_0x50[0x4f];      // 0x50 .. 0x9f
    u8 field_0x9f;              // 0x9f  state/flags byte
    i32 field_0xa0;             // 0xa0
    char field_0xa4[0x8];       // 0xa4 .. 0xac
    void *field_0xac;           // 0xac
    float field_0xb0;           // 0xb0
    undefined field_0xb4[0x6c]; // 0xb4 .. 0x120
    void *field_0x120;          // 0x120
    u8 field_0x124;             // 0x124
    undefined field_0x125[3];
    float field_0x128; // 0x128
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GIZOBSTACLE_s {
    char pad_0x00[0x18];
    f32 field_0x18; // 0x18
    f32 field_0x1c; // 0x1c
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    char pad_0x28[0x34 - 0x28];
    void *field_0x34; // 0x34
    char pad_0x38[0x3c - 0x38];
    f32 field_0x3c; // 0x3c
    char pad_0x40[0xa1 - 0x40];
    u8 field_a1_0xa1;
    char pad_0xa2[0xdc - 0xa2];
    i32 field_0xdc; // 0xdc
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};

// Node in the obstacle linked list (field_0x34 → field_0x18 chain).
struct GIZOBSTACLENODE_s {
    void *next;    // 0x00
    void *special; // 0x04
    char pad_0x08[0x18 - 0x08];
    void *field_0x18; // 0x18
};

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
};
struct GIZTURRET_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct GameThingManager; // defined after ThingManager (derives from it)
struct HATMACHINE_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct HudRadarPulse {
    HudRadarPulse(VuVec const &);
    void IsFinished();
    void Process(float);
    void Render();
};
struct LEVER_s {
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct LevelEditor {
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
    char pad_0x00[0xc];     // 0x00  holds the "mine" special handle at 0x00
    MINEENTRY_s mines[64];  // 0x0c .. 0x70c
    float mine_radius;      // 0x70c NuSpecialGetOriginRadius(mine special)
    float spawn_timer;      // 0x710 seeded from camera zoom / 1000000000.0f sentinel
    void *nomine_areas[10]; // 0x714 AISysFindArea("nomine_N")
    i16 nomine_count;       // 0x73c
    i16 mine_debris;        // 0x73e FindGameDebris("MINE_POP")
    i32 mine_part;          // 0x740 PARTLookupType("POD_MINE_PART")
    float update_timer;     // 0x744
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
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
struct PartObjectInterface {
    void GetPos(VuVec &, i32) const;
    void GetRadius() const;
    void GetTargetName() const;
    PartObjectInterface(PART_s &);
    virtual ~PartObjectInterface();
};
struct Placeable {
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
    void ClearMechObjectInterface();
    void GetMechObjectInterface();
};
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
