#pragma once

// HOST-ONLY: runs the real NuMain and checks that the title music actually
// reaches the host audio device through the decompiled NuSound pipeline:
// NuSound3PlayStereoV -> streamer -> NuSoundVoiceAndroid -> fake OpenSL
// buffer queue (opensl_host.cpp) -> SDL3 device. Success requires at least
// one hardware player created, several seconds of PCM drained by the device
// and a non-silent mixed signal.

#include <SDL3/SDL.h>

#include <cstdlib>

#include "decomp.h"
#include "globals.h"
#include "host-utils/nusound/opensl_host.hpp"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nuplatform/nuplatform.h"
#include "nu2api/nufile/nufile.h"

extern "C" i32 NuMain(i32 argc, char **argv);

#ifdef _WIN32
constexpr const char *host_audio_video_driver = "windows";
#else
constexpr const char *host_audio_video_driver = "x11";
#endif

namespace {

    void host_audio_sdl_init() {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, host_audio_video_driver);
        // The audio driver can be overridden for headless runs (SDL_AUDIODRIVER
        // via env is honoured by SDL itself; this only pins video).
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            LOG_ERR("SDL_Init failed: %s", SDL_GetError());
            return;
        }

        SDL_Window *window = SDL_CreateWindow("saga-audio", 1280, 720, 0);
        if (window == nullptr) {
            LOG_ERR("SDL_CreateWindow failed: %s", SDL_GetError());
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

    SDL_Thread *host_audio_numain_thread = nullptr;
    volatile bool host_audio_numain_done = false;
    i32 host_audio_numain_result = 0;

    int SDLCALL host_audio_numain_main(void *arg) {
        (void)arg;
        char *argv[] = {const_cast<char *>("saga"), nullptr};
        host_audio_numain_result = NuMain(1, argv);
        host_audio_numain_done = true;
        return host_audio_numain_result;
    }

} // namespace

static i32 host_run_audio() {
    LOG_INFO("audio utility: running NuMain and watching the host audio device");

    host_audio_sdl_init();

    void *buffer = malloc(0x1000000);
    VARIPTR ptr = VARIPTR{.void_ptr = buffer};
    NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &ptr, 0));

    NuPlatform::Create();
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_PVRTC_PLATFORM);

    host_audio_numain_thread = SDL_CreateThread(host_audio_numain_main, "numain", nullptr);

    // 3 seconds of stereo s16 at the 48 kHz device rate.
    const u64 min_consumed_bytes = 48000ull * 2 * 2 * 3;
    bool saw_signal = false;

    const Uint64 start = SDL_GetTicks();
    Uint64 last_report = 0;
    i32 result = 1;

    while (!host_audio_numain_done) {
        SDL_Delay(100);

        const Uint64 now = SDL_GetTicks();
        if (now - last_report < 1000) {
            continue;
        }
        last_report = now;

        const hostsl::HostStats stats = hostsl::HostGetStats();
        if (stats.last_mix_rms > 30.0f) {
            saw_signal = true;
        }
        LOG_INFO("[%5.1fs] players=%u playing=%u enqueued=%llukB consumed=%llukB rms=%.1f%s", (now - start) / 1000.0,
                 stats.players_created, stats.players_playing, stats.bytes_enqueued / 1024, stats.bytes_consumed / 1024,
                 stats.last_mix_rms, saw_signal ? " (signal)" : "");

        if (stats.players_created >= 1 && saw_signal && stats.bytes_consumed >= min_consumed_bytes) {
            LOG_INFO("audio utility: PASS — %u player(s), %llu kB consumed, rms %.1f", stats.players_created,
                     stats.bytes_consumed / 1024, stats.last_mix_rms);
            result = 0;
            break;
        }

        if (now - start > 75000) {
            LOG_ERR("audio utility: TIMEOUT after 75s (players=%u consumed=%llukB rms=%.1f)", stats.players_created,
                    stats.bytes_consumed / 1024, stats.last_mix_rms);
            result = 1;
            break;
        }
    }

    if (host_audio_numain_done) {
        LOG_INFO("NuMain returned %d after %.1fs", host_audio_numain_result, (SDL_GetTicks() - start) / 1000.0);
        const hostsl::HostStats stats = hostsl::HostGetStats();
        if (stats.players_created >= 1 && saw_signal && stats.bytes_consumed >= min_consumed_bytes) {
            result = 0;
        }
    }

    return result;
}
