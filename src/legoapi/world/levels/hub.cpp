#include "globals.h"
#include "batman.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/numtx.h"

extern void Customiser_LoadAll(CUSTOMISER *, WORLDINFO_s *);
extern void Customiser_Init(CUSTOMISER *);
extern void Customiser_Reset(CUSTOMISER *);
extern void Customiser_Draw3D(CUSTOMISER *);
extern void Customiser_Update(CUSTOMISER *, WORLDINFO_s *);
extern void Store_RestorePurchases();
extern void Store_HubDrawFloorTargets(WORLDINFO_s *);
extern void Store_HubInitFloorTargets(WORLDINFO_s *);
extern void InitShop(WORLDINFO_s *);
extern void DrawShop3D(WORLDINFO_s *);
extern void CutScenePlayer_Reset();
extern GIZBUILDIT_s *GizBuildIt_Find(WORLDINFO_s *, char *);
extern void GizBuildIt_SetToEnd(GIZBUILDIT_s *);
extern i32 Episode_CountOpenAreas(i32, i32, AREASAVE_s *);
extern void UpdateCharacterLoad();
extern void ResetForceBack();
extern void Hint_CancelCurrent();
extern f32 SeekLinearF(f32, f32, f32);
extern i32 qrand();
extern void Door_GoThrough(WORLDINFO_s *, DOOR_s *, i32);
extern STOREPACK StorePack[11];
extern GameObject_s *FindGameObject(i32, u32, i32, i32, i32);
extern void Store_RootPackCustodian(i32, GameObject_s *);
extern void NeedScreenGrab(i32);
extern void BackDrop_ResetColours();
extern void NewGameMode();
extern f32 MainRenderTargetTime;
extern void ResetIconWibble();
extern void MakeFreePlayModelList(i32 first_model, i32 second_model, i32 area, i32 level, i32 include_bonus);

// These two arrays are generic level-loader state rather than hub-owned state.
extern u64 LevHSpecialExists;
extern GIZBUILDIT_s *LevBuildIt[4];

typedef void (*HUBCALLBACK)(WORLDINFO_s *);

HUBCALLBACK Hub_UpdateAIFn = NULL;
HUBCALLBACK Hub_ResetAIFn = NULL;
HUBCALLBACK Hub_InitAIFn = NULL;

u8 Hub_LowEnd_IconsInsteadOfModels = 0;
f32 Hub_PreventDropOutTime = 0.0f;
f32 Hub_HologramTargetAlpha = 0.0f;
f32 Hub_HologramAlpha = 0.0f;
f32 hub_jabbaawake = 0.0f;
i32 hub_new_level = 0;
f32 hub_buildit_time = 0.0f;
i32 hub_buildit = 0;
f32 hub_area_time = 0.0f;
i32 hub_area = 0;
f32 hub_episode_time = 0.0f;
i32 hub_episode = 0;
f32 hub_minikitarea_opentime = 0.0f;
f32 hub_minikitarea_time = 0.0f;
i32 hub_minikitarea = 0;
i32 last_hub_area = 0;
f32 statstime = 0.0f;
f32 cointotaltime = 0.0f;
f32 goldbricktime = 0.0f;
u8 hub_custodians_finished_loading = 0;
i32 hub_freeplay_area = 0;
i32 freeplaymode = 0;
i32 freeplay_selected[2] = {};
f32 uprepeattime[2] = {};
f32 rightrepeattime[2] = {};
f32 downrepeattime[2] = {};
f32 leftrepeattime[2] = {};
u8 uprepeatcount[2] = {};
u8 rightrepeatcount[2] = {};
u8 downrepeatcount[2] = {};
u8 leftrepeatcount[2] = {};
u8 hub_makefreeplaylist_addotherid = 0;

static i32 buildits_reset = 0;
static GIZMO *hub_minikitviewer_gizmo = NULL;
static NUGSPLINE *hub_minikitviewer_camspl = NULL;
static f32 freeplaytime = 0.0f;
static f32 freeplayduration = 0.0f;
static i32 fpcount = 0;
static APICHARACTERMODELLIST_s fplist[341] = {};

void Hub_ClearStats();
void Hub_UpdateMiniKits(WORLDINFO_s *);
void Hub_InitFreePlaySelect(i32, i32, i32);
static void Hub_MakeFreePlayList(i32 first_model, i32 second_model);

enum HUB_DOOR_MENU_ID {
    HUB_DOOR_MENU_STANDARD = 15,
    HUB_DOOR_MENU_BONUS = 16,
    HUB_DOOR_MENU_VEHICLE = 17,
    HUB_DOOR_MENU_EPISODE_BONUS = 18,
};

enum HUB_VEHICLE_COLLECTION_MASK : u32 {
    HUB_VEHICLE_COLLECTION_ALLOWED = 0x04002000,
    HUB_VEHICLE_COLLECTION_REQUIRED = 0x00002000,
};

enum HUB_FREEPLAY_MODEL_FLAGS : u32 {
    HUB_FREEPLAY_MODEL_VEHICLE = 0x00002000,
    HUB_FREEPLAY_MODEL_MINIKIT = 0x04000000,
};

enum HUB_AUDIO_EVENT {
    HUB_AUDIO_EVENT_OPEN_DOOR_MENU = 0x2d,
};

enum {
    HUB_VEHICLE_ID_CAPACITY = 346,
};

struct HUBAREAINFO_s {
    const char *area_name;
    const char *door_name;
    const char *lock_name;
    const char *bonus_gizmo_name;
    const char *bonus_gizmo_name_2;
    f32 panel_offset;
    f32 panel_scale;
    i32 flags;
    AREADATA_s *area;
    GIZMO *door;
    nuhspecial_s lock;
    GIZMO *bonus_gizmo;
    GIZMO *bonus_gizmo_2;
};

struct HUBEPISODEINFO_s {
    i16 episode;
    i16 flags;
    const char *door_name;
    const char *lock_on_name;
    const char *lock_off_name;
    i32 force_open;
    EPISODEDATA *data;
    GIZMO *door;
    nuhspecial_s lock_on;
    nuhspecial_s lock_off;
};

#if UINTPTR_MAX == 0xffffffff
DECOMP_ASSERT(sizeof(HUBAREAINFO_s) == 0x3c, "HUBAREAINFO_s size");
DECOMP_ASSERT(sizeof(HUBEPISODEINFO_s) == 0x34, "HUBEPISODEINFO_s size");
#endif

// The original table is ordered exactly like areas.txt.  The last two entries
// are the network/bonus doors and the null record terminates every hub scan.
static HUBAREAINFO_s HubAreaInfo[] = {{"negotiations", "de1_1", "lock_1_1_on", NULL, NULL, 0.1f, 1.0f, 1},
                                      {"gungan", "de1_2", "lock_1_2_on", NULL, NULL, 0.025f, 0.8f, 1},
                                      {"palacerescue", "de1_3", "lock_1_3_on", NULL, NULL, 0.0f, 0.5f, 1},
                                      {"podsprint", "de1_4", "lock_1_4_on", NULL, NULL, 0.025f, 1.0f, 1},
                                      {"retakepalace", "de1_5", "lock_1_5_on", NULL, NULL, 0.075f, 1.0f, 1},
                                      {"maul", "de1_6", "lock_1_6_on", NULL, NULL, 0.225f, 1.0f, 1},
                                      {"e1vehiclebonus", "de1_7", "lock_1_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"pursuit", "de2_1", "lock_2_1_on", NULL, NULL, 0.125f, 1.0f, 1},
                                      {"kamino", "de2_2", "lock_2_2_on", NULL, NULL, 0.075f, 1.0f, 1},
                                      {"factory", "de2_3", "lock_2_3_on", NULL, NULL, 0.25f, 1.0f, 1},
                                      {"jedi", "de2_4", "lock_2_4_on", NULL, NULL, 0.0125f, 1.0f, 1},
                                      {"gunship", "de2_5", "lock_2_5_on", NULL, NULL, 0.11f, 1.0f, 1},
                                      {"dooku", "de2_6", "lock_2_6_on", NULL, NULL, 0.42f, 0.5f, 1},
                                      {"e2vehiclebonus", "de2_7", "lock_2_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"dogfight", "de3_1", "lock_3_1_on", NULL, NULL, 0.275f, 1.0f, 1},
                                      {"cruiser", "de3_2", "lock_3_2_on", NULL, NULL, 0.02f, 1.0f, 1},
                                      {"grievous", "de3_3", "lock_3_3_on", NULL, NULL, 0.1375f, 1.0f, 1},
                                      {"kashyyyk", "de3_4", "lock_3_4_on", NULL, NULL, 0.01f, 1.0f, 1},
                                      {"temple", "de3_5", "lock_3_5_on", NULL, NULL, 0.025f, 1.0f, 1},
                                      {"vader", "de3_6", "lock_3_6_on", NULL, NULL, 0.21f, 1.0f, 1},
                                      {"e3vehiclebonus", "de3_7", "lock_3_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"blockaderunner", "de4_1", "lock_4_1_on", NULL, NULL, 0.16f, 1.0f, 1},
                                      {"tatooine", "de4_2", "lock_4_2_on", NULL, NULL, 0.01f, 1.0f, 1},
                                      {"moseisley", "de4_3", "lock_4_3_on", NULL, NULL, 0.02f, 1.0f, 1},
                                      {"deathstarrescue", "de4_4", "lock_4_4_on", NULL, NULL, 0.17f, 1.0f, 1},
                                      {"deathstarescape", "de4_5", "lock_4_5_on", NULL, NULL, 0.025f, 1.0f, 1},
                                      {"deathstarbattle", "de4_6", "lock_4_6_on", NULL, NULL, 0.165f, 1.0f, 1},
                                      {"e4vehiclebonus", "de4_7", "lock_4_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"hothbattle", "de5_1", "lock_5_1_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"hothescape", "de5_2", "lock_5_2_on", NULL, NULL, 0.01f, 1.0f, 1},
                                      {"asteroidchase", "de5_3", "lock_5_3_on", NULL, NULL, 0.1825f, 1.0f, 1},
                                      {"dagobah", "de5_4", "lock_5_4_on", NULL, NULL, 0.165f, 1.0f, 1},
                                      {"cloudcitytrap", "de5_5", "lock_5_5_on", NULL, NULL, 0.1f, 1.0f, 1},
                                      {"cloudcityescape", "de5_6", "lock_5_6_on", NULL, NULL, 0.0125f, 1.0f, 1},
                                      {"e5vehiclebonus", "de5_7", "lock_5_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"jabbaspalace", "de6_1", "lock_6_1_on", NULL, NULL, 0.01f, 1.0f, 1},
                                      {"sarlaccpit", "de6_2", "lock_6_2_on", NULL, NULL, 0.01f, 0.8f, 1},
                                      {"speederchase", "de6_3", "lock_6_3_on", NULL, NULL, 0.165f, 1.0f, 1},
                                      {"endorbattle", "de6_4", "lock_6_4_on", NULL, NULL, 0.01f, 1.0f, 1},
                                      {"emperorfight", "de6_5", "lock_6_5_on", NULL, NULL, 0.245f, 1.0f, 1},
                                      {"deathstar2battle", "de6_6", "lock_6_6_on", NULL, NULL, 0.1825f, 1.0f, 1},
                                      {"e6vehiclebonus", "de6_7", "lock_6_7_on", NULL, NULL, 0.0f, 1.0f, 1},
                                      {"podrace", "de7_1", "lock_7_1_on", "frame_1", NULL, 0.0f, 0.0f, 1},
                                      {"anakinsflight", "de7_2", "lock_7_2_on", "frame_2", NULL, 0.0f, 0.0f, 1},
                                      {"bonus_gunship", "de7_3", "lock_7_3_on", "frame_3", NULL, 0.0f, 0.0f, 1},
                                      {"anewhope", "de7_4", "lock_7_4_on", "frame_4", NULL, 0.0f, 0.0f, 1},
                                      {"bonus2", "de7_5", "lock_7_5_on", "frame_5", NULL, 0.0f, 0.0f, 1},
                                      {"bonus", "de7_6", "lock_7_6_on", "frame_6", NULL, 0.0f, 0.0f, 1},
                                      {"losttemple", "de7_7", "lock_7_7_on", NULL, NULL, 0.0f, 0.0f, 0x101},
                                      {"senate", "network_door", NULL, NULL, NULL, 0.0f, 0.0f, 0x101},
                                      {}};

static HUBEPISODEINFO_s HubEpisodeInfo[] = {
    {0, 1, "DE1", "lock_1_on", "lock_1_off"},    {1, 1, "DE2", "lock_2_on", "lock_2_off"},
    {2, 1, "DE3", "lock_3_on", "lock_3_off"},    {3, 1, "DE4", "lock_4_on", "lock_4_off"},
    {4, 1, "DE5", "lock_5_on", "lock_5_off"},    {5, 1, "DE6", "lock_6_on", "lock_6_off"},
    {8, 1, "DE7", "lock_7_on", "lock_7_off", 1}, {6, 2, "hologram", "lock_8_on", "lock_8_off"},
    {7, 2, "jabba", "lock_B_on", "lock_B_off"},  {-1}};

f32 HUB_AREAPANELX_ONETRUEJEDIGOLDBRICK[6] = {-0.3f, 0.0f, 0.6f, -0.6f, 0.0f, 0.3f};
static f32 HUB_AREAPANELX_TWOTRUEJEDIGOLDBRICKS[6] = {-0.15f, -0.45f, 0.75f, -0.75f, 0.45f, 0.15f};
static f32 HUB_AREAPANELX_1TRUEJEDIGB_NOCHALLENGE[6] = {-0.201f, 0.201f, 0.6f, -0.6f, 0.0f, 0.0f};
f32 *HUB_AREAPANELX = HUB_AREAPANELX_ONETRUEJEDIGOLDBRICK;

void Hub_Draw3D(WORLDINFO_s *world) {
    Store_HubDrawFloorTargets(world);

    // The original draws the six episode-completion signs here before the
    // persistent hub systems.  Those signs are presentation-only; the three
    // calls below are the stateful draw chain used during ordinary play.
    Customiser_Draw3D(CharacterCustomiser);
    DrawShop3D(world);
    Hub_DrawMiniKits(world);

    nuhspecial_s *display = NULL;
    if (Store_IsPackUnlocked(6) == 0) {
        if (NuSpecialExistsFn(&LevHSpecial[16]) != 0 && NuSpecialExistsFn(&LevHSpecial[4]) != 0) {
            NUMTX draw_mtx = *NuSpecialGetDrawMtx(&LevHSpecial[16]);
            NuSpecialDrawAt(&LevHSpecial[4], &draw_mtx);
        }
    }

    display = &LevHSpecial[Store_IsPackUnlocked(5) != 0 ? 5 : 4];
    if (NuSpecialExistsFn(display) != 0) {
        NuSpecialDrawAt(display, &LevMtx);
    }
}

i32 Hub_InMenu() {
    const i32 menu = GetMenuID();
    if (menu == 8 || menu == 12 || menu == 13 || menu == 14 || (menu >= 15 && menu <= 23)) {
        return 1;
    }
    return 0;
}

void Hub_Update(WORLDINFO_s *world) {
    const i32 menu = GetMenuID();

    if (HubStartDoor != NULL) {
        DOOR_s *door = static_cast<DOOR_s *>(HubStartDoor);
        hub_new_level = door->level;
        Door_GoThrough(world, door, 1);
        HubStartDoor = NULL;
    }

    // With background loading enabled, the hub does not start its live logic
    // until all locked-pack custodian character models are resident.
    if (hub_custodians_finished_loading == 0) {
        if (BGLOAD == 0) {
            hub_custodians_finished_loading = 1;
        } else {
            i32 required = 0;
            i32 loaded = 0;
            for (i32 pack = 0; pack < 11; ++pack) {
                STOREPACK &store_pack = StorePack[pack];
                if (Store_IsPackUnlocked(pack) != 0 || store_pack.id == NULL) {
                    continue;
                }
                const i32 id = *store_pack.id;
                if (id == -1) {
                    continue;
                }
                ++required;
                if (APICharacterLoaded(id) != NULL) {
                    ++loaded;
                    const i32 camera_socket = store_pack.field44_0x32;
                    if ((camera_socket == 0xff || camera_socket == GameCam->sock_position.location.sock) &&
                        FindGameObject(id, 0, 0, 0, 0) == NULL) {
                        GameObject_s *custodian =
                            AddDynamicCreature(id, &store_pack.custodian_position, store_pack.custodian_angle,
                                               const_cast<char *>("party"), NULL, NULL, 1, NULL, NULL, 0, 1);
                        if (custodian != NULL) {
                            custodian->field_0xee8 = store_pack.custodian_position.x;
                            custodian->field_0xeec = store_pack.custodian_position.z;
                            custodian->field_0x106e = store_pack.custodian_angle;
                            Store_RootPackCustodian(pack, custodian);
                        }
                    }
                }
            }
            if (loaded == required) {
                hub_custodians_finished_loading = 1;
            }
        }
    }

    if (buildits_reset == 0) {
        const u8 completed_buildits = Game.field_0x7c26[2];
        for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
            if (i < 8 && HubAreaInfo[i].bonus_gizmo != NULL && (completed_buildits & (1U << i)) != 0) {
                GizBuildIt_SetToEnd(static_cast<GIZBUILDIT_s *>(HubAreaInfo[i].bonus_gizmo->object));
            }
        }
        if ((completed_buildits & 0x80) != 0 && LevGizmo[0] != NULL) {
            GizBuildIt_SetToEnd(static_cast<GIZBUILDIT_s *>(LevGizmo[0]->object));
        }
        ++buildits_reset;
    }

    if (MenuInMemoryCard() != 0) {
        Hub_ClearStats();
        Hub_PreventDropOutTime = 1.0f;
    } else if (menu >= 12 && menu <= 14) {
        Hub_PreventDropOutTime = 1.0f;
    }

    if (menu == 8) {
        hub_jabbaawake = 1.0f;
    } else if (hub_jabbaawake > 0.0f) {
        hub_jabbaawake -= FRAMETIME * 0.5f;
        if (hub_jabbaawake < 0.0f) {
            hub_jabbaawake = 0.0f;
        }
    }

    Hub_HologramAlpha = SeekLinearF(Hub_HologramAlpha, Hub_HologramTargetAlpha, FRAMETIME * 2.0f);
    if (Hub_HologramAlpha == Hub_HologramTargetAlpha) {
        Hub_HologramTargetAlpha = static_cast<f32>(qrand()) * (1.0f / 2147483647.0f) * 2.0f - 1.0f;
    }
    if (Hub_PreventDropOutTime > 0.0f) {
        Hub_PreventDropOutTime -= FRAMETIME;
    }

    UpdateCharacterLoad();
    Hub_UpdateMiniKits(world);

    if (hub_buildit_time != 0.0f) {
        hub_buildit_time = SeekLinearF(hub_buildit_time, 0.0f, FRAMETIME * 2.0f);
    }
    if (Hub_UpdateAIFn != NULL) {
        Hub_UpdateAIFn(world);
    }

    if (hub_episode != -1 || hub_area != -1 || hub_minikitarea != -1 || hub_buildit != -1) {
        Hint_CancelCurrent();
    } else {
        const i32 active_menu = GetMenuID();
        if (active_menu != -1 && active_menu != 13 && (active_menu != 15 || hub_new_level == -1)) {
            Hint_CancelCurrent();
        }
    }
    ResetForceBack();
}

void Hub_DrawPanel(WORLDINFO_s *) {
}

i32 Hub_PanelBusy() {
    if (hub_episode != -1) {
        return 1;
    }
    if (hub_area != -1) {
        return 1;
    }
    if (hub_minikitarea != -1) {
        return 1;
    }
    if (hub_buildit != -1) {
        return 1;
    }

    if (GetMenuID() == 20) {
        return 1;
    }
    if (GetMenuID() == 21) {
        return 1;
    }
    if (GetMenuID() == 22) {
        return 1;
    }
    if (GetMenuID() == 23) {
        return 1;
    }
    return 0;
}

void Hub_UpdateKit() {
}

void Hub_CallBarman(GameObject_s *) {
}

void Hub_ClearStats() {
    statstime = 0.0f;
    cointotaltime = 0.0f;
    goldbricktime = 0.0f;
}

void Hub_ResetPanel() {
    hub_episode = -1;
    hub_episode_time = 0.0f;
    hub_area = -1;
    hub_area_time = 0.0f;
    hub_buildit = -1;
    hub_buildit_time = 0.0f;
    hub_minikitarea = -1;
    hub_minikitarea_time = 0.0f;
}

bool HubShopUnlocked() {
    return true;
}

i32 Hub_BonusBuildIt(GIZBUILDIT_s *buildit) {
    for (HUBAREAINFO_s *info = HubAreaInfo; info->area_name != NULL; ++info) {
        if (info->bonus_gizmo != NULL && info->bonus_gizmo->object == buildit) {
            return 1;
        }
    }
    return 0;
}

void Hub_DrawMiniKits(WORLDINFO_s *) {
}

void Hub_InitMiniKits(WORLDINFO_s *) {
}

void Hub_DrawAreaStats(float, i32, i32) {
}

void Hub_DrawStarField() {
}

void Hub_MakeModelList() {
    Hub_ModelList[0] = {static_cast<i16>(PlayerID[0]), 1};
    Hub_ModelList[1] = {static_cast<i16>(PlayerID[1]), 1};

    i32 count = 2;
    if (HUB_ADATA != NULL && HUB_ADATA->hub_player_ids != NULL) {
        i16 *model_id = HUB_ADATA->hub_player_ids;
        while (count < 5 && *model_id != -1) {
            Hub_ModelList[count++] = {*model_id++, 1};
        }
    }
    Hub_ModelList[count].model_id = -1;

    PlayerList[0] = Hub_ModelList[0].model_id;
    PlayerList[1] = Hub_ModelList[1].model_id;
    PlayerList[2] = -1;
}

void Hub_UpdateMiniKits(WORLDINFO_s *) {
}

void Hub_LockUnlockDoors(WORLDINFO_s *) {
}

void Hub_ActivateDoorMenu(LEVELDATA_s **level) {
    if ((*level)->area_index == -1) {
        return;
    }

    const i32 area = last_hub_area;
    if (area != (*level)->area_index) {
        *level = NULL;
        return;
    }

    MakeMenuPacket();
    hub_new_level = ADataList[area].levels[0];
    *level = NULL;
    hub_episode_time = 0.0f;
    hub_area_time = 0.0f;
    MainRenderTargetTime = 0.0f;
    NeedScreenGrab(1);
    BackDrop_ResetColours();

    i32 menu_id;
    if (area != -1 && (ADataList[area].flags & AREAFLAG_BONUS_AREA) != 0) {
        menu_id = HUB_DOOR_MENU_BONUS;
        if (ADataList[area].episode_index != -1) {
            menu_id = HUB_DOOR_MENU_EPISODE_BONUS;
        }
    } else if (VEHICLES_ADATA != NULL && area == VEHICLES_ADATA->index) {
        i16 vehicle_ids[HUB_VEHICLE_ID_CAPACITY];
        i32 vehicle_count;
        Collection_GetIDList(&VehicleCollection, HUB_VEHICLE_COLLECTION_ALLOWED, HUB_VEHICLE_COLLECTION_REQUIRED,
                             vehicle_ids, &vehicle_count, NULL, 0);
        menu_id = HUB_DOOR_MENU_VEHICLE;
        hub_freeplaysource = -1;
        Hub_InitFreePlaySelect(area, vehicle_count, -1);
    } else {
        menu_id = HUB_DOOR_MENU_STANDARD;
        if (LOSTTEMPLE_ADATA != NULL && area == LOSTTEMPLE_ADATA->index) {
            menu_id = HUB_DOOR_MENU_EPISODE_BONUS;
        }
    }

    GameAudio_PlaySfx(HUB_AUDIO_EVENT_OPEN_DOOR_MENU, NULL, 0, 0);
    Hint_CancelCurrent();
    NewMenu(menu_id, -1, -1);
}

bool HubCustomiserUnlocked() {
    return true;
}

void Hub_DrawFreePlaySelect() {
}

void Hub_DrawImportantBrick(i32, float, float, float, i32, i32) {
}

void Hub_InitFreePlaySelect(i32 area, i32 first_model, i32 second_model) {
    COLLECTION_s *collection = GetFreePlayCollection(area);
    if (collection == &VehicleCollection) {
        if (first_model == -1) {
            first_model = VehicleCollection.list[0].id;
        }
        if (second_model == -1) {
            second_model = VehicleCollection.list[1].id;
        }

        Hub_MakeFreePlayList(first_model, second_model);
        if (first_model != -1) {
            MenuPacket.player_model[0] = static_cast<i16>(first_model);
            MenuPacket.player_model[1] = static_cast<i16>(second_model);
        } else {
            MenuPacket.player_model[0] = Player[0] != NULL ? Player[0]->id : -1;
            MenuPacket.player_model[1] = Player[1] != NULL ? Player[1]->id : -1;
        }
    } else if (collection == &MiniKitCollection) {
        if (first_model == -1) {
            first_model = MiniKitCollection.list[0].id;
        }
        if (second_model == -1) {
            second_model = MiniKitCollection.list[1].id;
        }

        Hub_MakeFreePlayList(first_model, second_model);
        if (first_model != -1 && second_model != -1) {
            MenuPacket.player_model[0] = static_cast<i16>(first_model);
            MenuPacket.player_model[1] = static_cast<i16>(second_model);
        } else {
            MenuPacket.player_model[0] = Player[0] != NULL ? Player[0]->id : -1;
            MenuPacket.player_model[1] = Player[1] != NULL ? Player[1]->id : -1;
        }
    } else {
        Hub_MakeFreePlayList(-1, -1);
        MenuPacket.player_model[0] = Player[0] != NULL ? Player[0]->id : -1;
        MenuPacket.player_model[1] = Player[1] != NULL ? Player[1]->id : -1;
    }

    freeplaytime = 0.0f;
    freeplaymode = 0;
    freeplayduration = 1.0f;
    freeplay_selected[0] = 0;
    freeplay_selected[1] = 0;
    for (i32 player_index = 0; player_index < 2; ++player_index) {
        uprepeattime[player_index] = 0.0f;
        downrepeattime[player_index] = 0.0f;
        leftrepeattime[player_index] = 0.0f;
        rightrepeattime[player_index] = 0.0f;
    }
    ResetIconWibble();
    hub_freeplay_area = area;
}

bool HubMinikitViewerUnlocked() {
    return true;
}

void Hub_UpdateFreePlaySelect() {
}

static void Hub_FindSpecial(WORLDINFO_s *world, nuhspecial_s *special, const char *name) {
    NuSpecialFind(world->current_gscn, special, const_cast<char *>(name), 1);
}

void Hub_Init(WORLDINFO_s *world) {
    static NUVEC arcadelightpos = {-22.49f, 0.79f, -49.9975f};

    Store_RestorePurchases();
    Hub_LowEnd_IconsInsteadOfModels = 0;

    if (BOTHTRUEJEDIGOLDBRICKS != 0) {
        HUB_AREAPANELX = HUB_AREAPANELX_TWOTRUEJEDIGOLDBRICKS;
    } else if (Store_IsPackUnlocked(8) == 0) {
        HUB_AREAPANELX = HUB_AREAPANELX_1TRUEJEDIGB_NOCHALLENGE;
    } else {
        HUB_AREAPANELX = HUB_AREAPANELX_ONETRUEJEDIGOLDBRICK;
    }

    for (HUBAREAINFO_s *info = HubAreaInfo; info->area_name != NULL; ++info) {
        info->door = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>(info->door_name));
        if (info->bonus_gizmo_name != NULL) {
            info->bonus_gizmo = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>(info->bonus_gizmo_name));
        }
        if (info->bonus_gizmo_name_2 != NULL) {
            info->bonus_gizmo_2 = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>(info->bonus_gizmo_name_2));
        }
        info->area = Area_FindByName(const_cast<char *>(info->area_name), NULL);
        if (info->lock_name != NULL) {
            Hub_FindSpecial(world, &info->lock, info->lock_name);
        }
    }

    for (HUBEPISODEINFO_s *info = HubEpisodeInfo; info->episode != -1; ++info) {
        info->data = NULL;
        if (info->episode >= 0 && info->episode < EPISODECOUNT) {
            info->data = &EDataList[info->episode];
        }
        info->door = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>(info->door_name));
        Hub_FindSpecial(world, &info->lock_on, info->lock_on_name);
        Hub_FindSpecial(world, &info->lock_off, info->lock_off_name);
    }

    hub_minikitviewer_gizmo =
        GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, const_cast<char *>("conveyor_test"));
    hub_minikitviewer_camspl = NuSplineFind(world->current_gscn, const_cast<char *>("mini_cam_e1_1"));
    LevGizObst[7] = GizObstacle_FindByName(world->giz_obstacle_sys, const_cast<char *>("network_door"));
    LevGizObst[6] = GizObstacle_FindByName(world->giz_obstacle_sys, const_cast<char *>("DE7"));

    static const char *const lock_names[] = {
        "lock_4_off", "lock_4_on",    "lock_5_off",  "lock_5_on",    "lock_6_off",  "lock_6_on",    "lock_b_off",
        "lock_b_on",  "lock_4_7_off", "lock_4_7_on", "lock_5_7_off", "lock_5_7_on", "lock_6_7_off", "lock_6_7_on",
    };
    for (i32 i = 0; i < 14; ++i) {
        Hub_FindSpecial(world, &LevHSpecial[i], lock_names[i]);
    }

    if (NuSpecialFind(world->current_gscn, &LevHSpecial[15], const_cast<char *>("holo"), 1) != 0) {
        NuSpecialSetVisibility(&LevHSpecial[15], 0);
    }
    if (NuSpecialFind(world->current_gscn, &LevHSpecial[16], const_cast<char *>("lock_7_on"), 1) != 0 &&
        Store_IsPackUnlocked(6) == 0) {
        NuSpecialSetVisibility(&LevHSpecial[16], 0);
    }
    NuSpecialFindMulti(world->current_gscn, &LevHSpecial[20], const_cast<char *>("ps2_only_"), 32, 0);
    NuSpecialFindMulti(world->current_gscn, &LevHSpecial[52], const_cast<char *>("psp_only_"), 32, 0);
    Hub_FindSpecial(world, &LevHSpecial[84], "fake_wall");

    Hub_ResetPanel();
    InitShop(world);
    Hub_LockUnlockDoors(world);
    Hub_InitMiniKits(world);
    LevTime[0] = 3.0f;
    LevTime[1] = 3.0f;
    Hub_HologramAlpha = 0.0f;
    Hub_HologramTargetAlpha = 0.0f;
    Customiser_Init(CharacterCustomiser);
    if (Hub_InitAIFn != NULL) {
        Hub_InitAIFn(world);
    }

    hub_startoutsidebonusdoor_area = -1;
    CutScenePlayer_Reset();
    LevBuildIt[0] = GizBuildIt_Find(world, const_cast<char *>("fountain"));
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>("fountain"));
    if (LevBuildIt[0] == NULL || LevGizmo[0] == NULL) {
        LevBuildIt[0] = NULL;
        LevGizmo[0] = NULL;
    }
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, -1, const_cast<char *>("garageLever"));
    Arcade = 0;
    NuMtxSetRotationZ(&LevMtx, 0x4000);
    NuMtxRotateY(&LevMtx, 0xf600);
    NuMtxTranslate(&LevMtx, &arcadelightpos);
    Store_HubInitFloorTargets(world);
    hub_custodians_finished_loading = 0;
}

// The level callback carries the streaming-buffer arguments used by most
// level loaders, but the original hub loader only forwards the world to the
// persistent character customiser.
void Hub_Load(WORLDINFO_s *world, variptr_u *, variptr_u *) {
    Customiser_LoadAll(CharacterCustomiser, world);
}

static void Hub_SetDoorState(GIZMO *door, nuhspecial_s *lock, i32 open) {
    if (door != NULL && door->object != NULL) {
        GIZOBSTACLE_s *obstacle = static_cast<GIZOBSTACLE_s *>(door->object);
        if (open != 0) {
            obstacle->runtime_flags &= static_cast<u8>(~GIZOBSTACLE_RUNTIME_FLAG_BLOCKED);
        } else {
            obstacle->runtime_flags |= GIZOBSTACLE_RUNTIME_FLAG_BLOCKED;
        }
    }
    NuSpecialSetVisibility(lock, open != 0);
}

void Hub_Reset(WORLDINFO_s *world) {
    Customiser_Reset(CharacterCustomiser);
    if (Hub_ResetAIFn != NULL) {
        Hub_ResetAIFn(world);
    }

    for (i32 i = 20; i < 52; ++i) {
        NuSpecialSetVisibility(&LevHSpecial[i], static_cast<i32>((LevHSpecialExists >> i) & 1));
    }

    if (NuSpecialExistsFn(&LevHSpecial[84]) != 0) {
        NuSpecialSetVisibility(&LevHSpecial[84], 0);
    }

    for (HUBAREAINFO_s *info = HubAreaInfo; info->area_name != NULL; ++info) {
        if (info->door == NULL || info->area == NULL) {
            continue;
        }
        i32 open = Episode_CountOpenAreas(info->area->episode_index, info->area->index, Game_AreaSave);
        Hub_SetDoorState(info->door, &info->lock, open);
    }

    for (HUBEPISODEINFO_s *info = HubEpisodeInfo; info->episode != -1; ++info) {
        if (info->door == NULL) {
            continue;
        }
        i32 open = info->force_open;
        if (open == 0) {
            open = Episode_CountOpenAreas(info->episode, -1, Game_AreaSave);
        }
        Hub_SetDoorState(info->door, &info->lock_on, open);
    }
    if (Store_IsPackUnlocked(6) == 0) {
        NuSpecialSetVisibility(&HubEpisodeInfo[8].lock_on, 0);
    }

    hub_jabbaawake = 0.0f;
    buildits_reset = 0;
    Hub_HologramAlpha = 0.0f;
    Hub_HologramTargetAlpha = 0.0f;
    Hub_PreventDropOutTime = 0.0f;
}

// Static Hub menu/drawing helpers. Stubbed to satisfy the symbol baseline.

static __used__ void Hub_DrawArcadeStats(float) {
}

static __used__ void Hub_DrawMiniKitCount(float, float, int, int, float) {
}

static void Hub_MakeFreePlayList(i32 first_model, i32 second_model) {
    fpcount = 0;
    const i32 area = LDataList[hub_new_level].area_index;

    if (hub_makefreeplaylist_addotherid != 0) {
        const i32 other_player = PlayerID[1];
        if (second_model == -1 && other_player != -1 && other_player != PlayerID[0] && other_player != first_model) {
            second_model = other_player;
        }
        hub_makefreeplaylist_addotherid = 0;
    }

    MakeFreePlayModelList(first_model, second_model, area, -1, 1);
    if ((ADataList[area].flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) ==
        (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) {
        for (i32 index = 0; index < FreePlayModelCount; ++index) {
            const i32 model = FreePlayModelList[index].model_id;
            if ((CDataList[model].model_flags & HUB_FREEPLAY_MODEL_MINIKIT) == 0 ||
                InCollectList_Index(model, MiniKitCollection.list, MiniKitCollection.count_y) == -1 ||
                PlayerList[0] == model || PlayerList[1] == model || Collection_Got(model) == 0) {
                continue;
            }
            fplist[fpcount++] = FreePlayModelList[index];
        }
    } else {
        const i32 selectable_count = FreePlayResidentCount + FreePlayBonusCount;
        for (i32 index = 2; index < selectable_count + 2 && FreePlayModelList[index].model_id != -1; ++index) {
            const i32 model = FreePlayModelList[index].model_id;
            const bool is_vehicle = (CDataList[model].model_flags & HUB_FREEPLAY_MODEL_VEHICLE) != 0;
            const bool area_uses_vehicles = area != -1 && (ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0;
            if ((area == -1 || is_vehicle == area_uses_vehicles) && Collection_Got(model) != 0) {
                fplist[fpcount++] = FreePlayModelList[index];
            }
        }
    }

    fplist[fpcount].model_id = -1;
    if (fpcount > 3) {
        for (i32 shuffle = 0; shuffle < 64; ++shuffle) {
            const i32 first_offset = qrand() / (0xffff / (fpcount - 2) + 1);
            const i32 first_index = first_offset + 2;
            const i32 second_offset = qrand() / (0xffff / (fpcount - 3) + 1);
            const i32 second_index = (second_offset + first_offset) % (fpcount - 2) + 2;
            const APICHARACTERMODELLIST_s saved = fplist[first_index];
            fplist[first_index] = fplist[second_index];
            fplist[second_index] = saved;
        }
    }
}

static __used__ void Hub_UpdateSelectMode() {
}

static __used__ void Hub_DrawBonusModeMenu(int, float) {
}

static __used__ void Hub_DrawSelectModeMenu(int, float) {
}

static __used__ void Hub_DrawSuperBonusStats(AREADATA_s *, float) {
}

void WipeBackToHub() {
    NewMenu(-1, -1, -1);
    ResetTimer(&JoinInTimer, 0.0f);
    NuStrCpy(Door_ExitName, Door_Last->name);
    StartDoorPositions();
    NewGameMode();
    bonusmodearcade = 0;
    HubMainRenderTimeHack = 1;
}
