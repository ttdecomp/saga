#include "test_load.hpp"
#include "test_window.hpp"

#include <string>

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
};

i32 main(i32 argc, char **argv) {
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
