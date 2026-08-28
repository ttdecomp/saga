#include "legoapi/items/objects/gameobjects.h"
#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

#include <string.h>

// NuCore profiling timebars (nucore_plain.cpp): NuTimeBarCreateSet returns a
// deferred-subsystem stub handle; the slot functions are no-op stubs.
extern "C" {
    void *NuTimeBarCreateSet(i32);
    void _NuTimeBarSlotBegin(void *, i32, char const *);
    void _NuTimeBarSlotEnd(void *, i32);
}

// Written by ThingManager's ctor (original global @0x124f2e0, .bss).
extern void *theThingManager;

void legoSetMusicVolume(float);

void GameShadow(GameObject_s *, nuvec_s *, float, i32) {
}

void GameTiming(WORLDINFO_s *, float *) {
}

void GameFog_Set() {
}

void GameRayCast(nuvec_s *, nuvec_s *, float, i32) {
}

void GameAIProcess() {
}

void GameAISysInit() {
}

void GameFog_Reset() {
}

void Game_KillPart(PART_s *, i32) {
}

void GameAISysReset(AISYS_s *) {
}

void GameAttackInit() {
}

void GameAudio_Init(GAMEAUDIO *) {
}

void GameFog_Update(WORLDINFO_s *) {
}

void GameAudio_Reset() {
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

void GameObjectOrigin(GameObject_s *) {
}

void Game_IgnoreInput() {
}

void GameAI_TotalScore() {
}

void GameAudio_PlaySfx(i32, nuvec_s *, i32, i32) {
}

void GameDrawMenuEntry(MENU_s *, char *) {
}

void GameAnimSys_Update(GAMEANIMSYS_s *) {
}

void GameAudio_GetSfxId(i32) {
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
// up (SuperOptions[20] == 0 on the titles level); the title menu code restores
// the user's volume via GameGetMusicVolume once the menu phase starts.
f32 GameSetMusicVolume(OPTIONSSAVE_s *options) {
    extern char SuperOptions[24];
    f32 volume = GameGetMusicVolume(options);
    if (SuperOptions[20] == 0 && WORLD->current_level == TITLES_LDATA) {
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

void GameObjectDimensions(GameObject_s *) {
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

void GameDrawCharacterModel(CHARACTERMODEL_s *, ANIMPACKET_s *, numtx_s *, numtx_s *, numtx_s *, numtx_s *,
                            GameObject_s *, u32) {
}

void GameObjectToCameraCode(GameObject_s *) {
}

void GameRegisterGizActions() {
}

void GameAudio_GetPlrSfxBits(void *) {
}

void GameBlowUpBlownUpFn_LSW(GIZMOBLOWUP_s *) {
}

void GameLoadCharacterModels(APICHARACTERMODELLIST_s *, i32, variptr_u *, variptr_u *, i32, i32) {
}

void Game_100PercentComplete() {
}

void Game_WorldInfo_InitMenu(WORLDINFO_s *, i32 *, i32 *) {
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

void ThingManager::ResetThings(ThingResetData *) {
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

void ScaleGameObject(GameObject_s *) {
}

void RemoveGameObject(GameObject_s *, i32) {
}

void TargetGameObject(GameObject_s *, nuvec_s *, nuvec_s *, float, float, u32, i32, i32, i32) {
}

void ManageGameObjects() {
}

void PowerUp_GetPanelY(i32) {
}

void PowerUp_Particles(WORLDINFO_s *, nuvec_s *) {
}

void UpdateGameObjects(WORLDINFO_s *) {
}

GameObject_s *AddDynamicCreature(i32, nuvec_s *, i32, char *, AIPATHINFO_s *, AIGROUP_s *, i32, nugspline_s *,
                                 nuvec_s *, i32, i32) {
    return NULL;
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

void RemoveChunkFromRenderStack(particlechunkrendertype_s *, particlechunkrendertype_s **) {
}

void RemoveChunkControlFromStack(debris_chunk_control_s *, debris_chunk_control_s **) {
}

void RemoveDebrisEffectFromStack(debkeydatatype_s *) {
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
