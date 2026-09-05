#include "nu2api/nucore/common.h"

#include "host/platform/graphics.hpp"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <SDL3/SDL_timer.h>
#include <pthread.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <vector>

// Frame capture belongs at the external EGL boundary. It must not replace the
// game's render device, framebuffer allocation, or presentation path.
namespace {
    pthread_mutex_t host_readback_mutex = PTHREAD_MUTEX_INITIALIZER;
    std::vector<u8> host_readback_pixels;
    i32 host_readback_width;
    i32 host_readback_height;
    std::atomic<bool> host_readback_enabled{false};
    std::atomic<bool> host_readback_requested{false};
    std::atomic<bool> host_fps_overlay_enabled{false};

    // Three-by-five glyphs, stored one three-bit row at a time. This keeps the
    // host overlay independent of the game's shaders and font resources.
    const u8 *host_fps_glyph(char character) {
        static const u8 digits[][5] = {
            {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
            {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 1, 1}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7},
        };
        static const u8 f[] = {7, 4, 6, 4, 4};
        static const u8 p[] = {6, 5, 6, 4, 4};
        static const u8 s[] = {7, 4, 7, 1, 7};
        static const u8 space[] = {0, 0, 0, 0, 0};
        if (character >= '0' && character <= '9') {
            return digits[character - '0'];
        }
        if (character == 'F') {
            return f;
        }
        if (character == 'P') {
            return p;
        }
        if (character == 'S') {
            return s;
        }
        return space;
    }

    void host_draw_fps_overlay(i32 width, i32 height) {
        if (!host_fps_overlay_enabled.load(std::memory_order_relaxed)) {
            return;
        }
        if (width <= 0 || height <= 0) {
            return;
        }

        static Uint64 interval_start_ns;
        static u32 interval_frames;
        static u32 displayed_fps;
        const Uint64 now_ns = SDL_GetTicksNS();
        if (interval_start_ns == 0) {
            interval_start_ns = now_ns;
        }
        ++interval_frames;
        const Uint64 elapsed_ns = now_ns - interval_start_ns;
        if (elapsed_ns >= 500000000) {
            displayed_fps = static_cast<u32>((interval_frames * 1000000000ULL + elapsed_ns / 2) / elapsed_ns);
            interval_frames = 0;
            interval_start_ns = now_ns;
        }

        char text[16];
        std::snprintf(text, sizeof(text), "%u FPS", displayed_fps);
        constexpr GLint scale = 3;
        constexpr GLint margin = 6;
        constexpr GLint glyph_height = 5 * scale;
        constexpr GLint advance = 4 * scale;
        const GLint text_width = static_cast<GLint>(std::strlen(text)) * advance - scale;
        const GLint baseline = height - margin - glyph_height;

        const GLboolean scissor_enabled = glIsEnabled(GL_SCISSOR_TEST);
        GLint old_scissor[4];
        GLfloat old_clear_color[4];
        GLboolean old_color_mask[4];
        glGetIntegerv(GL_SCISSOR_BOX, old_scissor);
        glGetFloatv(GL_COLOR_CLEAR_VALUE, old_clear_color);
        glGetBooleanv(GL_COLOR_WRITEMASK, old_color_mask);
        glEnable(GL_SCISSOR_TEST);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glScissor(margin - 3, baseline - 3, text_width + 6, glyph_height + 6);
        glClearColor(0.0f, 0.0f, 0.0f, 0.75f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        for (i32 glyph_index = 0; text[glyph_index] != '\0'; ++glyph_index) {
            const u8 *glyph = host_fps_glyph(text[glyph_index]);
            for (i32 row = 0; row < 5; ++row) {
                for (i32 column = 0; column < 3; ++column) {
                    if ((glyph[row] & (1u << (2 - column))) == 0) {
                        continue;
                    }
                    glScissor(margin + glyph_index * advance + column * scale, baseline + (4 - row) * scale, scale,
                              scale);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
            }
        }

        glClearColor(old_clear_color[0], old_clear_color[1], old_clear_color[2], old_clear_color[3]);
        glColorMask(old_color_mask[0], old_color_mask[1], old_color_mask[2], old_color_mask[3]);
        glScissor(old_scissor[0], old_scissor[1], old_scissor[2], old_scissor[3]);
        if (!scissor_enabled) {
            glDisable(GL_SCISSOR_TEST);
        }
    }
} // namespace

void HostCaptureCurrentSurface(EGLDisplay display, EGLSurface surface) {
    EGLint width = 0;
    EGLint height = 0;
    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        return;
    }
    HostCaptureCurrentFramebuffer(width, height);
}

void HostCaptureCurrentFramebuffer(i32 width, i32 height) {
    host_draw_fps_overlay(width, height);
    if (!host_readback_enabled.load(std::memory_order_relaxed) ||
        !host_readback_requested.exchange(false, std::memory_order_acq_rel)) {
        return;
    }
    if (width <= 0 || height <= 0) {
        return;
    }

    std::vector<u8> pixels(static_cast<usize>(width) * static_cast<usize>(height) * 4);
    GLint old_pack_alignment = 4;
    glGetIntegerv(GL_PACK_ALIGNMENT, &old_pack_alignment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT, old_pack_alignment);

    pthread_mutex_lock(&host_readback_mutex);
    host_readback_pixels.swap(pixels);
    host_readback_width = width;
    host_readback_height = height;
    pthread_mutex_unlock(&host_readback_mutex);
}

void HostSetReadbackEnabled(bool enabled) {
    host_readback_enabled.store(enabled, std::memory_order_relaxed);
    host_readback_requested.store(enabled, std::memory_order_release);
}

void HostRequestReadback(void) {
    if (host_readback_enabled.load(std::memory_order_relaxed)) {
        host_readback_requested.store(true, std::memory_order_release);
    }
}

void HostSetFpsOverlayEnabled(bool enabled) {
    host_fps_overlay_enabled.store(enabled, std::memory_order_relaxed);
}

void HostPaceFrame(const NUTIME *frame_start, f32 target_seconds) {
    NUTIME now;
    NUTIME elapsed;
    NuTimeGet(&now);
    NuTimeSub(&elapsed, &now, const_cast<NUTIME *>(frame_start));
    const f32 remaining_seconds = target_seconds - NuTimeSeconds(&elapsed);

    // Leave a short tail to the original polling loop so the game's timing
    // and minimum-frame semantics remain unchanged despite scheduler jitter.
    constexpr f32 spin_tail_seconds = 0.0005f;
    if (remaining_seconds > spin_tail_seconds) {
        const Uint64 sleep_ns = static_cast<Uint64>((remaining_seconds - spin_tail_seconds) * 1000000000.0f);
        SDL_DelayPrecise(sleep_ns);
    }
}

i32 HostReadbackPixels(u32 max_w, u32 max_h, u8 *rgba) {
    pthread_mutex_lock(&host_readback_mutex);
    const i32 width = std::min<i32>(host_readback_width, static_cast<i32>(max_w));
    const i32 height = std::min<i32>(host_readback_height, static_cast<i32>(max_h));
    if (width <= 0 || height <= 0 || rgba == nullptr) {
        pthread_mutex_unlock(&host_readback_mutex);
        return 0;
    }

    for (i32 y = 0; y < height; ++y) {
        memcpy(rgba + static_cast<usize>(y) * width * 4,
               host_readback_pixels.data() + static_cast<usize>(y) * host_readback_width * 4,
               static_cast<usize>(width) * 4);
    }
    pthread_mutex_unlock(&host_readback_mutex);
    return width * 1000 + height;
}
