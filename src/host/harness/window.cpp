#include <SDL3/SDL.h>

#include <atomic>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "decomp.h"
#include "globals.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "host/harness/window.hpp"
#include "host/platform/graphics.hpp"
#include "host/platform/input.hpp"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "nu2api/nu3d/NuRenderDevice.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nuscreen.hpp"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuplatform/nuplatform.h"

extern "C" i32 NuMain(i32 argc, char **argv);
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
extern i32 NewMode;
extern FadeSystem FadeSys;
extern GAMEPAD_s GamePad[64];

namespace {
    struct HostSpecialHandleLayout {
        NUGSCN *scene;
        void *special;
        void *display_special;
    };

    struct HostDisplaySpecialLayout {
        NUMTX mtx;
        NUMTX draw_mtx;
        NUVEC min;
        f32 min_w;
        NUVEC max;
        f32 max_w;
        u8 pad_a0[0x10];
        NUCLIPOBJECT *clip_objects;
        char *name;
        u32 flags;
        f32 *clip_range;
        i32 instance_ix;
        NUMTX *draw_mtx_ptr;
        i16 wind_speed;
        i16 wind_scale;
        u32 pad_cc;
    };

#ifdef __EMSCRIPTEN__
    constexpr const char *host_video_driver = "emscripten";
#elif defined(_WIN32)
    constexpr const char *host_video_driver = "windows";
#else
    constexpr const char *host_video_driver = "x11";
#endif

    constexpr i32 host_window_width = 1280;
    constexpr i32 host_window_height = 720;
    constexpr i32 host_poll_interval_ms = 16;
    constexpr i32 host_tail_frames = 30;
    // Leave enough time for the original asynchronous load result and its
    // one-second menu result delay to complete after the final scripted tap.
    constexpr Uint64 host_scripted_title_input_ms = 18000;
    constexpr Uint64 host_scripted_menu_settle_ms = 500;
    constexpr Uint64 host_scripted_play_move_ms = 2000;
    constexpr Uint64 host_scripted_play_settle_ms = 1000;

    enum class HostScriptedInputStage {
        title,
        new_or_load,
        select_controls,
        load_wait,
        save_slot,
        overwrite_confirm,
        cantina_wait,
        play_move,
        play_settle,
        complete,
    };

    struct HostScriptedPlaySnapshot {
        NUVEC player_position{};
        NUVEC camera_position{};
        NUVEC player_velocity{};
        u16 player_yrot = 0;
        u32 player_flags = 0;
        u32 player_motion_flags = 0;
        u32 buttons_held = 0;
        f32 input_magnitude = 0.0f;
        f32 movement_speed = 0.0f;
        f32 frame_time = 0.0f;
        i32 object_index = -1;
    };

    enum class HostScriptedMenuAction {
        waiting,
        navigating,
        confirmed,
    };

    static bool host_finite_vec(const NUVEC &vec) {
        return std::isfinite(vec.x) && std::isfinite(vec.y) && std::isfinite(vec.z);
    }

    static bool host_player_matrix_ready(const GameObject_s *object) {
        const NUMTX &matrix = object->apiobj.field_0xb8;
        const f32 basis_squared = matrix.m00 * matrix.m00 + matrix.m01 * matrix.m01 + matrix.m02 * matrix.m02 +
                                  matrix.m10 * matrix.m10 + matrix.m11 * matrix.m11 + matrix.m12 * matrix.m12 +
                                  matrix.m20 * matrix.m20 + matrix.m21 * matrix.m21 + matrix.m22 * matrix.m22;
        const f32 dx = matrix.m30 - object->apiobj.position.x;
        const f32 dy = matrix.m31 - object->apiobj.position.y;
        const f32 dz = matrix.m32 - object->apiobj.position.z;
        return std::isfinite(basis_squared) && basis_squared > 0.0001f && std::isfinite(matrix.m33) &&
               std::fabs(matrix.m33 - 1.0f) < 0.001f && dx * dx + dy * dy + dz * dz < 0.0001f;
    }

    static bool host_scripted_play_ready() {
        // current_level can point at the hub while the asynchronous world and
        // character loads are still finishing.  Require the active hub area,
        // its gameplay socket system, the normal idle load sentinels, and the
        // real Player 0 controller assignment before injecting any input.
        if (WORLD == nullptr || WORLD->loaded == 0 || WORLD->current_level != HUB_LDATA || WORLD->area != HUB_ADATA ||
            WORLD->sock_sys == nullptr || Player[0] == nullptr) {
            return false;
        }

        const u32 player_flags = Player[0]->apiobj.field_0x1f8;
        return NewLData == nullptr && NewMode == 0 && player == Player[0] && Player[1] == nullptr &&
               (player_flags & 0x1001) == 0x1001 && static_cast<i8>(player_flags) < 0 &&
               Player[0]->pad_gamepad == &GamePad[0] && host_player_matrix_ready(Player[0]) &&
               host_finite_vec(Player[0]->apiobj.position) && std::isfinite(global_camera.mtx.m30) &&
               std::isfinite(global_camera.mtx.m31) && std::isfinite(global_camera.mtx.m32) &&
               waiting_for_level == -1 && waiting_for_character == -1 && waiting_for_new_level == 0 && abort_load == 0;
    }

    static HostScriptedPlaySnapshot host_scripted_play_snapshot() {
        HostScriptedPlaySnapshot snapshot;
        snapshot.player_position = Player[0]->apiobj.position;
        snapshot.player_velocity = {Player[0]->apiobj.field_0x68, Player[0]->apiobj.field_0x6c,
                                    Player[0]->apiobj.field_0x70};
        snapshot.player_yrot = Player[0]->yrot;
        snapshot.player_flags = Player[0]->apiobj.field_0x1f8;
        snapshot.player_motion_flags = Player[0]->apiobj.field_0x1f4;
        snapshot.buttons_held = GamePad[0].buttons_held;
        snapshot.input_magnitude = GamePad[0].input_magnitude;
        snapshot.frame_time = FRAMETIME;
        if (Obj != nullptr && Player[0] >= Obj && Player[0] < Obj + HIGHGAMEOBJECT) {
            snapshot.object_index = static_cast<i32>(Player[0] - Obj);
        }
        if (Player[0]->apiobj.character_data != nullptr && Player[0]->apiobj.character_data->field11_0x24 != nullptr) {
            snapshot.movement_speed =
                static_cast<GAMECHARACTERDATA *>(Player[0]->apiobj.character_data->field11_0x24)->movement_speed;
        }
        snapshot.camera_position = {global_camera.mtx.m30, global_camera.mtx.m31, global_camera.mtx.m32};
        return snapshot;
    }

    static HostScriptedMenuAction host_scripted_menu_select(i32 row, i32 column, Uint64 elapsed_ticks,
                                                            Uint64 &last_action_ticks) {
        if (elapsed_ticks < last_action_ticks + host_scripted_menu_settle_ms) {
            return HostScriptedMenuAction::waiting;
        }

        const MENU &menu = GameMenu[GameMenuLevel];
        u32 button = GAMEPAD_JUMP;
        HostScriptedMenuAction result = HostScriptedMenuAction::confirmed;
        if (menu.selected_row < row) {
            button = GAMEPAD_DDOWN;
            result = HostScriptedMenuAction::navigating;
        } else if (menu.selected_row > row) {
            button = GAMEPAD_DUP;
            result = HostScriptedMenuAction::navigating;
        } else if (menu.selected_column < column) {
            button = GAMEPAD_DRIGHT;
            result = HostScriptedMenuAction::navigating;
        } else if (menu.selected_column > column) {
            button = GAMEPAD_DLEFT;
            result = HostScriptedMenuAction::navigating;
        }

        LOG_INFO("scripted gamepad menu=%d cursor=(%d,%d) target=(%d,%d) button=0x%x", GetMenuID(),
                 menu.selected_column, menu.selected_row, column, row, button);
        HostInputTap(0, button);
        last_action_ticks = elapsed_ticks;
        return result;
    }

    static bool host_write_ppm(const char *path, const u8 *rgba, i32 width, i32 height) {
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

    static u64 host_pixel_hash(const u8 *pixels, usize pixel_count) {
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

    static bool host_frame_has_visible_pixels(const u8 *pixels, usize pixel_count) {
        for (usize i = 0; i < pixel_count; ++i) {
            const u8 *pixel = pixels + i * 4;
            if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0) {
                return true;
            }
        }
        return false;
    }

    static bool host_read_frame(std::vector<u8> &pixels, i32 &width, i32 &height, u64 &hash) {
        pixels.resize(static_cast<usize>(host_window_width) * host_window_height * 4);
        const i32 packed = HostReadbackPixels(host_window_width, host_window_height, pixels.data());
        if (packed <= 0) {
            return false;
        }

        width = packed / 1000;
        height = packed % 1000;
        hash = host_pixel_hash(pixels.data(), static_cast<usize>(width) * height);
        return true;
    }

    static bool host_capture_frame(i32 frame, const std::vector<u8> &pixels, i32 width, i32 height) {
        char filename[64];
        snprintf(filename, sizeof(filename), ".work/capture/window_%04d.ppm", frame);

        return host_write_ppm(filename, pixels.data(), width, height);
    }

    static void host_sdl_init(bool offscreen, bool mute) {
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
#ifdef __EMSCRIPTEN__
        g_renderDevice.OnWindowCreated(nullptr);
#elif defined(_WIN32)
        HWND handle = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        g_renderDevice.OnWindowCreated(reinterpret_cast<ANativeWindow *>(handle));
#else
        auto handle = static_cast<i32>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
        g_renderDevice.OnWindowCreated(reinterpret_cast<ANativeWindow *>(handle));
#endif
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

    static int SDLCALL host_numain_thread_main(void *arg) {
        (void)arg;
        char *argv[] = {const_cast<char *>("saga"), nullptr};
        const i32 result = NuMain(1, argv);
        host_numain_result.store(result, std::memory_order_relaxed);
        host_numain_done.store(true, std::memory_order_release);
        return result;
    }

} // namespace

i32 host_run_window(const HostWindowOptions &options) {
    HostSetReadbackEnabled(options.capture);
    host_sdl_init(options.offscreen, options.mute);
    const char *documents_path = ".work/host-documents/";
    char scripted_documents_path[256];
    if (options.script_input) {
        SDL_CreateDirectory(".work/host-documents-scripted");
        snprintf(scripted_documents_path, sizeof(scripted_documents_path), ".work/host-documents-scripted/%llu/",
                 static_cast<unsigned long long>(SDL_GetTicksNS()));
        documents_path = scripted_documents_path;
    }
    if (!SDL_CreateDirectory(documents_path)) {
        LOG_ERR("failed to create host documents directory %s: %s", documents_path, SDL_GetError());
    }
    HostSetDocumentsPath(documents_path);

    void *buffer = malloc(0x1000000);
    VARIPTR ptr = VARIPTR{.void_ptr = buffer};
    NuDatSet(NuDatOpen("res/main.1060.com.wb.lego.tcs.obb", &ptr, 0));

    if (options.capture) {
        // Try to remove .work/capture/* before starting, but don't fail if it
        // does not exist. Readback is deliberately opt-in: glReadPixels must
        // synchronize with the render thread and noticeably affects pacing.
        system("rm -r .work/capture");
        system("mkdir -p .work/capture");
    }

    // AndroidMain performs the process-side lifecycle after the activity has
    // supplied its surface: platform, screen, render device, then NuMain.
    NuPlatform::Create();
    NuScreen::Create();
    g_renderDevice.Initialize();

    // Mesa advertises S3TC, but this bundled OBB contains Android ETC1 assets.
    // The host upload boundary decodes ETC1 when the driver cannot upload it.
    NuPlatform::Get()->SetCurrentPlatform(ANDROID_ETC1_PLATFORM);
    HostInputReset();

    host_numain_result.store(0, std::memory_order_relaxed);
    host_numain_done.store(false, std::memory_order_relaxed);
    host_numain_thread = SDL_CreateThread(host_numain_thread_main, "numain", nullptr);
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
    bool saw_visible_frame = false;
    bool image_changing = false;
    HostScriptedInputStage scripted_stage = HostScriptedInputStage::title;
    Uint64 scripted_stage_ticks = 0;
    i32 scripted_last_menu = -2;
    Uint64 scripted_menu_since = 0;
    HostScriptedPlaySnapshot scripted_play_before{};
    HostScriptedPlaySnapshot scripted_play_during{};
    HostScriptedPlaySnapshot scripted_play_after{};
    bool scripted_play_started = false;
    bool scripted_play_finished = false;
    bool scripted_play_movement_observed = false;
    bool scripted_play_input_held = false;
    u32 keyboard_held_buttons = 0;
    std::vector<u8> pixels;
    i32 capture_width = 0;
    i32 capture_height = 0;

    bool quit_requested = false;
    while (!quit_requested) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (options.offscreen) {
                continue;
            }
            if (event.type == SDL_EVENT_QUIT) {
                quit_requested = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                u32 button = 0;
                if (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE) {
                    button = event.key.key == SDLK_RETURN ? GAMEPAD_START | GAMEPAD_JUMP : GAMEPAD_JUMP;
                } else if (event.key.key == SDLK_UP || event.key.key == SDLK_W) {
                    button = GAMEPAD_DUP;
                } else if (event.key.key == SDLK_DOWN || event.key.key == SDLK_S) {
                    button = GAMEPAD_DDOWN;
                } else if (event.key.key == SDLK_LEFT || event.key.key == SDLK_A) {
                    button = GAMEPAD_DLEFT;
                } else if (event.key.key == SDLK_RIGHT || event.key.key == SDLK_D) {
                    button = GAMEPAD_DRIGHT;
                } else if (event.key.key == SDLK_ESCAPE) {
                    button = GAMEPAD_TAG;
                }
                if (button != 0) {
                    keyboard_held_buttons |= button;
                    HostInputSetHeld(0, keyboard_held_buttons);
                }
            } else if (event.type == SDL_EVENT_KEY_UP) {
                u32 button = 0;
                if (event.key.key == SDLK_RETURN) {
                    button = GAMEPAD_START | GAMEPAD_JUMP;
                } else if (event.key.key == SDLK_SPACE) {
                    button = GAMEPAD_JUMP;
                } else if (event.key.key == SDLK_UP || event.key.key == SDLK_W) {
                    button = GAMEPAD_DUP;
                } else if (event.key.key == SDLK_DOWN || event.key.key == SDLK_S) {
                    button = GAMEPAD_DDOWN;
                } else if (event.key.key == SDLK_LEFT || event.key.key == SDLK_A) {
                    button = GAMEPAD_DLEFT;
                } else if (event.key.key == SDLK_RIGHT || event.key.key == SDLK_D) {
                    button = GAMEPAD_DRIGHT;
                } else if (event.key.key == SDLK_ESCAPE) {
                    button = GAMEPAD_TAG;
                }
                if (button != 0) {
                    keyboard_held_buttons &= ~button;
                    HostInputSetHeld(0, keyboard_held_buttons);
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
        if (options.script_input) {
            const i32 menu_id = host_menu_id();
            if (menu_id != scripted_last_menu) {
                scripted_last_menu = menu_id;
                scripted_menu_since = elapsed_ticks;
            }

            if (scripted_stage == HostScriptedInputStage::title && menu_id == 0 &&
                elapsed_ticks >= host_scripted_title_input_ms) {
                HostInputTap(0, GAMEPAD_START | GAMEPAD_JUMP);
                scripted_stage = HostScriptedInputStage::new_or_load;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::new_or_load && menu_id == 1 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                // The original menu defaults to Load Game when any save is
                // present. This route deliberately exercises New Game.
                const HostScriptedMenuAction action =
                    host_scripted_menu_select(options.script_load ? 1 : 0, 0, elapsed_ticks, scripted_stage_ticks);
                if (action == HostScriptedMenuAction::confirmed && options.script_load) {
                    // Load Game may lead to the original No Data screen
                    // (menu 1005); dismiss it once it has settled.
                    scripted_stage = HostScriptedInputStage::load_wait;
                } else if (action == HostScriptedMenuAction::confirmed) {
                    // New Game now follows the original Select Controls
                    // screen (menu 33). Its default entry is Classic, so
                    // confirm it after the normal settle interval before
                    // looking for the save-slot menu.
                    scripted_stage = HostScriptedInputStage::select_controls;
                }
            } else if (scripted_stage == HostScriptedInputStage::select_controls && menu_id == 33 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                if (host_scripted_menu_select(0, 0, elapsed_ticks, scripted_stage_ticks) ==
                    HostScriptedMenuAction::confirmed) {
                    scripted_stage = HostScriptedInputStage::save_slot;
                }
            } else if (scripted_stage == HostScriptedInputStage::select_controls && menu_id == 1000) {
                // A connected keyboard-backed gamepad follows the original
                // controller path directly to save slots. Touch-only mode
                // visits menu 33 first; support both without forcing either.
                scripted_stage = HostScriptedInputStage::save_slot;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::load_wait && menu_id == 1012 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                i32 slot = 0;
                while (slot < SAVESLOTS && saveload_slotused[slot] == 0) {
                    ++slot;
                }
                if (slot < SAVESLOTS) {
                    if (host_scripted_menu_select(0, slot, elapsed_ticks, scripted_stage_ticks) ==
                        HostScriptedMenuAction::confirmed) {
                        scripted_stage = HostScriptedInputStage::complete;
                        scripted_stage_ticks = elapsed_ticks;
                    }
                }
            } else if (scripted_stage == HostScriptedInputStage::load_wait && menu_id == 1005 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                HostInputTap(0, GAMEPAD_JUMP);
                scripted_stage = HostScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::save_slot && menu_id == 1000 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                i32 slot = 0;
                while (slot < SAVESLOTS && saveload_slotused[slot] != 0) {
                    ++slot;
                }
                const bool overwrite = slot == SAVESLOTS;
                if (overwrite) {
                    slot = 0;
                }
                if (host_scripted_menu_select(0, slot, elapsed_ticks, scripted_stage_ticks) ==
                    HostScriptedMenuAction::confirmed) {
                    scripted_stage = overwrite ? HostScriptedInputStage::overwrite_confirm
                                               : (options.script_play ? HostScriptedInputStage::cantina_wait
                                                                      : HostScriptedInputStage::complete);
                }
            } else if (scripted_stage == HostScriptedInputStage::overwrite_confirm && menu_id == 1008 &&
                       elapsed_ticks - scripted_menu_since >= host_scripted_menu_settle_ms) {
                if (host_scripted_menu_select(0, 0, elapsed_ticks, scripted_stage_ticks) ==
                    HostScriptedMenuAction::confirmed) {
                    scripted_stage =
                        options.script_play ? HostScriptedInputStage::cantina_wait : HostScriptedInputStage::complete;
                }
            } else if (scripted_stage == HostScriptedInputStage::cantina_wait && host_scripted_play_ready()) {
                scripted_play_before = host_scripted_play_snapshot();
                scripted_play_started = true;
                const CHARACTERMODEL_s *player_model = Player[0]->apiobj.character_model;
                LOG_INFO("scripted play: cantina ready level=%s idx=%d area=%d player=%p pad=%p pad0=%p "
                         "flags=(state=0x%x,motion=0x%x) position=(%.3f,%.3f,%.3f) "
                         "velocity=(%.3f,%.3f,%.3f) yrot=%u "
                         "input=(held=0x%x,magnitude=%.3f,speed=%.3f) objects=(base=%p,high=%d,index=%d) "
                         "render=(model=%p,hierarchy=%p,draw=%u,mode=%u,scale=%.3f) "
                         "frametime=%.6f camera=(%.3f,%.3f,%.3f)",
                         WORLD->current_level->name, WORLD->current_level->idx, WORLD->area->index, Player[0],
                         Player[0]->pad_gamepad, &GamePad[0], scripted_play_before.player_flags,
                         scripted_play_before.player_motion_flags, scripted_play_before.player_position.x,
                         scripted_play_before.player_position.y, scripted_play_before.player_position.z,
                         scripted_play_before.player_velocity.x, scripted_play_before.player_velocity.y,
                         scripted_play_before.player_velocity.z, scripted_play_before.player_yrot,
                         scripted_play_before.buttons_held, scripted_play_before.input_magnitude,
                         scripted_play_before.movement_speed, Obj, HIGHGAMEOBJECT, scripted_play_before.object_index,
                         player_model, player_model != nullptr ? player_model->hierarchy : nullptr,
                         Player[0]->apiobj.model_draw_result, Player[0]->field_0x1086, Player[0]->apiobj.field_0xa8,
                         scripted_play_before.frame_time, scripted_play_before.camera_position.x,
                         scripted_play_before.camera_position.y, scripted_play_before.camera_position.z);
                HostInputSetHeld(0, GAMEPAD_DRIGHT);
                scripted_play_input_held = true;
                scripted_stage = HostScriptedInputStage::play_move;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::play_move &&
                       elapsed_ticks >= scripted_stage_ticks + host_scripted_play_move_ms) {
                scripted_play_during = host_scripted_play_snapshot();
                LOG_INFO("scripted play: during DRIGHT flags=(state=0x%x,motion=0x%x) "
                         "position=(%.3f,%.3f,%.3f) velocity=(%.3f,%.3f,%.3f) "
                         "input=(held=0x%x,magnitude=%.3f,speed=%.3f) object-index=%d frametime=%.6f",
                         scripted_play_during.player_flags, scripted_play_during.player_motion_flags,
                         scripted_play_during.player_position.x, scripted_play_during.player_position.y,
                         scripted_play_during.player_position.z, scripted_play_during.player_velocity.x,
                         scripted_play_during.player_velocity.y, scripted_play_during.player_velocity.z,
                         scripted_play_during.buttons_held, scripted_play_during.input_magnitude,
                         scripted_play_during.movement_speed, scripted_play_during.object_index,
                         scripted_play_during.frame_time);
                HostInputSetHeld(0, 0);
                scripted_play_input_held = false;
                scripted_stage = HostScriptedInputStage::play_settle;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::play_settle &&
                       elapsed_ticks >= scripted_stage_ticks + host_scripted_play_settle_ms) {
                scripted_play_after = host_scripted_play_snapshot();
                scripted_play_finished = true;
                const NUVEC player_delta = {
                    scripted_play_after.player_position.x - scripted_play_before.player_position.x,
                    scripted_play_after.player_position.y - scripted_play_before.player_position.y,
                    scripted_play_after.player_position.z - scripted_play_before.player_position.z,
                };
                const NUVEC camera_delta = {
                    scripted_play_after.camera_position.x - scripted_play_before.camera_position.x,
                    scripted_play_after.camera_position.y - scripted_play_before.camera_position.y,
                    scripted_play_after.camera_position.z - scripted_play_before.camera_position.z,
                };
                const f32 player_delta_squared =
                    player_delta.x * player_delta.x + player_delta.y * player_delta.y + player_delta.z * player_delta.z;
                scripted_play_movement_observed = player_delta_squared > 0.0001f;
                LOG_INFO("scripted play: held DRIGHT for %llu ms; "
                         "after position=(%.3f,%.3f,%.3f) velocity=(%.3f,%.3f,%.3f) yrot=%u "
                         "input=(held=0x%x,magnitude=%.3f,speed=%.3f) camera=(%.3f,%.3f,%.3f); "
                         "deltas player=(%.3f,%.3f,%.3f) yrot=%d camera=(%.3f,%.3f,%.3f); "
                         "movement_observed=%d",
                         static_cast<unsigned long long>(host_scripted_play_move_ms),
                         scripted_play_after.player_position.x, scripted_play_after.player_position.y,
                         scripted_play_after.player_position.z, scripted_play_after.player_velocity.x,
                         scripted_play_after.player_velocity.y, scripted_play_after.player_velocity.z,
                         scripted_play_after.player_yrot, scripted_play_after.buttons_held,
                         scripted_play_after.input_magnitude, scripted_play_after.movement_speed,
                         scripted_play_after.camera_position.x, scripted_play_after.camera_position.y,
                         scripted_play_after.camera_position.z, player_delta.x, player_delta.y, player_delta.z,
                         static_cast<i32>(scripted_play_after.player_yrot) -
                             static_cast<i32>(scripted_play_before.player_yrot),
                         camera_delta.x, camera_delta.y, camera_delta.z, scripted_play_movement_observed ? 1 : 0);
                scripted_stage = HostScriptedInputStage::complete;
                scripted_stage_ticks = elapsed_ticks;
            } else if (scripted_stage == HostScriptedInputStage::complete &&
                       elapsed_ticks >= scripted_stage_ticks + options.script_tail_ms) {
                break;
            }
        }

        SDL_Delay(host_poll_interval_ms);

        if (SDL_GetTicks() - start_ticks > options.timeout_ms) {
            LOG_ERR("window utility: timeout after %llu ms", static_cast<unsigned long long>(options.timeout_ms));
            break;
        }

        if (!options.capture) {
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
        if (!host_read_frame(pixels, capture_width, capture_height, current_hash)) {
            continue;
        }
        saw_visible_frame |= host_frame_has_visible_pixels(pixels.data(), static_cast<usize>(capture_width) *
                                                                              static_cast<usize>(capture_height));
        const Uint64 now = SDL_GetTicks();
        if (!have_hash) {
            host_capture_frame(frame_count, pixels, capture_width, capture_height);
            captured_hash = current_hash;
            last_capture_ticks = now;
            previous_hash = current_hash;
            have_hash = true;
        } else if (current_hash != previous_hash) {
            last_change_ticks = now;
            if (!image_changing || now - last_capture_ticks >= 500) {
                host_capture_frame(frame_count, pixels, capture_width, capture_height);
                captured_hash = current_hash;
                last_capture_ticks = now;
            }
            previous_hash = current_hash;
            image_changing = true;
        } else if (image_changing && now - last_change_ticks >= 500) {
            if (current_hash != captured_hash) {
                host_capture_frame(frame_count, pixels, capture_width, capture_height);
                captured_hash = current_hash;
                last_capture_ticks = now;
            }
            image_changing = false;
        }
    }

    if (scripted_play_input_held) {
        HostInputSetHeld(0, 0);
    }

    if (options.capture) {
        u64 final_hash = 0;
        if (host_read_frame(pixels, capture_width, capture_height, final_hash)) {
            saw_visible_frame |= host_frame_has_visible_pixels(pixels.data(), static_cast<usize>(capture_width) *
                                                                                  static_cast<usize>(capture_height));
            if (!have_hash || final_hash != captured_hash) {
                host_capture_frame(frame_count, pixels, capture_width, capture_height);
            }
            have_hash = true;
        }
        if (!have_hash || !saw_visible_frame) {
            LOG_ERR("capture verification failed: framebuffer was %s",
                    have_hash ? "black for the entire run" : "never available");
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

    if (options.script_input) {
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

        i32 active_objects = 0;
        i32 model_objects = 0;
        i32 drawn_objects = 0;
        for (i32 index = 0; Obj != nullptr && index < HIGHGAMEOBJECT; ++index) {
            const GameObject_s &object = Obj[index];
            if ((object.apiobj.field_0x1f8 & 1) == 0) {
                continue;
            }
            ++active_objects;
            if (object.apiobj.character_model != nullptr && object.apiobj.character_model->hierarchy != nullptr) {
                ++model_objects;
            }
            if (object.apiobj.model_draw_result != 0) {
                ++drawn_objects;
            }
        }
        LOG_INFO("scripted objects: high=%d active=%d models=%d drawn=%d", HIGHGAMEOBJECT, active_objects,
                 model_objects, drawn_objects);
        if (Player[0] != nullptr) {
            const CHARACTERMODEL_s *model = Player[0]->apiobj.character_model;
            i16 render_indices[32] = {};
            i32 render_count = 0;
            i32 clip_state = -1;
            if (model != nullptr && model->hierarchy != nullptr) {
                MAKELAYERLISTFN make_layer_list = GCDataList[model->model_id].make_layer_list;
                if (make_layer_list != nullptr) {
                    render_count =
                        make_layer_list(Player[0]->apiobj.character_model, render_indices, Player[0]->field_0x1054);
                }
                clip_state = NuCameraClipTestExtents(&model->hierarchy->bounds_min, &model->hierarchy->bounds_max,
                                                     &Player[0]->apiobj.field_0xb8, character_farclip, 0);
            }
            LOG_INFO("scripted player render: object=%p id=%d model=%p hierarchy=%p draw=%u "
                     "flags=(state=0x%x,motion=0x%x) transform-mode=%u scale=%.3f matrix-m33=%.3f "
                     "hierarchy=(renders=%d,joints=%d) layer=(count=%d,first=%d) clip=%d far=%.3f",
                     Player[0], Player[0]->id, model, model != nullptr ? model->hierarchy : nullptr,
                     Player[0]->apiobj.model_draw_result, Player[0]->apiobj.field_0x1f8, Player[0]->apiobj.field_0x1f4,
                     Player[0]->field_0x1086, Player[0]->apiobj.field_0xa8, Player[0]->apiobj.field_0xb8.m33,
                     model != nullptr && model->hierarchy != nullptr ? model->hierarchy->render_count : -1,
                     model != nullptr && model->hierarchy != nullptr ? model->hierarchy->joint_count : -1, render_count,
                     render_count > 0 ? render_indices[0] : -1, clip_state, character_farclip);
            if (model != nullptr && model->hierarchy != nullptr && render_count > 0) {
                nuhgobj_s *hierarchy = model->hierarchy;
                nuhgobjrender_s &part = hierarchy->render_parts[render_indices[0]];
                i32 rigid_count = 0;
                i32 alternate_rigid_count = 0;
                void *first_rigid = nullptr;
                void *first_alternate_rigid = nullptr;
                for (i32 joint = 0; part.rigid_specials != nullptr && joint < hierarchy->joint_count; ++joint) {
                    rigid_count += part.rigid_specials[joint] != nullptr;
                    if (first_rigid == nullptr) {
                        first_rigid = part.rigid_specials[joint];
                    }
                }
                for (i32 joint = 0; part.alternate_rigid_specials != nullptr && joint < hierarchy->joint_count;
                     ++joint) {
                    alternate_rigid_count += part.alternate_rigid_specials[joint] != nullptr;
                    if (first_alternate_rigid == nullptr) {
                        first_alternate_rigid = part.alternate_rigid_specials[joint];
                    }
                }
                HostSpecialHandleLayout *handle = static_cast<HostSpecialHandleLayout *>(
                    part.smooth_skin_special != nullptr
                        ? part.smooth_skin_special
                        : (part.alternate_smooth_skin_special != nullptr
                               ? part.alternate_smooth_skin_special
                               : (first_rigid != nullptr ? first_rigid : first_alternate_rigid)));
                HostDisplaySpecialLayout *special =
                    handle != nullptr ? static_cast<HostDisplaySpecialLayout *>(handle->display_special) : nullptr;
                NUDLDLISTSCENE *scene = handle != nullptr && handle->scene != nullptr
                                            ? static_cast<NUDLDLISTSCENE *>(handle->scene->display_list)
                                            : nullptr;
                NUCLIPOBJECT *clip_object = special != nullptr ? special->clip_objects : nullptr;
                LOG_INFO("scripted player special: part=%d visibility=%p rigid=(array=%p,count=%d) "
                         "smooth=%p alternate=(rigid=%p,count=%d,smooth=%p) handle=(scene=%p,legacy=%p,display=%p) "
                         "display=(name=%s,flags=0x%x,instance=%d,clip=%p,range=%p,bounds=(%.3f,%.3f,%.3f)-"
                         "(%.3f,%.3f,%.3f)) scene=(display=%p,items=%d,clip=%d,mtls=%u,specials=%d,flags=0x%x) "
                         "clip-object=(materials=%d,ids=%p,indices=%p)",
                         render_indices[0], part.visibility, part.rigid_specials, rigid_count, part.smooth_skin_special,
                         part.alternate_rigid_specials, alternate_rigid_count, part.alternate_smooth_skin_special,
                         handle != nullptr ? handle->scene : nullptr, handle != nullptr ? handle->special : nullptr,
                         handle != nullptr ? handle->display_special : nullptr,
                         special != nullptr && special->name != nullptr ? special->name : "-",
                         special != nullptr ? special->flags : 0, special != nullptr ? special->instance_ix : -1,
                         special != nullptr ? special->clip_objects : nullptr,
                         special != nullptr ? special->clip_range : nullptr, special != nullptr ? special->min.x : 0.0f,
                         special != nullptr ? special->min.y : 0.0f, special != nullptr ? special->min.z : 0.0f,
                         special != nullptr ? special->max.x : 0.0f, special != nullptr ? special->max.y : 0.0f,
                         special != nullptr ? special->max.z : 0.0f, scene, scene != nullptr ? scene->nitems : -1,
                         scene != nullptr ? scene->nclip_objects : -1, scene != nullptr ? scene->nmtls : 0,
                         scene != nullptr ? scene->nspecials : -1, scene != nullptr ? scene->flags : 0,
                         clip_object != nullptr ? clip_object->nmaterials : -1,
                         clip_object != nullptr ? clip_object->material_ids : nullptr,
                         clip_object != nullptr ? clip_object->indices : nullptr);
            }
        }
    }
    if (options.script_play && !scripted_play_finished) {
        LOG_INFO("scripted play: incomplete started=%d ready_now=%d stage=%d movement_observed=unavailable",
                 scripted_play_started ? 1 : 0, host_scripted_play_ready() ? 1 : 0, static_cast<i32>(scripted_stage));
        if (Player[0] != nullptr) {
            const NUMTX &matrix = Player[0]->apiobj.field_0xb8;
            const f32 basis_squared = matrix.m00 * matrix.m00 + matrix.m01 * matrix.m01 + matrix.m02 * matrix.m02 +
                                      matrix.m10 * matrix.m10 + matrix.m11 * matrix.m11 + matrix.m12 * matrix.m12 +
                                      matrix.m20 * matrix.m20 + matrix.m21 * matrix.m21 + matrix.m22 * matrix.m22;
            LOG_INFO("scripted play: readiness world=(%p,loaded=%d,level=%p,hub=%p,area=%p,hub-area=%p,sock=%p) "
                     "load=(new=%p,mode=%d,level-wait=%d,char-wait=%d,new-wait=%d,abort=%d) "
                     "player=(global=%p,p0=%p,p1=%p,flags=0x%x,pad=%p,pad0=%p) "
                     "matrix=(basis2=%.3f,m33=%.3f,pos=%.3f,%.3f,%.3f,translation=%.3f,%.3f,%.3f) "
                     "camera=(%.3f,%.3f,%.3f)",
                     WORLD, WORLD != nullptr ? WORLD->loaded : 0, WORLD != nullptr ? WORLD->current_level : nullptr,
                     HUB_LDATA, WORLD != nullptr ? WORLD->area : nullptr, HUB_ADATA,
                     WORLD != nullptr ? WORLD->sock_sys : nullptr, NewLData, NewMode, waiting_for_level,
                     waiting_for_character, waiting_for_new_level, abort_load, player, Player[0], Player[1],
                     Player[0]->apiobj.field_0x1f8, Player[0]->pad_gamepad, &GamePad[0], basis_squared, matrix.m33,
                     Player[0]->apiobj.position.x, Player[0]->apiobj.position.y, Player[0]->apiobj.position.z,
                     matrix.m30, matrix.m31, matrix.m32, global_camera.mtx.m30, global_camera.mtx.m31,
                     global_camera.mtx.m32);
        }
    }
    const bool scripted_play_passed =
        !options.script_play || (scripted_play_finished && scripted_play_movement_observed);
    const bool capture_passed = !options.capture || (have_hash && saw_visible_frame);
    LOG_INFO("presented %d frame_count", frame_count);
    if (numain_finished) {
        const i32 result = host_numain_result.load(std::memory_order_relaxed);
        free(buffer);
        return result != 0 ? result : (scripted_play_passed && capture_passed ? 0 : 1);
    }
    return scripted_play_passed && capture_passed ? 0 : 1;
}
