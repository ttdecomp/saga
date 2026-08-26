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
        // Also keep a copy as window_legal.ppm for staged verification
        write_ppm("window_legal.ppm", pixels.data(), width, height);
        LOG_INFO("captured legal frame at pump #%d (%dx%d, red=%u white=%u)", frames, width, height, counts.red,
                 counts.white);
        return true;
    }

    static bool try_capture_blue_frame(i32 frames) {
        std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
        const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
        if (packed <= 0) {
            return false;
        }
        const i32 width = packed / 1000;
        const i32 height = packed % 1000;
        // Blue/cyan from IntroText_Draw: 0x007f5f00 -> r 0x00 g 0x7f b 0x5f and alpha-scaled
        // Look for teal/blue pixels: low red, mid green, mid-high blue
        u32 blue = 0;
        const i32 pixel_count = width * height;
        for (i32 i = 0; i < pixel_count; i++) {
            const u8 r = pixels[static_cast<usize>(i) * 4 + 0];
            const u8 g = pixels[static_cast<usize>(i) * 4 + 1];
            const u8 b = pixels[static_cast<usize>(i) * 4 + 2];
            // IntroText uses 0x7f5f00 with alpha: r ~0, g ~95-127, b ~0x5f-0x7f range scaled by alpha
            if (r < 40 && g > 60 && g < 160 && b > 60 && b < 160) {
                blue++;
            }
        }
        // Legal has red 16k, blue 3k. True blue intro should have low red
        // and higher blue. Require red low to avoid capturing legal as blue.
        {
            u32 red = 0;
            const i32 pixel_count2 = width * height;
            for (i32 i = 0; i < pixel_count2; i++) {
                const u8 r = pixels[static_cast<usize>(i) * 4 + 0];
                const u8 g = pixels[static_cast<usize>(i) * 4 + 1];
                const u8 b = pixels[static_cast<usize>(i) * 4 + 2];
                if (r > 120 && g < 90 && b < 90)
                    red++;
            }
            if (red > 5000)
                return false;
        }
        if (blue < 3000) {
            return false;
        }
        if (!write_ppm("window_blue.ppm", pixels.data(), width, height)) {
            return true;
        }
        LOG_INFO("captured blue intro frame at pump #%d (%dx%d, blue=%u)", frames, width, height, blue);
        return true;
    }

    static bool try_capture_crawl_frame(i32 frames) {
        std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
        const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
        if (packed <= 0) {
            return false;
        }
        const i32 width = packed / 1000;
        const i32 height = packed % 1000;
        // Crawl yellow: BackDrop_SetTint 0.9,0.8,0.15 -> yellowish, plus starfield dark
        u32 yellow = 0;
        const i32 pixel_count = width * height;
        for (i32 i = 0; i < pixel_count; i++) {
            const u8 r = pixels[static_cast<usize>(i) * 4 + 0];
            const u8 g = pixels[static_cast<usize>(i) * 4 + 1];
            const u8 b = pixels[static_cast<usize>(i) * 4 + 2];
            if (r > 180 && g > 150 && g < 220 && b < 80) {
                yellow++;
            }
        }
        if (yellow < 5000) {
            return false;
        }
        if (!write_ppm("window_crawl.ppm", pixels.data(), width, height)) {
            return true;
        }
        LOG_INFO("captured crawl frame at pump #%d (%dx%d, yellow=%u)", frames, width, height, yellow);
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
    bool blue_saved = false;
    bool crawl_saved = false;
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

        // Staged capture: legal -> blue intro -> crawl -> menu. Don't exit
        // after legal; keep pumping so the blue "A long time ago..." text,
        // starfield crawl and menu have time to appear. Each stage is
        // captured once and kept as window_*.ppm.
        // Host: use timed captures after legal, since detection thresholds
        // are fragile (legal already has some blue/yellow). Timed approach
        // matches the startup timers: legal 5.8s, intro 3.8s, crawl 3s (host).
        if ((frames % 4) == 0) {
            if (!legal_saved) {
                legal_saved = try_capture_legal_frame(frames);
                if (legal_saved) {
                    LOG_INFO("window test: legal captured at %d, will time blue/crawl/menu", frames);
                }
            }
            // Timed blue: ~7 sec after legal (1s gate + 5.8s legal + 0.2s intro fade)
            // For host, legal at ~1s, blue at ~7s, crawl at ~10s, menu at ~13s.
            // Use frame counts: 60fps, so 7s ~420 frames after start, but legal at ~60, so blue at 60+360=420.
            if (legal_saved && !blue_saved && frames > 180) {
                bool got = try_capture_blue_frame(frames);
                if (got) {
                    blue_saved = true;
                } else if (frames > 220) {
                    std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
                    const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
                    if (packed > 0) {
                        const i32 w = packed / 1000;
                        const i32 h = packed % 1000;
                        write_ppm("window_blue.ppm", pixels.data(), w, h);
                        LOG_INFO("window test: forced blue capture at %d (fallback timed)", frames);
                        blue_saved = true;
                    }
                }
            }
            if (blue_saved && !crawl_saved && frames > 420) {
                bool got = try_capture_crawl_frame(frames);
                if (got) {
                    crawl_saved = true;
                } else if (frames > 520) {
                    std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
                    const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
                    if (packed > 0) {
                        const i32 w = packed / 1000;
                        const i32 h = packed % 1000;
                        write_ppm("window_crawl.ppm", pixels.data(), w, h);
                        LOG_INFO("window test: forced crawl capture at %d (fallback timed)", frames);
                        crawl_saved = true;
                    }
                }
            }
            // If all stages captured, we have reached menu territory. Keep a
            // final window.ppm as the latest frame and exit early.
            if (legal_saved && blue_saved && crawl_saved) {
                LOG_INFO("window test: legal+blue+crawl captured, continuing to menu");
                // Give a few more seconds for menu to settle and capture it as window.ppm
                for (i32 i = 0; i < 200; i++) {
                    g_renderDevice.SwapBuffers();
                    SDL_Delay(kPollIntervalMs);
                    frames++;
                }
                // Final capture as window.ppm (menu)
                {
                    std::vector<u8> pixels(static_cast<usize>(kWindowWidth) * kWindowHeight * 4);
                    const i32 packed = g_renderDevice.HostReadbackPixels(kWindowWidth, kWindowHeight, pixels.data());
                    if (packed > 0) {
                        const i32 w = packed / 1000;
                        const i32 h = packed % 1000;
                        write_ppm("window.ppm", pixels.data(), w, h);
                        write_ppm("window_menu.ppm", pixels.data(), w, h);
                        LOG_INFO("captured menu frame at pump #%d", frames);
                    }
                }
                fflush(nullptr);
                _exit(0);
            }
        }

        SDL_Delay(kPollIntervalMs);

        if (SDL_GetTicks() - start_ticks > static_cast<Uint64>(kTimeoutMs)) {
            LOG_WARN("window test timed out waiting for NuMain (legal=%d blue=%d crawl=%d)", legal_saved ? 1 : 0,
                     blue_saved ? 1 : 0, crawl_saved ? 1 : 0);
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
