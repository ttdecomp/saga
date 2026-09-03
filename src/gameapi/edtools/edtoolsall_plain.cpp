#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edstubs.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numath/numtx.h"

#include <string.h>

struct part_emit_s {
    i32 state_words[18];
    i32 instance_id;
    i32 trailing_state_words[8];
};
DECOMP_ASSERT(sizeof(part_emit_s) == 0x6c, "part_emit_s size");

struct edanim_param_s {
    i32 instance_id;
    u8 reserved_004[0x2d4 - 0x004];
};
DECOMP_ASSERT(sizeof(edanim_param_s) == 0x2d4, "edanim_param_s size");

struct edbridge_s {
    i32 instance_id;
    u8 reserved_04[0x18];
    u8 connection_index;
    u8 reserved_1d[0x44 - 0x1d];
};
DECOMP_ASSERT(sizeof(edbridge_s) == 0x44, "edbridge_s size");

extern "C" {
    extern debinftype *effecttypes;
    extern debinftype **debtab;
    extern i32 EDPP_MAX_TYPES;
    extern i32 edpp_types_used;
    extern usize edpp_page_scene[8];
    extern i32 edpp_page_used[8];
    extern i32 DEBPAGE_GENERAL;
    extern i32 DEBPAGE_CHARACTER;
    extern i32 DEBPAGE_AREA;
    part_emit_s part_emits[512];
    i32 part_page_on[8];
    i32 part_page_used[8];
    i32 edpart_instances_used;
    edanim_param_s AnimParams[64];
    i32 edanim_next_param;
    i32 edanim_params_used;
    i32 edanim_page_on[8];
    i32 edanim_page_used[8];
    edbridge_s edBridges[64];
    i32 edbri_bridges_used;
    i32 edbri_page_on[8];
    i32 edbri_page_scene[8];
    i32 edbri_page_used[8];
}

void FileLoadSingleEffectType(debinftype *, i32, char);
extern "C" void NuBridgeInit(void);

void edppDetermineNearest(float);

extern "C" void do_Pad_Standard_camera(edcam_s *camera, f32 delta_time, nupad_s *pad);
extern "C" void do_maya_mouse_camera(edcam_s *camera);
extern "C" void do_mouse_flymode_camera(edcam_s *camera, f32 delta_time);
extern "C" i32 NuKeyboard(i32 key);

static edcam_s gp_cam = {
    {0.0f, 0.0f, 0.0f},
    0,
    0,
    -2.0f,
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    1,
    1,
    {0.0f, 0.0f, 0.0f},
    0,
    0,
    {0.0015625f, 0.0015625f, 0.0015625f},
    2,
    2,
    0.0078125f,
    0.5f,
    0.2f,
    0.2f,
    4.0f,
    0.15f,
    0.2f,
    0.01f,
    0.1f,
    EDCAM_FREEDOM_POSITION_X | EDCAM_FREEDOM_POSITION_Y | EDCAM_FREEDOM_POSITION_Z | EDCAM_FREEDOM_PITCH |
        EDCAM_FREEDOM_YAW | EDCAM_FREEDOM_DISTANCE,
    {0, 0, 0},
};

static NUCAMERA *edmaincam = NULL;
static NUCAMERA *edinternalcam = NULL;

extern "C" {
    i32 PadFlyMode = 0;
    NUMTX *ed_remap_mtx = NULL;
    i32 edmain_cursor_enabled = 0;

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
        for (i32 i = 0; i < 64; ++i) {
            AnimParams[i].instance_id = -1;
        }
        edanim_next_param = 0;
        edanim_params_used = 0;
        memset(edanim_page_used, 0, sizeof(edanim_page_used));
        memset(edanim_page_on, 0, sizeof(edanim_page_on));
    }
    void edanimParticleDestroy(void) {
    }
    void edanimRegisterCubeDumpInfo(void) {
    }
    void edanimStartPage(i32) {
    }
    void edanimStopPage(i32) {
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
        NuBridgeInit();
        for (i32 i = 0; i < 64; ++i) {
            edBridges[i].instance_id = -1;
            edBridges[i].connection_index = 0xff;
        }
        memset(edbri_page_used, 0, sizeof(edbri_page_used));
        memset(edbri_page_scene, 0, sizeof(edbri_page_scene));
        memset(edbri_page_on, 0, sizeof(edbri_page_on));
        edbri_bridges_used = 0;
    }
    void edbriClearPage(void) {
    }
    void edbriStartAllPages(void) {
    }
    void edbriStopPage(void) {
    }
    f32 edcamGetDist(void) {
        return gp_cam.distance;
    }
    edcam_s *edcamGetEdCam(void) {
        return &gp_cam;
    }
    void edcamGetOffset(NUVEC *offset) {
        *offset = gp_cam.offset;
    }
    void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw) {
        if (position != NULL) {
            *position = gp_cam.position;
        }
        if (pitch != NULL) {
            *pitch = gp_cam.pitch;
        }
        if (yaw != NULL) {
            *yaw = gp_cam.yaw;
        }
    }
    void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw) {
        if (position != NULL) {
            *position = gp_cam.snapped_position;
        }
        if (pitch != NULL) {
            *pitch = gp_cam.snapped_pitch;
        }
        if (yaw != NULL) {
            *yaw = gp_cam.snapped_yaw;
        }
    }
    NUVEC *edcamGetPosPointer(void) {
        return &gp_cam.position;
    }
    void edcamMove(nupad_s *pad) {
        edcamMoveEx(pad, NuTimeGetFrameTime());
    }
    void edcamMoveEx(nupad_s *pad, f32 delta_time) {
        if (edmainGetCursorEnabled() != 0) {
            if (PadFlyMode == 0 || NuKeyboard(0x38) != 0) {
                do_maya_mouse_camera(&gp_cam);
            } else {
                do_mouse_flymode_camera(&gp_cam, delta_time);
            }
        }
        if (pad != NULL) {
            if (PadFlyMode == 0) {
                do_Pad_Standard_camera(&gp_cam, delta_time, pad);
            } else {
                do_Pad_flymode_camera(&gp_cam, delta_time, pad);
            }
        }
    }
    void edcamMtx(NUMTX *matrix) {
        NUVEC distance = {0.0f, 0.0f, gp_cam.distance};
        NuMtxSetTranslation(matrix, &distance);
        NuMtxRotateX(matrix, gp_cam.pitch);
        NuMtxRotateY(matrix, gp_cam.yaw);
        NuMtxTranslate(matrix, &gp_cam.position);
        NuMtxTranslate(matrix, &gp_cam.offset);
        if (ed_remap_mtx != NULL) {
            NuMtxMul(matrix, matrix, ed_remap_mtx);
            ed_remap_mtx = NULL;
        }
    }
    void edcamSet(void) {
        NUMTX matrix;
        edcamMtx(&matrix);
        edmainSetCamera(&matrix);
    }
    void edcamSetAdjustFreedom(bool position_x, bool position_y, bool position_z, bool pitch, bool yaw, bool distance) {
        gp_cam.allow_position_x = position_x;
        gp_cam.allow_position_y = position_y;
        gp_cam.allow_position_z = position_z;
        gp_cam.allow_pitch = pitch;
        gp_cam.allow_yaw = yaw;
        gp_cam.allow_distance = distance;
    }
    void edcamSetAng(i32 pitch, i32 yaw) {
        gp_cam.pitch = pitch;
        gp_cam.yaw = yaw;
    }
    void edcamSetAutoSpeed(f32 move_base, f32 move_distance_scale, f32 zoom_base, f32 zoom_distance_scale) {
        gp_cam.auto_move_base = move_base;
        gp_cam.auto_move_dist_scale = move_distance_scale;
        gp_cam.auto_zoom_base = zoom_base;
        gp_cam.auto_zoom_dist_scale = zoom_distance_scale;
    }
    void edcamSetDist(f32 distance) {
        gp_cam.distance = distance;
    }
    void edcamSetMouseSensitivity(f32 pitch, f32 yaw, f32 movement) {
        gp_cam.mouse_pitch_speed = pitch;
        gp_cam.mouse_yaw_speed = yaw;
        gp_cam.mouse_move_speed = movement;
    }
    void edcamSetOffset(NUVEC *offset) {
        gp_cam.offset = *offset;
    }
    void edcamSetPos(NUVEC *position) {
        gp_cam.position = *position;
        gp_cam.offset = {0.0f, 0.0f, 0.0f};
    }
    void edcamSetPosAng(NUVEC *position, i32 pitch, i32 yaw) {
        gp_cam.position = *position;
        gp_cam.offset = {0.0f, 0.0f, 0.0f};
        gp_cam.pitch = pitch;
        gp_cam.yaw = yaw;
    }
    void edcamSetSpeed(f32 position_x, f32 position_y, f32 position_z, f32 distance) {
        gp_cam.position_speed = {position_x, position_y, position_z};
        gp_cam.distance_speed = distance;
    }
    void edcamSetSpeedPos(f32 position_x, f32 position_y, f32 position_z) {
        gp_cam.position_speed = {position_x, position_y, position_z};
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
    void edgraStopPage(i8) {
    }
    void edmainActivate(void) {
    }
    void edmainClose(void) {
    }
    void edmainCurrent(void) {
    }
    void edmainExtCamera(NUCAMERA *camera) {
        edmaincam = camera != NULL ? camera : edinternalcam;
    }
    NUCAMERA *edmainGetCamera(void) {
        return edmaincam;
    }
    i32 edmainGetCursorEnabled(void) {
        return edmain_cursor_enabled;
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
    void edmainSetCamera(NUMTX *matrix) {
        edmaincam->mtx = *matrix;
        NuCameraSet(edmaincam);
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
        part_emit_s *emit = part_emits;
        part_emit_s *const emit_end = part_emits + 512;
        do {
            emit->instance_id = -1;
        } while (++emit != emit_end);
        memset(part_page_used, 0, sizeof(part_page_used));
        memset(part_page_on, 0, sizeof(part_page_on));
        edpart_instances_used = 0;
    }
    void edpartRegisterPointerToGameCharLocation(void) {
    }
    void edpartStopPage(i8 page) {
        (void)page;
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
    i32 edppLoadPage(char *path, i32 flag, usize scene) {
        (void)scene;
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
        edpp_page_scene[page_index] = scene;

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
    void edppStopPage(i32) {
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
