#include "gameapi/gui/apimenu.h"

#include <stdio.h>
#include <string.h>

#include "decomp.h"
#include "nu2api.saga/nu3d/numtl.h"

#define TOTAL_MENUS_COUNT 100
#define RESERVED_MENUS_COUNT 25

MENU GameMenu[10];
int MenuSFX = -1;
int GameMenuLevel = -1;
MENUFNINFO GameMenuInfo[33] = {1};            // TODO: initialize with proper values
MENUFNINFO MenuInfo[TOTAL_MENUS_COUNT] = {1}; // TODO: initialize with proper values
int MenusUsed = RESERVED_MENUS_COUNT;
int MenuLanguages = 1;
char MenuHeader[64];
static int header_r;
static int header_g;
static int header_b;
unsigned char MENUHEADERR = 0;
unsigned char MENUHEADERG = 127;
unsigned char MENUHEADERB = 255;
int MenuFadeEnabled;
int MenuDrawDropShadows;
NUMTL *MenuFadeMtl;

void MenuReset(void) {
    memset(GameMenu, 0, sizeof(GameMenu));
    GameMenu[0].menu = -1;
    MenuSFX = -1;
    GameMenuLevel = 0;
}

void APIMenuDrawMemCardSlots(MENU *menu, float time) {
    UNIMPLEMENTED();
}

void APIMenuDrawGameState(float x, float y, int highlight, int slot) {
    UNIMPLEMENTED();
}

void (*drawslotsfn)(MENU *, float) = APIMenuDrawMemCardSlots;
void (*drawslotinfofn)(float, float, int, int) = APIMenuDrawGameState;

void MenuInitialiseEx(MENUFNINFO *menu_infos, int menu_infos_count, int language_count,
                      void (*draw_save_slots_info_fn)(float, float, int, int), int fade_enabled, int shadow_enabled) {
    char menus_used_str[64];

    int menus_used = TOTAL_MENUS_COUNT - RESERVED_MENUS_COUNT;
    if (menu_infos_count <= TOTAL_MENUS_COUNT - RESERVED_MENUS_COUNT) {
        menus_used = menu_infos_count;
    }

    for (int i = 0; i < menus_used; i++) {
        MenuInfo[RESERVED_MENUS_COUNT + i] = menu_infos[i];
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

    MenuFadeEnabled = fade_enabled;
    MenuDrawDropShadows = shadow_enabled;

    NUMTL *menu_fade_mtl = NuMtlCreate(1);
    MenuFadeMtl = menu_fade_mtl;

    menu_fade_mtl->attribs.alpha_mode = 1;
    menu_fade_mtl->attribs.filter_mode = 1;
    menu_fade_mtl->attribs.unknown_1_1_2 = 1;
    menu_fade_mtl->attribs.unknown_1_4_8 = 1;
    menu_fade_mtl->attribs.z_mode = 3;
    menu_fade_mtl->attribs.unknown_2_1_2 = 2;
    menu_fade_mtl->attribs.unknown_2_4 = 1;

    NuMtlUpdate(menu_fade_mtl);
}

void MenuInitialise(MENUFNINFO *menu_infos, int menu_infos_count, int language_count,
                    void (*draw_save_slots_fn)(MENU *, float), int fade_enabled, int shadow_enabled) {
    MenuInitialiseEx(menu_infos, menu_infos_count, language_count, NULL, fade_enabled, shadow_enabled);

    if (draw_save_slots_fn != NULL) {
        drawslotsfn = draw_save_slots_fn;
    }
}
