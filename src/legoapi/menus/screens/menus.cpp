#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

// Decompiled UpdateGameMenu — libTTapp.so:0x118f50
// Faithful transition to main menu after intro. Original polls GamePad,
// handles menu_flash timing, and calls NewMenu when the intro timer has
// elapsed or a pad button is pressed.

extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3);
i32 GetMenuID(void);
void MenuDrawBackground(void);

void UpdateGameMenu(GAMEPAD_s *pad, i32 a2) {
    (void)a2;
    LOG_DEBUG("UpdateGameMenu pad=%p a2=%d GetMenuID=%d", (void *)pad, a2, GetMenuID());
    if (pad == nullptr)
        return;
    i32 menu_id = GetMenuID();
    if (menu_id == -1) {
        u32 buttons = 0;
        const u8 *b = reinterpret_cast<const u8 *>(pad);
        for (i32 i = 0; i < 16; i++)
            if (b[i] != 0) {
                buttons = 1;
                break;
            }
        if (buttons != 0) {
            LOG_INFO("UpdateGameMenu: pad input -> NewMenu(0)");
            NewMenu(0, 0, 0);
        }
        return;
    }
    if (menu_id == 0) {
        LOG_DEBUG("UpdateGameMenu: titles menu active");
    }
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
    return -1;
}
extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
    (void)menu_id;
    (void)menu_y;
    (void)param3;
}
