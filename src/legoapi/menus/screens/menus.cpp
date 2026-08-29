#include "decomp.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3);
i32 GetMenuID(void);
void MenuDrawBackground(void);
extern u32 GAMEPAD_START;

void UpdateGameMenu(GAMEPAD_s *pad, i32 a2) {
    (void)a2;
    if (pad == nullptr || GameMenuLevel < 0)
        return;

    MENU *menu = &GameMenu[GameMenuLevel];
    i16 menu_index = menu->menu;
    if (MenuValidated == 0 || menu_index == -1)
        return;

    if (MenuFadeEnabled == 0) {
        MenuAlpha = 1.0f;
    } else if (MenuAlpha < 1.0f) {
        MenuAlpha += FRAMETIME + FRAMETIME;
        if (MenuAlpha > 1.0f)
            MenuAlpha = 1.0f;
    }
    MenuA = static_cast<i32>(NuTrigTable[(static_cast<i32>(MenuAlpha * 16384.0f) >> 1) & 0x7fff] * 128.0f);

    menu->start_pressed = pad->buttons_down_08 & GAMEPAD_START;
    menu->menu_time += FRAMETIME;
    menu->unk += FRAMETIME;
    if (MenuInfo[menu_index].update_fn != NULL)
        MenuInfo[menu_index].update_fn(menu);
}
void GetParentMenuID() {
}
void GetMenuActiveChild(eduimenu_s *) {
}
void ResizePauseScreenTexture(i32, i32) {
}
void CodeMenu(MENU_s *) {
}
void ItemMenu(MENU_s *) {
}
i32 GetMenuID(void) {
    if (GameMenu[GameMenuLevel].menu != -1) {
        return MenuInfo[GameMenu[GameMenuLevel].menu].id;
    }
    return -1;
}
extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
    (void)param3;

    i32 menu_index = -1;
    for (i32 i = 0; i < MenusUsed; ++i) {
        if (MenuInfo[i].id == menu_id) {
            menu_index = i;
            break;
        }
    }

    if (menu_index == -1) {
        GameMenu[GameMenuLevel].menu = -1;
        return;
    }
    if (GameMenuLevel + 1 < 10) {
        ++GameMenuLevel;
    }

    MENU *menu = &GameMenu[GameMenuLevel];
    menu->menu_time = 0.0f;
    menu->unk = 0.0f;
    menu->menu = static_cast<i16>(menu_index);
    (void)menu_y;

    MenuStopDraw = 1;
    if (MenuInfo[menu_index].draw_fn != NULL) {
        MenuInfo[menu_index].draw_fn(menu);
    }
    MenuStopDraw = 0;
    if (MenuInfo[menu_index].enter_fn != NULL) {
        MenuInfo[menu_index].enter_fn(menu);
    }
}
