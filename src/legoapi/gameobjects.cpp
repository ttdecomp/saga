#include "legoapi_types.h"

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

void GameBufferAlloc(variptr_u *, variptr_u *, i32) {
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

void GameGetMusicVolume(OPTIONSSAVE_s *) {
}

void GameObjIsCableTied(GameObject_s *) {
}

void GameObjectRotation(GameObject_s *, i32) {
}

void GameSetMusicVolume(OPTIONSSAVE_s *) {
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

void Game_Exit(i32) {
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

void ThingManager::AddThing(BaseThing *) {
}

void ThingManager::AddThingAfterThis(BaseThing *) {
}

void ThingManager::DisplayThings(ThingRenderData *) {
}

void ThingManager::EffectsThings(ThingRenderData *) {
}

void ThingManager::EnableActions(i32, i32, i32) {
}

void ThingManager::EnterLevelThings(ThingLevelData *) {
}

void ThingManager::ExitLevelThings(ThingLevelData *) {
}

void ThingManager::ProcessThings(ThingProcessData *) {
}

void ThingManager::RemoveDependanciesThings(ThingRemoveData *) {
}

void ThingManager::RemoveTemporaryThings() {
}

void ThingManager::RenderThings(ThingRenderData *) {
}

void ThingManager::ResetThings(ThingResetData *) {
}

ThingManager::ThingManager(i32) {
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

void GameThingManager::AddOnceOnlyThings() {
}

GameThingManager::GameThingManager(i32) {
}

GameThingManager::~GameThingManager() {
}

CantPickupBombTimerAddon::CantPickupBombTimerAddon(MechObjectInterface &, float) {
}

void CantPickupBombTimerAddon::OnProcess(MechAddon::ProcessStage, float) {
}

CantPickupBombTimerAddon::~CantPickupBombTimerAddon() {
}

BaseThing::BaseThing() {
}

BaseThing::~BaseThing() {
}
