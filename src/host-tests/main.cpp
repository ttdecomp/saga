#include "test_load.hpp"
#include "test_window.hpp"

#include <string>

struct Test {
    std::string name;
    int (*func)(int argc, char **argv);
};

const Test tests[] = {
    {"load", test_load},
    {"window", test_window},
};

int main(int argc, char **argv) {
    if (argc != 2) {
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

    return 1;
}
