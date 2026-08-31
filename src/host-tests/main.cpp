#include "test_audio.hpp"
#include "test_load.hpp"
#include "test_window.hpp"

#include <string>
#include <stdlib.h>
#include <stdio.h>

// HOST-ONLY: the ASAN runtime inside the host toolchain flags the allocator
// mismatch coming out of Mesa/LLVM's GL driver stack (new/delete vs their
// internal allocators) and aborts the process before the test can present.
// The original binary has no ASAN; this only silences that false positive.
extern "C" const char *__asan_default_options() {
    return "alloc_dealloc_mismatch=0";
}

struct Test {
    std::string name;
    i32 (*func)(i32 argc, char **argv);
};

const Test tests[] = {
    {"load", test_load},
    {"window", test_window},
    {"audio", test_audio},
};

i32 main(i32 argc, char **argv) {
    // HOST-ONLY: on device Java fills the engine locale string via
    // nativeSetLanguage before NuMain runs; emulate that from the environment.
    {
        const char *lang = getenv("LANG");
        if (lang == NULL)
            lang = "en-us";
        extern char g_language[16];
        snprintf(g_language, sizeof(g_language), "%.15s", lang);
        for (char *c = g_language; *c; c++) {
            if (*c == '_')
                *c = '-';
        }
    }

#ifdef __EMSCRIPTEN__
    char *wasm_argv[] = {argv[0], const_cast<char *>("window"), nullptr};
    if (argc < 2) {
        argc = 2;
        argv = wasm_argv;
    }
#endif

    if (argc < 2) {
        printf("Usage: %s <test>\n", argv[0]);
        printf("Available tests:\n");
        for (const auto &test : tests) {
            printf("  %s\n", test.name.c_str());
        }

        return 1;
    }

    for (const auto &test : tests) {
        if (test.name == argv[1]) {
            return test.func(argc - 1, argv + 1);
        }
    }

    printf("Unknown test: %s\n", argv[1]);
    return 1;
}
