#include "host/platform/input.hpp"

#include <atomic>

#include "MechInputTouch/MechInputTouch_types.h"
#include "gameapi/gui/apimenu.h"
#include "legoapi/characters/core/players.h"
#include "nu2api/numath/nufloat.h"

extern i32 GetMenuID();

namespace {
    std::atomic<i32> host_touch_x;
    std::atomic<i32> host_touch_y;
    std::atomic<i32> host_touch_width;
    std::atomic<i32> host_touch_height;
    std::atomic<bool> host_touch_pending;
} // namespace

void HostInputResetPlatform() {
    host_touch_pending.store(false, std::memory_order_relaxed);
}

void HostInputTouch(i32 x, i32 y, i32 width, i32 height) {
    host_touch_x.store(x, std::memory_order_relaxed);
    host_touch_y.store(y, std::memory_order_relaxed);
    host_touch_width.store(width, std::memory_order_relaxed);
    host_touch_height.store(height, std::memory_order_relaxed);
    host_touch_pending.store(true, std::memory_order_release);
}

u32 HostInputConsumePlatform(i32 port) {
    if (port != 0 || !host_touch_pending.exchange(false, std::memory_order_acquire)) {
        return 0;
    }

    const i32 width = host_touch_width.load(std::memory_order_relaxed);
    const i32 height = host_touch_height.load(std::memory_order_relaxed);
    if (width <= 0 || height <= 0 || GameMenuLevel < 0 || GameMenuLevel >= 10) {
        return 0;
    }

    const f32 x =
        static_cast<f32>(host_touch_x.load(std::memory_order_relaxed)) / static_cast<f32>(width) * 2.0f - 1.0f;
    const f32 y =
        1.0f - static_cast<f32>(host_touch_y.load(std::memory_order_relaxed)) / static_cast<f32>(height) * 2.0f;
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
        } else if (NuFabs(y + 0.75f) < 0.14f && NuFabs(x) < 0.5f) {
            menu.queued_item = menu.last_row;
            menu.queued_column = menu.selected_column;
            menu.queued_row = menu.last_row;
        }
    }
    return 0;
}
