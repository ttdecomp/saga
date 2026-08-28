#include "gameapi/edtools/edgra.h"

extern "C" {

    void EdFileBackup(void) {
    }
    void EdFileReadMemCard(void) {
    }
    void EdFileWriteChar(void) {
    }
    void EdFileWriteFloat(void) {
    }
    void EdFileWriteInt(void) {
    }
    void EdFileWriteMemCard(void) {
    }
    void EdFileWriteNuVec(void) {
    }
    void EdFileWriteShort(void) {
    }
    void EdFileWriteUnsignedChar(void) {
    }
    void EdFileWriteUnsignedInt(void) {
    }
    void EdFileWriteUnsignedShort(void) {
    }
    void edDrawCross(void) {
    }
    void edGetMainMenu(void) {
    }
    void edGraDisableTerrainSwap(void) {
    }
    void edGraEnableTerrainSwap(void) {
    }
    void edGraInitTerrainSwapProtection(void) {
    }
    void edanimClearPage(void) {
    }
    void edanimLookupSpecial(void) {
    }
    void edanimParamReset(void) {
    }
    void edanimParticleDestroy(void) {
    }
    void edanimRegisterCubeDumpInfo(void) {
    }
    void edanimStartPage(void) {
    }
    void edanimStopPage(void) {
    }
    void edanimUpdateObjects(void) {
    }
    void edbitsDrawBBox(void) {
    }
    void edbitsDrawBasicCube(void) {
    }
    void edbitsDrawCircleTilted(void) {
    }
    void edbitsDrawCircleXY(void) {
    }
    void edbitsDrawCross(void) {
    }
    void edbitsDrawCube(void) {
    }
    void edbitsDrawDiagonalCross(void) {
    }
    void edbitsDrawOvalTilted(void) {
    }
    void edbitsDrawOvalXY(void) {
    }
    void edbitsDrawSphere(void) {
    }
    void edbitsDrawTorus(void) {
    }
    void edbitsGetSoundName(void) {
    }
    void edbitsLookupInstance(void) {
    }
    void edbitsLookupSound(void) {
    }
    void edbitsLookupSoundFX(void) {
    }
    void edbitsProcessCubemapDump(void) {
    }
    void edbitsRegisterDataPath(void) {
    }
    void edbitsRegisterEditMode(void) {
    }
    void edbitsRegisterEditorEnabledFlag(void) {
    }
    void edbitsRegisterLevel(void) {
    }
    void edbitsRegisterPlaySound(void) {
    }
    void edbitsRegisterRequestSound(void) {
    }
    void edbitsRegisterSaveFormat(void) {
    }
    void edbitsRegisterSoundEffect(void) {
    }
    void edbitsRegisterThingsScene(void) {
    }
    void edbitsSetSoundFxVolume(void) {
    }
    void edbitsStartCubemapDump(void) {
    }
    void edbitsVector2YZRot(void) {
    }
    void edbriBridgesReset(void) {
    }
    void edbriClearPage(void) {
    }
    void edbriStartAllPages(void) {
    }
    void edbriStopPage(void) {
    }
    void edcamGetDist(void) {
    }
    void edcamGetEdCam(void) {
    }
    void edcamGetOffset(void) {
    }
    void edcamGetPosAng(void) {
    }
    void edcamGetPosAngSnap(void) {
    }
    void edcamGetPosPointer(void) {
    }
    void edcamMove(void) {
    }
    void edcamMoveEx(void) {
    }
    void edcamMtx(void) {
    }
    void edcamSet(void) {
    }
    void edcamSetAdjustFreedom(void) {
    }
    void edcamSetAng(void) {
    }
    void edcamSetAutoSpeed(void) {
    }
    void edcamSetDist(void) {
    }
    void edcamSetMouseSensitivity(void) {
    }
    void edcamSetOffset(void) {
    }
    void edcamSetPos(void) {
    }
    void edcamSetPosAng(void) {
    }
    void edcamSetSpeed(void) {
    }
    void edcamSetSpeedPos(void) {
    }
    void edgraBufferUsage(void) {
    }
    void edgraClearPage(void) {
    }
    void edgraClumpsReset(void) {
    }
    void edgraInitAllClumps(void) {
    }
    void edgraSetMemoryBuffer(void) {
    }
    void edgraSetThinning(void) {
    }
    void edgraSetup(void) {
    }
    void edgraStopPage(i32) {
    }
    void edmainActivate(void) {
    }
    void edmainClose(void) {
    }
    void edmainCurrent(void) {
    }
    void edmainExtCamera(void) {
    }
    void edmainGetCamera(void) {
    }
    void edmainGetCursorEnabled(void) {
    }
    void edmainInit(void) {
    }
    void edmainInitEx(void) {
    }
    void edmainProcess(void) {
    }
    void edmainQueryLocVec(void) {
    }
    void edmainRegister(void) {
    }
    void edmainRegisterLocVec(void) {
    }
    void edmainRender(void) {
    }
    void edmainSetCamera(void) {
    }
    void edmainSetCursorEnabled(void) {
    }
    void edmainSetMainMenuScale(void) {
    }
    void edmainSetReturn(void) {
    }
    void edpartClearPage(void) {
    }
    void edpartDestroyAllParticles(void) {
    }
    void edpartLoadPageEx(void) {
    }
    void edpartParticleReset(void) {
    }
    void edpartRegisterPointerToGameCharLocation(void) {
    }
    void edpartStopPage(void) {
    }
    void edppClearPage(void) {
    }
    void edppDeleteEffect(void) {
    }
    void edppDestroyAllEffects(void) {
    }
    void edppDestroyAllParticles(void) {
    }
    void edppDrawSpheres(void) {
    }
    void edppDrawTorus(void) {
    }
    void edppFindAllSounds(void) {
    }
    // Parts-page loader (edppLoadPage @0x36c630) — a deferred subsystem. It
    // parses the .ptl particle/debris effect files into the registry consumed
    // by LookupDebrisEffectPage*. Until transcribed it loads nothing and
    // reports failure (-1), so every debris lookup misses as before a load.
    i32 edppLoadPage(char *path, i32 flag, i32 flags) {
        (void)path;
        (void)flag;
        (void)flags;
        return -1;
    }
    void edppRegisterPointerToGameCharLocation(void) {
    }
    void edppRestartAllEffectsInLevel(void) {
    }
    void edppSetSaveName(void) {
    }
    void edppStopPage(void) {
    }
    void edqrand(void) {
    }
    void edrtlBurnoutLoad(void) {
    }
    void edrtlCalculateBurnout(void) {
    }
    void edrtlCalculateBurnoutEx(void) {
    }
    void edrtlDrawFog(void) {
    }
    void edrtlDrawLight(void) {
    }
    void edrtlDrawLightEx(void) {
    }
    void edrtlGetFogSet(void) {
    }
    void eduiAddPropTextPickEnt(void) {
    }
    void eduiAddTextPickEnt(void) {
    }
    void eduiAddTextPickEntEx(void) {
    }
    void eduiCheckForPadMenuCancel(void) {
    }
    void eduiClearActiveMenu(void) {
    }
    void eduiCreate3LineMessageMenu(void) {
    }
    void eduiCreateMessageMenu(void) {
    }
    void eduiCursorOverMenu(void) {
    }
    void eduiFlushInteracts(void) {
    }
    void eduiGetActiveMenu(void) {
    }
    void eduiGetActiveMenuParent(void) {
    }
    void eduiGetAnalougePadValue(void) {
    }
    void eduiGetCameraEnabled(void) {
    }
    void eduiGetCursorCoords(void) {
    }
    void eduiGetCursorDelta(void) {
    }
    void eduiGetTopLevelParent(void) {
    }
    void eduiGetUsingMenuFocus(void) {
    }
    void eduiGradPickRead(void) {
    }
    void eduiGradStageAdd(void) {
    }
    void eduiGradStageAddRGB(void) {
    }
    void eduiGradStageDelete(void) {
    }
    void eduiGradStageSetHSV(void) {
    }
    void eduiGradStageSetRGB(void) {
    }
    void eduiIitemExpanderSetDepth(void) {
    }
    void eduiInit(void) {
    }
    void eduiInitMaterials(void) {
    }
    void eduiItemCheckCreate(void) {
    }
    void eduiItemColourPickCreate(void) {
    }
    void eduiItemColourPickSetHSV(void) {
    }
    void eduiItemColourPickSetRGB(void) {
    }
    void eduiItemColourSliderCreate(void) {
    }
    void eduiItemDataGradPickCreate(void) {
    }
    void eduiItemExpanderAddChild(void) {
    }
    void eduiItemExpanderCreate(void) {
    }
    void eduiItemFilePickCreate(void) {
    }
    void eduiItemFilePickSetFmt(void) {
    }
    void eduiItemFilterAddItem(void) {
    }
    void eduiItemFilterCreate(void) {
    }
    void eduiItemFilterRemoveItem(void) {
    }
    void eduiItemGradPickCreate(void) {
    }
    void eduiItemGraphAddOnionSkin(void) {
    }
    void eduiItemGraphCreate(void) {
    }
    void eduiItemGraphSetCursor(void) {
    }
    void eduiItemGraphSetLabels(void) {
    }
    void eduiItemGreyGradPickCreate(void) {
    }
    void eduiItemGreyPickCreate(void) {
    }
    void eduiItemNumberCreate(void) {
    }
    void eduiItemPropCreate(void) {
    }
    void eduiItemPropCreateEx(void) {
    }
    void eduiItemPropSetText(void) {
    }
    void eduiItemRender(void) {
    }
    void eduiItemSelCreate(void) {
    }
    void eduiItemSelWithClipColourCreate(void) {
    }
    void eduiItemSeparatorCreate(void) {
    }
    void eduiItemSetText(void) {
    }
    void eduiItemSliderCreate(void) {
    }
    void eduiItemSliderCreateInt(void) {
    }
    void eduiItemSliderSetFmt(void) {
    }
    void eduiItemSliderSetGranularity(void) {
    }
    void eduiItemSliderSetVal(void) {
    }
    void eduiItemSliderSetValEx(void) {
    }
    void eduiItemTextPickCreate(void) {
    }
    void eduiItemTextPickSetFmt(void) {
    }
    void eduiItemTextSelectorCreate(void) {
    }
    void eduiItemTexturePickCreate(void) {
    }
    void eduiItemToggleCreate(void) {
    }
    void eduiMenuAddItem(void) {
    }
    void eduiMenuAddItemAfter(void) {
    }
    void eduiMenuAddItemBefore(void) {
    }
    void eduiMenuAddItemFirst(void) {
    }
    void eduiMenuAddItemLast(void) {
    }
    void eduiMenuAttach(void) {
    }
    void eduiMenuCreate(void) {
    }
    void eduiMenuDestroy(void) {
    }
    void eduiMenuDestroyItems(void) {
    }
    void eduiMenuDetach(void) {
    }
    void eduiMenuEnsureSelection(void) {
    }
    void eduiMenuFitOnScreen(void) {
    }
    void eduiMenuFitWidth(void) {
    }
    void eduiMenuHighlight(void) {
    }
    void eduiMenuIsActive(void) {
    }
    void eduiMenuItemMoveDown(void) {
    }
    void eduiMenuItemMoveUp(void) {
    }
    void eduiMenuProcess(void) {
    }
    void eduiMenuProcessAux(void) {
    }
    void eduiMenuProcessInput(void) {
    }
    void eduiMenuProcessSelectedItem(void) {
    }
    void eduiMenuRemoveItem(void) {
    }
    void eduiMenuRender(void) {
    }
    void eduiMenuSelectFirstEntry(void) {
    }
    void eduiMenuSetAttr(void) {
    }
    void eduiMenuSetDisabled(void) {
    }
    void eduiMenuSetTransparency(void) {
    }
    void eduiMenuSortItemsByTxt(void) {
    }
    void eduiProcessCursor(void) {
    }
    void eduiProcessCursorDefault(void) {
    }
    void eduiProcessInteracts(void) {
    }
    void eduiRenderCursor(void) {
    }
    void eduiRenderInteracts(void) {
    }
    void eduiSetActiveMenu(void) {
    }
    void eduiSetCameraEnabled(void) {
    }
    void eduiSetCursorColour(void) {
    }
    void eduiSetCursorCoords(void) {
    }
    void eduiSetDefaultActiveMenu(void) {
    }
    void eduiSetFont(void) {
    }
    void eduiSetFontScale(void) {
    }
    void eduiSetGlobalSliderAccel(void) {
    }
    void eduiSetRenderPlane(void) {
    }
    void eduiSetUsingMenuFocus(void) {
    }
    void eduiShowCursor(void) {
    }
    void eduiUsedAlgPad(void) {
    }
    void eduicbCancelMessageMenu(void) {
    }
    void eduicbInteractSlider(void) {
    }
    void eduicbItemDestroy(void) {
    }
    void eduicbItemDestroyProp(void) {
    }
    void eduicbMenuCloseAllexpanders(void) {
    }
    void eduicbMenuOpenAllexpanders(void) {
    }
}