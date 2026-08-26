#pragma once

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv);

namespace {

    constexpr i32 kWindowWidth = 1280;
    constexpr i32 kWindowHeight = 720;
    constexpr i32 kLegalRedThreshold = 3000;
    constexpr i32 kLegalWhiteThreshold = 30000;
    constexpr i32 kPollIntervalMs = 16;
    constexpr i32 kTimeoutMs = 90000;
    constexpr i32 kTailFrames = 30;

#ifdef _WIN32
    constexpr const char *kVideoDriver = "windows";
#else
    constexpr const char *kVideoDriver = "x11";
#endif

    static bool g_headless = false;

    struct PixelCounts {
        u32 red = 0;
        u32 white = 0;
        u32 non_black = 0;
    };

    static PixelCounts count_pixels(const u8 *rgba, i32 width, i32 height) {
        PixelCounts counts{};
        const i32 pixel_count = width * height;
        for (i32 i = 0; i < pixel_count; i++) {
            const u8 r = rgba[static_cast<usize>(i) * 4 + 0];
            const u8 g = rgba[static_cast<usize>(i) * 4 + 1];
            const u8 b = rgba[static_cast<usize>(i) * 4 + 2];
            if (r > 120 && g < 90 && b < 90) {
                counts.red++;
            } else if (r > 200 && g > 200 && b > 200) {
                counts.white++;
            }
            if (r > 8 || g > 8 || b > 8) {
                counts.non_black++;
            }
        }
        return counts;
    }

    static bool write_ppm(const char *path, const u8 *rgba, i32 width, i32 height) {
        FILE *file = fopen(path, "wb");
        if (file == nullptr) {
            return false;
        }
        fprintf(file, "P6\n%d %d\n255\n", width, height);
        // HostReadbackPixels returns bottom-up GL pixels (0,0 at bottom-left).
        // The window presents the same texture with V flipped, so the on-screen
        // image's top corresponds to the buffer's bottom row. Write rows in
        // buffer order (no extra flip) so the PPM matches what is seen in the
        // window.
        for (i32 y = 0; y < height; y++) {
            for (i32 x = 0; x < width; x++) {
                const usize src = (static_cast<usize>(y) * static_cast<usize>(width) + static_cast<usize>(x)) * 4;
                fwrite(&rgba[src], 1, 3, file);
            }
        }
        fclose(file);
        return true;
    }

    static bool try_capture_legal_frame(i32 frames) {
        std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
        const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
        if (packed <= 0) {
            return false;
        }
        const i32 width = packed / 1000;
        const i32 height = packed % 1000;
        const PixelCounts counts = count_pixels(pixels.data(), width, height);
        if (counts.red < static_cast<u32>(kLegalRedThreshold) &&
            counts.white < static_cast<u32>(kLegalWhiteThreshold)) {
            return false;
        }
        if (!write_ppm("window.ppm", pixels.data(), width, height)) {
            return true;
        }
        LOG_INFO("captured legal frame at pump #%d (%dx%d, red=%u white=%u)", frames, width, height, counts.red,
                 counts.white);
        return true;
    }

    static void host_init() {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, kVideoDriver);

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERR("SDL_Init(VIDEO) failed: %s", SDL_GetError());
            LOG_ERR("headless mode (no window)");
            g_headless = true;
            return;
        }
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        SDL_Window *window = SDL_CreateWindow("saga", kWindowWidth, kWindowHeight, 0);
        if (window == nullptr) {
            LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
            g_headless = true;
            return;
        }

        if (strcmp(SDL_GetCurrentVideoDriver(), kVideoDriver) != 0) {
            LOG_ERR("unexpected video driver: %s", SDL_GetCurrentVideoDriver());
            g_headless = true;
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

    static void nufile_open_wad() {
        static char wad_buffer[0x1000000];
        VARIPTR ptr = VARIPTR{.void_ptr = &wad_buffer};
        NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &ptr, 0));
    }

    static SDL_Thread *g_numain_thread = nullptr;
    static i32 g_numain_result = 0;
    static volatile bool g_numain_done = false;

    static int SDLCALL numain_thread_main(void *arg) {
        (void)arg;
        char *argv[] = {const_cast<char *>("saga"), nullptr};
        g_numain_result = NuMain(1, argv);
        g_numain_done = true;
        return g_numain_result;
    }

} // namespace

inline i32 test_window(i32 argc, char **argv) {
    (void)argc;
    (void)argv;

    host_init();
    nufile_open_wad();

    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    g_numain_thread = SDL_CreateThread(numain_thread_main, "numain", nullptr);

    const Uint64 start_ticks = SDL_GetTicks();
    i32 frames = 0;
    bool legal_saved = false;
    bool quit_requested = false;

    while (!quit_requested) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit_requested = true;
            }
        }
        if (quit_requested) {
            break;
        }

        if (g_numain_done) {
            for (i32 i = 0; i < kTailFrames; i++) {
                g_renderDevice.SwapBuffers();
                SDL_Delay(kPollIntervalMs);
                frames++;
            }
            break;
        }

        g_renderDevice.SwapBuffers();
        frames++;

        if (!legal_saved && (frames % 4 == 0)) {
            legal_saved = try_capture_legal_frame(frames);
        }
        if (legal_saved) {
            LOG_INFO("window test: legal frame saved, exiting");
            fflush(nullptr);
            _exit(0);
        }

        SDL_Delay(kPollIntervalMs);

        if (SDL_GetTicks() - start_ticks > static_cast<Uint64>(kTimeoutMs)) {
            LOG_WARN("window test timed out waiting for NuMain");
            break;
        }
    }

    if (g_numain_thread != nullptr) {
        SDL_DetachThread(g_numain_thread);
        g_numain_thread = nullptr;
    }

    if (!g_headless && !legal_saved) {
        std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
        const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
        if (packed > 0) {
            const i32 width = packed / 1000;
            const i32 height = packed % 1000;
            if (write_ppm("window.ppm", pixels.data(), width, height)) {
                const PixelCounts counts = count_pixels(pixels.data(), width, height);
                LOG_INFO("wrote window.ppm %dx%d  non-black px=%u", width, height, counts.non_black);
                fflush(nullptr);
                _exit(0);
            }
        } else {
            LOG_WARN("readback: no window surface/context, skipping PPM");
        }
    }

    LOG_INFO("presented %d frames (headless=%d)", frames, g_headless ? 1 : 0);
    _exit(0);
}
