#include "gameapi/gui/apimenu.h"

#include <stdio.h>
#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "gameframework/saveload.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/startup/main.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/movies.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numath/nutrig.h"

#define TOTAL_MENUS_COUNT 100
#define RESERVED_MENUS_COUNT 25

static void MenuDrawTitles(MENU *);
static void MenuUpdateTitles(MENU *);
static void MenuEnterTitles(MENU *);
static i32 UpdateTitleSequence(MENU *);
static void MenuDrawBonusComplete(MENU *);
static void MenuDrawBonusWin(MENU *);
static void MenuDrawHowToPlay(MENU *);
static void MenuDrawMiniKit(MENU *);
static void MenuDrawPauseCut(MENU *);
static void MenuDrawPauseExit(MENU *);
static void MenuDrawPauseMain(MENU *);
static void MenuDrawPauseRestart(MENU *);
static void MenuDrawSelectLanguage(MENU *);
static void MenuDrawShop(MENU *);
static void MenuExitHowToPlay(MENU *);
static void MenuInitHowToPlay(MENU *);
static void MenuUpdateBonusComplete(MENU *);
static void MenuUpdateBonusWin(MENU *);
static void MenuUpdateHowToPlay(MENU *);
static void MenuUpdateMiniKit(MENU *);
static void MenuUpdatePauseCut(MENU *);
static void MenuUpdatePauseExit(MENU *);
static void MenuUpdatePauseMain(MENU *);
static void MenuUpdatePauseRestart(MENU *);
static void MenuUpdateSelectLanguage(MENU *);
static void MenuUpdateShop(MENU *);

extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3);
extern "C" void BackupMenu(void);
extern "C" void NuPadSetStatus(i32 port, i32 status);
extern "C" void NuIOS_RecordFlurryEvent(char *event_name);
f32 GameGetMusicVolume(OPTIONSSAVE_s *options);
i32 GameAudio_GetSfxId(i32 sfx);
void legoSetMusicVolume(f32 volume);
void NewGame(void);
void MenuDrawNewGame(MENU *menu);
void MenuEnterNewGame(MENU *menu);
void MenuExitNewGame(MENU *menu);
void MenuUpdateNewGame(MENU *menu);
void MenuDrawLoad(MENU *menu);
void MenuDrawLoading(MENU *menu);
void MenuDrawAutoSaveCancel(MENU *menu);
void MenuDrawAutoSaveWarning(MENU *menu);
void MenuDrawCardWarning(MENU *menu);
void MenuDrawDeleteConfirm(MENU *menu);
void MenuDrawDeleting(MENU *menu);
void MenuDrawDoNotRemoveCard(MENU *menu);
void MenuDrawFileCorrupt(MENU *menu);
void MenuDrawFormatCancel(MENU *menu);
void MenuDrawFormatConfirm(MENU *menu);
void MenuDrawFormatting(MENU *menu);
void MenuDrawInsertCard(MENU *menu);
void MenuDrawLoadCancel(MENU *menu);
void MenuDrawLoadConfirm(MENU *menu);
void MenuDrawNoData(MENU *menu);
void MenuDrawNoMemoryCard(MENU *menu);
void MenuDrawNotEnoughSpace(MENU *menu);
void MenuDrawSave(MENU *menu);
void MenuDrawSaveCancel(MENU *menu);
void MenuDrawSaveConfirm(MENU *menu);
void MenuDrawSaving(MENU *menu);
void MenuEnterAutoSaveCancel(MENU *menu);
void MenuEnterAutoSaveWarning(MENU *menu);
void MenuEnterCardWarning(MENU *menu);
void MenuEnterHeaderSave(MENU *menu);
void MenuEnterInsertCard(MENU *menu);
void MenuEnterLoad(MENU *menu);
void MenuEnterNoMemoryCard(MENU *menu);
void MenuEnterSave(MENU *menu);
void MenuEnterSaveConfirm(MENU *menu);
void MenuEnterStartNewGame(MENU *menu);
void MenuExitCardWarning(MENU *menu);
void MenuExitLoad(MENU *menu);
void MenuExitSave(MENU *menu);
void MenuUpdateAutoSaveCancel(MENU *menu);
void MenuUpdateAutoSaveWarning(MENU *menu);
void MenuUpdateCardWarning(MENU *menu);
void MenuUpdateDeleteConfirm(MENU *menu);
void MenuUpdateDeleting(MENU *menu);
void MenuUpdateDoNotRemoveCard(MENU *menu);
void MenuUpdateFileCorrupt(MENU *menu);
void MenuUpdateFormatCancel(MENU *menu);
void MenuUpdateFormatConfirm(MENU *menu);
void MenuUpdateFormatting(MENU *menu);
void MenuUpdateInsertCard(MENU *menu);
void MenuUpdateLoad(MENU *menu);
void MenuUpdateLoadCancel(MENU *menu);
void MenuUpdateLoadConfirm(MENU *menu);
void MenuUpdateLoading(MENU *menu);
void MenuUpdateNoData(MENU *menu);
void MenuUpdateNoMemoryCard(MENU *menu);
void MenuUpdateNotEnoughSpace(MENU *menu);
void MenuUpdateSave(MENU *menu);
void MenuUpdateSaveCancel(MENU *menu);
void MenuUpdateSaveConfirm(MENU *menu);
void MenuUpdateSaving(MENU *menu);
extern char *Trailer[3];
extern OPTIONSSAVE TempOptions;
extern VARIPTR characterbuffer_ptr;
extern VARIPTR characterbuffer_end;
void MusicClearAll(void);
extern "C" void SoundKillAll(void);
void GameDrawMenuEntry(MENU *menu, char *text);
void DropOut(i32 player, i32 show_effect, i32 silent, i32 force);
i32 CanSaveAndExit(WORLDINFO_s *world);
extern i32 SuperStory;
extern GAMEPAD_s GamePad[64];
extern i32 pause_i_pad;

extern i16 tHELPANDOPTIONS;
extern i16 tEXTRAS;
extern i16 tDROPOUT;
extern i16 tMAP;
extern i16 tRESTARTDEMO;
extern i16 tQUIT;
extern i16 tRESTARTLEVEL;
extern i16 tYES;
extern i16 tNO;
extern f32 PauseMenus_X;
extern i32 PauseMenus_Align;
extern u8 MENUEXITR;
extern u8 MENUEXITG;
extern u8 MENUEXITB;
extern i32 reset_restart;
extern u32 ResetBits;
void Door_Reset(void);
void NewGameMode(void);

void Arcade_DrawEndMenu(MENU *menu);
void Arcade_UpdateEndMenu(MENU *menu);
void Credits_UpdateMenu(MENU *menu);
void CustomiserMenu_Draw(MENU *menu);
void CustomiserMenu_Update(MENU *menu);
void MenuDrawBonusMode(MENU *menu);
void MenuDrawClips(MENU *menu);
void MenuDrawDebugStore(MENU *menu);
void MenuDrawEndChallenge(MENU *menu);
void MenuDrawEndMission(MENU *menu);
void MenuDrawEpisodes(MENU *menu);
void MenuDrawExtras(MENU *menu);
void MenuDrawFreePlay(MENU *menu);
void MenuDrawHints(MENU *menu);
void MenuDrawMissions(MENU *menu);
void MenuDrawOptions(MENU *menu);
void MenuDrawRestoreNewGame(MENU *menu);
void MenuDrawSelectControls(MENU *menu);
void MenuDrawSelectMode(MENU *menu);
void MenuDrawStore(MENU *menu);
void MenuDrawStoreHolding(MENU *menu);
void MenuDrawStorePurchase(MENU *menu);
void MenuDrawStoreRestoring(MENU *menu);
void MenuEnterOptions(MENU *menu);
void MenuExitOptions(MENU *menu);
void MenuExitStore(MENU *menu);
void MenuExitStoreHolding(MENU *menu);
void MenuExitStorePurchase(MENU *menu);
void MenuExitStoreRestoring(MENU *menu);
void MenuInitBonusMode(MENU *menu);
void MenuInitClips(MENU *menu);
void MenuInitEpisodes(MENU *menu);
void MenuInitFreePlay(MENU *menu);
void MenuInitMissions(MENU *menu);
void MenuInitSelectMode(MENU *menu);
void MenuInitStore(MENU *menu);
void MenuInitStoreHolding(MENU *menu);
void MenuInitStorePurchase(MENU *menu);
void MenuInitStoreRestoring(MENU *menu);
void MenuUpdateBonusMode(MENU *menu);
void MenuUpdateClips(MENU *menu);
void MenuUpdateDebugStore(MENU *menu);
void MenuUpdateEndChallenge(MENU *menu);
void MenuUpdateEndMission(MENU *menu);
void MenuUpdateEpisodes(MENU *menu);
void MenuUpdateExtras(MENU *menu);
void MenuUpdateFreePlay(MENU *menu);
void MenuUpdateHints(MENU *menu);
void MenuUpdateMissions(MENU *menu);
void MenuUpdateOptions(MENU *menu);
void MenuUpdateRestoreNewGame(MENU *menu);
void MenuUpdateSelectControls(MENU *menu);
void MenuUpdateSelectMode(MENU *menu);
void MenuUpdateStore(MENU *menu);
void MenuUpdateStoreHolding(MENU *menu);
void MenuUpdateStorePurchase(MENU *menu);
void MenuUpdateStoreRestoring(MENU *menu);

MENU GameMenu[10];

i32 MenuSFX = -1;
i32 MENUSFX_MENUMOVE = -1;
i32 MENUSFX_MENUNOENTRY = -1;
i32 MENUSFX_MENUSELECT = -1;
i32 GameMenuLevel = -1;
i32 MenuValidated;
i32 MenuResult;
i32 CurrentMenuId;
i32 startnewgame;
i32 startnewgame_initiated;
f32 newgamecamtime;
i32 newgamecam;
f32 MenuAlpha;
i32 MenuA;
f32 MENUTEXTSCALE = 0.5f;

extern i16 tTOUCHTOSTART;
extern i16 tNEWGAME;
extern i16 tLOADGAME;
extern i16 tCONTROLS;
extern i16 tCONTROLLERCONNECTED;
extern i16 tTOUCH;
extern i16 tCONSOLE;
extern "C" i32 TestForController();
extern "C" void SmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment,
                            u8 red, u8 green, u8 blue, f32 max_width, i32 max_lines, void *message_box,
                            i32 suppress_draw, u32 alpha);

MENUFNINFO GameMenuInfo[33] = {
    {0, MenuEnterTitles, MenuDrawTitles, MenuUpdateTitles, NULL, -1, -1, 0},
    {1, MenuEnterNewGame, MenuDrawNewGame, MenuUpdateNewGame, MenuExitNewGame, -1, -1, 0},
    {2, NULL, MenuDrawPauseMain, MenuUpdatePauseMain, NULL, -1, -1, 0},
    {3, NULL, MenuDrawPauseRestart, MenuUpdatePauseRestart, NULL, -1, -1, 0},
    {4, MenuEnterOptions, MenuDrawOptions, MenuUpdateOptions, MenuExitOptions, 0, 0, 0},
    {5, NULL, MenuDrawExtras, MenuUpdateExtras, NULL, 0, 0, 0},
    {6, NULL, MenuDrawPauseExit, MenuUpdatePauseExit, NULL, -1, -1, 0},
    {7, NULL, MenuDrawSelectLanguage, MenuUpdateSelectLanguage, NULL, 0, 0, 0},
    {8, MenuInitMissions, MenuDrawMissions, MenuUpdateMissions, NULL, -1, -1, 0},
    {9, NULL, MenuDrawEndMission, MenuUpdateEndMission, NULL, -1, -1, 0},
    {10, NULL, MenuDrawEndChallenge, MenuUpdateEndChallenge, NULL, -1, -1, 0},
    {11, NULL, Arcade_DrawEndMenu, Arcade_UpdateEndMenu, NULL, -1, -1, 0},
    {12, NULL, CustomiserMenu_Draw, CustomiserMenu_Update, NULL, -1, -1, 0},
    {13, NULL, MenuDrawShop, MenuUpdateShop, NULL, -1, -1, 0},
    {14, NULL, MenuDrawMiniKit, MenuUpdateMiniKit, NULL, -1, -1, 0},
    {15, MenuInitSelectMode, MenuDrawSelectMode, MenuUpdateSelectMode, NULL, -1, -1, 0},
    {16, MenuInitBonusMode, MenuDrawBonusMode, MenuUpdateBonusMode, NULL, -1, -1, 0},
    {17, MenuInitFreePlay, MenuDrawFreePlay, MenuUpdateFreePlay, NULL, -1, -1, 0},
    {18, MenuInitEpisodes, MenuDrawEpisodes, MenuUpdateEpisodes, NULL, -1, -1, 0},
    {19, MenuInitClips, MenuDrawClips, MenuUpdateClips, NULL, -1, -1, 0},
    {20, MenuInitStoreHolding, MenuDrawStoreHolding, MenuUpdateStoreHolding, MenuExitStoreHolding, -1, -1, 0},
    {21, MenuInitStore, MenuDrawStore, MenuUpdateStore, MenuExitStore, -1, -1, 0},
    {22, MenuInitStoreRestoring, MenuDrawStoreRestoring, MenuUpdateStoreRestoring, MenuExitStoreRestoring, -1, -1, 0},
    {23, MenuInitStorePurchase, MenuDrawStorePurchase, MenuUpdateStorePurchase, MenuExitStorePurchase, -1, -1, 0},
    {24, NULL, MenuDrawDebugStore, MenuUpdateDebugStore, NULL, 0, 0, 0},
    {25, MenuInitHowToPlay, MenuDrawHowToPlay, MenuUpdateHowToPlay, MenuExitHowToPlay, 0, 0, 0},
    {26, NULL, MenuDrawHints, MenuUpdateHints, NULL, 0, 0, 0},
    {27, NULL, MenuDrawPauseCut, MenuUpdatePauseCut, NULL, -1, -1, 0},
    {29, NULL, MenuDrawRestoreNewGame, MenuUpdateRestoreNewGame, NULL, -1, -1, 0},
    {30, NULL, NULL, Credits_UpdateMenu, NULL, -1, -1, 0},
    {31, NULL, MenuDrawBonusWin, MenuUpdateBonusWin, NULL, -1, -1, 0},
    {32, NULL, MenuDrawBonusComplete, MenuUpdateBonusComplete, NULL, -1, -1, 0},
    {33, NULL, MenuDrawSelectControls, MenuUpdateSelectControls, NULL, -1, -1, 0},
};
MENUFNINFO MenuInfo[TOTAL_MENUS_COUNT] = {
    {1000, MenuEnterSave, MenuDrawSave, MenuUpdateSave, MenuExitSave, -1, -1, 0},
    {1001, NULL, MenuDrawDoNotRemoveCard, MenuUpdateDoNotRemoveCard, NULL, -1, -1, 0},
    {1002, MenuEnterInsertCard, MenuDrawInsertCard, MenuUpdateInsertCard, NULL, -1, -1, 0},
    {1004, MenuEnterNoMemoryCard, MenuDrawNoMemoryCard, MenuUpdateNoMemoryCard, NULL, -1, -1, 0},
    {1003, NULL, MenuDrawNotEnoughSpace, MenuUpdateNotEnoughSpace, NULL, -1, -1, 0},
    {1005, NULL, MenuDrawNoData, MenuUpdateNoData, NULL, -1, -1, 0},
    {1006, NULL, MenuDrawFormatConfirm, MenuUpdateFormatConfirm, NULL, -1, -1, 0},
    {1007, NULL, MenuDrawFormatting, MenuUpdateFormatting, NULL, -1, -1, 0},
    {1008, MenuEnterSaveConfirm, MenuDrawSaveConfirm, MenuUpdateSaveConfirm, NULL, -1, -1, 0},
    {1009, NULL, MenuDrawSaving, MenuUpdateSaving, NULL, -1, -1, 0},
    {1010, MenuEnterAutoSaveWarning, MenuDrawAutoSaveWarning, MenuUpdateAutoSaveWarning, NULL, -1, -1, 0},
    {1011, MenuEnterAutoSaveCancel, MenuDrawAutoSaveCancel, MenuUpdateAutoSaveCancel, NULL, -1, -1, 0},
    {1012, MenuEnterLoad, MenuDrawLoad, MenuUpdateLoad, MenuExitLoad, -1, -1, 0},
    {1013, NULL, MenuDrawLoadConfirm, MenuUpdateLoadConfirm, NULL, -1, -1, 0},
    {1014, NULL, MenuDrawLoading, MenuUpdateLoading, NULL, -1, -1, 0},
    {1015, NULL, MenuDrawDeleting, MenuUpdateDeleting, NULL, -1, -1, 0},
    {1016, NULL, MenuDrawSaveCancel, MenuUpdateSaveCancel, NULL, -1, -1, 0},
    {1017, NULL, MenuDrawLoadCancel, MenuUpdateLoadCancel, NULL, -1, -1, 0},
    {1018, NULL, MenuDrawFormatCancel, MenuUpdateFormatCancel, NULL, -1, -1, 0},
    {1019, MenuEnterCardWarning, MenuDrawCardWarning, MenuUpdateCardWarning, MenuExitCardWarning, -1, -1, 0},
    {1020, NULL, MenuDrawFileCorrupt, MenuUpdateFileCorrupt, NULL, -1, -1, 0},
    {1021, MenuEnterHeaderSave, NULL, NULL, NULL, -1, -1, 0},
    {1022, MenuEnterStartNewGame, NULL, NULL, NULL, -1, -1, 0},
    {1023, NULL, MenuDrawDeleteConfirm, MenuUpdateDeleteConfirm, NULL, -1, -1, 0},
    {1024, NULL, NULL, NULL, NULL, -1, -1, 0},
};

i32 MenusUsed = RESERVED_MENUS_COUNT;
i32 MenuLanguages = 1;
char MenuHeader[64];

static i32 header_r;
static i32 header_g;
static i32 header_b;

u8 MENUHEADERR = 0;
u8 MENUHEADERG = 0x7f;
u8 MENUHEADERB = 0xff;

i32 MenuFadeEnabled;
NUMTL *MenuFadeMtl;

i32 MenuDrawDropShadows;

char *apiGameName = "Game";
char *apiGameTitle = "Game";
char *apiGameSubTitle = "";

char *apitxt_YES = "Yes";
char *apitxt_NO = "No";
char *apitxt_OK = "OK";
char *apitxt_CANCEL = "Cancel";
char *apitxt_RETRY = "Retry";
char *apitxt_SLOT = "Profile";
char *apitxt_PRESENT = "Present";
char *apitxt_EMPTY = "Empty";
char *apitxt_NOSPACE = "No Space";
char *apitxt_CONTINUE = "Continue";
char *apitxt_SAVEGAME = "Save Game";
char *apitxt_LOADGAME = "Load Game";
char *apitxt_DELETEGAME = "Delete Game";
char *apitxt_GAMESAVED = "Save Successful";
char *apitxt_GAMEOVERWRITTEN = "Overwrite Successful";
char *apitxt_GAMELOADED = "Game Loaded";
char *apitxt_DELETING = "Deleting data.";
char *apitxt_FORMATTING = "Formatting.";
char *apitxt_GAMEOVERWRITING = "Overwriting data.";
char *apitxt_CORRUPTLOAD = "Load Corrupted";
char *apitxt_CONFIRMLOAD = "Confirm Load?";
char *apitxt_CONFIRMSAVE = "Confirm Save?";
char *apitxt_CONFIRMOVERWRITE = "Are you sure you wish to overwrite this save file?";
char *apitxt_DOYOUWANTTOSTART = "Do you want to start the game?";
char *apitxt_DOYOUWANTTOABORT = "Do you want to cancel the Save?";
char *apitxt_DOYOUWANTTOABORTLOAD = "Do you want to cancel the Load?";
char *apitxt_DOYOUWANTTOABORTFORMAT = "Do you want to cancel the Format?";
char *apitxt_CONTINUEWITHOUTSAVING = "Continue without Saving";
char *apitxt_NEWSAVE = "New Save";
char *apitxt_CONTINUEWITHOUTLOADING = "Continue without loading";
char *apitxt_COMPLETED = "Completed";
char *apitxt_SELECT = "Select";
char *apitxt_BACK = "Back";
char *apitxt_VIBRATION = "Vibration";
char *apitxt_CONFIRMDELETE = "Are you sure you wish to delete this save file?";
char *apitxt_PROGRESS = "Progress: %.1f%%";
char *apitxt_TIMEPLAYED = "Time Played: %s";
char *apitxt_CONTROLLERREMOVED = "The controller in controller port %i has been removed. Please insert a controller "
                                 "into controller port %i to continue with the game.";
char *apitxt_PRESSSTART = "Press START";
char *apitxt_GAMESAVING = "Saving game...";
char *apitxt_GAMELOADING = "Loading game...";
char *apitxt_FAILEDTOLOAD = "Failed to Load";
char *apitxt_FAILEDTOSAVE = "Failed to Save";
char *apitxt_FAILEDTOOVERWRITE = "Failed to Overwrite";
char *apitxt_NODATAAVAILABLE = "No %s data present";
char *apitxt_AUTOSAVE_ENABLED = "Autosave will now use Game %i";
char *apitxt_FORMATTINGCOMPLETE = "Formatting complete";
char *apitxt_DELETECOMPLETE = "Delete complete";
char *apitxt_NOTENOUGHSPACE = "Insufficient drive space to save games.";
char *apitxt_AUTOSAVE_CANCEL1 = " ";
char *apitxt_AUTOSAVE_CANCEL2 = " ";
char *apitxt_AUTOSAVE_CANCEL3 = " ";
char *apitxt_AUTOSAVE_WARNING = " ";
char *apitxt_LOADING = " ";
char *apitxt_SAVING = " ";

f32 AUTOSAVEICONY = 0.745f;
f32 AUTOSAVEICONX = 0.775f;
f32 AUTOSAVEICONSIZE = 0.075f;

void MenuReset(void) {
    memset(GameMenu, 0, sizeof(GameMenu));
    GameMenu[0].menu = -1;
    MenuSFX = -1;
    GameMenuLevel = 0;
}

void APIMenuDrawMemCardSlots(MENU *menu, f32 time) {
    UNIMPLEMENTED();
}

void APIMenuDrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    UNIMPLEMENTED();
}

void (*drawslotsfn)(MENU *, f32) = APIMenuDrawMemCardSlots;
void (*drawslotinfofn)(f32, f32, i32, i32) = APIMenuDrawGameState;

void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_info_count, i32 language_count,
                      void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled, i32 is_shadow_enabled) {
    char menus_used_str[64];

    i32 menus_used = TOTAL_MENUS_COUNT - RESERVED_MENUS_COUNT;
    if (menu_info_count <= TOTAL_MENUS_COUNT - RESERVED_MENUS_COUNT) {
        menus_used = menu_info_count;
    }

    for (i32 i = 0; i < menus_used; i++) {
        MenuInfo[RESERVED_MENUS_COUNT + i] = menu_info[i];
    }

    MenusUsed = menus_used + RESERVED_MENUS_COUNT;
    sprintf(menus_used_str, "Menus used: %d", MenusUsed);

    MenuLanguages = language_count;
    MenuHeader[0] = '\0';

    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;

    if (draw_save_slots_info_fn != NULL) {
        drawslotinfofn = draw_save_slots_info_fn;
    }

    MenuFadeEnabled = is_fade_enabled;
    MenuDrawDropShadows = is_shadow_enabled;

    NUMTL *menu_fade_mtl = NuMtlCreate(1);
    MenuFadeMtl = menu_fade_mtl;

    menu_fade_mtl->attribs.z_mode = 3;
    menu_fade_mtl->attribs.alpha_mode = 1;

    menu_fade_mtl->attribs.unknown_2_1_2 = 2;

    menu_fade_mtl->attribs.unknown_1_1_2 = 1;
    menu_fade_mtl->attribs.unknown_1_4_8 = 1;

    menu_fade_mtl->attribs.unknown_2_4 = 1;

    menu_fade_mtl->attribs.filter_mode = 1;

    NuMtlUpdate(menu_fade_mtl);
}

void MenuInitialise(MENUFNINFO *menu_info, i32 menu_info_count, i32 language_count,
                    void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled) {
    MenuInitialiseEx(menu_info, menu_info_count, language_count, NULL, is_fade_enabled, is_shadow_enabled);

    if (draw_save_slots_fn != NULL) {
        drawslotsfn = draw_save_slots_fn;
    }
}

void MenuLoadTechnicalStrings(char *filepath, char *language, VARIPTR *buf, VARIPTR buf_end) {
    NuStringTableSetBank(2);
    NuStringTableSetFormat(0);
    NuStringTableLoadCSV(filepath, buf, buf_end, "LABEL", language);

    apiGameName = (char *)NuStringTableGetByName("GAMENAME");
    apiGameTitle = (char *)NuStringTableGetByName("GAMETITLE");
    apiGameSubTitle = (char *)NuStringTableGetByName("GAMESUBTITLE");

    apitxt_YES = (char *)NuStringTableGetByName("GEN_YES");
    apitxt_NO = (char *)NuStringTableGetByName("GEN_NO");
    apitxt_OK = (char *)NuStringTableGetByName("GEN_OK");
    apitxt_CANCEL = (char *)NuStringTableGetByName("GEN_CANCEL");
    apitxt_RETRY = (char *)NuStringTableGetByName("GEN_RETRY");
    apitxt_SLOT = (char *)NuStringTableGetByName("GEN_SLOT");
    apitxt_PRESENT = (char *)NuStringTableGetByName("GEN_PRESENT");
    apitxt_EMPTY = (char *)NuStringTableGetByName("GEN_EMPTY");
    apitxt_NOSPACE = (char *)NuStringTableGetByName("GEN_NOSPACE");
    apitxt_CONTINUE = (char *)NuStringTableGetByName("GEN_CONTINUE");
    apitxt_SAVEGAME = (char *)NuStringTableGetByName("GEN_SAVEGAME");
    apitxt_LOADGAME = (char *)NuStringTableGetByName("GEN_LOADGAME");
    apitxt_DELETEGAME = (char *)NuStringTableGetByName("GEN_DELETEGAME");
    apitxt_GAMESAVED = (char *)NuStringTableGetByName("GEN_GAMESAVED");
    apitxt_GAMEOVERWRITTEN = (char *)NuStringTableGetByName("GEN_GAMEOVERWRITTEN");
    apitxt_GAMELOADED = (char *)NuStringTableGetByName("GEN_GAMELOADED");
    apitxt_DELETING = (char *)NuStringTableGetByName("GEN_DELETING");
    apitxt_FORMATTING = (char *)NuStringTableGetByName("GEN_FORMATTING");
    apitxt_CORRUPTLOAD = (char *)NuStringTableGetByName("GEN_CORRUPTLOAD");
    apitxt_CONFIRMLOAD = (char *)NuStringTableGetByName("GEN_CONFIRMLOAD");
    apitxt_CONFIRMSAVE = (char *)NuStringTableGetByName("GEN_CONFIRMSAVE");
    apitxt_CONFIRMOVERWRITE = (char *)NuStringTableGetByName("GEN_CONFIRMOVERWRITE");
    apitxt_DOYOUWANTTOSTART = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOSTART");
    apitxt_DOYOUWANTTOABORT = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORT");
    apitxt_DOYOUWANTTOABORTLOAD = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORTLOAD");
    apitxt_DOYOUWANTTOABORTFORMAT = (char *)NuStringTableGetByName("GEN_DOYOUWANTTOABORTFORMAT");
    apitxt_CONTINUEWITHOUTSAVING = (char *)NuStringTableGetByName("GEN_CONTINUEWITHOUTSAVING");
    apitxt_GAMEOVERWRITING = (char *)NuStringTableGetByName("GEN_GAMEOVERWRITING");
    apitxt_NEWSAVE = (char *)NuStringTableGetByName("GEN_NEWSAVE");
    apitxt_CONTINUEWITHOUTLOADING = (char *)NuStringTableGetByName("GEN_CONTINUEWITHOUTLOADING");
    apitxt_COMPLETED = (char *)NuStringTableGetByName("GEN_COMPLETED");
    apitxt_SELECT = (char *)NuStringTableGetByName("GEN_SELECT");
    apitxt_BACK = (char *)NuStringTableGetByName("GEN_BACK");
    apitxt_VIBRATION = (char *)NuStringTableGetByName("GEN_VIBRATION");
    apitxt_CONFIRMDELETE = (char *)NuStringTableGetByName("GEN_CONFIRMDELETE");
    apitxt_PROGRESS = (char *)NuStringTableGetByName("GEN_PROGRESS");
    apitxt_TIMEPLAYED = (char *)NuStringTableGetByName("GEN_TIMEPLAYED");
    apitxt_PRESSSTART = (char *)NuStringTableGetByName("GEN_PRESSSTART");

    apitxt_GAMESAVING = (char *)NuStringTableGetByName("PC_GAMESAVING");
    apitxt_GAMELOADING = (char *)NuStringTableGetByName("PC_GAMELOADING");
    apitxt_FAILEDTOLOAD = (char *)NuStringTableGetByName("PC_FAILEDTOLOAD");
    apitxt_FAILEDTOSAVE = (char *)NuStringTableGetByName("PC_FAILEDTOSAVE");
    apitxt_FAILEDTOOVERWRITE = (char *)NuStringTableGetByName("PC_FAILEDTOOVERWRITE");
    apitxt_NODATAAVAILABLE = (char *)NuStringTableGetByName("PC_NODATAAVAILABLE");
    apitxt_AUTOSAVE_ENABLED = (char *)NuStringTableGetByName("PC_AUTOSAVE_ENABLED");
    apitxt_FORMATTINGCOMPLETE = (char *)NuStringTableGetByName("PC_FORMATTINGCOMPLETE");
    apitxt_DELETECOMPLETE = (char *)NuStringTableGetByName("PC_DELETECOMPLETE");
    apitxt_NOTENOUGHSPACE = (char *)NuStringTableGetByName("PC_NOTENOUGHSPACE");
    apitxt_CONTROLLERREMOVED = (char *)NuStringTableGetByName("PC_CONTROLLERREMOVED");
}

static __used__ void MenuDrawShop(MENU *) {
}
static __used__ void MenuDrawTitles(MENU *) {
    if (GameTimer.time_elapsed >= 3.0f && GameTimer.time_elapsed_mod_seconds < 0.666f && TTab != NULL &&
        TTab[tTOUCHTOSTART] != NULL) {
        SmartTextEx(TTab[tTOUCHTOSTART], 0.0f, (pNuCam->fov / pNuCam->aspect) * -0.5f, 1.0f, MENUTEXTSCALE,
                    MENUTEXTSCALE, MENUTEXTSCALE, 0, 255, 255, 255, 1.7f, 1, NULL, 0,
                    static_cast<i32>(static_cast<f32>(MenuA) * newgamealpha));
    }
}
static __used__ void MenuUpdateShop(MENU *) {
}
static i32 UpdateTitleSequence(MENU *menu) {
    f32 music_volume = 0.0f;
    if (SuperOptions.music_enabled != 0) {
        music_volume = GameGetMusicVolume(&TempOptions);
    }

    switch (newgamefade) {
        case 0:
            legoSetMusicVolume(music_volume);
            return 0;

        case 1:
            if (newgamewait > 0.0f) {
                newgamewait -= FRAMETIME;
                legoSetMusicVolume(music_volume);
                return 1;
            }

            newgamealpha += FRAMETIME * 2.0f;
            if (newgamealpha > 1.0f) {
                newgamealpha = 1.0f;
                newgamefade = 0;
            }
            legoSetMusicVolume(music_volume);
            return 1;

        case 2:
            if (newgamewait > 0.0f) {
                newgamewait -= FRAMETIME;
                legoSetMusicVolume(0.0f);
                return 1;
            }

            newgamealpha -= FRAMETIME * 2.0f;
            if (newgamealpha > 0.0f) {
                legoSetMusicVolume(music_volume * newgamealpha);
                return 1;
            }

            newgamealpha = 0.0f;
            newgamewait = 0.1f;
            legoSetMusicVolume(0.0f);
            return 1;

        default:
            if (newgamewait > 0.0f) {
                newgamewait -= FRAMETIME;
                legoSetMusicVolume(0.0f);
                return 1;
            }
            break;
    }

    if (NOSOUND == 0) {
        music_man.StopAll(0);
        MusicClearAll();
        SoundKillAll();
    }

    const f32 movie_volume = static_cast<f32>(static_cast<u8>(Game.options_save.field4_0x4)) / 10.0f;
    Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL, movie_volume);
    FinishLoop(2);
    ResetTimer(&GameTimer, music_volume);
    menu->menu_time = music_volume;
    menu->unk = music_volume;
    newgamefade = 1;
    newgamealpha = music_volume;
    newgamewait = 0.5f;

    if (NOSOUND == 0 && NOMUSIC == 0) {
        music_man.StopAll(0);
        GamePlayMusic(WORLD->current_level, 0, &Game.options_save);
    }

    legoSetMusicVolume(music_volume);
    return 1;
}
static __used__ void MenuUpdateTitles(MENU *menu) {
    if (UpdateTitleSequence(menu) != 0 || GameTimer.time_elapsed < 4.0f || GameTimer.time_elapsed < 3.0f) {
        return;
    }

    if (menu->start_pressed == 0 && MechInputTouchMenuController::AnyTouchesThisFrame <= 0) {
        return;
    }

    MechInputTouchMenuController::AnyTouchesThisFrame = 0;
    NuPadSetStatus(0, 1);
    NewMenu(1, saveload_savepresent != 0, -1);
    MenuSFX = GameAudio_GetSfxId(0x30);
    NuIOS_RecordFlurryEvent("loadscreen_startgame");
}

void MenuDrawNewGame(MENU *menu) {
    if (MenuLoadOccurred != 0 || newgame_menudrawoff != 0) {
        return;
    }

    menu->draw_y = -0.5f - menu->centre_offset;
    f32 title_alpha = 0.0f;
    if (GameTimer.time_elapsed >= 4.0f) {
        title_alpha = 1.0f;
    } else if (GameTimer.time_elapsed >= 3.5f) {
        const i32 angle = static_cast<i32>((GameTimer.time_elapsed - 3.5f) * 16384.0f);
        title_alpha = NuTrigTable[angle & 0x7fff];
    }
    const i32 menu_alpha = static_cast<i32>(static_cast<f32>(MenuA) * newgamealpha);
    const i32 alpha = static_cast<i32>(static_cast<f32>(menu_alpha) * title_alpha);
    if (TTab != NULL) {
        DrawMenuEntryEx(menu, TTab[tNEWGAME], alpha);
        DrawMenuEntryEx(menu, TTab[tLOADGAME], alpha);
        if (TestForController() != 0) {
            DrawMenuEntryEx(menu, TTab[tCONTROLLERCONNECTED], alpha / 2);
        } else {
            char controls[64];
            NuStrCpy(controls, TTab[tCONTROLS]);
            NuStrCat(controls, ": ");
            const i16 control_text = MechInputTouchSystem::s_baseControlMode == 0 ? tCONSOLE : tTOUCH;
            NuStrCat(controls, TTab[control_text] != NULL ? TTab[control_text] : "?");
            DrawMenuEntryEx(menu, controls, alpha);
        }
    }
}

void MenuEnterNewGame(MENU *menu) {
    (void)menu;
    if (MenuLoadOccurred == 0 && startnewgame_initiated == 0 && startnewgame == 0) {
        saveload_autosave = -1;
    }
    MenuLoadOccurred = 0;
    MenuSaveOccurred = 0;
}

void MenuExitNewGame(MENU *) {
}

void MenuUpdateNewGame(MENU *menu) {
    if (menu->close_requested != 0 || menu->cancel_pressed != 0) {
        BackupMenu();
        return;
    }

    if (MenuLoadOccurred != 0) {
        MenuLoadOccurred = 0;
        NewLData = LOADGAME_LDATA;
        UsePlayerList = 2;
        return;
    }

    if (UpdateTitleSequence(menu) != 0 || GameTimer.time_elapsed < 4.0f) {
        return;
    }

    if (startnewgame_initiated != 0 || saveload_autosave != -1) {
        startnewgame_initiated = 0;
        startnewgame = 0;
        const i32 save_occurred = MenuSaveOccurred;
        NewGame();
        MenuSaveOccurred = save_occurred;
        NewLData = NEWGAME_LDATA;
        UsePlayerList = 2;
        newgamecam = 1;
        newgamecamtime = 0.0f;
        return;
    }

    if (menu->confirm_pressed == 0) {
        return;
    }

    if (menu->selected_item == 0) {
        MenuSFX = GameAudio_GetSfxId(0x30);
        if (PlayerProgress[0].active == 0 && PlayerProgress[1].active == 0) {
            PlayerProgress[0].active = 1;
        }
        if (TestForController() != 0) {
            NewMenu(1000, -1, -1);
        } else {
            NewMenu(33, -1, -1);
        }
        NuIOS_RecordFlurryEvent("newgame");
    } else if (menu->selected_item == 1) {
        MenuSFX = GameAudio_GetSfxId(0x30);
        NewMenu(1012, -1, -1);
        NuIOS_RecordFlurryEvent("loadgame");
    } else if (menu->selected_item == 2) {
        if (TestForController() != 0) {
            MenuSFX = GameAudio_GetSfxId(0x32);
            return;
        }

        MenuSFX = GameAudio_GetSfxId(0x30);
        SuperOptions.touch_controls = SuperOptions.touch_controls == 0;
        MechSystems::Get()->input_touch_system.control_mode = SuperOptions.touch_controls != 0 ? 2 : 1;
        TriggerExtraDataSave();
    }
}
static __used__ void MenuDrawMiniKit(MENU *) {
}
static __used__ void MenuEnterTitles(MENU *) {
}
static __used__ void MenuUpdateMiniKit(MENU *) {
}
static __used__ void MenuDrawBonusWin(MENU *) {
}
static __used__ void MenuDrawPauseCut(MENU *) {
}
static __used__ void MenuDrawHowToPlay(MENU *) {
}
static __used__ void MenuDrawPauseExit(MENU *) {
}
static __used__ void MenuDrawPauseMain(MENU *menu) {
    const i32 both_players_active = Players_BothActive();

    dme_r = 0;
    dme_g = 0xff;
    dme_b = 0x3f;

    if (TestForController() != 0) {
        DrawMenuEntryEx(menu, TTab[tCONTROLLERCONNECTED], MenuA / 2);
    } else {
        char controls[64];
        NuStrCpy(controls, TTab[tCONTROLS]);
        NuStrCat(controls, ": ");
        NuStrCat(controls, TTab[MechInputTouchSystem::s_baseControlMode == 0 ? tCONSOLE : tTOUCH]);
        GameDrawMenuEntry(menu, controls);
    }

    GameDrawMenuEntry(menu, TTab[tHELPANDOPTIONS]);

    if (SuperStory != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL) {
        DrawMenuEntryEx(menu, TTab[tEXTRAS], MenuA / 2);
    } else {
        GameDrawMenuEntry(menu, TTab[tEXTRAS]);
    }
    if (both_players_active != 0) {
        GameDrawMenuEntry(menu, TTab[tDROPOUT]);
    }

    if (WORLD->current_level == HUB_LDATA) {
        GameDrawMenuEntry(menu, TTab[tQUIT]);
    } else if (GAMEDEMO != 0) {
        GameDrawMenuEntry(menu, TTab[tRESTARTDEMO]);
    } else {
        GameDrawMenuEntry(menu, TTab[tMAP]);
    }
}
static __used__ void MenuExitHowToPlay(MENU *) {
}
static __used__ void MenuInitHowToPlay(MENU *) {
}
static __used__ void MenuUpdateBonusWin(MENU *) {
}
static __used__ void MenuUpdatePauseCut(MENU *) {
}
static __used__ void MenuUpdateHowToPlay(MENU *) {
}
static __used__ void MenuUpdatePauseExit(MENU *) {
}
static __used__ void MenuUpdatePauseMain(MENU *menu) {
    if (menu->confirm_pressed == 0) {
        return;
    }

    const i32 both_players_active = Players_BothActive();
    MenuSFX = GameAudio_GetSfxId(0x30);

    if (menu->selected_item == 0) {
        if (TestForController() != 0) {
            MenuSFX = GameAudio_GetSfxId(0x32);
            return;
        }

        SuperOptions.touch_controls = static_cast<u8>((SuperOptions.touch_controls + 1) % 2);
        MechSystems::Get()->input_touch_system.control_mode = SuperOptions.touch_controls != 0 ? 2 : 1;
        TriggerExtraDataSave();
        return;
    }

    if (menu->selected_item == 1) {
        NewMenu(25, -1, -1);
        return;
    }

    if (menu->selected_item == 2) {
        if (SuperStory != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL) {
            MenuSFX = GameAudio_GetSfxId(0x32);
        } else {
            NewMenu(5, -1, -1);
            NuIOS_RecordFlurryEvent("pause_extras");
        }
        return;
    }

    i32 next_item = 3;
    if (both_players_active != 0) {
        if (menu->selected_item == next_item) {
            for (i32 player = 0; player < 8; ++player) {
                if (Player[player] != NULL && Player[player]->pad_gamepad == &GamePad[pause_i_pad]) {
                    DropOut(player, 1, 0, 0);
                    break;
                }
            }
            return;
        }
        ++next_item;
    }

    if (menu->selected_item == next_item) {
        const i32 selected_row = CanSaveAndExit(WORLD) == 1 ? 2 : 1;
        NewMenu(6, selected_row, -1);
    }
}
static __used__ void MenuDrawPauseRestart(MENU *menu) {
    if (MenuStopDraw == 0) {
        SmartTextEx(TTab[tRESTARTLEVEL], PauseMenus_X, menu->draw_y - MENUDY * 0.25f, 1.0f, MENUTEXTSCALE,
                    MENUTEXTSCALE, MENUTEXTSCALE, PauseMenus_Align, MENUEXITR, MENUEXITG, MENUEXITB, 1.7f, 1, NULL, 0,
                    MenuA);
    }
    GameDrawMenuEntry(menu, TTab[tYES]);
    GameDrawMenuEntry(menu, TTab[tNO]);
}
static __used__ void MenuDrawBonusComplete(MENU *) {
}
static __used__ void MenuDrawSelectLanguage(MENU *) {
}
static __used__ void MenuUpdatePauseRestart(MENU *menu) {
    if (menu->confirm_pressed != 0 && menu->selected_item == 0) {
        Door_Reset();
        Players_InitPositions(WORLD);
        NewGameMode();
        reset_restart = 1;
        ResetBits |= 0x21;
        MenuSFX = GameAudio_GetSfxId(0x30);
    } else if (menu->cancel_pressed != 0 || menu->confirm_pressed != 0) {
        BackupMenu();
        MenuSFX = GameAudio_GetSfxId(0x31);
    }
}
static __used__ void MenuUpdateBonusComplete(MENU *) {
}
static __used__ void MenuUpdateSelectLanguage(MENU *) {
}
