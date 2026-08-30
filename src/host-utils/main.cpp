#include "audio.hpp"
#include "load.hpp"
#include "window.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

extern char g_language[16];

// HOST-ONLY: the ASAN runtime inside the host toolchain flags the allocator
// mismatch coming out of Mesa/LLVM's GL driver stack (new/delete vs their
// internal allocators) and aborts the process before a utility can run.
// The original binary has no ASAN; this only silences that false positive.
extern "C" const char *__asan_default_options() {
    return "alloc_dealloc_mismatch=0";
}

namespace {

    enum class HostUtility {
        audio,
        load,
        window,
    };

    struct HostHarnessOptions {
        HostUtility utility = HostUtility::window;
        HostLoadOptions load;
        HostWindowOptions window;
    };

    void host_print_usage(const char *program) {
        printf("Usage: %s <utility> [options]\n", program);
        printf("\nHost utilities:\n");
        printf("  audio                  Verify audio playback through the host device\n");
        printf("  load [list|extract]    Inspect or extract game data\n");
        printf("  window [options]       Run the game in an SDL window\n");
        printf("\nRun '%s <utility> --help' for utility-specific options.\n", program);
    }

    void host_print_audio_usage(const char *program) {
        printf("Usage: %s audio\n", program);
    }

    void host_print_load_usage(const char *program) {
        printf("Usage:\n");
        printf("  %s load\n", program);
        printf("  %s load list [filter]\n", program);
        printf("  %s load extract <dat-path> [output]\n", program);
    }

    void host_print_window_usage(const char *program) {
        printf("Usage: %s window [options]\n", program);
        printf("\nOptions:\n");
        printf("  --capture              Capture changed frames under .work/capture\n");
        printf("  --script-input         Exercise the new-game menu flow\n");
        printf("  --script-load          Exercise the load-game menu flow\n");
        printf("  --script-play          Exercise the new-game flow and player movement\n");
        printf("  --offscreen            Create a hidden, non-focusable window\n");
        printf("  --mute                 Use SDL's dummy audio driver\n");
        printf("  --script-tail-ms <ms>  Wait after scripted input completes (default: 8000)\n");
        printf("  --timeout-ms <ms>      Stop the window utility after this time (default: 90000)\n");
    }

    bool host_parse_milliseconds(const char *option, const char *value, Uint64 &result) {
        errno = 0;
        char *end = nullptr;
        const u64 parsed = strtoull(value, &end, 10);
        if (*value == '-' || errno != 0 || end == value || *end != '\0') {
            fprintf(stderr, "Invalid value for %s: %s\n", option, value);
            return false;
        }
        result = static_cast<Uint64>(parsed);
        return true;
    }

    bool host_parse_audio_arguments(i32 argc, char **argv, const char *program) {
        if (argc == 0) {
            return true;
        }
        fprintf(stderr, "Unexpected argument for audio: %s\n", argv[0]);
        host_print_audio_usage(program);
        return false;
    }

    bool host_parse_load_arguments(i32 argc, char **argv, const char *program, HostLoadOptions &options) {
        if (argc == 0) {
            return true;
        }
        if (strcmp(argv[0], "list") == 0) {
            if (argc > 2) {
                fprintf(stderr, "Too many arguments for load list\n");
                host_print_load_usage(program);
                return false;
            }
            options.action = HostLoadAction::list;
            options.filter = argc == 2 ? argv[1] : nullptr;
            return true;
        }
        if (strcmp(argv[0], "extract") == 0) {
            if (argc < 2 || argc > 3) {
                fprintf(stderr, "load extract requires a DAT path and accepts one optional output path\n");
                host_print_load_usage(program);
                return false;
            }
            options.action = HostLoadAction::extract;
            options.dat_path = argv[1];
            options.output_path = argc == 3 ? argv[2] : ".work/extracted.bin";
            return true;
        }
        fprintf(stderr, "Unknown load action: %s\n", argv[0]);
        host_print_load_usage(program);
        return false;
    }

    bool host_parse_window_arguments(i32 argc, char **argv, const char *program, HostWindowOptions &options) {
        for (i32 i = 0; i < argc; ++i) {
            const char *argument = argv[i];
            if (strcmp(argument, "--capture") == 0) {
                options.capture = true;
            } else if (strcmp(argument, "--script-input") == 0) {
                options.script_input = true;
            } else if (strcmp(argument, "--script-load") == 0) {
                options.script_input = true;
                options.script_load = true;
            } else if (strcmp(argument, "--script-play") == 0) {
                options.script_input = true;
                options.script_play = true;
            } else if (strcmp(argument, "--offscreen") == 0) {
                options.offscreen = true;
            } else if (strcmp(argument, "--mute") == 0) {
                options.mute = true;
            } else if (strcmp(argument, "--script-tail-ms") == 0 || strcmp(argument, "--timeout-ms") == 0) {
                if (++i == argc) {
                    fprintf(stderr, "Missing value for %s\n", argument);
                    host_print_window_usage(program);
                    return false;
                }
                Uint64 &destination =
                    strcmp(argument, "--script-tail-ms") == 0 ? options.script_tail_ms : options.timeout_ms;
                if (!host_parse_milliseconds(argument, argv[i], destination)) {
                    host_print_window_usage(program);
                    return false;
                }
            } else {
                fprintf(stderr, "Unknown window option: %s\n", argument);
                host_print_window_usage(program);
                return false;
            }
        }
        return true;
    }

    enum class HostParseResult {
        run,
        help,
        error,
    };

    HostParseResult host_parse_arguments(i32 argc, char **argv, HostHarnessOptions &options) {
        const char *program = argv[0];
        if (argc < 2) {
            host_print_usage(program);
            return HostParseResult::error;
        }
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            host_print_usage(program);
            return HostParseResult::help;
        }

        const i32 utility_argc = argc - 2;
        char **utility_argv = argv + 2;
        if (strcmp(argv[1], "audio") == 0) {
            options.utility = HostUtility::audio;
            if (utility_argc == 1 && strcmp(utility_argv[0], "--help") == 0) {
                host_print_audio_usage(program);
                return HostParseResult::help;
            }
            return host_parse_audio_arguments(utility_argc, utility_argv, program) ? HostParseResult::run
                                                                                   : HostParseResult::error;
        }
        if (strcmp(argv[1], "load") == 0) {
            options.utility = HostUtility::load;
            if (utility_argc == 1 && strcmp(utility_argv[0], "--help") == 0) {
                host_print_load_usage(program);
                return HostParseResult::help;
            }
            return host_parse_load_arguments(utility_argc, utility_argv, program, options.load)
                       ? HostParseResult::run
                       : HostParseResult::error;
        }
        if (strcmp(argv[1], "window") == 0) {
            options.utility = HostUtility::window;
            if (utility_argc == 1 && strcmp(utility_argv[0], "--help") == 0) {
                host_print_window_usage(program);
                return HostParseResult::help;
            }
            return host_parse_window_arguments(utility_argc, utility_argv, program, options.window)
                       ? HostParseResult::run
                       : HostParseResult::error;
        }

        fprintf(stderr, "Unknown host utility: %s\n", argv[1]);
        host_print_usage(program);
        return HostParseResult::error;
    }

    void host_initialize_language() {
        // On device Java fills the engine locale string via nativeSetLanguage
        // before NuMain runs; emulate that once for every host utility.
        const char *lang = getenv("LANG");
        if (lang == nullptr) {
            lang = "en-us";
        }
        snprintf(g_language, sizeof(g_language), "%.15s", lang);
        for (char *character = g_language; *character != '\0'; ++character) {
            if (*character == '_') {
                *character = '-';
            }
        }
    }

    [[noreturn]] void host_finish(i32 status) {
        // Engine globals contain pointers that are deliberately not owned by
        // their static containers. Avoid their destructor pass for every host
        // utility, including help and argument errors.
        fflush(nullptr);
        _exit(status);
    }

} // namespace

i32 main(i32 argc, char **argv) {
    HostHarnessOptions options;
    const HostParseResult result = host_parse_arguments(argc, argv, options);
    if (result != HostParseResult::run) {
        host_finish(result == HostParseResult::help ? 0 : 1);
    }

    host_initialize_language();
    i32 utility_result = 1;
    switch (options.utility) {
        case HostUtility::audio:
            utility_result = host_run_audio();
            break;
        case HostUtility::load:
            utility_result = host_run_load(options.load);
            break;
        case HostUtility::window:
            utility_result = host_run_window(options.window);
            break;
    }

    host_finish(utility_result);
}
