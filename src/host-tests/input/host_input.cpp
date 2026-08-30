#include "host-tests/input/host_input.h"

#include <atomic>

#include "MechInputTouch/MechInputTouch_types.h"
#include "decomp.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nufloat.h"

extern GAMEPAD_s GamePad[64];
extern i32 GetMenuID();

namespace {
    std::atomic<u32> pending_buttons[2];
    std::atomic<i32> pending_touch_x;
    std::atomic<i32> pending_touch_y;
    std::atomic<i32> pending_touch_width;
    std::atomic<i32> pending_touch_height;
    std::atomic<bool> pending_touch;

    u32 ConsumeTouch() {
        if (!pending_touch.exchange(false, std::memory_order_acquire)) {
            return 0;
        }

        const i32 width = pending_touch_width.load(std::memory_order_relaxed);
        const i32 height = pending_touch_height.load(std::memory_order_relaxed);
        if (width <= 0 || height <= 0) {
            return 0;
        }

        const f32 x =
            static_cast<f32>(pending_touch_x.load(std::memory_order_relaxed)) / static_cast<f32>(width) * 2.0f - 1.0f;
        const f32 y =
            1.0f - static_cast<f32>(pending_touch_y.load(std::memory_order_relaxed)) / static_cast<f32>(height) * 2.0f;
        const i32 menu_id = GetMenuID();
        MENU &menu = GameMenu[GameMenuLevel];

        MechInputTouchMenuController::AnyTouchesThisFrame = 1;
        if (menu_id == 0) {
            return GAMEPAD_START | GAMEPAD_JUMP;
        }

        if (menu_id == 1) {
            const f32 centre_offset = static_cast<f32>(menu.last_row - menu.first_row) * MENUDY * -0.5f;
            const f32 first_y = -0.5f - centre_offset;
            const i32 row = static_cast<i32>((y - first_y) / MENUDY + 0.5f);
            const f32 row_y = first_y + static_cast<f32>(row) * MENUDY;
            if (x > -0.7f && x < 0.7f && row >= menu.first_row && row <= menu.last_row &&
                NuFabs(y - row_y) < NuFabs(MENUDY) * 0.6f) {
                menu.queued_item = row;
                menu.queued_column = menu.selected_column;
                menu.queued_row = row;
                return 0;
            }
            return 0;
        }

        if (menu_id == 1000 || menu_id == 1012) {
            if (NuFabs(y) < 0.22f && x > -0.75f && x < 0.75f) {
                i32 column = static_cast<i32>((x + 0.75f) / 0.5f);
                if (column > menu.last_column) {
                    column = menu.last_column;
                }
                menu.queued_item = menu.first_row;
                menu.queued_column = column;
                menu.queued_row = menu.first_row;
                return 0;
            }
            if (NuFabs(y + 0.75f) < 0.14f && NuFabs(x) < 0.5f) {
                menu.queued_item = menu.last_row;
                menu.queued_column = menu.selected_column;
                menu.queued_row = menu.last_row;
                return 0;
            }
        }

        return 0;
    }
} // namespace

void HostInputReset() {
    pending_buttons[0].store(0, std::memory_order_relaxed);
    pending_buttons[1].store(0, std::memory_order_relaxed);
    pending_touch.store(false, std::memory_order_relaxed);
    saveload_slotid = -1;
}

void HostInputTouch(i32 x, i32 y, i32 width, i32 height) {
    LOG_INFO("host input: touch (%d, %d) in %dx%d", x, y, width, height);
    pending_touch_x.store(x, std::memory_order_relaxed);
    pending_touch_y.store(y, std::memory_order_relaxed);
    pending_touch_width.store(width, std::memory_order_relaxed);
    pending_touch_height.store(height, std::memory_order_relaxed);
    pending_touch.store(true, std::memory_order_release);
    // The Android touch callback updates the menu's queued selection at event
    // time. Do the same here; waiting for ReadPad can lose the touch when the
    // game is temporarily not polling controllers inside a menu flow.
    (void)ConsumeTouch();
    const MENU &menu = GameMenu[GameMenuLevel];
    LOG_INFO("host input: queued menu=%d item=%d column=%d row=%d validated=%d", GetMenuID(), menu.queued_item,
             menu.queued_column, menu.queued_row, MenuValidated);
}

void HostInputTap(i32 x, i32 y, u32 buttons) {
    const MENU &menu = GameMenu[GameMenuLevel];
    LOG_INFO("host input: tap (%d, %d), buttons=0x%x, menu=%d, cursor=(%d,%d), save_status=%d, save_slot=%d", x, y,
             buttons, GetMenuID(), menu.selected_column, menu.selected_row, saveload_status, saveload_slotid);
    MechInputTouchMenuController::AnyTouchesThisFrame = 1;
    pending_buttons[0].fetch_or(buttons, std::memory_order_release);
}

i32 ReadPad(i32 port) {
    if (port < 0 || port >= 2) {
        return 0;
    }

    GAMEPAD_s &gamepad = GamePad[port];
    u32 buttons = pending_buttons[port].exchange(0, std::memory_order_acq_rel);
    if (port == 0) {
        buttons |= ConsumeTouch();
    }

    gamepad.unknown_04 = buttons;
    gamepad.buttons_down_08 = buttons;
    gamepad.unknown_0c = 0;
    gamepad.unknown_10 = 0;
    gamepad.unknown_14 = 0;
    gamepad.unknown_18 = 0;
    gamepad.unknown_1c = 0;

    return 2;
}

void DoInput(WORLDINFO_s *) {
    ReadPads();
}
