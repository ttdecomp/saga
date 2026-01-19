#pragma once

#include <format>
#include <iostream>

#define CHECK(condition, fmt, ...) __check_impl((condition), (fmt), #condition, __VA_ARGS__)

template <typename... Args>
static void __check_impl(bool condition, const char *condition_str, std::format_string<Args...> fmt, Args &&...args) {
    std::string RESET{"\33[0m"};
    std::string RED{"\33[31m"};
    std::string GREEN{"\33[32m"};
    std::string BOLD{"\33[1m"};

    if (!condition) {

        std::string buffer;
        std::format_to(std::back_inserter(buffer), "{}CHECK({}) failed:", BOLD + RED, condition_str);
        std::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
        std::format_to(std::back_inserter(buffer), "{}{} at {}:{} in {}{}\n", RESET, RED, __FILENAME__, __LINE__,
                       __func__, RESET);
        std::cerr << buffer;

        std::exit(EXIT_FAILURE);
    } else {
        std::string buffer;
        std::format_to(std::back_inserter(buffer), "{}CHECK({}) passed.{}", BOLD + GREEN, condition_str, RESET);
        std::cout << buffer << std::endl;
    }
}