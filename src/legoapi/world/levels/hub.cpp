#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"

extern void Customiser_LoadAll(CUSTOMISER *, WORLDINFO_s *);

void Hub_Draw3D(WORLDINFO_s *) {
}

void Hub_InMenu() {
}

void Hub_Update(WORLDINFO_s *) {
}

void Hub_Outside() {
}

void Hub_DrawPanel(WORLDINFO_s *) {
}

void Hub_PanelBusy() {
}

void Hub_UpdateKit() {
}

void Hub_CallBarman(GameObject_s *) {
}

void Hub_ClearStats() {
}

void Hub_ResetPanel() {
}

void HubShopUnlocked() {
}

void Hub_CurrentArea() {
}

void Hub_BonusBuildIt(GIZBUILDIT_s *) {
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

void Hub_ActivateDoorMenu(LEVELDATA_s **) {
}

void HubCustomiserUnlocked() {
}

void Hub_GetRandomCharType() {
}

void Hub_DrawFreePlaySelect() {
}

void Hub_DrawImportantBrick(i32, float, float, float, i32, i32) {
}

void Hub_InitFreePlaySelect(i32, i32, i32) {
}

void HubMinikitViewerUnlocked() {
}

void Hub_UpdateFreePlaySelect() {
}

void Hub_Init(WORLDINFO_s *) {
}

// The level callback carries the streaming-buffer arguments used by most
// level loaders, but the original hub loader only forwards the world to the
// persistent character customiser.
void Hub_Load(WORLDINFO_s *world, variptr_u *, variptr_u *) {
    Customiser_LoadAll(CharacterCustomiser, world);
}

void Hub_Reset(WORLDINFO_s *) {
}

// Static Hub menu/drawing helpers. Stubbed to satisfy the symbol baseline.

static __used__ void Hub_DrawArcadeStats(float) {
}

static __used__ void Hub_DrawMiniKitCount(float, float, int, int, float) {
}

static __used__ void Hub_MakeFreePlayList(int, int) {
}

static __used__ void Hub_UpdateSelectMode() {
}

static __used__ void Hub_DrawBonusModeMenu(int, float) {
}

static __used__ void Hub_DrawSelectModeMenu(int, float) {
}

static __used__ void Hub_DrawSuperBonusStats(AREADATA_s *, float) {
}

static __used__ void Hub_MakeListCharactersAvailable(i16 *) {
}

void WipeBackToHub() {
}

void Condition_InHubArea(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char *, void *) {
}

void Condition_InHubAreaInit(AISYS_s *, char *, AISCRIPT_s *) {
}
