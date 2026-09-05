#include "decomp.h"
#include <string.h>

#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3);
extern "C" void loadsaveCallEachFrame(void);
extern "C" i32 TestForController(void);
i32 GetMenuID(void);
void MenuDrawBackground(void);
extern u32 GAMEPAD_START;
extern u32 GAMEPAD_SELECT;
extern u32 GAMEPAD_MENUSELECT;
extern u32 GAMEPAD_MENUCANCEL;
extern GAMEPAD_s GamePad[64];

void UpdateGameMenu(GAMEPAD_s *pad, i32 a2) {
    (void)a2;
    if (pad == nullptr || GameMenuLevel < 0)
        return;

    const u32 held = GamePad[0].unknown_04 | GamePad[1].unknown_04;
    const u32 pressed = GamePad[0].buttons_down_08 | GamePad[1].buttons_down_08;
    const u32 alternate_held = GamePad[0].unknown_0c | GamePad[1].unknown_0c;
    const u32 alternate_pressed = GamePad[0].unknown_10 | GamePad[1].unknown_10;
    UpdateMenu(held, pressed, alternate_held, alternate_pressed, FRAMETIME, GAMEPAD_MENUSELECT, GAMEPAD_MENUCANCEL,
               GAMEPAD_START, GAMEPAD_SELECT);
    loadsaveCallEachFrame();
}
i32 GetParentMenuID() {
    if (GameMenuLevel <= 1) {
        return -1;
    }

    const i16 parent_menu = GameMenu[GameMenuLevel - 1].menu;
    if (parent_menu == -1) {
        return -1;
    }
    return MenuInfo[parent_menu].id;
}
void GetMenuActiveChild(eduimenu_s *) {
}
void ResizePauseScreenTexture(i32, i32) {
}
i32 CodeMenu(MENU_s *) {
    return 0;
}

extern void GameAudio_PlaySfx(i32, nuvec_s *, i32, i32);
extern void GameCam_Blend(GAMECAMERA_s *, f32, f32, i32);
extern void Hint_CancelCurrent(void);
extern void DrawSubItemMenu2D(void);
extern void DrawSubItemMenu3D(void);
extern void DrawCodeMenu(void);
extern void DrawCodeMenu3D(void);
static i32 SubItemMenu(MENU_s *) {
    return 0;
}

static f32 ShopClamp01(f32 value) {
    return NuFmax(0.0f, NuFmin(value, 1.0f));
}

static f32 ShopSinePhase(f32 phase) {
    const i32 angle = static_cast<i32>(phase * 65536.0f + 16384.0f);
    return (NuTrigTable[(angle >> 1) & 0x7fff] + 1.0f) * 0.5f;
}

static __used__ i32 Shop_GetInput(SHOPINPUT *input) {
    memset(input, 0, sizeof(*input));

    const u32 held = GamePad[0].buttons_held | GamePad[1].buttons_held;
    const u32 pressed = GamePad[0].buttons_pressed | GamePad[1].buttons_pressed;
    const u32 alternate_held = GamePad[0].left_directions | GamePad[1].left_directions;
    const u32 alternate_pressed = GamePad[0].right_directions | GamePad[1].right_directions;
    const u32 active = held | pressed | alternate_held | alternate_pressed;

    // ItemMenu consumes the horizontal pair at offsets 0/4.  The vertical
    // pair follows at offsets 8/c in the original SHOPINPUT layout.
    input->value[0] = ((active & GAMEPAD_DLEFT) != 0) ? 1 : 0;
    input->value[1] = ((active & GAMEPAD_DRIGHT) != 0) ? 1 : 0;
    input->value[2] = ((active & GAMEPAD_DUP) != 0) ? 1 : 0;
    input->value[3] = ((active & GAMEPAD_DDOWN) != 0) ? 1 : 0;
    input->value[8] = ((pressed | alternate_pressed) & GAMEPAD_MENUSELECT) != 0 ? 1 : 0;
    input->value[9] = ((pressed | alternate_pressed) & GAMEPAD_MENUCANCEL) != 0 ? 1 : 0;
    return 0;
}

i32 ItemMenu(MENU_s *menu) {
    SHOPINPUT input;
    const i32 entry_picked = picked;
    Shop_GetInput(&input);

    if (slidetimer >= 0.0f) {
        slidetimer -= FRAMETIME;
        FRAMETIME = 0.0f;
    }
    if (ExitMenu)
        FRAMETIME = 0.0f;

    i32 candidate = entry_picked;
    i32 transition = 0;
    if (menu != nullptr && menu->confirm_pressed != 0 && menu->selected_row == 0) {
        candidate = static_cast<i32>(menu->selected_column) + 1;
        if (candidate == entry_picked) {
            input.value[8] = 1;
        }
    }

    itemchanged = 0;
    Hint_CancelCurrent();
    hintdrawwait = 0.2f;

    if (slidetimer < 0.0f) {
        movesfxlock = 0;

        if (input.value[0] != 0 && picked > 1) {
            candidate = picked - 1;
            if (candidate == 3)
                candidate = 2;
            if (candidate == 4) {
                if (SHOPGOLDBRICKS == 0)
                    candidate = picked - 2;
            }
            transition = 1;
        } else if (input.value[1] != 0 && picked <= 3) {
            candidate = picked + 1;
            if (candidate == 3)
                candidate = picked + 2;
            if (candidate == 4 && SHOPGOLDBRICKS == 0)
                candidate = picked;
            transition = 1;
        } else if (input.value[8] != 0) {
            if (picked == 0) {
                picked = 1;
            } else if (picked == 5) {
                picked = SHOPGOLDBRICKS != 0 ? 4 : 3;
            }
            candidate = picked;
        } else if (input.value[9] != 0) {
            transition = 1;
        }

        if (transition || candidate != entry_picked) {
            lastitem = picked;
            SubMenu = 0;
            picked = candidate <= 0 ? 1 : candidate > 5 ? 5 : candidate;

            slidetimer = 0.125f;
            if (movesfxlock == 0) {
                movesfxlock = 1;
                GameAudio_PlaySfx(0x2f, reinterpret_cast<nuvec_s *>(SubShelfPos) + 5, 0, 0);
            }
            GameCam_Blend(GameCam, 0.3f, 0.0f, 1);
            FRAMETIME = 0.0f;
        }
    }

    if (slidetimer >= 0.0f && lastitem != picked) {
        const f32 reverse_phase = slidetimer * 8.0f;
        const f32 old_factor = ShopClamp01(1.0f - ShopSinePhase(1.0f - reverse_phase));
        const f32 forward_factor = ShopClamp01(1.0f - ShopSinePhase(reverse_phase));

        topscale[lastitem] = TopBigScale[lastitem] + (TopShelfScale[lastitem] - TopBigScale[lastitem]) * old_factor;
        toppush[lastitem] = TopShelfPush[lastitem] + (TopBigPush[lastitem] - TopShelfPush[lastitem]) * forward_factor;

        if (picked != -1 && lastitem != -1) {
            topscale[picked] = TopBigScale[picked] + (TopShelfScale[picked] - TopBigScale[picked]) * old_factor;
            toppush[picked] = TopShelfPush[picked] + (TopBigPush[picked] - TopShelfPush[picked]) * old_factor;
        }
    }

    if (SubMenu == 0) {
        if (ExitMenu && slidetimer <= 0.0f) {
            menuptr = nullptr;
            return 5;
        }
        return 7;
    }

    if (slidetimer > 0.0f)
        return 7;

    picked = lastitem;
    SubMenu = 0;
    if (TopShelf[lastitem].type == 3) {
        lastitem = -1;
        easesubin = 1;
        inoutscale = 0.0f;
        cheatname = 0;
        slidetimer = 0.125f;
        shopmenu = 11;
        col = 0;

        ++currentmenulevel;
        menuparent[currentmenulevel] = menuptr;
        menuptr = CodeMenu;
        memcpy(usercode, "AAAAAA", 6);

        ++currentdrawlevel;
        drawparent[currentdrawlevel] = drawpanelptr;
        drawpanelptr = DrawCodeMenu;
        drawptr = DrawCodeMenu3D;
    } else {
        easesubin = 1;
        ++currentmenulevel;
        inoutscale = 0.0f;
        menuparent[currentmenulevel] = menuptr;
        menuptr = SubItemMenu;

        ++currentdrawlevel;
        slidetimer = 0.125f;
        drawparent[currentdrawlevel] = drawpanelptr;
        drawpanelptr = DrawSubItemMenu2D;
        drawptr = DrawSubItemMenu3D;
    }
    return 7;
}
i32 GetMenuID(void) {
    if (GameMenu[GameMenuLevel].menu != -1) {
        return MenuInfo[GameMenu[GameMenuLevel].menu].id;
    }
    return -1;
}
extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
    (void)param3;

    CurrentMenuId = menu_id;

    i32 menu_index = -1;
    for (i32 i = 0; i < MenusUsed; ++i) {
        if (MenuInfo[i].id == menu_id) {
            menu_index = i;
            break;
        }
    }

    if (menu_index == -1) {
        MENU *menu = &GameMenu[GameMenuLevel];
        menu->field_bc = 0;
        MenuRememberCursor(menu);
        menu->menu = -1;
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuValidated = 0;
        return;
    }

    MENU *previous = &GameMenu[GameMenuLevel];
    const i16 previous_menu = previous->menu;
    previous->field_bc = 0;
    MenuRememberCursor(previous);
    if (GameMenuLevel + 1 < 10) {
        ++GameMenuLevel;
    }

    MENU *menu = &GameMenu[GameMenuLevel];
    menu->menu_time = 0.0f;
    menu->unk = 0.0f;
    menu->menu = static_cast<i16>(menu_index);
    menu->previous_menu = static_cast<i8>(previous_menu);
    menu->first_column = 0;
    menu->first_row = 0;
    menu->last_column = 0;
    menu->last_row = 0;
    menu->state = 0;
    menu->draw_item = 0;
    menu->field_b8 = 1;
    menu->field_b4 = 1;
    menu->field_b0 = 0;
    menu->field_c0 = 0;
    menu->field_50 = 0;

    MenuStopDraw = 1;
    if (MenuInfo[menu_index].draw_fn != NULL) {
        MenuInfo[menu_index].draw_fn(menu);
    }
    MenuStopDraw = 0;
    menu->last_row = static_cast<i16>(menu->draw_item - 1);
    menu->selected_row = (menu_y >= menu->first_row && menu_y <= menu->last_row) ? static_cast<i16>(menu_y) : 0;
    menu->selected_column = 0;

    if (MenuInfo[menu_index].enter_fn != NULL) {
        MenuInfo[menu_index].enter_fn(menu);
    }

    if (MenuInfo[menu_index].memory_x != -1) {
        menu->selected_column = MenuInfo[menu_index].memory_x;
        if (menu->selected_column < menu->first_column) {
            menu->selected_column = menu->first_column;
        } else if (menu->selected_column > menu->last_column) {
            menu->selected_column = menu->last_column;
        }
    }
    if (menu_y < menu->first_row || menu_y > menu->last_row) {
        if (MenuInfo[menu_index].memory_y != -1) {
            menu->selected_row = MenuInfo[menu_index].memory_y;
            if (menu->selected_row < menu->first_row) {
                menu->selected_row = menu->first_row;
            } else if (menu->selected_row > menu->last_row) {
                menu->selected_row = menu->last_row;
            }
        }
    } else {
        menu->selected_row = static_cast<i16>(menu_y);
    }

    menu->menu_time = 0.0f;
    menu->unk = 0.0f;
    menu->flags_17 = -1;
    menu->transition_time = 0.0f;
    menu->transition_duration = 0.0f;
    memset(menu->item_width, 0, sizeof(menu->item_width));
    memset(menu->item_height, 0, sizeof(menu->item_height));
    menu->queued_item = -1;
    MenuAlpha = 0.0f;
    MenuA = 0;
    MenuValidated = 0;
}
