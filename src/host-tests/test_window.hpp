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

// HOST-ONLY: read the current frame back and report whether it shows the
// legal-screen texture (the LEGAL_ENGLISH logo: red/white LEGO lettering on a
// dark gradient). The gradient alone is drawn from the very first frame, so
// plain "non-black" is not enough; require a meaningful amount of the logo's
// saturated red or white pixels before writing window.ppm.
static bool TryCaptureLegalFrame(int frames) {
    std::vector<u8> px(1280 * 720 * 4);
    int packed = g_renderDevice.HostReadbackPixels(1280, 720, px.data());
    if (packed <= 0) {
        return false;
    }
    int rbw = packed / 1000, rbh = packed % 1000;
    u64 redpx = 0, whitepx = 0;
    for (int i = 0; i < rbw * rbh; i++) {
        u8 r = px[((usize)i) * 4 + 0], g = px[((usize)i) * 4 + 1], b = px[((usize)i) * 4 + 2];
        if (r > 120 && g < 90 && b < 90) {
            redpx++;
        } else if (r > 200 && g > 200 && b > 200) {
            whitepx++;
        }
    }
    if (redpx < 3000 && whitepx < 30000) {
        return false;
    }
    FILE *f = fopen("window.ppm", "wb");
    if (!f) {
        return true;
    }
    fprintf(f, "P6\n%d %d\n255\n", rbw, rbh);
    std::vector<u8> row((usize)rbw * 4);
    for (int y = 0; y < rbh; y++) {
        memcpy(row.data(), &px[((usize)(rbh - 1 - y)) * rbw * 4], (usize)rbw * 4);
        for (int x = 0; x < rbw; x++) {
            fwrite(&row[(usize)x * 4], 1, 3, f);
        }
    }
    fclose(f);
    LOG_INFO("captured legal frame at pump #%d (%dx%d, red=%llu white=%llu)", frames, rbw, rbh,
             (unsigned long long)redpx, (unsigned long long)whitepx);
    return true;
}

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

static SDL_Thread *g_numain_thread = NULL;
static int g_numain_result = 0;
static volatile bool g_numain_done = false;

static int SDLCALL numain_thread_main(void *arg) {
    char *argv[] = {(char *)"saga", NULL};
    g_numain_result = NuMain(1, argv);
    g_numain_done = true;
    return g_numain_result;
}

int test_window(int argc, char **argv) {
    host_init();       // PS: SDL window + EGL context
    nufile_open_wad(); // PS/entry: make the wad readable, then NuMain

    // The Android boot selects the platform the engine targets; the legal
    // texture is stored per-platform (`_ios.tex`), so pick the PVRTC one.
    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    // Run the game boot on a worker thread (like the Android game thread) and
    // pump the engine's present path from this thread concurrently, mirroring
    // the real render-thread split.
    g_numain_thread = SDL_CreateThread(numain_thread_main, "numain", NULL);

    Uint64 start = SDL_GetTicks();
    int frames = 0;
    bool legal_saved = false;

    while (true) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT)
                goto done;
        }

        if (g_numain_done) {
            // NuMain returned: give the present loop a short tail so the last
            // committed frame reaches the window surface, then stop.
            for (int i = 0; i < 30; i++) {
                g_renderDevice.SwapBuffers();
                SDL_Delay(16);
                frames++;
            }
            break;
        }

        g_renderDevice.SwapBuffers();
        frames++;

        // HOST-ONLY capture policy: the loading-screen loop only composites
        // the legal texture while its fade timer is inside (0, 5.8) s, which
        // on a slow host build is long before NuMain() returns. Grab a frame
        // every pump and keep the first one that actually shows content.
        if (!legal_saved && frames % 4 == 0) {
            legal_saved = TryCaptureLegalFrame(frames);
        }
        if (legal_saved) {
            // The engine main loop runs until quit by design; once the legal
            // texture is captured the window test is satisfied. HOST-ONLY:
            // hard-exit because the decompilation has no clean shutdown path
            // yet (original exits via Android activity lifecycle).
            LOG_INFO("window test: legal frame saved, exiting");
            fflush(NULL);
            _exit(0);
        }

        SDL_Delay(16);

        if (SDL_GetTicks() - start > 90000) {
            LOG_WARN("window test timed out waiting for NuMain");
            break;
        }
    }
done:
    // HOST-ONLY: detach rather than join — NuMain keeps running (the game loop
    // never returns on a live device either), and the decompiled shutdown path
    // is not reachable from the host harness.
    if (g_numain_thread != NULL) {
        SDL_DetachThread(g_numain_thread);
        g_numain_thread = NULL;
    }
    // PS present-loop readback: grab whatever the engine drew and write it to a
    // PPM so a human/check can confirm non-blank content. If a legal-screen
    // frame was already captured mid-run, keep it (the loading loop blanks to
    // a plain gradient once its fade timer expires).
    if (!g_headless && !legal_saved) {
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
                fflush(NULL);
                _exit(0);
            }
        } else {
            LOG_WARN("readback: no window surface/context, skipping PPM");
        }
    }
    LOG_INFO("presented %d frames (headless=%d)", frames, g_headless);
    _exit(0);
}
