#include "legoapi/items/objects/gameobjects.h"
#include "decomp.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/world/area.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numusic/sfx.h"

#include <string.h>

// NuCore profiling timebars (nucore_plain.cpp): NuTimeBarCreateSet returns a
// deferred-subsystem stub handle; the slot functions are no-op stubs.
extern "C" {
    void *NuTimeBarCreateSet(i32);
    void _NuTimeBarSlotBegin(void *, i32, char const *);
    u32 _NuTimeBarSlotEnd(void *, i32);
    void AddToAIGroup(AIGROUP_s *group, GameObject_s *object);
    extern NUVEC plr_lastpos;
}

// Written by ThingManager's ctor (original global @0x124f2e0, .bss).
extern void *theThingManager;

void legoSetMusicVolume(float);
void MovePlayer(GameObject_s *object);
void AnimatePlayer(GameObject_s *object);
void TerrainPlayer(GameObject_s *object);
void SetPlayer();
void InitPlayerAI(GameObject_s *object);
void ResetPlayerMoves(GameObject_s *object);
void GetTopBot(GameObject_s *object);
void ResetRumble(RUMBLEPACKET *packet);
void ResetLights(NUVEC *position, rtldata_s *data, void *set);
void InitSurfaceInfo(GameObject_s *object);
void SetObjOnSurface(GameObject_s *object, i32 mode);
void PortalGameObject(GameObject_s *object, i32 enable, i32 immediate, i16 portal, nugscn_s *scene);
void Arcade_GetMode(u32 *mode);

f32 GameShadow(GameObject_s *, NUVEC *, f32, i32) {
}

extern f32 MainRenderTime;
extern f32 MainRenderTargetTime;
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

struct FOGSET_s;

static i32 GameFogSnap;
static FOGSET_s *GameFogSet;
static f32 GameFogDuration;
static f32 GameFogTime;

void GameFog_Set() {
}

void GameRayCast(nuvec_s *, nuvec_s *, float, i32) {
}

void GameAIProcess() {
}

void GameAISysInit() {
}

void GameFog_Reset() {
    GameFogSnap = 1;
    GameFogDuration = 0.0f;
    GameFogSet = NULL;
    GameFogTime = 0.0f;
}

void Game_KillPart(PART_s *, i32) {
}

void GameAISysReset(AISYS_s *) {
}

void GameAttackInit() {
}

extern "C" void MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry);
i32 GameAudio_GetSfxId(i32 sfx);

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
}

void GameAudio_AddSfx(i32, i32 *, i32 *, i32) {
}

void GameObjectOrigin(GameObject_s *object) {
    // Common non-model-origin path at 0x46ec68 (Ghidra 0x47ec68).
    object->apiobj.field_0x1f4 &= ~0x100u;

    const f32 frame_time = FRAMETIME;
    const f32 vertical_offset = (object->field_0xffc + object->field_0x1000) * object->apiobj.field_0xa8 * 0.5f;
    const NUVEC center = {
        object->apiobj.position.x + object->apiobj.previous_velocity_x * frame_time,
        object->apiobj.position.y + object->apiobj.previous_velocity_y * frame_time + vertical_offset,
        object->apiobj.position.z + object->apiobj.previous_velocity_z * frame_time,
    };

    object->apiobj.pos_x = center.x;
    object->apiobj.pos_y = center.y;
    object->apiobj.pos_z = center.z;

    const f32 radius = object->apiobj.field_0x1dc;
    const f32 half_height = object->apiobj.field_0x1e0;
    object->apiobj.collision_min.x = center.x - radius;
    object->apiobj.collision_min.y = center.y - half_height;
    object->apiobj.collision_min.z = center.z - radius;
    object->apiobj.collision_max.x = center.x + radius;
    object->apiobj.collision_max.y = center.y + half_height;
    object->apiobj.collision_max.z = center.z + radius;

    object->apiobj.upper_position.x = center.x;
    object->apiobj.upper_position.y = object->apiobj.collision_max.y;
    object->apiobj.upper_position.z = center.z;
    object->apiobj.lower_position.x = center.x;
    object->apiobj.lower_position.y = object->apiobj.collision_min.y;
    object->apiobj.lower_position.z = center.z;
    object->apiobj.collision_origin.x = center.x;
    object->apiobj.collision_origin.y =
        object->apiobj.collision_min.y + object->apiobj.previous_velocity_y * frame_time;
    object->apiobj.collision_origin.z = center.z;
    object->ai.terrain_origin = object->apiobj.collision_origin;
}

void Game_IgnoreInput() {
}

void GameAI_TotalScore() {
}

void GameAudio_PlaySfx(i32, nuvec_s *, i32, i32) {
}

void GameDrawMenuEntry(MENU_s *menu, char *text) {
    if (Paused != 0) {
        dme_align = PauseMenus_Align;
        menu->draw_x = PauseMenus_X;
    }
    DrawMenuEntryEx(menu, text, static_cast<u8>(MenuA));
}

void GameAnimSys_Update(GAMEANIMSYS_s *) {
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

void GameAISysStartFrame(AISYS_s *) {
}

void GameDisplaySettings(LEVELDATADISPLAY *, i32 *) {
}

void GameObjectSetCanUse(GameObject_s *, void *, unsigned char, unsigned char, float) {
}

void GameObjOwnsAnyCables(GameObject_s *) {
}

void GameObjectDimensionsExtra_LSW(GameObject_s *object);

void GameObjectDimensions(GameObject_s *object) {
    object->apiobj.field_0x1dc = object->apiobj.collision_radius;
    object->apiobj.field_0x1e0 = object->apiobj.collision_height;
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

void GameAPIOBJECTFromObjID(unsigned char) {
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
}

void GameAudio_GetPlrSfxBits(void *) {
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

void ThingManager::DisplayThings(ThingRenderData *) {
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
// ProcessEvenWhenPaused (skip flag 0x20); then, per ThingProcessData.paused,
// either Process (skip 0x10) or ProcessOnlyWhenPaused (skip 0x40). The count
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
        if (thing == NULL || (thing->flags & 0x20)) {
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
            if (thing == NULL || (thing->flags & 0x40)) {
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
            if (thing == NULL || (thing->flags & 0x10)) {
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

// ThingManager::RenderThings @0x425390. Single pass over Render (skip 0x80),
// bracketed with timebar slot 1 ("Rnd").
void ThingManager::RenderThings(ThingRenderData *data) {
    static const char *name = "Rnd"; // timebar slot name @0x5734df

    if (this->count <= 0) {
        return;
    }
    for (i32 i = 0; i < this->count; i++) {
        BaseThing *thing = this->things[i];
        if (thing == NULL || (thing->flags & 0x80)) {
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
            i++;
        } while (i < this->count);
    }
}

// ThingManager::ThingManager @0x425870. Stores the manager in theThingManager
// and carves the thing-pointer array (max_things * 4 bytes) from the
// theMemoryManager linear pool. On allocation failure the array is NULL — the
// manager then simply never accepts things (AddThing's count < max check).
ThingManager::ThingManager(i32 max_things) {
    u8 *mm = theMemoryManager;
    u32 *cursor = *reinterpret_cast<u32 **>(mm + 0x8);
    u32 *endp = *reinterpret_cast<u32 **>(mm + 0xc);
    u32 need = (u32)max_things * 4;

    u32 array = 0;
    if ((u32)(*endp - *cursor) > need) {
        array = (*cursor + 0xf) & ~0xfu;
        *cursor = array + need;
        memset(reinterpret_cast<void *>(array), 0, need);
        *reinterpret_cast<u32 *>(mm + 0x14) += need;
        *reinterpret_cast<u32 *>(mm + 0x18) -= need;
        *reinterpret_cast<u32 *>(mm + 0x10) = *cursor;
    }
    this->things = reinterpret_cast<BaseThing **>(array);
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

char const *BaseThing::GetName() {
    return NULL;
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

GameObject_s *FindGameObject(i32, u32, i32, i32, i32) {
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
    object->apiobj.collision_height = object->apiobj.collision_radius * object->collision_height_scale;
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

    // The original has separate object and player passes. Ordinary hub
    // players reach this player pass with no movement override or spline.
    for (i32 i = 0; i < 8; ++i) {
        GameObject_s *object = Player[i];
        if (object == NULL || (object->apiobj.field_0x1f8 & 0x1001) != 0x1001) {
            continue;
        }
        if (object->move_override != NULL) {
            object->move_override(object);
        } else {
            MovePlayer(object);
        }
    }

    // Integrate the same ordinary active-object path as the original. The
    // terrain pass below is responsible for correcting the proposed player
    // position and updating contact state.
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        GameObject_s *object = &Obj[i];
        if ((object->apiobj.field_0x1f8 & 0x1001) != 0x1001 || (object->apiobj.field_0x1f4 & 0x400) == 0) {
            continue;
        }

        PreResetCode(object);
        PostResetCode(object);
        object->apiobj.position.x += object->apiobj.field_0x68 * FRAMETIME;
        object->apiobj.position.y += object->vertical_velocity * FRAMETIME;
        object->apiobj.position.z += object->apiobj.field_0x70 * FRAMETIME;
    }

    for (i32 i = 0; i < 8; ++i) {
        GameObject_s *object = Player[i];
        if (object == NULL || (object->apiobj.field_0x1f8 & 0x1001) != 0x1001) {
            continue;
        }

        TerrainPlayer(object);
        object->field_0x10c8 = object->apiobj.position.x;
        object->field_0x10cc = object->apiobj.position.y;
        object->field_0x10d0 = object->apiobj.position.z;
        AnimatePlayer(object);
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

    object->apiobj.field_0x1f4 |= 0x400;
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
            AddToAIGroup(group, object);
        }
    }

    // The spline-position branch continues through InitSplinePosition and
    // PointAlongSpline in the original. Keep the recovered parameters named
    // until that complete path is reconstructed rather than inventing a host
    // approximation here.
    (void)script_name;
    (void)path_info;
    (void)spline_offset;
    (void)spline_mode;

    ResetPlayerMoves(object);
    object->apiobj.pos_x = object->apiobj.position.x;
    object->apiobj.pos_y = object->apiobj.position.y;
    object->apiobj.pos_z = object->apiobj.position.z;
    object->apiobj.start_position[0] = object->apiobj.position.x;
    object->apiobj.start_position[1] = object->apiobj.position.y;
    object->apiobj.start_position[2] = object->apiobj.position.z;
    object->apiobj.initial_position[0] = object->apiobj.position.x;
    object->apiobj.initial_position[1] = object->apiobj.position.y;
    object->apiobj.initial_position[2] = object->apiobj.position.z;
    plr_lastpos = object->apiobj.position;
    object->apiobj.field_0x68 = v000.x;
    object->apiobj.field_0x6c = v000.y;
    object->apiobj.field_0x70 = v000.z;

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
    memset(object->ai.reset_state, 0, sizeof(object->ai.reset_state));
    object->ai.field_0x124 = -1;
    object->ai.field_0x138 = 0xff;
    object->ai.field_0x139 = 0;
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

void EquivalentObject_Find(WORLDINFO_s *, nuhspecial_s *) {
}

void FindNearestGameObject(nuvec_s *, GameObject_s *, u32, float, float, i32, i32, i32, float *, i32,
                           i32 (*)(GameObject_s *), bool) {
}

void SetAllInstancesHidden(nugscn_s *) {
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
