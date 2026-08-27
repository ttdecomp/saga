#pragma once

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string.h>

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv);

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

        for (i32 y = 0; y < height; y++) {
            for (i32 x = 0; x < width; x++) {
                const usize src = (static_cast<usize>(y) * static_cast<usize>(width) + static_cast<usize>(x)) * 4;
                fwrite(&rgba[src], 1, 3, file);
            }
        }
        fclose(file);

        return true;
    }

    static bool capture_frame(i32 frame) {
        std::vector<u8> pixels(static_cast<usize>(host_window_width) * host_window_height * 4);
        const i32 packed = g_renderDevice.HostReadbackPixels(host_window_width, host_window_height, pixels.data());
        if (packed <= 0) {
            return false;
        }

        const i32 width = packed / 1000;
        const i32 height = packed % 1000;

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

    sdl_init();

    void *buffer = malloc(0x1000000);
    VARIPTR ptr = VARIPTR{.void_ptr = buffer};
    NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &ptr, 0));

    // try to remove .work/capture/* before starting, but don't fail if it doesn't exist
    system("rm -r .work/capture");
    system("mkdir -p .work/capture");

    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    host_numain_thread = SDL_CreateThread(numain_thread_main, "numain", nullptr);

    const Uint64 start_ticks = SDL_GetTicks();
    i32 frame_count = 0;

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

        if (host_numain_done) {
            for (i32 i = 0; i < host_tail_frames; i++) {
                g_renderDevice.SwapBuffers();
                SDL_Delay(host_poll_interval_ms);
                frame_count++;
            }
            capture_frame(frame_count);
            break;
        }

        g_renderDevice.SwapBuffers();
        frame_count++;

        SDL_Delay(host_poll_interval_ms);

        if (SDL_GetTicks() - start_ticks > static_cast<Uint64>(host_timeout_ms)) {
            LOG_ERR("test_window: timeout after %d ms", host_timeout_ms);
            break;
        }

        if (frame_count % 10 == 0) {
            capture_frame(frame_count);
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
