#pragma once

#include <SDL3/SDL.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "host-tests/input/host_input.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv);
extern i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba);
extern i32 GetMenuID();
extern i32 memcard_loadneeded;
extern i32 memcard_loadstarted;
extern i32 memcard_loadfailed;
extern i32 memcard_loadcorrupt;
extern i32 waiting_for_level;
extern i32 waiting_for_character;
extern i32 waiting_for_new_level;
extern i32 abort_load;
extern i32 reset_load;
extern i32 CharacterDataLoad;
extern FadeSystem FadeSys;

bool g_hostOffscreenRendering = false;

namespace {
#ifdef _WIN32
    constexpr const char *host_video_driver = "windows";
#else
    constexpr const char *host_video_driver = "x11";
#endif

    constexpr i32 host_window_width = 1280;
    constexpr i32 host_window_height = 720;
    constexpr i32 host_poll_interval_ms = 16;
    constexpr i32 host_timeout_ms = 90000;
    constexpr i32 host_tail_frames = 30;
    // Leave enough time for the original asynchronous load result and its
    // one-second menu result delay to complete after the final scripted tap.
    constexpr Uint64 scripted_input_tail_ms = 8000;

    constexpr Uint64 scripted_title_touch_ms = 18000;
    constexpr Uint64 scripted_menu_settle_ms = 500;

    enum class ScriptedInputStage {
        title,
        new_or_load,
        select_controls,
        load_wait,
        save_slot,
        overwrite_confirm,
        overwrite_accept,
        complete,
    };

    static void touch_menu_item(i32 menu_id, i32 row, i32 column) {
        const MENU &menu = GameMenu[GameMenuLevel];
        if (menu_id == 1) {
            const f32 centre_offset = static_cast<f32>(menu.last_row - menu.first_row) * MENUDY * -0.5f;
            const f32 first_y = -0.5f - centre_offset;
            const f32 y = first_y + static_cast<f32>(row) * MENUDY;
            const i32 screen_y = static_cast<i32>((1.0f - y) * 0.5f * host_window_height);
            HostInputTouch(host_window_width / 2, screen_y, host_window_width, host_window_height);
            return;
        }

        const f32 x = -0.75f + (static_cast<f32>(column - menu.first_column) + 0.5f) * 0.5f;
        const i32 screen_x = static_cast<i32>((x + 1.0f) * 0.5f * host_window_width);
        HostInputTouch(screen_x, host_window_height / 2, host_window_width, host_window_height);
    }

    struct PixelCounts {
        u32 red = 0;
        u32 white = 0;
        u32 non_black = 0;
    };

    static bool write_ppm(const char *path, const u8 *rgba, i32 width, i32 height) {
        FILE *file = fopen(path, "wb");
        if (file == nullptr) {
            LOG_ERR("failed to open %s for writing: %s", path, strerror(errno));
            return false;
        }
        fprintf(file, "P6\n%d %d\n255\n", width, height);

        for (i32 y = height - 1; y >= 0; y--) {
            for (i32 x = 0; x < width; x++) {
                const usize src = (static_cast<usize>(y) * static_cast<usize>(width) + static_cast<usize>(x)) * 4;
                fwrite(&rgba[src], 1, 3, file);
            }
        }
        fclose(file);

        return true;
    }

    static u64 pixel_hash(const u8 *pixels, usize pixel_count) {
        u64 hash = 1469598103934665603ULL;
        // The capture files contain RGB. Ignore framebuffer alpha as well:
        // blend-state changes can alter it without changing the visible image.
        for (usize i = 0; i < pixel_count; i++) {
            const u8 *pixel = pixels + i * 4;
            for (usize channel = 0; channel < 3; channel++) {
                hash ^= pixel[channel];
                hash *= 1099511628211ULL;
            }
        }
        return hash;
    }

    static bool read_frame(std::vector<u8> &pixels, i32 &width, i32 &height, u64 &hash) {
        pixels.resize(static_cast<usize>(host_window_width) * host_window_height * 4);
        const i32 packed = HostReadbackPixels(host_window_width, host_window_height, pixels.data());
        if (packed <= 0) {
            return false;
        }

        width = packed / 1000;
        height = packed % 1000;
        hash = pixel_hash(pixels.data(), static_cast<usize>(width) * height);
        return true;
    }

    static bool capture_frame(i32 frame, const std::vector<u8> &pixels, i32 width, i32 height) {
        char filename[64];
        snprintf(filename, sizeof(filename), ".work/capture/window_%04d.ppm", frame);

        return write_ppm(filename, pixels.data(), width, height);
    }

    static void sdl_init(bool offscreen, bool mute) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, host_video_driver);
        if (mute) {
            SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
        }

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERR("SDL_Init(VIDEO) failed: %s", SDL_GetError());
            return;
        }
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        const SDL_WindowFlags window_flags =
            offscreen ? static_cast<SDL_WindowFlags>(SDL_WINDOW_HIDDEN | SDL_WINDOW_NOT_FOCUSABLE) : 0;
        SDL_Window *window = SDL_CreateWindow("saga", host_window_width, host_window_height, window_flags);
        if (window == nullptr) {
            LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
            return;
        }

        if (strcmp(SDL_GetCurrentVideoDriver(), host_video_driver) != 0) {
            LOG_ERR("unexpected video driver: %s", SDL_GetCurrentVideoDriver());
            return;
        }

        const SDL_PropertiesID props = SDL_GetWindowProperties(window);
#ifdef _WIN32
        HWND handle = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
#else
        auto handle = static_cast<i32>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
#endif
        g_renderDevice.OnWindowCreated(reinterpret_cast<ANativeWindow *>(handle));
    }

    static SDL_Thread *host_numain_thread = nullptr;
    static std::atomic<i32> host_numain_result{0};
    static std::atomic<bool> host_numain_done{false};

    static i32 host_menu_id() {
        // GetMenuID is an original game helper and assumes the menu system has
        // already been initialized. The host thread starts polling before
        // NuMain reaches APIInitMenu, so do that host-only lifetime check here.
        if (GameMenuLevel < 0 || GameMenuLevel >= 10) {
            return -1;
        }
        return GetMenuID();
    }

    static int SDLCALL numain_thread_main(void *arg) {
        (void)arg;
        char *argv[] = {const_cast<char *>("saga"), nullptr};
        const i32 result = NuMain(1, argv);
        host_numain_result.store(result, std::memory_order_relaxed);
        host_numain_done.store(true, std::memory_order_release);
        return result;
    }

} // namespace

inline i32 test_window(i32 argc, char **argv) {
    LOG_INFO("test_window(argc=%d, argv=%p)", argc, argv);

    bool capture_enabled = false;
    bool scripted_input_enabled = false;
    bool scripted_load_enabled = false;
    bool offscreen_enabled = false;
    bool mute_enabled = false;
    Uint64 scripted_tail_ms = scripted_input_tail_ms;
    Uint64 timeout_ms = host_timeout_ms;
    for (i32 i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--capture") == 0) {
            capture_enabled = true;
        } else if (strcmp(argv[i], "--script-input") == 0) {
            scripted_input_enabled = true;
        } else if (strcmp(argv[i], "--script-load") == 0) {
            scripted_input_enabled = true;
            scripted_load_enabled = true;
        } else if (strcmp(argv[i], "--offscreen") == 0) {
            offscreen_enabled = true;
        } else if (strcmp(argv[i], "--mute") == 0) {
            mute_enabled = true;
        } else if (strcmp(argv[i], "--script-tail-ms") == 0 && i + 1 < argc) {
            scripted_tail_ms = strtoull(argv[++i], nullptr, 10);
        } else if (strcmp(argv[i], "--timeout-ms") == 0 && i + 1 < argc) {
            timeout_ms = strtoull(argv[++i], nullptr, 10);
        }
    }

    g_hostOffscreenRendering = offscreen_enabled;
    sdl_init(offscreen_enabled, mute_enabled);
    if (!SDL_CreateDirectory(".work/host-documents")) {
        LOG_ERR("failed to create host documents directory: %s", SDL_GetError());
    }

    void *buffer = malloc(0x1000000);
    VARIPTR ptr = VARIPTR{.void_ptr = buffer};
    NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &ptr, 0));

    if (capture_enabled) {
        // Try to remove .work/capture/* before starting, but don't fail if it
        // does not exist. Readback is deliberately opt-in: glReadPixels must
        // synchronize with the render thread and noticeably affects pacing.
        system("rm -r .work/capture");
        system("mkdir -p .work/capture");
    }

    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);
    HostInputReset();

    host_numain_result.store(0, std::memory_order_relaxed);
    host_numain_done.store(false, std::memory_order_relaxed);
    host_numain_thread = SDL_CreateThread(numain_thread_main, "numain", nullptr);
    if (host_numain_thread == nullptr) {
        LOG_ERR("failed to create NuMain thread: %s", SDL_GetError());
        free(buffer);
        return 1;
    }

    const Uint64 start_ticks = SDL_GetTicks();
    i32 frame_count = 0;
    u64 previous_hash = 0;
    u64 captured_hash = 0;
    Uint64 last_capture_ticks = 0;
    Uint64 last_change_ticks = 0;
    Uint64 next_readback_ticks = 0;
    bool have_hash = false;
    bool image_changing = false;
    ScriptedInputStage scripted_stage = ScriptedInputStage::title;
    Uint64 scripted_stage_ticks = 0;
    i32 scripted_last_menu = -2;
    Uint64 scripted_menu_since = 0;
    std::vector<u8> pixels;
    i32 capture_width = 0;
    i32 capture_height = 0;

    bool quit_requested = false;
    while (!quit_requested) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (offscreen_enabled) {
                continue;
            }
            if (event.type == SDL_EVENT_QUIT) {
                quit_requested = true;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                HostInputTouch(static_cast<i32>(event.button.x), static_cast<i32>(event.button.y), host_window_width,
                               host_window_height);
            } else if (event.type == SDL_EVENT_FINGER_DOWN) {
                HostInputTouch(static_cast<i32>(event.tfinger.x * host_window_width),
                               static_cast<i32>(event.tfinger.y * host_window_height), host_window_width,
                               host_window_height);
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE) {
                    HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_START | GAMEPAD_JUMP);
                } else if (event.key.key == SDLK_UP || event.key.key == SDLK_W) {
                    HostInputTap(host_window_width / 2, 0, GAMEPAD_DUP);
                } else if (event.key.key == SDLK_DOWN || event.key.key == SDLK_S) {
                    HostInputTap(host_window_width / 2, host_window_height - 1, GAMEPAD_DDOWN);
                } else if (event.key.key == SDLK_LEFT || event.key.key == SDLK_A) {
                    HostInputTap(0, host_window_height / 2, GAMEPAD_DLEFT);
                } else if (event.key.key == SDLK_RIGHT || event.key.key == SDLK_D) {
                    HostInputTap(host_window_width - 1, host_window_height / 2, GAMEPAD_DRIGHT);
                } else if (event.key.key == SDLK_ESCAPE) {
                    HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_TAG);
                }
            }
        }
        if (quit_requested) {
            break;
        }

        if (host_numain_done.load(std::memory_order_acquire)) {
            for (i32 i = 0; i < host_tail_frames; i++) {
                SDL_Delay(host_poll_interval_ms);
                frame_count++;
            }
            break;
        }

        frame_count++;

        const Uint64 elapsed_ticks = SDL_GetTicks() - start_ticks;
        if (scripted_input_enabled) {
            const i32 menu_id = host_menu_id();
            if (menu_id != scripted_last_menu) {
                scripted_last_menu = menu_id;
                scripted_menu_since = elapsed_ticks;
            }

            if (scripted_stage == ScriptedInputStage::title && menu_id == 0 &&
                elapsed_ticks >= scripted_title_touch_ms) {
                HostInputTouch(host_window_width / 2, host_window_height / 2, host_window_width, host_window_height);
                scripted_stage = ScriptedInputStage::new_or_load;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::new_or_load && menu_id == 1 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                // The original menu defaults to Load Game when any save is
                // present. This route deliberately exercises New Game.
                touch_menu_item(menu_id, scripted_load_enabled ? 1 : 0, 0);
                if (scripted_load_enabled) {
                    // Load Game may lead to the original No Data screen
                    // (menu 1005); dismiss it once it has settled.
                    scripted_stage = ScriptedInputStage::load_wait;
                } else {
                    // New Game now follows the original Select Controls
                    // screen (menu 33). Its default entry is Classic, so
                    // confirm it after the normal settle interval before
                    // looking for the save-slot menu.
                    scripted_stage = ScriptedInputStage::select_controls;
                }
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::select_controls && menu_id == 33 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_JUMP);
                scripted_stage = ScriptedInputStage::save_slot;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::load_wait && menu_id == 1012 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                i32 slot = 0;
                while (slot < SAVESLOTS && saveload_slotused[slot] == 0) {
                    ++slot;
                }
                if (slot < SAVESLOTS) {
                    touch_menu_item(menu_id, 0, slot);
                }
                scripted_stage = ScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::load_wait && menu_id == 1005 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_JUMP);
                scripted_stage = ScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::save_slot && menu_id == 1000 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                i32 slot = 0;
                while (slot < SAVESLOTS && saveload_slotused[slot] != 0) {
                    ++slot;
                }
                const bool overwrite = slot == SAVESLOTS;
                if (overwrite) {
                    slot = 0;
                }
                touch_menu_item(menu_id, 0, slot);
                scripted_stage = overwrite ? ScriptedInputStage::overwrite_confirm : ScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::overwrite_confirm && menu_id == 1008 &&
                       elapsed_ticks - scripted_menu_since >= scripted_menu_settle_ms) {
                HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_DUP);
                scripted_stage = ScriptedInputStage::overwrite_accept;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::overwrite_accept && menu_id == 1008 &&
                       elapsed_ticks >= scripted_stage_ticks + scripted_menu_settle_ms) {
                HostInputTap(host_window_width / 2, host_window_height / 2, GAMEPAD_JUMP);
                scripted_stage = ScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == ScriptedInputStage::complete &&
                       elapsed_ticks >= scripted_stage_ticks + scripted_tail_ms) {
                break;
            }
        }

        SDL_Delay(host_poll_interval_ms);

        if (SDL_GetTicks() - start_ticks > timeout_ms) {
            LOG_ERR("test_window: timeout after %llu ms", static_cast<unsigned long long>(timeout_ms));
            break;
        }

        if (!capture_enabled) {
            continue;
        }

        const Uint64 readback_ticks = SDL_GetTicks();
        if (readback_ticks < next_readback_ticks) {
            continue;
        }
        // glReadPixels synchronizes with the render thread. Sampling it every
        // host-loop iteration throttles the game itself, so inspect at 10 Hz
        // and retain the existing 500 ms capture cadence while images move.
        next_readback_ticks = readback_ticks + 100;

        u64 current_hash = 0;
        if (!read_frame(pixels, capture_width, capture_height, current_hash)) {
            continue;
        }
        const Uint64 now = SDL_GetTicks();
        if (!have_hash) {
            capture_frame(frame_count, pixels, capture_width, capture_height);
            captured_hash = current_hash;
            last_capture_ticks = now;
            previous_hash = current_hash;
            have_hash = true;
        } else if (current_hash != previous_hash) {
            last_change_ticks = now;
            if (!image_changing || now - last_capture_ticks >= 500) {
                capture_frame(frame_count, pixels, capture_width, capture_height);
                captured_hash = current_hash;
                last_capture_ticks = now;
            }
            previous_hash = current_hash;
            image_changing = true;
        } else if (image_changing && now - last_change_ticks >= 500) {
            if (current_hash != captured_hash) {
                capture_frame(frame_count, pixels, capture_width, capture_height);
                captured_hash = current_hash;
                last_capture_ticks = now;
            }
            image_changing = false;
        }
    }

    if (capture_enabled) {
        u64 final_hash = 0;
        if (read_frame(pixels, capture_width, capture_height, final_hash) &&
            (!have_hash || final_hash != captured_hash)) {
            capture_frame(frame_count, pixels, capture_width, capture_height);
        }
    }

    const bool numain_finished = host_numain_done.load(std::memory_order_acquire);
    if (host_numain_thread != nullptr && numain_finished) {
        i32 thread_result = 0;
        SDL_WaitThread(host_numain_thread, &thread_result);
        host_numain_result.store(thread_result, std::memory_order_relaxed);
        host_numain_thread = nullptr;
    } else if (host_numain_thread != nullptr) {
        SDL_DetachThread(host_numain_thread);
        host_numain_thread = nullptr;
    }

    if (scripted_input_enabled) {
        LOG_INFO("scripted input stopped on menu id %d, save slot %d, status %d, load=(%d,%d,%d,%d), occurred=%d, "
                 "new-level=%p idx=%d name=%s current-level=%p idx=%d name=%s fade=(%.3f,%d,%d), "
                 "waits=(level=%d,character=%d,new=%d,abort=%d,reset=%d), players=(%d:%p,%d:%p), char-load=%d",
                 host_menu_id(), saveload_slotid, saveload_status, memcard_loadneeded, memcard_loadstarted,
                 memcard_loadfailed, memcard_loadcorrupt, MenuLoadOccurred, NewLData,
                 NewLData != nullptr ? NewLData->idx : -1, NewLData != nullptr ? NewLData->name : "-",
                 WORLD != nullptr ? WORLD->current_level : nullptr,
                 WORLD != nullptr && WORLD->current_level != nullptr ? WORLD->current_level->idx : -1,
                 WORLD != nullptr && WORLD->current_level != nullptr ? WORLD->current_level->name : "-", FadeSys.fade,
                 FadeSys.pending_type, FadeSys.busy, waiting_for_level, waiting_for_character, waiting_for_new_level,
                 abort_load, reset_load, PlayerID[0], Player[0], PlayerID[1], Player[1], CharacterDataLoad);
    }
    LOG_INFO("presented %d frame_count", frame_count);
    if (numain_finished) {
        const i32 result = host_numain_result.load(std::memory_order_relaxed);
        free(buffer);
        return result;
    }
    _exit(0);
}
