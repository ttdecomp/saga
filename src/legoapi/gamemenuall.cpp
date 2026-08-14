#include "decomp.h"
#include "legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void MenuDrawLoad(MENU_s *) {
}

void MenuDrawSave(MENU_s *) {
}

void MenuExitLoad(MENU_s *) {
}

void MenuExitSave(MENU_s *) {
}

void MenuDrawClips(MENU_s *) {
}

void MenuDrawHints(MENU_s *) {
}

void MenuDrawStore(MENU_s *) {
}

void MenuEnterLoad(MENU_s *) {
}

void MenuEnterSave(MENU_s *) {
}

void MenuExitStore(MENU_s *) {
}

void MenuInitClips(MENU_s *) {
}

void MenuInitStore(MENU_s *) {
}

void MenuStartLoad() {
}

void MenuStartSave() {
}

void RenderFileSel() {
}

void MakeMenuPacket() {
}

void MenuDrawExtras(MENU_s *) {
}

void MenuDrawNoData(MENU_s *) {
}

void MenuDrawSaving(MENU_s *) {
}

void MenuUpdateLoad(MENU_s *) {
}

void MenuUpdateSave(MENU_s *) {
}

void ProcessFileSel(float, nupad_s *) {
}

void RenderFileSel3(i32) {
}

void EndMissionsMenu() {
}

void IconScenes_Dump() {
}

void IconScenes_Init(char *, variptr_u *, variptr_u *) {
}

void IconScenes_Load(APICHARACTERMODELLIST_s *, i32, variptr_u *, variptr_u *) {
}

void MenuDrawLoading(MENU_s *) {
}

void MenuDrawNewGame(MENU_s *) {
}

void MenuDrawOptions(MENU_s *) {
}

void MenuExitNewGame(MENU_s *) {
}

void MenuExitOptions(MENU_s *) {
}

void MenuIsAvailable() {
}

void MenuUpdateClips(MENU_s *) {
}

void MenuUpdateHints(MENU_s *) {
}

void MenuUpdateStore(MENU_s *) {
}

void ProcessFileSel3(float, nupad_s *) {
}

void MenuDrawDeleting(MENU_s *) {
}

void MenuDrawEpisodes(MENU_s *) {
}

void MenuDrawFreePlay(MENU_s *) {
}

void MenuDrawMissions(MENU_s *) {
}

void MenuEnterNewGame(MENU_s *) {
}

void MenuEnterOptions(MENU_s *) {
}

void MenuInitEpisodes(MENU_s *) {
}

void MenuInitFreePlay(MENU_s *) {
}

void MenuInitMissions(MENU_s *) {
}

void MenuUpdateExtras(MENU_s *) {
}

void MenuUpdateNoData(MENU_s *) {
}

void MenuUpdateSaving(MENU_s *) {
}

void MenuDrawBonusMode(MENU_s *) {
}

void MenuInitBonusMode(MENU_s *) {
}

void MenuUpdateLoading(MENU_s *) {
}

void MenuUpdateNewGame(MENU_s *) {
}

void MenuUpdateOptions(MENU_s *) {
}

void IconScene_FindById(i32) {
}

void MenuDrawDebugStore(MENU_s *) {
}

void MenuDrawEndMission(MENU_s *) {
}

void MenuDrawFormatting(MENU_s *) {
}

void MenuDrawInsertCard(MENU_s *) {
}

void MenuDrawLoadCancel(MENU_s *) {
}

void MenuDrawSaveCancel(MENU_s *) {
}

void MenuDrawSelectMode(MENU_s *) {
}

void MenuInitSelectMode(MENU_s *) {
}

void MenuUpdateDeleting(MENU_s *) {
}

void MenuUpdateEpisodes(MENU_s *) {
}

void MenuUpdateFreePlay(MENU_s *) {
}

void MenuUpdateMissions(MENU_s *) {
}

void MenuDrawCardWarning(MENU_s *) {
}

void MenuDrawFileCorrupt(MENU_s *) {
}

void MenuDrawLoadConfirm(MENU_s *) {
}

void MenuDrawSaveConfirm(MENU_s *) {
}

void MenuEnterHeaderSave(MENU_s *) {
}

void MenuEnterInsertCard(MENU_s *) {
}

void MenuExitCardWarning(MENU_s *) {
}

void MenuUpdateBonusMode(MENU_s *) {
}

void MenuDrawEndChallenge(MENU_s *) {
}

void MenuDrawFormatCancel(MENU_s *) {
}

void MenuDrawNoMemoryCard(MENU_s *) {
}

void MenuDrawStoreHolding(MENU_s *) {
}

void MenuEnterCardWarning(MENU_s *) {
}

void MenuEnterSaveConfirm(MENU_s *) {
}

void MenuExitStoreHolding(MENU_s *) {
}

void MenuInitStoreHolding(MENU_s *) {
}

void MenuUpdateDebugStore(MENU_s *) {
}

void MenuUpdateEndMission(MENU_s *) {
}

void MenuUpdateFormatting(MENU_s *) {
}

void MenuUpdateInsertCard(MENU_s *) {
}

void MenuUpdateLoadCancel(MENU_s *) {
}

void MenuUpdateSaveCancel(MENU_s *) {
}

void MenuUpdateSelectMode(MENU_s *) {
}

void MakeFreePlayModelList(i32, i32, i32, i32, i32) {
}

void MenuDrawDeleteConfirm(MENU_s *) {
}

void MenuDrawFormatConfirm(MENU_s *) {
}

void MenuDrawStorePurchase(MENU_s *) {
}

void MenuEnterNoMemoryCard(MENU_s *) {
}

void MenuEnterStartNewGame(MENU_s *) {
}

void MenuExitStorePurchase(MENU_s *) {
}

void MenuInitStorePurchase(MENU_s *) {
}

void MenuUpdateCardWarning(MENU_s *) {
}

void MenuUpdateFileCorrupt(MENU_s *) {
}

void MenuUpdateLoadConfirm(MENU_s *) {
}

void MenuUpdateSaveConfirm(MENU_s *) {
}

void MenuDrawAutoSaveCancel(MENU_s *) {
}

void MenuDrawNotEnoughSpace(MENU_s *) {
}

void MenuDrawRestoreNewGame(MENU_s *) {
}

void MenuDrawSelectControls(MENU_s *) {
}

void MenuDrawStoreRestoring(MENU_s *) {
}

void MenuExitStoreRestoring(MENU_s *) {
}

void MenuInitStoreRestoring(MENU_s *) {
}

void MenuUpdateEndChallenge(MENU_s *) {
}

void MenuUpdateFormatCancel(MENU_s *) {
}

void MenuUpdateNoMemoryCard(MENU_s *) {
}

void MenuUpdateStoreHolding(MENU_s *) {
}

void MenuDrawAutoSaveWarning(MENU_s *) {
}

void MenuDrawDoNotRemoveCard(MENU_s *) {
}

void MenuDrawViewTextStrings(MENU_s *) {
}

void MenuEnterAutoSaveCancel(MENU_s *) {
}

void MenuUpdateDeleteConfirm(MENU_s *) {
}

void MenuUpdateFormatConfirm(MENU_s *) {
}

void MenuUpdateStorePurchase(MENU_s *) {
}

void MenuEnterAutoSaveWarning(MENU_s *) {
}

void MenuUpdateAutoSaveCancel(MENU_s *) {
}

void MenuUpdateNotEnoughSpace(MENU_s *) {
}

void MenuUpdateRestoreNewGame(MENU_s *) {
}

void MenuUpdateSelectControls(MENU_s *) {
}

void MenuUpdateStoreRestoring(MENU_s *) {
}

void MenuUpdateAutoSaveWarning(MENU_s *) {
}

void MenuUpdateDoNotRemoveCard(MENU_s *) {
}

void MenuUpdateViewTextStrings(MENU_s *) {
}

extern "C" {

        void BackupMenu(void) {
        }

        void BackupMenuNoFn(void) {
        }

        void CreateColourPicker(void) {
        }

        void CreateTestMenu(void) {
        }

        void DrawMenu(void) {
        }

        void DrawMenuBottomMessage(void) {
        }

        void DrawMenuButtonPrompts(void) {
        }

        void DrawMenuButtonPromptsEx(void) {
        }

        void DrawMenuEntry(void) {
        }

        void DrawMenuEntryEx(void) {
        }

        void DrawMenuHeader(void) {
        }

        void DrawMenuHeaderMessage(void) {
        }

        void DrawMenuTopMessage(void) {
        }

        void Draw_CANCEL(void) {
        }

        void Draw_CHECKINGMEMORYCARD(void) {
        }

        void Draw_DONOTREMOVEMEMORYCARD(void) {
        }

        void Draw_NOTENOUGHSPACE(void) {
        }

        void Draw_SPACENEEDED(void) {
        }

        void FileSelKill(void) {
        }

        void FlushMenuHighlights(void) {
        }

        void MenuCurrentID(void) {
        }

        void MenuDrawBackground(void) {
        }

        void MenuGetSlotNum(void) {
        }

        void MenuInCriticalMemoryCard(void) {
        }

        void MenuInMemoryCard(void) {
        }

        void MenuInMemoryCardLoad(void) {
        }

        void MenuInMemoryCardWarning(void) {
        }

        void MenuRegisterSoundFX(void) {
        }

        void MenuRememberCursor(void) {
        }

        void MenuRepeat(void) {
        }

        void MenuSetColours(void) {
        }

        void MenuSetHeaderDrawFn(void) {
        }

        void MenuSetPreDrawFn(void) {
        }

        void MenuSetPulsateSpeed(void) {
        }

        void MenuSetTopBottom(void) {
        }

        void MessageBoxInitMtl(void) {
        }

        void PetesHackOfDeath(void) {
        }

        void ProcessFileSel2(void) {
        }

        void RemapAddr(void) {
        }

        void RenderFileSel2(void) {
        }

        void SetButtonScaleMode(void) {
        }

        void StartFileSel(void) {
        }

        void TestMenu(void) {
        }

        void UpdateMenu(void) {
        }

        void cbCancelSubMenu(void) {
        }

        void cbCancelSubMenuFromItem(void) {
        }

        void cbCompateDirentByDateAsc(void) {
        }

        void cbCompateDirentByDateDec(void) {
        }

        void cbCompateDirentByNameAsc(void) {
        }

        void cbCompateDirentByNameDec(void) {
        }

        void cbCompateDirentBySizeAsc(void) {
        }

        void cbCompateDirentBySizeDec(void) {
        }

        void cbInteractMenuKeySelect(void) {
        }

        void cbInteractMenuScrollDown(void) {
        }

        void cbInteractMenuScrollTo(void) {
        }

        void cbInteractMenuScrollUp(void) {
        }

        void cbInteractMenuTitle(void) {
        }

        void cbModifierAdjust(void) {
        }

        void cbTriggerSubMenu(void) {
        }

        void nugraphAddPoint(void) {
        }

        void nugraphCalcCurve(void) {
        }

        void nugraphDeletePoint(void) {
        }

        void nugraphFileLoadGraph(void) {
        }

        void nugraphFileLoadGraphOnly(void) {
        }

        void nugraphFileLoadGraphOnlyToFile(void) {
        }

        void nugraphFileLoadTable(void) {
        }

        void nugraphFileSave(void) {
        }

        void nugraphFileSaveGraphOnly(void) {
        }

        void nugraphFileSaveGraphOnlyToFile(void) {
        }

        void nugraphFreeTempCurveData(void) {
        }

        void nugraphGenerateLookupTable(void) {
        }

        void nugraphGetXatT(void) {
        }

        void nugraphGetYatT(void) {
        }

        void nugraphGetYatX(void) {
        }

        void nugraphGetYatXScaled(void) {
        }

        void nugraphInit(void) {
        }

} // extern "C"

