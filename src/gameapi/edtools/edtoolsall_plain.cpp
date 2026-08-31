#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edstubs.h"
#include "legoapi/legoapi_types.h"

extern "C" {
    extern debinftype *effecttypes;
    extern debinftype **debtab;
    extern i32 EDPP_MAX_TYPES;
    extern i32 edpp_types_used;
    extern i32 edpp_page_scene[8];
    extern i32 edpp_page_used[8];
    extern i32 DEBPAGE_GENERAL;
    extern i32 DEBPAGE_CHARACTER;
    extern i32 DEBPAGE_AREA;
}

void FileLoadSingleEffectType(debinftype *, i32, char);

void edppDetermineNearest(float);

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
    void edanimUpdateObjects(float) {
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
    void edbitsRegisterThingsScene(NUGSCN *) {
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
    void edgraSetup(VARIPTR *, VARIPTR, i32, i32, i32) {
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
    // Parts-page loader (edppLoadPage @0x36c630).  The normal general (0) and
    // character (5) pages only contain effect-type records; instance records
    // are read by the page-1/0 branches in the original and are deliberately
    // not entered here.
    i32 edppLoadPage(char *path, i32 flag, i32 flags) {
        u8 category;
        i32 page_index;
        if (flag == 0) {
            category = 0;
            page_index = 0;
        } else if (flag == 5) {
            category = 5;
            page_index = 1;
        } else {
            // The remaining page kinds have their own instance-record paths;
            // they are outside the general/character pages recovered here.
            return -1;
        }

        EdFileSetMedia(1);
        if (EdFileOpen(path, NUFILE_READ) == 0) {
            return -1;
        }
        EdFileSetReadWrongEndianess(1);

        i32 version = EdFileReadInt();
        if (version < 5 || version > 41) {
            EdFileSetReadWrongEndianess(0);
            EdFileClose();
            return -1;
        }

        edpp_page_used[page_index] = 1;
        edpp_page_scene[page_index] = flags;

        i32 requested = EdFileReadInt();
        i32 available = EDPP_MAX_TYPES - edpp_types_used;
        if (requested > available) {
            requested = available;
        }
        if (requested < 0) {
            requested = 0;
        }

        for (i32 n = 0; n < requested; n++) {
            i32 index = 1;
            while (index < EDPP_MAX_TYPES && debtab[index] != NULL) {
                index++;
            }
            if (index >= EDPP_MAX_TYPES) {
                break;
            }

            debinftype *effect = &effecttypes[index];
            FileLoadSingleEffectType(effect, version, static_cast<char>(category));
            effect->native_data = NULL;
            effect->last_render_time = 0.0f;
            effect->page = static_cast<u8>(page_index);
            debtab[index] = effect;
            edpp_types_used++;
        }

        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        edppDetermineNearest(1.0f);

        if (flag == 0) {
            DEBPAGE_GENERAL = page_index;
        } else if (flag == 5) {
            DEBPAGE_CHARACTER = page_index;
        }
        return page_index;
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
