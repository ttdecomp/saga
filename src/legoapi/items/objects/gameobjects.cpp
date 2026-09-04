#include "legoapi/items/objects/gameobjects.h"
#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/ai/core/ai_sys_stubs.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/area.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nuandroid/ios_graphics.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nucore/nustring.h"

#include <stdio.h>
#include <string.h>

// NuCore profiling timebars (nucore_plain.cpp): NuTimeBarCreateSet returns a
// deferred-subsystem stub handle; the slot functions are no-op stubs.
extern "C" {
    void *NuTimeBarCreateSet(i32);
    void _NuTimeBarSlotBegin(void *, i32, char const *);
    u32 _NuTimeBarSlotEnd(void *, i32);
    void AddToAIGroup(AIGROUP_s *group, APIOBJECT_s *object);
    extern NUVEC plr_lastpos;
}

// Written by ThingManager's ctor (original global @0x124f2e0, .bss).
extern void *theThingManager;

void legoSetMusicVolume(float);
void MovePlayer(GameObject_s *object);
void AnimatePlayer(GameObject_s *object);
void TerrainPlayer(GameObject_s *object);
void KeepOnScreen(GameObject_s *object);
void SetPlayer();
void InitPlayerAI(GameObject_s *object);
void ResetPlayerMoves(GameObject_s *object);
void SnapCreaturePos(GameObject_s *object, NUVEC *position, i32 angle, AIPATHINFO_s *path_info, i32 set_on_surface);
void MovePlayerSpline(GameObject_s *object);
void GetTopBot(GameObject_s *object);
void ResetRumble(RUMBLEPACKET *packet);
void ResetLights(NUVEC *position, rtldata_s *data, void *set);
void LightGameObject(GameObject_s *object, void *set);
void InitSurfaceInfo(GameObject_s *object);
i32 SetObjOnSurface(GameObject_s *object, i32 mode);
void PortalGameObject(GameObject_s *object, i32 enable, i32 immediate, i16 portal, nugscn_s *scene);
void Arcade_GetMode(u32 *mode);
void StarWars_GameAISysInit();
void GameAISysSetGame();
void ClearAICreatures();
APIOBJECT *GameAPIOBJECTFromObjID(u8 object_id);
i32 EquivalentObject_Find(WORLDINFO_s *world, nuhspecial_s *special);
void AIPathCnxControlSysReset(AIPATHCNXCONTROLSYS_s *system);
void AIPathCnxHelperSysReset(WORLDINFO_s *world, AIPATHCNXHELPERSYS_s *system);
void InitAICreatures(AISYS_s *system);
void ResetAICreatures(AISYS_s *system);
void LevelScriptReStoreProgress(WORLDINFO_s *world, LEVELSCRIPTPROCESS_s *process);
void GizmoSysAddGizmos(GIZMOSYS_s *gizmo_sys, GIZFLOW_s *giz_flow, void *world);

f32 Condition_InHubArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *);
void *Condition_InHubAreaInit(AISYS_s *, char *, AISCRIPT_s *);

extern i32 LEGO_AIPATHCNX_BLOCKAGE;

extern "C" i32 AISysSetLevelPath(AISYS_s *system, char *path_name);

extern "C" void NuLightFogX(f32 start, f32 end, u32 colour, f32 unused_start, f32 unused_end, i32 high_quality,
                            f32 density);

GAMEFOG_STATE GameFog = {};

static i32 GameFogSnap;
static i32 GameFogSet;
static f32 GameFogDuration;
static f32 GameFogTime;

enum AI_ACTION_SPEED_MODE : u8 {
    AI_ACTION_SPEED_LEGO = 0,
    AI_ACTION_SPEED_RUN = 1,
    AI_ACTION_SPEED_WALK = 2,
};

enum SCRIPT_ERROR_LEVEL : u32 {
    SCRIPT_ERROR_LEVEL_NONE = 0,
    SCRIPT_ERROR_LEVEL_WARNING = 1,
    SCRIPT_ERROR_LEVEL_STRICT = 2,
    SCRIPT_ERROR_LEVEL_COUNT = 3,
};

enum AI_DEFAULTS : u8 {
    AI_DEFAULT_ACTIVATE_DIFFICULTY = 1,
};

enum AI_OBJECT_ROUTE_STATE : u8 {
    AI_OBJECT_ROUTE_STATE_SCRIPT_VISIBLE = 3,
};

enum CHARACTER_AI_MODEL_FLAGS : u32 {
    CHARACTER_AI_MODEL_FLAG_SNAP_ON_BIG_JUMP = 0x00200000,
    CHARACTER_AI_MODEL_FLAG_DISABLE_RESPAWN = 0x00400000,
};

enum AI_GAME_OBJECT_TYPE : u8 {
    AI_GAME_OBJECT_TYPE_VEHICLE = 0x2b,
};

enum GAME_OBJECT_AI_UPDATE_FLAGS : u8 {
    GAME_OBJECT_AI_UPDATE_PROCESS = 0x08,
    GAME_OBJECT_AI_UPDATE_FORCED = 0x10,
    GAME_OBJECT_AI_UPDATE_SPECIAL_STATE = 0x20,
};

// Enabled in the target data image.  Ordinary background characters are
// staggered across frames; special movement states opt back into full-rate
// processing through the forced-update path.
i32 timebase_updates = 1;

static i32 GameObjectAIUpdateInterval(WORLDINFO_s *world, GameObject_s *object) {
    if (object->apiobj.field_0x27d == 0 || object->apiobj.character_data == NULL ||
        object->apiobj.character_data->field11_0x24 == NULL) {
        return 1;
    }

    // The target updates characters outside the visible portal set every ten
    // frames.  This avoids doing a full script, controller and terrain pass
    // for every off-screen Cantina inhabitant on the same frame.
    const i16 room = object->room_id;
    if (world == NULL || world->rooms_visible_ptr == NULL || room < 0 || world->rooms_visible_ptr[room] == 0 ||
        object->apiobj.model_draw_result == 0) {
        return 10;
    }

    const GAMECHARACTERDATA *character = static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
    const f32 distance = object->ai_update_distance;
    i32 interval = character->ai_update_interval_0;
    if (interval == 0 || distance <= character->ai_update_distance_0) {
        interval = character->ai_update_interval_1;
        if (interval == 0 || distance <= character->ai_update_distance_1) {
            interval = character->ai_update_interval_2;
            if (interval == 0 || distance <= character->ai_update_distance_2) {
                interval = character->ai_update_interval_3;
                if (interval == 0 || distance <= character->ai_update_distance_3) {
                    return 1;
                }
            }
        }
    }

    // Cadence entries are distance-tier sentinels: zero advances to the next
    // tier rather than selecting a zero-frame interval.  Values at or above
    // seven use the target's capped seven-frame cadence.
    return interval < 7 ? interval : 7;
}

static const f32 AI_RESPAWN_DELAY = 2.0f;

extern "C" {
    // Game-specific script registries are still populated incrementally as
    // their action and condition callbacks are reconstructed.  The null
    // terminators keep registration safe in the meantime.
    AICONDITIONDEF lego_aiconditiondefs[] = {
        {"SpecialAtStart", NULL, NULL},
        {"NumInSetAlive", NULL, NULL},
        {"BeenToLevel", NULL, NULL},
        {"Message", NULL, NULL},
        {"CutSceneFinished", NULL, NULL},
        {"Freeplay", NULL, NULL},
        {"InHubArea", &Condition_InHubArea, &Condition_InHubAreaInit},
        {"IsLowEndDevice", NULL, NULL},
        {"RandomMapCharsAvailable", NULL, NULL},
        {"CharacterLoaded", NULL, NULL},
        {NULL, NULL, NULL},
    };

    f32 default_path_heighttol = 0.2f;
    u8 default_activate_difficulty = AI_DEFAULT_ACTIVATE_DIFFICULTY;
    u8 default_min_n_respawns;
    u8 default_max_n_respawns;
    f32 default_min_t_respawn;
    f32 default_max_t_respawn;

    CHARACTERNAMEFN *LevelCharacterNameFn;
    CHARACTERNAMEFN *SpecialRouteCharacterNameFn;
    CHARACTERGLOBALIDFN *LevelCharacterGlobalIDFn;
    GLOBALCHARACTERNAMEFN *GlobalCharacterNameFn;
    CHARACTERHGOBJFN *GlobalCharacterHGobjFn;
    CHARACTERRENDERFN *GlobalCharacterRenderFn;
    CHARACTERGOALSPEEDFN *GetCharacterGoalSpeedFn;
    CHARACTERTYPEIDFN *LevelCharacterTypeIDFn;

    AICHARACTERTYPEID *GlobalCharacterTypeIDFn;
    AISPECIALROUTECHARACTERTYPEID *SpecialRouteCharacterTypeIDFn;
    AICHARACTERDISTANCE *GetViewRangeFn;
    AICHARACTERDISTANCE *GetHearDistanceFn;
    AICHARACTERDISTANCE *GetMaxViewHeightFn;
    AICHARACTERDISTANCE *GetMinViewHeightFn;
    GAMEAILOAD *GameAILoadFn;
    AIACTIONPARSESPEED *AIActionParseSpeedFn;
    AIBIGJUMPTODESTINATION *AIBigJumpToDestinationFn;
    AIRESPAWNONPATH *AIRespawnOnPathFn;
    AICLEARCREATURES *ClearAICreaturesFn;
    APIOBJECTFROMOBJID *APIOBJECTFromObjIDFn;
    AIFINDALTERNATIVESPECIALOBJECT *FindAlternativeSpecialObjectFn;
    AIGETNAMEDAPIOBJECT *GetNamedAPIObjectFn;
    AIGETCREATUREORIGIN *GetAICreatureOriginFn;
}

static SCRIPT_ERROR_LEVEL ScriptErrorLevel;

// The special-route list reserves the first ten ids for suit characters.  The
// remaining ids enumerate the current story list while omitting variants that
// are represented by those dedicated routes.
static const char *skip_chars[] = {"Batman", "Robin", "Glide_Pack", NULL};

static char *LevelCharacterName(u8 character_index) {
    if (character_index == 0xff || CurrentStoryCList == NULL) {
        return NULL;
    }

    const i16 character_type = CurrentStoryCList[character_index].model_id;
    if (character_type == -1) {
        return NULL;
    }
    return apicharsys->char_data[character_type].file;
}

static i32 LevelCharacterGlobalID(u8 character_index) {
    if (character_index == 0xff || CurrentStoryCList == NULL) {
        return -1;
    }
    return CurrentStoryCList[character_index].model_id;
}

static char *GlobalCharacterName(i32 character_type) {
    if (character_type == -1 || character_type >= apicharsys->character_count) {
        return NULL;
    }
    return apicharsys->char_data[character_type].file;
}

static void *GlobalCharacterHGobj(i32 character_type) {
    if (character_type == -1) {
        return NULL;
    }

    const i16 model_index = apicharsys->playermodelids[character_type];
    if (model_index == -1) {
        return NULL;
    }
    return apicharsys->models[model_index].hierarchy;
}

static f32 GetViewRange(i32 character_type) {
    if (character_type == -1) {
        return 0.0f;
    }
    return static_cast<GAMECHARACTERDATA *>(apicharsys->char_data[character_type].field11_0x24)->viewdistance;
}

static f32 GetHearDistance(i32 character_type) {
    if (character_type == -1) {
        return 0.0f;
    }
    return static_cast<GAMECHARACTERDATA *>(apicharsys->char_data[character_type].field11_0x24)->heardistance;
}

static f32 GetMaxViewHeight(i32 character_type) {
    if (character_type == -1) {
        return 0.0f;
    }
    return static_cast<GAMECHARACTERDATA *>(apicharsys->char_data[character_type].field11_0x24)->maxviewheight;
}

static f32 GetMinViewHeight(i32 character_type) {
    if (character_type == -1) {
        return 0.0f;
    }
    return static_cast<GAMECHARACTERDATA *>(apicharsys->char_data[character_type].field11_0x24)->minviewheight;
}

static NUVEC *GetAICreatureOrigin(AISYS *, AIPACKET *) {
    return NULL;
}

static APIOBJECT *GetNamedAPIObject(AISYS *, char *) {
    return NULL;
}

static i32 GlobalCharacterTypeID(char *name) {
    for (i32 character_type = 0; character_type < apicharsys->character_count; ++character_type) {
        if (NuStrICmp(name, apicharsys->char_data[character_type].file) == 0) {
            return character_type;
        }
    }
    return -1;
}

static i32 GameFindAlternativeSpecialObject(AISYS *, nuhspecial_s *special) {
    return EquivalentObject_Find(WorldInfo_CurrentlyActive(), special);
}

static void GameAILoad(AISYS *, i32, NUGSCN *, VARIPTR *, VARIPTR *) {
}

static void GlobalCharacterRender(NUVEC *, i16, i32, i32, EDCREATURE_s *) {
}

static f32 GetCharacterGoalSpeed(APIOBJECT *object) {
    if (object == NULL || object->ai == NULL) {
        return 0.0f;
    }

    switch (object->ai->goal_speed_mode) {
        case AI_ACTION_SPEED_LEGO:
            return static_cast<GAMECHARACTERDATA *>(object->character_data->field11_0x24)->movement_speed * FRAMETIME;
        case AI_ACTION_SPEED_RUN:
            return static_cast<GAMECHARACTERDATA *>(object->character_data->field11_0x24)->field_0x18 * FRAMETIME;
        case AI_ACTION_SPEED_WALK:
            return static_cast<GAMECHARACTERDATA *>(object->character_data->field11_0x24)->field_0x14 * FRAMETIME;
        default:
            return 0.0f;
    }
}

static i32 GameAIActionParseSpeed(char *name, u8 *speed) {
    if (NuStrICmp(name, "LEGO") == 0) {
        *speed = AI_ACTION_SPEED_LEGO;
        return 1;
    }
    if (NuStrICmp(name, "RUN") == 0) {
        *speed = AI_ACTION_SPEED_RUN;
        return 1;
    }
    if (NuStrICmp(name, "WALK") == 0) {
        *speed = AI_ACTION_SPEED_WALK;
        return 1;
    }
    return 0;
}

static char *SpecialRouteCharacterName(u8 route_id) {
    if (route_id == 0xff) {
        return NULL;
    }
    if (route_id < 10) {
        return Suit[route_id].suit_character_name;
    }

    i32 skipped_count = 0;
    for (i32 route_index = 0; route_index < 64;) {
        const i32 list_index = route_index + skipped_count;
        const i16 character_type = CurrentStoryCList[list_index].model_id;
        if (character_type == -1 || list_index > 63) {
            return NULL;
        }

        char *name = apicharsys->char_data[character_type].file;
        bool skip = false;
        for (const char **skip_name = &skip_chars[1]; *skip_name != NULL; ++skip_name) {
            if (NuStrICmp(name, *skip_name) == 0) {
                skip = true;
                break;
            }
        }
        if (skip) {
            ++skipped_count;
            continue;
        }
        if (route_index + 10 == route_id) {
            return name;
        }
        ++route_index;
    }
    return NULL;
}

static i32 LevelCharacterTypeID(char *name) {
    if (NuStrICmp(name, "Everyone") == 0) {
        return 0x40;
    }
    if (CurrentStoryCList == NULL) {
        return -1;
    }

    i32 character_index = 0;
    i16 character_type = CurrentStoryCList[character_index].model_id;
    if (character_type == -1) {
        return -1;
    }

    while (true) {
        if (NuStrICmp(name, apicharsys->char_data[character_type].file) == 0) {
            return character_index;
        }

        ++character_index;
        character_type = CurrentStoryCList[character_index].model_id;
        if (character_type == -1) {
            return -1;
        }
        if (character_index == 64) {
            return -1;
        }
    }
}

static u32 AIBigJumpToDestination(APIOBJECT *object, NUVEC *destination) {
    if (destination == NULL || object == NULL || object->objptr == NULL || object->field_0x287 != 0 ||
        object->objptr->field_0x7a5 == AI_GAME_OBJECT_TYPE_VEHICLE) {
        return 1;
    }

    GameObject_s *game_object = object->objptr;
    if (game_object->field_0xcc0 != NULL) {
        SnapCreaturePos(game_object, destination, 0, NULL, 0);
    } else if ((object->character_data->model_flags & CHARACTER_AI_MODEL_FLAG_SNAP_ON_BIG_JUMP) != 0) {
        object->start_position = *destination;
        object->position = *destination;
        object->velocity = v000;
        ResetPlayerMoves(game_object);
        object->respawn_timer = 0.0f;
    } else {
        StartBigJump(game_object, destination, 0, 0.5f, 1.0f, 0, 0);
    }

    return 1;
}

static u32 AIRespawnOnPath(APIOBJECT *object) {
    if (object->field_0x287 != 0 || (object->ai->path_info.flags & AI_RESPAWN_FLAG_DISABLED) != 0 ||
        (object->flags_high & APIOBJECT_HIGH_FLAG_RESPAWN_ENABLED) == 0) {
        return 0;
    }

    const u32 model_flags = object->character_data->model_flags;
    if ((model_flags & CHARACTER_AI_MODEL_FLAG_DISABLE_RESPAWN) != 0) {
        return 0;
    }

    GameObject_s *game_object = object->objptr;
    if (game_object->field_0x7a5 == AI_GAME_OBJECT_TYPE_VEHICLE || game_object->field_0xcc0 != NULL) {
        return 0;
    }

    if (object->respawn_timer > AI_RESPAWN_DELAY) {
        if ((model_flags & CHARACTER_AI_MODEL_FLAG_SNAP_ON_BIG_JUMP) != 0) {
            object->start_position = object->respawn_position;
            object->position = object->respawn_position;
            object->velocity = v000;
            ResetPlayerMoves(game_object);
            object->respawn_timer = 0.0f;
        } else {
            StartBigJump(game_object, &object->respawn_position, 0, 0.5f, 1.0f, 0, 0);
        }
    }

    return 0;
}

static i32 SpecialRouteCharacterTypeID(char *name) {
    if (NuStrICmp(name, "Everyone") == 0) {
        return 0x40;
    }

    for (i32 suit_index = 0; suit_index < 10; ++suit_index) {
        if (NuStrICmp(name, Suit[suit_index].suit_character_name) == 0) {
            return suit_index;
        }
    }

    if (CurrentStoryCList == NULL) {
        return -1;
    }
    i32 skipped_count = 0;
    for (i32 route_index = 0; route_index < 64;) {
        const i32 list_index = route_index + skipped_count;
        const i16 character_type = CurrentStoryCList[list_index].model_id;
        if (character_type == -1 || list_index > 63) {
            return -1;
        }

        char *character_name = apicharsys->char_data[character_type].file;
        bool skip = false;
        for (const char **skip_name = &skip_chars[1]; *skip_name != NULL; ++skip_name) {
            if (NuStrICmp(character_name, *skip_name) == 0) {
                skip = true;
                break;
            }
        }
        if (skip) {
            ++skipped_count;
            continue;
        }
        if (NuStrICmp(name, character_name) == 0) {
            return route_index + 10;
        }
        ++route_index;
    }
    return -1;
}

extern "C" {
    f32 NewShadowEx(NUVEC *position, i32 handle, f32 height_above, f32 height_below, i32 terrain_mask);
    void PlatOnOff(i32 platform_id, i32 enabled);
}
extern i32 TimingBarSet;
extern i32 SHADOWCALLS;

f32 GameShadow(GameObject_s *object, nuvec_s *position, f32 probe_height, i32 terrain_mask) {
    i32 object_platform_id = -1;
    if (object != NULL && object->field_0x107c != -1) {
        object_platform_id = object->field_0x107c;
        PlatOnOff(object_platform_id, 0);
    }

    if (TimingBarSet == 2) {
        TBOPENFN("Ter", 2);
    }
    const f32 shadow_height = NewShadowEx(position, 0, probe_height, probe_height, terrain_mask);
    ++SHADOWCALLS;
    if (TimingBarSet == 2) {
        TBCLOSEFN("Ter", 2);
    }

    if (object_platform_id != -1) {
        PlatOnOff(object_platform_id, 1);
    }
    return shadow_height;
}

extern f32 MainRenderTime;
extern f32 MainRenderTargetTime;
extern f32 backdrop_top_r;
extern f32 backdrop_top_g;
extern f32 backdrop_top_b;
extern f32 backdrop_bot_r;
extern f32 backdrop_bot_g;
extern f32 backdrop_bot_b;
extern void (*BackDrop_AlphaFn)(f32 *alpha);
extern void BackDrop_UpdateColours(i32 instant);
extern i32 Paused;
extern f32 PauseMenus_X;
extern i32 PauseMenus_Align;
extern i32 CutScenePlayer_Active();

void GameTiming(WORLDINFO_s *, float *game_time) {
    if (Paused == 0) {
        if (game_time != NULL) {
            *game_time += FRAMETIME;
        }
        UpdateTimer(&GameTimer);
        UpdateTimer(&LevelTimer);
        UpdateTimer(&AreaTimer);
    }

    UpdateTimer(&GlobalTimer);
    menu_flash = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f;

    f32 pulse_time = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f);
    game_pulse = NuTrigTable[(i32)(pulse_time * 2.0f * 65536.0f) >> 1 & 0x7fff];
    pulse_time = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f);
    global_pulse = NuTrigTable[(i32)(pulse_time * 2.0f * 65536.0f) >> 1 & 0x7fff];

    MainRenderTime = SeekLinearF(MainRenderTime, MainRenderTargetTime, FRAMETIME);
    qrand();
}

void GameFog_Set() {
    if (NuIOS_IsLowEndDevice()) {
        NuLightFogX(GameFog.low_quality_start, GameFog.low_quality_end, GameFog.colour, 0.0f, 0.0f, 0, 0.0f);
        return;
    }

    NuLightFogX(GameFog.high_quality_start, GameFog.high_quality_end, GameFog.colour, 0.0f, 0.0f, 1,
                GameFog.high_quality_density);
}

void GameRayCast(nuvec_s *, nuvec_s *, float, i32) {
}

void GameAIProcess() {
    if (WORLD == NULL || WORLD->ai_sys == NULL || Obj == NULL) {
        return;
    }

    APIOBJECT *first_player = player != NULL ? &player->apiobj : NULL;
    APIOBJECT *second_player = player2 != NULL ? &player2->apiobj : NULL;
    AISysProcess(WORLD->ai_sys, first_player, second_player);

    for (i32 index = 0; index < HIGHGAMEOBJECT; ++index) {
        GameObject_s *object = &Obj[index];
        const u16 character_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & character_flags) != character_flags || object->apiobj.field_0x287 != 0) {
            continue;
        }

        i32 ground_checks = object->apiobj.field_0x27d != 0;
        if (ground_checks == 0 && object->apiobj.character_data != NULL) {
            ground_checks = (object->apiobj.character_data->model_flags >> 13) & 1;
        }

        const i32 process_ai = (object->field_0xf00 & GAME_OBJECT_AI_UPDATE_PROCESS) != 0;
        AISysProcessCharacter(WORLD->ai_sys, &object->apiobj, &object->ai, ground_checks, object->ai_elapsed_time, 0,
                              process_ai);
    }
}

extern "C" {
    void InitFn_LevelCharacterTypeID(CHARACTERTYPEIDFN *function) {
        LevelCharacterTypeIDFn = function;
        SpecialRouteCharacterTypeIDFn = function;
    }

    void InitFn_SpecialRouteCharacterTypeID(CHARACTERTYPEIDFN *function) {
        SpecialRouteCharacterTypeIDFn = function;
    }

    void InitFn_LevelCharacterName(CHARACTERNAMEFN *function) {
        LevelCharacterNameFn = function;
        SpecialRouteCharacterNameFn = function;
    }

    void InitFn_SpecialRouteCharacterName(CHARACTERNAMEFN *function) {
        SpecialRouteCharacterNameFn = function;
    }

    void InitFn_LevelCharacterGlobalID(CHARACTERGLOBALIDFN *function) {
        LevelCharacterGlobalIDFn = function;
    }

    void InitFn_GlobalCharacterTypeID(CHARACTERTYPEIDFN *function) {
        GlobalCharacterTypeIDFn = function;
    }

    void InitFn_GlobalCharacterName(GLOBALCHARACTERNAMEFN *function) {
        GlobalCharacterNameFn = function;
    }

    void InitFn_GlobalCharacterRender(CHARACTERRENDERFN *function) {
        GlobalCharacterRenderFn = function;
    }

    void InitFn_GlobalCharacterHGobj(CHARACTERHGOBJFN *function) {
        GlobalCharacterHGobjFn = function;
    }

    void InitFn_ClearAICreatures(AICLEARCREATURES *function) {
        ClearAICreaturesFn = function;
    }

    void InitFn_GetCharacterGoalSpeedFn(CHARACTERGOALSPEEDFN *function) {
        GetCharacterGoalSpeedFn = function;
    }

    void InitFn_GetViewRange(CHARACTERDISTANCEFN *function) {
        GetViewRangeFn = function;
    }

    void InitFn_GetHearDistance(CHARACTERDISTANCEFN *function) {
        GetHearDistanceFn = function;
    }

    void InitFn_GlobalGetMaxViewHeight(CHARACTERDISTANCEFN *function) {
        GetMaxViewHeightFn = function;
    }

    void InitFn_GlobalGetMinViewHeight(CHARACTERDISTANCEFN *function) {
        GetMinViewHeightFn = function;
    }

    void InitFn_GameAILoad(GAMEAILOAD *function) {
        GameAILoadFn = function;
    }

    void InitFn_AIActionParseSpeed(AIACTIONPARSESPEED *function) {
        AIActionParseSpeedFn = function;
    }

    void InitFn_AIRespawnOnPath(AIRESPAWNONPATH *function) {
        AIRespawnOnPathFn = function;
    }

    void InitFn_AIBigJumpToDestination(AIBIGJUMPTODESTINATION *function) {
        AIBigJumpToDestinationFn = function;
    }

    void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *function) {
        FindAlternativeSpecialObjectFn = function;
    }

    void InitFn_APIOBJECTFromObjIDFn(APIOBJECTFROMOBJID *function) {
        APIOBJECTFromObjIDFn = function;
    }

    void InitFn_GetNamedAPIObject(AIGETNAMEDAPIOBJECT *function) {
        GetNamedAPIObjectFn = function;
    }

    void InitFn_GetAICreatureOrigin(AIGETCREATUREORIGIN *function) {
        GetAICreatureOriginFn = function;
    }

    void SetScriptErrorLevel(SCRIPT_ERROR_LEVEL level) {
        if (level < SCRIPT_ERROR_LEVEL_COUNT) {
            ScriptErrorLevel = level;
        }
    }
}

void GameAISysInit() {
    RegisterAIScriptActions(lego_aiactiondefs);
    RegisterAIScriptConditions(lego_aiconditiondefs);
    InitFn_LevelCharacterTypeID(LevelCharacterTypeID);
    InitFn_SpecialRouteCharacterTypeID(SpecialRouteCharacterTypeID);
    InitFn_LevelCharacterName(LevelCharacterName);
    InitFn_SpecialRouteCharacterName(SpecialRouteCharacterName);
    InitFn_LevelCharacterGlobalID(LevelCharacterGlobalID);
    InitFn_GlobalCharacterTypeID(GlobalCharacterTypeID);
    InitFn_GlobalCharacterName(GlobalCharacterName);
    InitFn_GlobalCharacterRender(GlobalCharacterRender);
    InitFn_GlobalCharacterHGobj(GlobalCharacterHGobj);
    InitFn_ClearAICreatures(ClearAICreatures);
    InitFn_GetCharacterGoalSpeedFn(GetCharacterGoalSpeed);
    InitFn_GetViewRange(GetViewRange);
    InitFn_GetHearDistance(GetHearDistance);
    InitFn_GlobalGetMaxViewHeight(GetMaxViewHeight);
    InitFn_GlobalGetMinViewHeight(GetMinViewHeight);
    InitFn_GameAILoad(GameAILoad);
    InitFn_AIActionParseSpeed(GameAIActionParseSpeed);
    InitFn_AIRespawnOnPath(AIRespawnOnPath);
    InitFn_AIBigJumpToDestination(AIBigJumpToDestination);
    InitFn_FindAlternativeSpecialObjectFn(GameFindAlternativeSpecialObject);
    InitFn_APIOBJECTFromObjIDFn(GameAPIOBJECTFromObjID);
    InitFn_GetNamedAPIObject(GetNamedAPIObject);
    InitFn_GetAICreatureOrigin(GetAICreatureOrigin);

    default_path_heighttol = 0.2f;
    default_activate_difficulty = AI_DEFAULT_ACTIVATE_DIFFICULTY;
    default_min_n_respawns = 0;
    default_max_n_respawns = 0;
    default_min_t_respawn = 0.0f;
    default_max_t_respawn = 0.0f;

    SetScriptErrorLevel(SCRIPT_ERROR_LEVEL_WARNING);
    GameAISysSetGame();
}

void GameFog_Reset() {
    GameFogSnap = 1;
    GameFogDuration = 0.0f;
    GameFogSet = 0;
    GameFogTime = 0.0f;
}

void Game_KillPart(PART_s *, i32) {
}

void GameAISysReset(AISYS_s *system) {
    if (system == NULL) {
        return;
    }

    AISysSetLevelPath(system, NULL);

    WORLD->processor_count = 0;
    for (i32 script_index = 0; script_index < 32; ++script_index) {
        char script_name[16];
        if (script_index != 0) {
            sprintf(script_name, "Level%d", script_index);
        } else {
            sprintf(script_name, "Level");
        }

        if (AIScriptFind(WORLD->ai_sys, script_name, 0, 1, 0) == NULL) {
            continue;
        }

        AIScriptProcessorInit(system, NULL, &WORLD->processors[WORLD->processor_count].processor, NULL, script_name,
                              NULL, 0, NULL, NULL);
        NuStrCpy(WORLD->processors[WORLD->processor_count].name, script_name);
        LevelScriptReStoreProgress(WORLD, &WORLD->processors[WORLD->processor_count]);
        ++WORLD->processor_count;
    }

    AIPATHSYS *path_system = system->path_sys;
    if (path_system != NULL) {
        for (i32 path_index = 0; path_index < path_system->path_count; ++path_index) {
            AIPATH *path = path_system->paths[path_index];
            AIPATHCNX *connection = path->connections;
            for (i32 connection_index = 0; connection_index < path->connection_count;
                 ++connection_index, ++connection) {
                connection->node_a = connection->previous_node_a;
                connection->node_b = connection->previous_node_b;

                if (LEGO_AIPATHCNX_BLOCKAGE != 0) {
                    connection->node_a &= ~LEGO_AIPATHCNX_BLOCKAGE;
                    connection->node_b &= ~LEGO_AIPATHCNX_BLOCKAGE;
                }
            }
        }
    }

    AIPathCnxControlSysReset(WORLD->ai_path_cnx_control_sys);
    AIPathCnxHelperSysReset(WORLD, WORLD->ai_path_cnx_helper_sys);
    InitAICreatures(system);
    ResetAICreatures(system);

    if (WORLD->processor_count != 0) {
        GizmoSysAddGizmos(WORLD->gizmo_sys, WORLD->giz_flow, WORLD);
    }
}

void GameAttackInit() {
}

extern "C" void MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry);
i32 GameAudio_GetSfxId(i32 sfx);
void GameAudio_PlaySfxById(i32 sfx_id, nuvec_s *position, i32 flags, i32 volume);

static GAMEAUDIO GameAudio_Default;
static GAMEAUDIO *GameAudio;

void GameAudio_Init(GAMEAUDIO *audio) {
    GameAudio = audio;
    for (i32 i = 0; i < 0x55; ++i) {
        audio->sfx_ids[i] = static_cast<i16>(GetSfxId(audio->sfx_names[i]));
    }

    MenuRegisterSoundFX(GameAudio_GetSfxId(0x2f), GameAudio_GetSfxId(0x30), GameAudio_GetSfxId(0x31),
                        GameAudio_GetSfxId(0x32));
}

void GameFog_Update(WORLDINFO_s *) {
}

void GameAudio_Reset() {
    memset(&GameAudio_Default, 0, sizeof(GameAudio_Default));
    GameAudio = &GameAudio_Default;
    memset(GameAudio_Default.sfx_names, 0, sizeof(GameAudio_Default.sfx_names));
    for (i32 i = 0; i < 0x55; ++i) {
        GameAudio_Default.sfx_ids[i] = -1;
    }
}

void *GameBufferAlloc(variptr_u *buf, variptr_u *buf_end, i32 size) {
    // Carves `size` bytes out of the permanent buffer (original at
    // 0x4890a0); returns the previous cursor.
    void *ptr = (void *)(usize)buf->addr;
    buf->addr += size;
    return ptr;
}

void GameObj_GetName(i32, GameObject_s *, char *) {
}

void Game_AutoSaving() {
}

void GameAISysSetGame() {
    AIPathCnxHelperSysInitFn = NULL;
    StarWars_GameAISysInit();
}

void GameAudio_AddSfx(i32, i32 *, i32 *, i32) {
}

void GameObjectOrigin(GameObject_s *object) {
    APIOBJECT &api = object->apiobj;
    api.field_0x1f4 |= 0x100u;

    f32 predicted_vertical_displacement;
    f32 origin_x;
    f32 origin_z;
    if (object->use_model_origin != 0) {
        PLAYERCHARACTERCONFIG_s *config = api.character_data->player_config;
        const i32 model_origin_joint = config->model_origin_joint;
        CHARACTERMODEL_s *model = api.character_model;
        if (model_origin_joint != -1 && model != NULL && model->points_of_interest[model_origin_joint] != NULL) {
            const f32 frame_time = FRAMETIME;
            const f32 predicted_x = api.previous_velocity.x * frame_time;
            predicted_vertical_displacement = api.previous_velocity.y * frame_time;
            const f32 predicted_z = api.previous_velocity.z * frame_time;
            if ((object->field_0xe24 & GAMEOBJECT_E24_FLAG_JOINT_MATRICES_UPDATED) == 0) {
                NUVEC local_origin = {0.0f, -object->character_bottom, 0.0f};
                NuVecMtxRotate(&api.collision_position, &local_origin, &api.field_0xb8);
                NuVecAdd(&api.collision_position, &api.collision_position, &api.position);
                api.collision_position.x += predicted_x;
                api.collision_position.y += predicted_vertical_displacement;
                api.collision_position.z += predicted_z;
            } else {
                const NUVEC &joint_position = *NUMTX_GET_ROW_VEC(&object->joint_matrices[model_origin_joint], 3);
                api.collision_position.x = joint_position.x + predicted_x * 2.0f;
                api.collision_position.y = joint_position.y + predicted_vertical_displacement * 2.0f;
                api.collision_position.y += (object->character_bottom + object->character_top) * api.field_0xa8 * 0.5f;
                api.collision_position.z = joint_position.z + predicted_z * 2.0f;
            }
            origin_x = api.position.x + predicted_x;
            origin_z = api.position.z + predicted_z;
            goto update_bounds;
        }

        const i32 collision_origin_joint = config->collision_origin_joint;
        if (object->field_0xd24 == 1.0f && collision_origin_joint != -1 &&
            model->points_of_interest[collision_origin_joint] != NULL && api.field_0x288 != 0) {
            api.collision_position = *NUMTX_GET_ROW_VEC(&object->joint_matrices[collision_origin_joint], 3);
            const f32 frame_time = FRAMETIME;
            origin_x = api.position.x + api.previous_velocity.x * frame_time;
            origin_z = api.position.z + api.previous_velocity.z * frame_time;
            predicted_vertical_displacement = api.previous_velocity.y * frame_time;
            goto update_bounds;
        }
    }

    api.field_0x1f4 &= ~0x100u;
    predicted_vertical_displacement = api.previous_velocity.y * FRAMETIME;
    api.collision_position.x = api.position.x + api.previous_velocity.x * FRAMETIME;
    api.collision_position.y = api.position.y + predicted_vertical_displacement;
    api.collision_position.y += (object->character_bottom + object->character_top) * api.field_0xa8 * 0.5f;
    api.collision_position.z = api.position.z + api.previous_velocity.z * FRAMETIME;
    origin_x = api.collision_position.x;
    origin_z = api.collision_position.z;

update_bounds:
    const f32 radius = api.field_0x1dc;
    const f32 half_height = api.field_0x1e0;
    api.collision_min.x = api.collision_position.x - radius;
    api.collision_min.y = api.collision_position.y - half_height;
    api.collision_min.z = api.collision_position.z - radius;
    api.collision_max.x = api.collision_position.x + radius;
    api.collision_max.y = api.collision_position.y + half_height;
    api.collision_max.z = api.collision_position.z + radius;

    api.upper_position.x = origin_x;
    api.upper_position.y = api.collision_max.y;
    api.upper_position.z = origin_z;
    api.lower_position.x = origin_x;
    api.lower_position.y = api.collision_min.y;
    api.lower_position.z = origin_z;
    api.collision_origin.x = origin_x;
    api.collision_origin.y = api.collision_min.y + predicted_vertical_displacement;
    api.collision_origin.z = origin_z;

    object->ai.terrain_origin = api.collision_origin;
    const u32 context_flags = CInfo[object->character_context].flags;
    if ((context_flags & CHARACTER_CONTEXT_INFO_FLAG_TERRAIN_ORIGIN_AT_TOP) != 0 &&
        (object->context_variant_flags & 0x08) != 0) {
        object->ai.terrain_origin.y += api.collision_max.y - api.collision_min.y;
    } else if ((context_flags & CHARACTER_CONTEXT_INFO_FLAG_TERRAIN_ORIGIN_AT_POSITION) != 0) {
        object->ai.terrain_origin = api.position;
    } else {
        object->ai.terrain_origin.y = api.collision_origin.y - object->terrain_origin_floor_offset;
        if (object->ai.terrain_origin.y < api.field_0x218 &&
            api.field_0x218 < object->ai.terrain_origin.y + api.scaled_height) {
            object->ai.terrain_origin.y = api.field_0x218;
        }
    }
}

i32 Game_IgnoreInput() {
    extern i32 newgamecam;
    return newgamecam != 0;
}

void GameAI_TotalScore() {
}

void GameAudio_PlaySfx(i32 sfx, nuvec_s *position, i32 flags, i32 volume) {
    if ((u32)sfx < 0x55) {
        GameAudio_PlaySfxById(GameAudio->sfx_ids[sfx], position, flags, volume);
    }
}

void GameDrawMenuEntry(MENU_s *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, static_cast<u8>(MenuA));
}

void GameAnimSys_Update(GAMEANIMSYS_s *system) {
    if (system == NULL) {
        return;
    }

    GAMEANIMSET_s *set = reinterpret_cast<GAMEANIMSET_s *>(NuLinkedListGetHead(&system->active_sets));
    while (set != NULL) {
        GAMEANIMSET_s *next_set =
            reinterpret_cast<GAMEANIMSET_s *>(NuLinkedListGetNext(&system->active_sets, &set->links));

        const u8 was_no_visibility_test = set->flags & GAMEANIMSET_FLAG_NO_VISIBILITY_TEST;
        set->flags &= ~GAMEANIMSET_FLAG_NO_VISIBILITY_TEST;

        if ((set->flags & GAMEANIMSET_FLAG_STOP_REQUESTED) != 0) {
            set->flags &= ~(GAMEANIMSET_FLAG_NO_VISIBILITY_TEST | GAMEANIMSET_FLAG_STOP_REQUESTED);
            GameAnimSet_RemoveFromSystemList(set);
            set = next_set;
            continue;
        }

        set->state = GAMEANIMSET_STATE_AT_START;
        i32 all_playing_forward = 1;
        i32 all_at_start = 1;
        i32 all_at_end = 1;
        i32 any_special_no_visibility_test = 0;

        GAMEANIMOBJ_s *object = set->objects;
        while (object != NULL) {
            if (NuSpecialGetNoVisiTestFn(&object->special) != 0) {
                any_special_no_visibility_test = 1;
            }

            if (object->instance_animation != NULL) {
                const f32 direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
                const f32 previous_frame = object->instance_animation->ltime;

                if (previous_frame * direction >= object->end_frame * direction) {
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        object->instance_animation->ltime = object->end_frame;
                    } else if (object->instance_animation->repeating != 0) {
                        object->instance_animation->ltime = previous_frame - object->end_frame + object->start_frame;
                    } else {
                        object->instance_animation->ltime = object->end_frame;
                        object->instance_animation->playing = 0;
                    }
                } else if (previous_frame * direction <= object->start_frame * direction) {
                    if (object->instance_animation->tfactor * direction > 0.0f) {
                        object->instance_animation->ltime = object->start_frame;
                    } else if (object->instance_animation->repeating != 0) {
                        object->instance_animation->ltime = object->end_frame - (object->start_frame - previous_frame);
                    } else {
                        object->instance_animation->ltime = object->start_frame;
                        object->instance_animation->playing = 0;
                    }
                }

                if (previous_frame != object->instance_animation->ltime) {
                    EvalAnim2(&object->special, object->instance_animation->ltime);
                }

                if (object->instance_animation->playing != 0) {
                    set->flags |= GAMEANIMSET_FLAG_NO_VISIBILITY_TEST;
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        all_playing_forward = 0;
                    }
                }

                const f32 directed_frame = object->instance_animation->ltime * direction;
                if (directed_frame > object->start_frame * direction) {
                    all_at_start = 0;
                }
                if (directed_frame < object->end_frame * direction) {
                    all_at_end = 0;
                }
            }
            object = object->next;
        }

        if ((set->flags & GAMEANIMSET_FLAG_NO_VISIBILITY_TEST) != any_special_no_visibility_test) {
            object = set->objects;
            while (object != NULL) {
                NuSpecialSetNoVisiTest(&object->special, set->flags & GAMEANIMSET_FLAG_NO_VISIBILITY_TEST);
                object = object->next;
            }
        }

        if ((set->flags & GAMEANIMSET_FLAG_NO_VISIBILITY_TEST) != 0) {
            set->state =
                all_playing_forward != 0 ? GAMEANIMSET_STATE_ACTIVE_FORWARD : GAMEANIMSET_STATE_ACTIVE_BACKWARD;
        } else {
            if (all_at_end != 0) {
                set->state = GAMEANIMSET_STATE_AT_END;
            } else if (all_at_start == 0) {
                set->state = GAMEANIMSET_STATE_BETWEEN_ENDPOINTS;
            }
            if (was_no_visibility_test != 0) {
                set->flags |= GAMEANIMSET_FLAG_STOP_REQUESTED;
            }
        }

        if ((set->flags & (GAMEANIMSET_FLAG_NO_VISIBILITY_TEST | GAMEANIMSET_FLAG_STOP_REQUESTED)) == 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
        set = next_set;
    }
}

i32 GameAudio_GetSfxId(i32 sfx) {
    if (static_cast<u32>(sfx) <= 0x54) {
        return GameAudio->sfx_ids[sfx];
    }
    return -1;
}

void GameObjIsCableTied(GameObject_s *) {
}

void GameObjectRotation(GameObject_s *, i32) {
}

// Original: reads the user's music volume from the options save as the product
// of two 0..10 sliders scaled to 0..1 (option bytes at 0x4 and 0x5).
f32 GameGetMusicVolume(OPTIONSSAVE_s *options) {
    return ((f32)(u8)options->field5_0x5 / 10.0f) * ((f32)(u8)options->field4_0x4 / 10.0f);
}

// Original: applies GameGetMusicVolume, zeroing it while the title logos are
// up (SuperOptions.music_enabled == 0 on the titles level); the title menu restores
// the user's volume via GameGetMusicVolume once the menu phase starts.
f32 GameSetMusicVolume(OPTIONSSAVE_s *options) {
    f32 volume = GameGetMusicVolume(options);
    if (SuperOptions.music_enabled == 0 && WORLD->current_level == TITLES_LDATA) {
        volume = 0.0f;
    }
    legoSetMusicVolume(volume);
    return volume;
}

void GameAISysStartFrame(AISYS_s *system) {
    if (system == NULL || netclient != 0) {
        return;
    }

    if (system->path_sys != NULL && system->path_sys->path_count != 0) {
        for (i32 index = 0; index < system->path_sys->path_count; ++index) {
            memset(&system->path_sys->paths[index]->updated_node_bits[0], 0, 0x20);
            memmove(&system->path_sys->paths[index]->updated_node_bits[0x20],
                    system->path_sys->paths[index]->inside_node_bits, 0x20);
            memset(system->path_sys->paths[index]->inside_node_bits, 0,
                   sizeof(system->path_sys->paths[index]->inside_node_bits));
        }

        // Moving specials can carry path endpoints. Update both ends of each
        // connection; AIPathNodeUpdatePos uses updated_node_bits to ensure a
        // shared endpoint is transformed only once this frame.
        for (i32 path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            AIPATH *path = system->path_sys->paths[path_index];
            for (i32 connection_index = 0; connection_index < path->connection_count; ++connection_index) {
                AIPATHCNX *connection = &path->connections[connection_index];
                AIPATHNODE *node = &path->nodes[connection->direction_a];
                if (node->has_special != 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
                node = &path->nodes[connection->direction_b];
                if (node->has_special != 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
            }
        }
    }

    // Spread the object/area overlap work across frames. Area runtime flags
    // summarize the kinds of live characters inside it, while every object
    // retains a 64-area occupancy mask for script queries.
    if (system->next_area_check < system->area_count) {
        AIAREA *area = &system->areas[system->next_area_check];
        const i32 area_index = static_cast<i32>(area - WORLD->ai_sys->areas);
        const u64 area_bit = 1ULL << area_index;
        area->runtime_flags &= static_cast<u8>(
            ~(AIAREA_RUNTIME_PLAYER_PRESENT | AIAREA_RUNTIME_OBJECT_STATE_CLEAR | AIAREA_RUNTIME_OBJECT_STATE_SET));

        GameObject_s *object = Obj;
        for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
            if ((object->apiobj.field_0x1f8 & (APIOBJECT_FLAG_CHARACTER | APIOBJECT_FLAG_IN_USE)) !=
                (APIOBJECT_FLAG_CHARACTER | APIOBJECT_FLAG_IN_USE)) {
                continue;
            }
            if (object->apiobj.field_0x287 != 0 && !(object->field_0x101c > 0.0f) &&
                static_cast<i8>(object->apiobj.flags_low) >= 0) {
                continue;
            }

            NUVEC local_position;
            NuVecSub(&local_position, &object->apiobj.position, &area->position);
            NuVecRotateY(&local_position, &local_position, -area->rotation);
            const bool is_inside = local_position.x >= -area->half_width && local_position.y >= -0.1f &&
                                   local_position.z >= -area->half_depth && local_position.x <= area->half_width &&
                                   local_position.y <= area->height && local_position.z <= area->half_depth;
            if (!is_inside) {
                object->ai_area_mask &= ~area_bit;
                continue;
            }

            object->ai_area_mask |= area_bit;
            if ((object->apiobj.flags_low & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                area->runtime_flags |= AIAREA_RUNTIME_PLAYER_PRESENT;
            }
            if (object->apiobj.field_0x27c != -1) {
                area->runtime_flags |= AIAREA_RUNTIME_CHARACTER_SLOT_SEEN;
            }
            if ((object->apiobj.field_0x1f4 & 1) != 0) {
                area->runtime_flags |= AIAREA_RUNTIME_OBJECT_STATE_SET;
            } else if ((object->apiobj.field_0x1f4 & 4) == 0) {
                area->runtime_flags |= AIAREA_RUNTIME_OBJECT_STATE_CLEAR;
            }
        }

        ++system->next_area_check;
        if (system->next_area_check >= system->area_count) {
            system->next_area_check = 0;
        }
    }

    // Level scripts are ordinary AI processors without an object or packet.
    // Disabled processors retain their state but do not advance this frame.
    for (i32 index = 0; index < WORLD->processor_count; ++index) {
        if (!WORLD->processors[index].processor.is_disabled) {
            AIScriptProcess(system, NULL, NULL, &WORLD->processors[index].processor, FRAMETIME);
        }
    }
}

void GameDisplaySettings(LEVELDATADISPLAY *display, i32 *background_colours) {
    CUTINFO *cut = static_cast<CUTINFO *>(CutStopInfo);
    if (cut != NULL && cut->camera_near_clip != 0.0f) {
        pNuCam->near_clip = cut->camera_near_clip;
    } else {
        pNuCam->near_clip = display->unknown_04;
    }

    u16 far_clip;
    if (cut != NULL) {
        far_clip = cut->camera_far_clip;
        if (far_clip == 0) {
            far_clip = static_cast<u16>(display->unknown_14);
        }
    } else {
        far_clip = static_cast<u16>(display->unknown_14);
    }
    pNuCam->far_clip = static_cast<f32>(static_cast<u32>(far_clip));

    LEVELDATA *level = WORLD->current_level;
    const bool use_backdrop = level == TITLES_LDATA || (level->flags & LEVEL_STATUS) != 0 || level == STATUS_LDATA ||
                              level == CREDITS_LDATA || MainRenderTime < 0.0f;
    if (!use_backdrop) {
        background_colours[0] =
            0x80000000u | static_cast<u8>(display->bg_red_top) |
            (static_cast<u8>(display->bg_green_top) << 8 | static_cast<u8>(display->bg_blue_top) << 16);
        background_colours[1] =
            0x80000000u | static_cast<u8>(display->bg_red_bottom) |
            (static_cast<u8>(display->bg_green_bottom) << 8 | static_cast<u8>(display->bg_blue_bottom) << 16);
        return;
    }

    BackDrop_UpdateColours(0);

    i32 top_g = static_cast<i32>(backdrop_top_g);
    i32 top_r = static_cast<i32>(backdrop_top_r);
    i32 top_b = static_cast<i32>(backdrop_top_b);
    i32 bottom_r = static_cast<i32>(backdrop_bot_r);
    i32 bottom_g = static_cast<i32>(backdrop_bot_g);
    i32 bottom_b = static_cast<i32>(backdrop_bot_b);

    f32 alpha = 1.0f;
    if (BackDrop_AlphaFn != NULL) {
        BackDrop_AlphaFn(&alpha);
        if (alpha != 0.0f) {
            top_r = static_cast<i32>(static_cast<f32>(top_r) * alpha);
            top_g = static_cast<i32>(static_cast<f32>(top_g) * alpha);
            top_b = static_cast<i32>(static_cast<f32>(top_b) * alpha);
            bottom_r = static_cast<i32>(static_cast<f32>(bottom_r) * alpha);
            bottom_g = static_cast<i32>(static_cast<f32>(bottom_g) * alpha);
            bottom_b = static_cast<i32>(static_cast<f32>(bottom_b) * alpha);
        }
    }

    background_colours[0] = 0x80000000u | (top_r & 0xff) | (top_g & 0xff) << 8 | (top_b & 0xff) << 16;
    background_colours[1] = 0x80000000u | (bottom_r & 0xff) | (bottom_g & 0xff) << 8 | (bottom_b & 0xff) << 16;
}

void GameObjectSetCanUse(GameObject_s *, void *, unsigned char, unsigned char, float) {
}

void GameObjOwnsAnyCables(GameObject_s *) {
}

void GameObjectDimensionsExtra_LSW(GameObject_s *object);

void GameObjectDimensions(GameObject_s *object) {
    APIOBJECT &api = object->apiobj;
    PLAYERCHARACTERCONFIG_s *config = api.character_data->player_config;
    const i32 collision_origin_joint = config->collision_origin_joint;
    if (object->use_model_origin != 0 && object->field_0xd24 == 1.0f && collision_origin_joint != -1 &&
        api.character_model->points_of_interest[collision_origin_joint] != NULL && api.field_0x288 != 0) {
        const f32 radius = object->field_0x1004 * config->collision_origin_radius;
        api.field_0x1dc = radius;
        api.field_0x1e0 = radius;
    } else {
        api.field_0x1dc = api.collision_radius;
        api.field_0x1e0 = api.collision_height;
    }
    GameObjectDimensionsExtra_LSW(object);
}

void GameObjectUsingLever(GameObject_s *, LEVER_s *) {
}

void GameAntiNodeData_Init(GAMEANTINODEDATA_s *, nuhspecial_s *) {
}

void GameAntiNodeData_Read(GAMEANTINODEDATA_s *) {
}

void GameAudio_PlaySfxById(i32, nuvec_s *, i32, i32) {
}

void Game_GotAllGoldBricks() {
}

APIOBJECT *GameAPIOBJECTFromObjID(u8 object_id) {
    if (object_id >= HIGHGAMEOBJECT) {
        return NULL;
    }

    GameObject_s *object = &Obj[object_id];
    if ((object->apiobj.flags_low & APIOBJECT_FLAG_IN_USE) == 0) {
        return NULL;
    }

    if ((object->apiobj.flags_high & APIOBJECT_HIGH_FLAG_PLAYER_CHARACTER) == 0 &&
        object->ai.reset_mode != AI_OBJECT_ROUTE_STATE_SCRIPT_VISIBLE) {
        return NULL;
    }
    if (object->apiobj.field_0x287 != 0 && object->field_0x101c <= 0.0f) {
        return NULL;
    }
    return &object->apiobj;
}

i32 GameDrawCharacterModel(CHARACTERMODEL_s *model, ANIMPACKET_s *animation, NUMTX *matrix, NUMTX *secondary_matrix,
                           NUMTX *reflection_matrix, NUMTX *auxiliary_matrix, GameObject_s *object, u32 flags) {
    if (model == NULL) {
        return 0;
    }

    drawcharactermodel_keepmergeaction = game_keepmergeaction;
    MakeLayerList = GCDataList[model->model_id].make_layer_list;

    CHARACTERDATA *character_data =
        object != NULL ? object->apiobj.character_data : &apicharsys->char_data[model->model_id];

    // The original reserves a fixed 256-matrix evaluation array in this
    // wrapper before calling APIDrawCharacterModel.
    NUMTX output_matrices[256];
    return APIDrawCharacterModel(model, character_data, animation, matrix, secondary_matrix, reflection_matrix, 0,
                                 auxiliary_matrix, object, flags, NULL, 0, WORLD, FRAMETIME, output_matrices, 0, NULL);
}

void GameObjectToCameraCode(GameObject_s *) {
}

void GameRegisterGizActions() {
    RegisterGizActions(game_gizactiondefs);
}

i32 GameAudio_GetPlrSfxBits(void *object_ptr) {
    APIOBJECT *object = static_cast<APIOBJECT *>(object_ptr);
    i32 sfx_bits = 0;
    if (object != NULL && static_cast<i8>(object->flags_low) < 0) {
        sfx_bits = 1 << object->field_0x27c;
    }
    return sfx_bits;
}

void GameBlowUpBlownUpFn_LSW(GIZMOBLOWUP_s *) {
}

void GameLoadCharacterModels(APICHARACTERMODELLIST_s *list, i32 append, VARIPTR *buf, VARIPTR *buf_end, i32 area_models,
                             i32 area) {
    if (area_models != 0 && CutScenePlayer_Active() != 0 && area != -1 && &ADataList[area] != HUB_ADATA) {
        area_models = 0;
    }

    APILoadCharacterModels(list, append, buf, *buf_end, area_models);
}

void Game_100PercentComplete() {
}

void Game_WorldInfo_InitMenu(WORLDINFO_s *world, i32 *menu_id, i32 *) {
    if (world->current_level == TITLES_LDATA) {
        *menu_id = 0;
    } else if (world->current_level == CREDITS_LDATA) {
        *menu_id = 30;
    }
}

void GameAnimSys_StoreProgress(GAMEANIMSYS_s *, i32) {
}

void GameAnimSys_GetProgressData(i32) {
}

void GameAnimSys_ReStoreProgress(GAMEANIMSYS_s *, i32) {
}

void GameObjectToCameraDistances() {
    const NUVEC camera_position = GameCam->pos;
    GameObject_s *object = Obj;
    for (i32 object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        const u16 required_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & required_flags) != required_flags) {
            continue;
        }

        const f32 dx = camera_position.x - object->apiobj.position.x;
        const f32 dy = camera_position.y - object->apiobj.position.y;
        const f32 dz = camera_position.z - object->apiobj.position.z;
        object->ai_update_distance = NuFsqrt(dx * dx + dy * dy + dz * dz);
    }
}

void GameAudio_PlaySfxAndSetVolume(i32, nuvec_s *, float) {
}

void GameAudio_SetActionMusicTimes(float, float) {
}

void GameCreatureOpponentSelection(AISYS_s *, i32, APIOBJECT_s **, i32, APIOBJECT_s **, i32, APIOBJECT_s **, u64,
                                   float) {
}

void GameObjectDimensionsExtra_LSW(GameObject_s *) {
}

void GameObjectStuffAfterAnimation() {
}

void GameMsg_DrawAdjustNewPos_CoinToTotal(GAMEMESSAGE_s *) {
}

void GameAnimSys_AllocateLevelProgressData(variptr_u *, variptr_u *, i32, i32) {
}

i32 Game_Exit(i32) {
    return 0;
}

void GameObject_s::ClearAddons() {
}

void GameObject_s::ClearMechObjectInterface() {
}

void GameObject_s::GetAddons(bool) {
}

void GameObject_s::GetMechObjectInterface() {
}

void GameObject_s::IsRunningTaskType(HashedKey const &) {
}

void GameObject_s::KillTasks() {
}

// ThingManager::AddThing @0x424c10. Appends at count; the pending
// AddThingAfterThis reservation (field_0x14) is folded into the index and
// cleared here.
void ThingManager::AddThing(BaseThing *thing) {
    i32 index = this->count;
    if (thing != NULL) {
        if (index < this->max_things) {
            this->things[index] = thing;
            index = index + 1;
        }
    }
    index = index + this->field_0x14;
    this->field_0x14 = 0;
    this->count = index;
}

// ThingManager::AddThingAfterThis @0x424c40. Reserves the slot after the
// current tail: bumps field_0x14 and stores the thing at count+field_0x14;
// the next AddThing folds the reservation into count.
void ThingManager::AddThingAfterThis(BaseThing *thing) {
    if (thing != NULL) {
        i32 index = this->field_0x14 + 1;
        this->field_0x14 = index;
        index = index + this->count;
        if (index < this->max_things) {
            this->things[index] = thing;
        }
    }
}

// ThingManager::DisplayThings @0x4252c0. Single pass over Display,
// bracketed with timebar slot 3 ("Dis"). PanelRender uses this pass for the
// display-layer things that render on top of the gameplay panel.
void ThingManager::DisplayThings(ThingRenderData *data) {
    static const char *name = "Dis"; // timebar slot name @0x5734db

    if (this->count <= 0) {
        return;
    }
    for (i32 i = 0; i < this->count; i++) {
        BaseThing *thing = this->things[i];
        if (thing == NULL || (thing->flags & THING_FLAG_SKIP_DISPLAY)) {
            continue;
        }
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotBegin(this->timebar, 3, name);
        }
        thing->Display(data);
        thing = this->things[i];
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotEnd(this->timebar, 3);
        }
    }
}

void ThingManager::EffectsThings(ThingRenderData *) {
}

// ThingManager::EnableActions @0x425930. Finds the first thing whose 0x4 id
// matches and sets (invert==0) or clears (invert!=0) the given flags bits.
void ThingManager::EnableActions(i32 id, i32 flags, i32 invert) {
    i32 count = this->count;
    if (count <= 0) {
        return;
    }
    for (i32 i = 0; i < count; i++) {
        BaseThing *thing = this->things[i];
        if (thing == NULL) {
            continue;
        }
        if (thing->field_0x4 == (u32)id) {
            if (invert == 0) {
                thing->flags |= (u32)flags;
            } else {
                thing->flags &= ~(u32)flags;
            }
            return;
        }
    }
}

void ThingManager::EnterLevelThings(ThingLevelData *) {
}

void ThingManager::ExitLevelThings(ThingLevelData *) {
}

// ThingManager::ProcessThings @0x425460. Pass 1 always runs
// ProcessEvenWhenPaused first; then, per ThingProcessData.paused, either
// Process or ProcessOnlyWhenPaused. Each pass has its own opt-out flag. The count
// is re-read every iteration because thing Process calls may add things.
// Profiling: things with a non-NULL profiling handle are bracketed with
// NuTimeBarSlotBegin/End (stubbed no-ops on this build).
void ThingManager::ProcessThings(ThingProcessData *data) {
    static const char *name = "PROC"; // timebar slot name @0x5734e3

    if (this->count <= 0) {
        return;
    }
    for (i32 i = 0; i < this->count; i++) {
        BaseThing *thing = this->things[i];
        if (thing == NULL || (thing->flags & THING_FLAG_SKIP_PROCESS_EVEN_WHEN_PAUSED)) {
            continue;
        }
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotBegin(this->timebar, 0, name);
        }
        thing->ProcessEvenWhenPaused(data);
        thing = this->things[i];
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotEnd(this->timebar, 0);
        }
    }
    if (data->paused != 0) {
        if (this->count <= 0) {
            return;
        }
        for (i32 i = 0; i < this->count; i++) {
            BaseThing *thing = this->things[i];
            if (thing == NULL || (thing->flags & THING_FLAG_SKIP_PROCESS_ONLY_WHEN_PAUSED)) {
                continue;
            }
            if (thing->profiling_0xc != NULL) {
                _NuTimeBarSlotBegin(this->timebar, 0, name);
            }
            thing->ProcessOnlyWhenPaused(data);
            thing = this->things[i];
            if (thing->profiling_0xc != NULL) {
                _NuTimeBarSlotEnd(this->timebar, 0);
            }
        }
    } else {
        if (this->count <= 0) {
            return;
        }
        for (i32 i = 0; i < this->count; i++) {
            BaseThing *thing = this->things[i];
            if (thing == NULL || (thing->flags & THING_FLAG_SKIP_PROCESS)) {
                continue;
            }
            if (thing->profiling_0xc != NULL) {
                _NuTimeBarSlotBegin(this->timebar, 0, name);
            }
            thing->Process(data);
            thing = this->things[i];
            if (thing->profiling_0xc != NULL) {
                _NuTimeBarSlotEnd(this->timebar, 0);
            }
        }
    }
}

void ThingManager::RemoveDependanciesThings(ThingRemoveData *) {
}

void ThingManager::RemoveTemporaryThings() {
}

// ThingManager::RenderThings @0x425390. Single pass over Render,
// bracketed with timebar slot 1 ("Rnd").
void ThingManager::RenderThings(ThingRenderData *data) {
    static const char *name = "Rnd"; // timebar slot name @0x5734df

    if (this->count <= 0) {
        return;
    }
    for (i32 i = 0; i < this->count; i++) {
        BaseThing *thing = this->things[i];
        if (thing == NULL || (thing->flags & THING_FLAG_SKIP_RENDER)) {
            continue;
        }
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotBegin(this->timebar, 1, name);
        }
        thing->Render(data);
        thing = this->things[i];
        if (thing->profiling_0xc != NULL) {
            _NuTimeBarSlotEnd(this->timebar, 1);
        }
    }
}

void ThingManager::ResetThings(ThingResetData *data) {
    const char *name = "Res";

    i32 i = 0;
    if (this->count > 0) {
        do {
            BaseThing *thing = this->things[i];
            if (thing != NULL && (thing->flags & 8) == 0) {
                if (thing->profiling_0xc != NULL) {
                    _NuTimeBarSlotBegin(this->timebar, 4, name);
                }
                thing = this->things[i];
                thing->Reset(data);
                thing = this->things[i];
                if (thing->profiling_0xc != NULL) {
                    _NuTimeBarSlotEnd(this->timebar, 4);
                }
            }
            ++i;
        } while (i < this->count);
    }
}

// ThingManager::ThingManager @0x425870. Stores the manager in theThingManager
// and carves the thing-pointer array from the
// theMemoryManager linear pool. On allocation failure the array is NULL — the
// manager then simply never accepts things (AddThing's count < max check).
ThingManager::ThingManager(i32 max_things) {
    const usize need = static_cast<usize>(max_things) * sizeof(*things);

    BaseThing **array = NULL;
    if (*theMemoryManager.end_cell - *theMemoryManager.cursor_cell > need) {
        const usize aligned = ALIGN(*theMemoryManager.cursor_cell, 0x10);
        *theMemoryManager.cursor_cell = aligned + need;
        array = reinterpret_cast<BaseThing **>(aligned);
        memset(array, 0, need);
        theMemoryManager.allocated += need;
        theMemoryManager.remaining -= need;
        theMemoryManager.high_water = *theMemoryManager.cursor_cell;
    }
    this->things = array;
    this->max_things = max_things;
    // Profiling sets are a deferred subsystem; the handle is only ever passed
    // to the NuTimeBarSlotBegin/End stubs, so NULL behaves like the original
    // with profiling disabled.
    this->timebar = NuTimeBarCreateSet(0);
    theThingManager = this;
}

ThingManager::~ThingManager() {
}

void ThingManager::cbEdTimingSelect(eduimenu_s *, eduiitem_s *, u32) {
}

void ThingManager::cbEdTrackCancel(eduimenu_s *, eduimenu_s *) {
}

void ThingManager::edTimingEnter() {
}

void ThingManager::edTimingInit() {
}

void ThingManager::edTimingProc(float, nupad_s *) {
}

void ThingManager::edTimingRender() {
}

void SpecialObject::Exists() const {
}

void SpecialObject::GetCollision() const {
}

void SpecialObject::GetCurrentPosition() const {
}

void SpecialObject::GetCurrentTransform() const {
}

void SpecialObject::GetInitialPosition() const {
}

void SpecialObject::GetInitialTransform() const {
}

void SpecialObject::GetMtl(i32) const {
}

void SpecialObject::GetName() const {
}

void SpecialObject::GetNumMtls() const {
}

void SpecialObject::GetRadius() const {
}

void SpecialObject::GetVisibility() const {
}

void SpecialObject::Render(VuMtx const *) const {
}

void SpecialObject::SetCollision(i32) {
}

void SpecialObject::SetCurrentPosition(VuVec const *) {
}

void SpecialObject::SetCurrentTransform(VuMtx const *) {
}

void SpecialObject::SetInitialPosition(VuVec const *) {
}

void SpecialObject::SetInitialTransform(VuMtx const *) {
}

void SpecialObject::SetVisibility(i32) {
}

SpecialObject::SpecialObject() {
}

void GameThingManager::AddLevelOnlyThings() {
}

// GameThingManager::AddOnceOnlyThings @0x4e8bb0: registers the MechSystems
// singleton as the manager's once-only thing (via the virtual AddThing slot).
void GameThingManager::AddOnceOnlyThings() {
    this->AddThing(MechSystems::Get());
}

// GameThingManager::GameThingManager @0x4e8b00: stores the object in
// theGameThings (the vptr switch to the derived vtable is compiler-generated).
GameThingManager::GameThingManager(i32 max_things) : ThingManager(max_things) {
    theGameThings = this;
}

// GameThingManager D1 dtor @0x4e8a80 clears the global before destruction.
GameThingManager::~GameThingManager() {
    theGameThings = NULL;
}

CantPickupBombTimerAddon::CantPickupBombTimerAddon(MechObjectInterface &, float) {
}

void CantPickupBombTimerAddon::OnProcess(MechAddon::ProcessStage, float) {
}

CantPickupBombTimerAddon::~CantPickupBombTimerAddon() {
}

// BaseThing::BaseThing @0x425840 zeroes the data fields after the vptr.
BaseThing::BaseThing() {
    this->field_0x4 = 0;
    this->flags = 0;
    this->profiling_0xc = NULL;
}

// BaseThing defaults @0x424bf0 (dtor) and 0x425990..0x425a20 (interface
// defaults); RemoveDependancies returns 1, the rest are no-ops. GetName holds
// a 0 slot in the original base vtable (pure) — see basething.h.
BaseThing::~BaseThing() {
}

i32 BaseThing::RemoveDependancies(ThingRemoveData *) {
    return 1;
}

void BaseThing::EnterLevel(ThingLevelData *) {
}

void BaseThing::ExitLevel(ThingLevelData *) {
}

void BaseThing::Reset(ThingResetData *) {
}

void BaseThing::Process(ThingProcessData *) {
}

void BaseThing::ProcessEvenWhenPaused(ThingProcessData *) {
}

void BaseThing::ProcessOnlyWhenPaused(ThingProcessData *) {
}

void BaseThing::Render(ThingRenderData *) {
}

void BaseThing::Display(ThingRenderData *) {
}

void BaseThing::Effects(ThingRenderData *) {
}

static __used__ void LEGO_100PercentFn() {
}
static __used__ void LEGO_AllGoldBricksFn() {
}

void NoLayerKill(GameObject_s *) {
}

void GetUsageMask(NuShaderUsageMask_s *) {
}

void TakeOverCode(GameObject_s *, i32) {
}

void InitExtraList() {
}

GameObject_s *FindGameObject(i32 character_id, u32 required_flags, i32 alive_only, i32 vehicle_only,
                             i32 non_level_only) {
    for (i32 index = 0; index < HIGHGAMEOBJECT; ++index) {
        GameObject_s *object = &Obj[index];
        if ((object->apiobj.field_0x1f8 & APIOBJECT_FLAG_IN_USE) == 0) {
            continue;
        }
        if (vehicle_only != 0 && (object->apiobj.field_0x1f8 & 0x1000) == 0) {
            continue;
        }
        if (required_flags != 0 && (object->apiobj.field_0x1f4 & required_flags) != required_flags) {
            continue;
        }
        if (character_id != -1 && object->id != character_id) {
            continue;
        }
        if (alive_only != 0 && object->apiobj.field_0x287 != 0) {
            continue;
        }
        if (non_level_only != 0 && object->field_0x107c != -1) {
            continue;
        }
        return object;
    }
    return NULL;
}

void KillGameObject(GameObject_s *, i32, i32) {
}

void PowerUp_Update(GameObject_s *) {
}

void TakeOver2GetIn(GameObject_s *, GameObject_s *) {
}

void PowerUp_AddPart(nuvec_s *, nuvec_s *, float, float) {
}

void ScaleGameObject(GameObject_s *object) {
    const f32 scale = object->apiobj.field_0xa8;
    CHARACTERDATA *character = object->apiobj.character_data;
    object->apiobj.scaled_radius = character->field13_0x2c * scale;
    object->apiobj.collision_radius = object->field_0x1008 * scale;
    object->apiobj.collision_height = object->apiobj.collision_radius * object->collision_y_scale;
    object->apiobj.scaled_height = (character->field16_0x38 - character->field15_0x34) * object->field_0x1004;
}

void DestroySnakeBody(GameObject_s *obj);

void RemoveGameObject(GameObject_s *obj, i32) {
    if (obj == NULL) {
        return;
    }

    obj->KillTasks();
    obj->ClearAddons();
    obj->ClearMechObjectInterface();

    const u32 low_mask = ~obj->apiobj.field_0x1e4;
    const u32 high_mask = ~obj->apiobj.field_0x1e8;
    const u8 index = obj->apiobj.field_0x289;
    const u32 index_low_mask = index < 32 ? ~(1u << index) : ~0u;
    const u32 index_high_mask = index < 32 ? ~0u : ~(1u << (index - 32));
    for (i32 i = 0; i < HIGHGAMEOBJECT; i++) {
        Obj[i].apiobj.field_0x1ec &= low_mask;
        Obj[i].apiobj.field_0x1f0 &= high_mask;
        Obj[i].apiobj.field387_0x2a0 &= index_low_mask;
        Obj[i].apiobj.field388_0x2a4 &= index_high_mask;
        Obj[i].field_0xebc &= index_low_mask;
        Obj[i].field_0xec0 &= index_high_mask;
        Obj[i].field_0xec4 &= index_low_mask;
        Obj[i].field_0xec8 &= index_high_mask;
    }

    if (obj->pad_gamepad != NULL) {
        obj->pad_gamepad->allocated_5a &= ~1u;
    }
    DestroySnakeBody(obj);
    APIObjectDestroy(WORLD->api_object_sys, &obj->apiobj);

    HIGHGAMEOBJECT = 0;
    for (i32 i = 0; i < 64; i++) {
        if ((Obj[i].apiobj.field_0x1f8 & 1) != 0) {
            HIGHGAMEOBJECT = i + 1;
        }
    }
    for (i32 i = 0; i < 8; i++) {
        if (Player[i] == obj) {
            Player[i] = NULL;
        }
    }
}

void TargetGameObject(GameObject_s *, nuvec_s *, nuvec_s *, float, float, u32, i32, i32, i32) {
}

void ManageGameObjects() {
}

void PowerUp_GetPanelY(i32) {
}

void PowerUp_Particles(WORLDINFO_s *, nuvec_s *) {
}

void UpdateGameObjects(WORLDINFO_s *world) {
    if (world == NULL || Obj == NULL) {
        return;
    }

    SetPlayer();

    // AI updates are scheduled before the object/player movement passes. The
    // elapsed value is accumulated until an object becomes eligible for its
    // next script and path update.
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        GameObject_s *object = &Obj[i];
        const u16 character_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & character_flags) != character_flags) {
            continue;
        }

        if ((object->field_0xf00 & GAME_OBJECT_AI_UPDATE_PROCESS) != 0) {
            object->ai_elapsed_time = 0.0f;
        }
        object->ai_elapsed_time += FRAMETIME;

        const bool force_update =
            timebase_updates == 0 || (object->apiobj.field_0x1f4 & APIOBJECT_MOTION_FLAG_AI_CONTROLLED) == 0 ||
            (object->field_0xf00 & GAME_OBJECT_AI_UPDATE_SPECIAL_STATE) != 0 ||
            object->apiobj.supporting_platform_id != -1 || object->apiobj.field_0x27d == 0 ||
            (object->field_0xcc0 != NULL && object->character_context == CHARACTER_CONTEXT_LINKED_OBJECT) ||
            (object->ai.group != NULL && object->ai.group->is_in_formation);
        const i32 interval = force_update ? 1 : GameObjectAIUpdateInterval(world, object);

        if (interval <= 1) {
            object->field_0xf00 |= GAME_OBJECT_AI_UPDATE_FORCED | GAME_OBJECT_AI_UPDATE_PROCESS;
        } else {
            object->field_0xf00 &= ~GAME_OBJECT_AI_UPDATE_FORCED;
            const u32 update_phase =
                static_cast<u32>(object->apiobj.field_0x289) + static_cast<u32>(GameTimer.update_count);
            if (update_phase % static_cast<u32>(interval) == 0) {
                object->field_0xf00 |= GAME_OBJECT_AI_UPDATE_PROCESS;
            } else {
                object->field_0xf00 &= ~GAME_OBJECT_AI_UPDATE_PROCESS;
            }
        }

        // AI positions between scheduled updates are render extrapolations.
        // The target restores the last terrain-resolved position here before
        // GameAIProcess and the movement/terrain passes (0x140c2..0x140f0),
        // so an extrapolated floor offset never becomes the next collision
        // query's starting point.
        const u32 authoritative_position_flags =
            APIOBJECT_MOTION_FLAG_AI_CONTROLLED | APIOBJECT_STATE_FLAG_IGNORE_DOORS;
        if ((object->field_0xf00 & GAME_OBJECT_AI_UPDATE_PROCESS) != 0 &&
            (object->apiobj.field_0x1f4 & authoritative_position_flags) == APIOBJECT_MOTION_FLAG_AI_CONTROLLED) {
            object->apiobj.position.x = object->field_0x10c8;
            object->apiobj.position.y = object->field_0x10cc;
            object->apiobj.position.z = object->field_0x10d0;
        }
    }

    GameAIProcess();

    // The original has separate object and player passes. Ordinary hub
    // players reach this player pass with no movement override or spline.
    for (i32 i = 0; i < 8; ++i) {
        GameObject_s *object = Player[i];
        const u16 player_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_PLAYER_CHARACTER;
        if (object == NULL || (object->apiobj.field_0x1f8 & player_flags) != player_flags) {
            continue;
        }
        if (object->move_override != NULL) {
            object->move_override(object);
        } else {
            MovePlayer(object);
        }
    }

    // AI movement controllers run only on their scheduled AI frame and use
    // the full interval accumulated since the previous scheduled update.
    // Between scheduled updates the terrain/animation pass below advances the
    // last resolved velocity; calling MovePlayer here every frame would both
    // reselect path targets and integrate that velocity twice.
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        GameObject_s *object = &Obj[i];
        const u16 character_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & character_flags) != character_flags ||
            (object->apiobj.field_0x1f4 & APIOBJECT_MOTION_FLAG_AI_CONTROLLED) == 0 ||
            (object->field_0xf00 & GAME_OBJECT_AI_UPDATE_PROCESS) == 0) {
            continue;
        }

        bool is_local_player = false;
        for (i32 player_index = 0; player_index < 8; ++player_index) {
            if (Player[player_index] == object) {
                is_local_player = true;
                break;
            }
        }
        if (is_local_player) {
            continue;
        }

        const f32 frame_time = FRAMETIME;
        FRAMETIME = object->ai_elapsed_time;
        if (object->move_override != NULL) {
            object->move_override(object);
        } else if (object->movement_spline == NULL) {
            MovePlayer(object);
        } else {
            MovePlayerSpline(object);
        }
        FRAMETIME = frame_time;
    }

    // AI-controlled characters use their accumulated AI interval for a full
    // terrain update when their script was processed. Between those updates,
    // the original advances the last resolved velocity and still animates the
    // object every frame.
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        GameObject_s *object = &Obj[i];
        const u16 character_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & character_flags) != character_flags ||
            (object->apiobj.field_0x1f4 & APIOBJECT_MOTION_FLAG_AI_CONTROLLED) == 0) {
            continue;
        }

        bool is_local_player = false;
        for (i32 player_index = 0; player_index < 8; ++player_index) {
            if (Player[player_index] == object) {
                is_local_player = true;
                break;
            }
        }
        if (is_local_player) {
            // Player-owned objects have a dedicated terrain/animation pass
            // below. Processing them here as AI as well applies terrain state
            // twice and can discard the movement produced by MovePlayer.
            continue;
        }

        if ((object->field_0xf00 & GAME_OBJECT_AI_UPDATE_PROCESS) != 0) {
            const f32 frame_time = FRAMETIME;
            FRAMETIME = object->ai_elapsed_time;
            TerrainPlayer(object);

            object->field_0x10c8 = object->apiobj.position.x;
            object->field_0x10cc = object->apiobj.position.y;
            object->field_0x10d0 = object->apiobj.position.z;

            const f32 vertical_displacement = object->apiobj.position.y - object->apiobj.start_position.y;
            if (vertical_displacement == 0.0f || object->ai_elapsed_time == 0.0f) {
                object->vertical_velocity = 0.0f;
            } else {
                object->vertical_velocity = vertical_displacement / object->ai_elapsed_time;
            }
            FRAMETIME = frame_time;
        } else {
            PreResetCode(object);
            PostResetCode(object);
            if ((object->apiobj.field_0x1f4 & APIOBJECT_STATE_FLAG_IGNORE_DOORS) != 0) {
                GameObjectOrigin(object);
            } else {
                object->apiobj.position.x += object->apiobj.velocity.x * FRAMETIME;
                object->apiobj.position.y += object->vertical_velocity * FRAMETIME;
                object->apiobj.position.z += object->apiobj.velocity.z * FRAMETIME;
            }
        }

        AnimatePlayer(object);
        object->context_target_position = NULL;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
    }

    for (i32 i = 0; i < 8; ++i) {
        GameObject_s *object = Player[i];
        const u16 player_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_PLAYER_CHARACTER;
        if (object == NULL || (object->apiobj.field_0x1f8 & player_flags) != player_flags) {
            continue;
        }

        TerrainPlayer(object);
        KeepOnScreen(object);
        Doors_Check(world, object);
        object->field_0x10c8 = object->apiobj.position.x;
        object->field_0x10cc = object->apiobj.position.y;
        object->field_0x10d0 = object->apiobj.position.z;
        AnimatePlayer(object);
        object->context_target_position = NULL;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
    }

    // Character lighting is refreshed only for models which were visible in
    // the preceding render pass. The original amortises this pass across the
    // object array; the complete active set is small enough to update here.
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        GameObject_s *object = &Obj[i];
        const u16 character_flags = APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        if ((object->apiobj.field_0x1f8 & character_flags) == character_flags &&
            object->apiobj.model_draw_result != 0) {
            LightGameObject(object, world->rtl_set);
        }
    }
}

GameObject_s *AddDynamicCreature(i32 model, nuvec_s *position, i32 angle, char *script_name, AIPATHINFO_s *path_info,
                                 AIGROUP_s *group, i32 set_on_surface, nugspline_s *spline, nuvec_s *spline_offset,
                                 i32 spline_mode, i32 creature_set) {
    const bool has_no_spline = spline == NULL;
    if (position == NULL && spline == NULL) {
        return NULL;
    }

    if (NOAICREATURES != 0 && model != id_DRAGBOMB && (GCDataList[model].flags_090 & 0x40) == 0) {
        return NULL;
    }

    u32 arcade_mode = 0;
    Arcade_GetMode(&arcade_mode);
    if ((arcade_mode & 0x10) != 0) {
        return NULL;
    }

    if (static_cast<u32>(model) >= 0x154 || apicharsys->playermodelids[model] == -1) {
        return NULL;
    }

    GameObject_s *object = AddCreature(model, 0);
    if (object == NULL) {
        return NULL;
    }

    object->apiobj.field_0x1f4 |= APIOBJECT_MOTION_FLAG_AI_CONTROLLED;
    const u32 model_flags = apicharsys->char_data[model].model_flags;
    if ((model_flags & 0x200) != 0) {
        object->apiobj.field_0x1f4 |= 0x404;
    } else if ((model_flags & 0x400) != 0) {
        object->apiobj.field_0x1f4 |= 0x401;
    }
    object->field_0x1050 |= (model_flags & 0x1000) != 0 ? 5 : 1;

    GAMECHARACTERDATA &game_character = GCDataList[model];
    object->apiobj.viewdistance = game_character.viewdistance;
    object->apiobj.heardistance = game_character.heardistance;
    object->apiobj.maxviewheight = game_character.maxviewheight;
    object->apiobj.minviewheight = game_character.minviewheight;
    object->field_0xef9 &= static_cast<u8>(~8u);
    object->ai.field_0xe0 = 0x4e6e6b28;
    object->ai.field_0xf0 = 0x4e6e6b28;
    object->field_0xef8 &= static_cast<u8>(~1u);
    object->ai.field_0x1e5 &= static_cast<u8>(~0x50u);
    object->apiobj.field387_0x2a0 = 0;
    object->apiobj.field388_0x2a4 = 0;
    object->field_0xebc = 0;
    object->field_0xec0 = 0;
    object->field_0xecc = 0;
    object->field_0xed0 = 0;
    object->field_0xed8 = 0;
    object->ai.nearest_opponent = NULL;
    object->ai.nearest_opponent_metric = 0.0f;
    object->ai.field_0xdc = 0;
    object->ai.field_0xec = 0;
    object->ai.opponent = NULL;
    object->ai.antinode_timer = 0.0f;
    InitPlayerAI(object);

    if (position != NULL) {
        object->apiobj.position = *position;
        object->apiobj.facing_angle = static_cast<u16>(angle);
        object->apiobj.movement_facing_angle = static_cast<u16>(angle);
        object->apiobj.field_0x276 = static_cast<u16>(angle);
        if (group != NULL) {
            AddToAIGroup(group, &object->apiobj);
        }
    }

    // The spline-position branch continues through InitSplinePosition and
    // PointAlongSpline in the original. Keep the recovered parameters named
    // until that complete path is reconstructed rather than inventing a host
    // approximation here.
    (void)spline_offset;
    (void)spline_mode;

    ResetPlayerMoves(object);
    object->apiobj.pos_x = object->apiobj.position.x;
    object->apiobj.pos_y = object->apiobj.position.y;
    object->apiobj.pos_z = object->apiobj.position.z;
    object->apiobj.start_position = object->apiobj.position;
    object->apiobj.initial_position = object->apiobj.position;
    plr_lastpos = object->apiobj.position;
    object->apiobj.velocity = v000;

    GetTopBot(object);
    GameObjectDimensions(object);
    ResetRumble(&object->pad_gamepad->rumble_packet);
    ResetLights(&object->apiobj.position, &object->light_data, WORLD->rtl_set);

    object->sock_location_flags = 0;
    object->field_0x661 = 0xff;
    object->sock_segment = -1;
    object->facing_direction.x = NU_SIN_LUT(angle);
    object->facing_direction.y = 0.0f;
    object->facing_direction.z = NU_COS_LUT(angle);
    object->apiobj.model_draw_result = 1;
    object->use_model_origin = 0;
    object->apiobj.field_0x288 = 0;
    object->field_0xefe &= ~4u;

    if (has_no_spline) {
        InitSurfaceInfo(object);
        if (set_on_surface != 0) {
            SetObjOnSurface(object, 0);
        }
    } else {
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
    }

    object->apiobj.field_0x1f4 &= ~0x100u;
    object->apiobj.field_0x1f8 &= static_cast<u16>(~4u);
    object->field_0x1004 = 1.0f;
    object->apiobj.field_0x287 = 0;
    object->field_0x7a5 = 0xff;
    object->apiobj.field_0x285 = 0;
    memset(&object->ai.path_info, 0, sizeof(object->ai.path_info));
    object->ai.field_0x124 = -1;
    object->ai.field_0x138 = 0xff;
    object->ai.field_0x139 = 0;
    if (path_info != NULL) {
        AISysCharacterSetPath(&object->ai, path_info->path);
        AISysCharacterSetPathCnx(&object->ai, &object->apiobj.position, path_info->connection, path_info->direction);
    }
    if (object->ai.movement_target == NULL) {
        AISysGetCharacterPathPos(WORLD->ai_sys, &object->apiobj, &object->ai, 0xff,
                                 static_cast<i8>(object->apiobj.field_0x27d));
    }
    AIScriptProcessorInit(WORLD->ai_sys, &object->ai, reinterpret_cast<AISCRIPTPROCESS *>(&object->ai), NULL,
                          script_name, NULL, 1, NULL, NULL);
    object->apiobj.field_0x214 = 2000000.0f;
    PreResetCode(object);
    PostResetCode(object);
    GameObjectOrigin(object);
    object->apiobj.previous_position[0] = object->apiobj.position.x;
    object->apiobj.previous_position[1] = object->apiobj.position.y;
    object->apiobj.previous_position[2] = object->apiobj.position.z;
    object->field_0x10c8 = object->apiobj.position.x;
    object->field_0x10cc = object->apiobj.position.y;
    object->field_0x10d0 = object->apiobj.position.z;
    if (static_cast<u32>(creature_set - 1) < 16) {
        object->ai.creature_set = static_cast<u8>(creature_set);
        ++aicreature_sets_alive[creature_set - 1];
    }
    return object;
}

GameObject_s *GetNamedGameObject(AISYS_s *, char *) {
    return NULL;
}

void TakeOverGameObject(GameObject_s *, GameObject_s *, i32, i32) {
}

void TakeOverGameObject2(GameObject_s *, GameObject_s *, i32) {
}

void DeactivateGameObject(GameObject_s *) {
}

i32 EquivalentObject_Find(WORLDINFO_s *, nuhspecial_s *) {
    return 0;
}

void FindNearestGameObject(nuvec_s *, GameObject_s *, u32, float, float, i32, i32, i32, float *, i32,
                           i32 (*)(GameObject_s *), bool) {
}

void SetAllInstancesHidden(nugscn_s *) {
    memset(PortalVisiFlags, 0, sizeof(PortalVisiFlags));
}

void RemoveAnyChunkControls(i32 *) {
}

void RemoveChunkFromRenderStack(particlechunkrendertype_s *chunk, particlechunkrendertype_s **stack) {
    if (chunk->previous != NULL) {
        chunk->previous->next = chunk->next;
    } else if (*stack == chunk) {
        *stack = chunk->next;
    }
    if (chunk->next != NULL) {
        chunk->next->previous = chunk->previous;
    }
    chunk->previous = NULL;
    chunk->next = NULL;
}

void RemoveChunkControlFromStack(debris_chunk_control_s *control, debris_chunk_control_s **stack) {
    debris_chunk_control_s *current = *stack;
    while (current != NULL && current != control) {
        stack = &current->next;
        current = current->next;
    }
    if (current == control) {
        *stack = control->next;
    }
    control->next = NULL;
}

extern "C" debkeydatatype_s *debris_keystack;

void RemoveDebrisEffectFromStack(debkeydatatype_s *key) {
    if (key->next == NULL) {
        debris_keystack = key->previous;
        if (debris_keystack != NULL) {
            debris_keystack->next = NULL;
        }
    } else {
        key->next->previous = key->previous;
        if (key->previous != NULL) {
            key->previous->next = key->next;
        }
    }
    key->next = NULL;
    key->previous = NULL;
}

void ReStoreStatusTakeOverObjectSys(i32) {
}

extern "C" {

    i32 InModelList(APICHARACTERMODELLIST_s *list, i32 id, i32 *out_index) {
        if (list != NULL) {
            i32 i = 0;
            for (; list->model_id != -1; list++, i++) {
                if (list->model_id == id) {
                    if (out_index != NULL)
                        *out_index = i;
                    return 1;
                }
            }
        }
        if (out_index != NULL)
            *out_index = -1;
        return 0;
    }

} // extern "C"
