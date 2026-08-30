#pragma once

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "MechInputTouch/MechInputTouch_types.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv);
extern i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba);

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

    static void sdl_init() {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, host_video_driver);

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERR("SDL_Init(VIDEO) failed: %s", SDL_GetError());
            return;
        }
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        SDL_Window *window = SDL_CreateWindow("saga", host_window_width, host_window_height, 0);
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
    static i32 host_numain_result = 0;
    static volatile bool host_numain_done = false;

    static int SDLCALL numain_thread_main(void *arg) {
        (void)arg;
        char *argv[] = {const_cast<char *>("saga"), nullptr};
        host_numain_result = NuMain(1, argv);
        host_numain_done = true;
        return host_numain_result;
    }

} // namespace

inline i32 test_window(i32 argc, char **argv) {
    LOG_INFO("test_window(argc=%d, argv=%p)", argc, argv);

    bool capture_enabled = false;
    for (i32 i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--capture") == 0) {
            capture_enabled = true;
        }
    }

    sdl_init();

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

    host_numain_thread = SDL_CreateThread(numain_thread_main, "numain", nullptr);

    const Uint64 start_ticks = SDL_GetTicks();
    i32 frame_count = 0;
    u64 previous_hash = 0;
    u64 captured_hash = 0;
    Uint64 last_capture_ticks = 0;
    Uint64 last_change_ticks = 0;
    Uint64 next_readback_ticks = 0;
    bool have_hash = false;
    bool image_changing = false;
    std::vector<u8> pixels;
    i32 capture_width = 0;
    i32 capture_height = 0;

    bool quit_requested = false;
    while (!quit_requested) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit_requested = true;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_FINGER_DOWN ||
                       (event.type == SDL_EVENT_KEY_DOWN &&
                        (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE))) {
                // Android reports a newly pressed title/menu touch through
                // this per-frame latch. Let a host click, tap, Enter, or Space
                // exercise the same original menu-update path.
                MechInputTouchMenuController::AnyTouchesThisFrame = 1;
            }
        }
        if (quit_requested) {
            break;
        }

        if (host_numain_done) {
            for (i32 i = 0; i < host_tail_frames; i++) {
                SDL_Delay(host_poll_interval_ms);
                frame_count++;
            }
            break;
        }

        frame_count++;

        SDL_Delay(host_poll_interval_ms);

        if (SDL_GetTicks() - start_ticks > static_cast<Uint64>(host_timeout_ms)) {
            LOG_ERR("test_window: timeout after %d ms", host_timeout_ms);
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

    if (host_numain_thread != nullptr) {
        SDL_DetachThread(host_numain_thread);
        host_numain_thread = nullptr;
    }

    LOG_INFO("presented %d frame_count", frame_count);
    _exit(0);

    free(buffer);
}
