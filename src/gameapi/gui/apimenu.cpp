#include "gameapi/gui/apimenu.h"

#include <stdio.h>
#include <string.h>

#include "decomp.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nustring.h"

#define TOTAL_MENUS_COUNT 100
#define RESERVED_MENUS_COUNT 25

MENU GameMenu[10];

i32 MenuSFX = -1;
i32 GameMenuLevel = -1;

MENUFNINFO GameMenuInfo[33] = {1};            // TODO: initialize with proper values
MENUFNINFO MenuInfo[TOTAL_MENUS_COUNT] = {1}; // TODO: initialize with proper values

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
