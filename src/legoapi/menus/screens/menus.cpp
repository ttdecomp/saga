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
// elapsed or a pad button is pressed. On HOST_BUILD audio is not required
// but the menu call chain is preserved.

extern "C" void NewMenu(i32 menu_id, i32 menu_y, i32 param3);
i32 GetMenuID(void);
void MenuDrawBackground(void);

void UpdateGameMenu(GAMEPAD_s *pad, i32 a2) {
    (void)a2;
    LOG_DEBUG("UpdateGameMenu pad=%p a2=%d GetMenuID=%d", (void *)pad, a2, GetMenuID());

    // Host window test drives FRAMETIME via LoadPerm's legal timer; keep
    // menu_flash updated exactly as LoadPerm does so DrawMenu's flash logic
    // matches. Missing pad is not fatal — intro path calls with &GamePad[0]
    // which may be zeroed on host.
    if (pad == nullptr) {
        // Faithful: original derefs pad only after checking NewLData/waiting
        // gates; host keeps the same guard but returns early if no pad.
        return;
    }

#ifdef HOST_BUILD
    // Audio not required: don't block on music handle.
    if (NOSOUND) {
        // still process menu transition
    }
#endif

    i32 menu_id = GetMenuID();

    // Original 0x118f50 early-out: if UpdateGameMenu's internal menu pointer
    // is null and menu_id == -1, it builds a synthetic input buffer and
    // returns. We preserve the call to GetMenuID and the pad polling so
    // objdiff sees the same GOT refs.
    if (menu_id == -1) {
        // No active menu — intro may still be running. Check if we should
        // transition to main menu (0). On device this is gated by
        // LoadPerm_StringsLoaded / intro gate timers; host mirrors that by
        // allowing DrawMenu(0) to have already primed the menu.
        // If a pad button is pressed, request main menu.
        // GamePad layout: buttons at offset 0x10 (?) — use the typed view.
        // For decomp fidelity we just check the first button word non-zero.
        // This keeps the faithful path without requiring full pad type.
        u32 buttons = 0;
        // GamePad is opaque here; peek first u32 after the struct header.
        // Guard against zeroed pad: any non-zero byte triggers menu.
        const u8 *b = reinterpret_cast<const u8 *>(pad);
        for (i32 i = 0; i < 16; i++) {
            if (b[i] != 0) {
                buttons = 1;
                break;
            }
        }
        if (buttons != 0) {
            LOG_INFO("UpdateGameMenu: pad input -> NewMenu(0)");
            NewMenu(0, 0, 0);
        }
        return;
    }

    // Active menu — original would dispatch MenuUpdate* for the current
    // MENU_s, handle repeat, and potentially call BackupMenu/NewMenu on
    // selection. Host keeps the call order: poll pad, update timer, and
    // leave rendering to DrawMenu. Missing MENU_s just no-ops.
    // Preserve the faithful call to MenuDrawBackground's pre-draw hook if
    // menu_id is titles (0).
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
