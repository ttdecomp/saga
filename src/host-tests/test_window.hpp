#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "decomp.h"
#include "globals.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv); // the real game boot (batman.cpp)

const char VIDEO_DRIVER[] =
#ifdef _WIN32
    "windows"
#else
    "x11"
#endif
    ;

static bool g_headless = false;

// Host platform specific (PS) bootstrap: open a window and hand it to the
// engine's render device so it can create its EGL/GLES2 context. This is the
// only real window work; the game itself is booted by NuMain() below. Video is
// required; audio is optional (its backend may be absent), so it is tolerated.
void host_init() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, VIDEO_DRIVER);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        LOG_ERR("SDL_Init(VIDEO) failed: %s", SDL_GetError());
        LOG_ERR("headless mode (no window)");
        g_headless = true;
        return;
    }
    SDL_InitSubSystem(SDL_INIT_AUDIO); // best effort

    // SDL3 shows windows by default.
    SDL_Window *window = SDL_CreateWindow("saga", 1280, 720, 0);
    if (!window) {
        LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
        g_headless = true;
        return;
    }

    if (strcmp(SDL_GetCurrentVideoDriver(), VIDEO_DRIVER) != 0) {
        LOG_ERR("unexpected video driver: %s", SDL_GetCurrentVideoDriver());
        g_headless = true;
        return;
    }

    SDL_PropertiesID prop_id = SDL_GetWindowProperties(window);
#ifdef _WIN32
    HWND handle = (HWND)SDL_GetPointerProperty(prop_id, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
    i32 handle = SDL_GetNumberProperty(prop_id, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
#endif

    g_renderDevice.OnWindowCreated((ANativeWindow *)handle);
}

// Open the game wad over the engine's NuDat file system. The real Android
// entry does this before calling NuMain so every asset (incl. the first
// "legal" texture) is readable by path.
void nufile_open_wad() {
    static char pbuf[0x1000000];
    VARIPTR p = VARIPTR{.void_ptr = &pbuf};
    NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &p, 0));
}

int test_window(int argc, char **argv) {
    host_init();       // PS: SDL window + EGL context
    nufile_open_wad(); // PS/entry: make the wad readable, then NuMain

    // The Android boot selects the platform the engine targets; the legal
    // texture is stored per-platform (`_ios.tex`), so pick the PVRTC one.
    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    NuMain(argc, argv); // the actual game boot: InitOnce + LoadPerm (reads
                        // the legal texture) + post-config init

    // Present the engine's frame output. The per-frame scene/legal draw is
    // still being reconstructed, so this drives the engine's present path.
    Uint64 start = SDL_GetTicks();
    int frames = 0;
    while (SDL_GetTicks() - start < 3000) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT)
                goto done;
        }
        g_renderDevice.SwapBuffers();
        SDL_Delay(16);
        frames++;
    }
done:
    // PS present-loop readback: grab whatever the engine drew and write it to a
    // PPM so a human/check can confirm non-blank content (e.g. the legal screen).
    if (!g_headless) {
        std::vector<u8> px(1280 * 720 * 4);
        int packed = g_renderDevice.HostReadbackPixels(1280, 720, px.data());
        if (packed > 0) {
            int rbw = packed / 1000, rbh = packed % 1000;
            FILE *f = fopen("window.ppm", "wb");
            if (f) {
                fprintf(f, "P6\n%d %d\n255\n", rbw, rbh);
                // GL readback is bottom-up; flip rows for the PPM.
                std::vector<u8> row((usize)rbw * 4);
                for (int y = 0; y < rbh; y++) {
                    memcpy(row.data(), &px[((usize)(rbh - 1 - y)) * rbw * 4], (usize)rbw * 4);
                    for (int x = 0; x < rbw; x++) {
                        fwrite(&row[(usize)x * 4], 1, 3, f); // RGBA -> RGB
                    }
                }
                fclose(f);
                // Summarize content: count non-grey/black pixels so a human/check
                // can quickly confirm a textured frame (e.g. the legal screen).
                u64 nonblack = 0;
                for (int i = 0; i < rbw * rbh; i++) {
                    u8 r = px[((usize)i) * 4 + 0], g = px[((usize)i) * 4 + 1], b = px[((usize)i) * 4 + 2];
                    if (r > 8 || g > 8 || b > 8) {
                        nonblack++;
                    }
                }
                LOG_INFO("wrote window.ppm %dx%d  non-black px=%llu", rbw, rbh, (unsigned long long)nonblack);
            }
        } else {
            LOG_WARN("readback: no window surface/context, skipping PPM");
        }
    }
    LOG_INFO("presented %d frames (headless=%d)", frames, g_headless);
    _exit(0);
}
