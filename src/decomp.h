#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void __attribute__((noreturn)) __unimplemented(const char *file, int line, const char *func, const char *msg) {
    fprintf(stderr, "%s:%d: %s: UNIMPLEMENTED: %s\n", file, line, func, msg);
    exit(EXIT_FAILURE);
}

#define UNIMPLEMENTED(...) __unimplemented(__FILE__, __LINE__, __func__, #__VA_ARGS__)

#ifdef HOST_BUILD
#define LOG(...)                                                                                                       \
    do {                                                                                                               \
        fprintf(stderr, "%s:%d: %s: ", __FILENAME__, __LINE__, __func__);                                              \
        fprintf(stderr, __VA_ARGS__);                                                                                  \
        fprintf(stderr, "\n");                                                                                         \
    } while (0)
#else
#define LOG(...)
#endif

#define BUFFER_ALLOC(buffer, T) (T *)buffer_alloc_aligned((buffer), sizeof(T), _Alignof(T))
#define BUFFER_ALLOC_ARRAY(buffer, count, T) (T *)buffer_alloc_aligned((buffer), sizeof(T) * (count), _Alignof(T))

static inline void *buffer_alloc_aligned(void **buffer, size_t size, size_t align) {
    size_t current = (size_t)(*buffer);
    size_t aligned = (current + (align - 1)) & ~(align - 1);
    *buffer = (void *)(aligned + size);
    return (void *)aligned;
}

#ifdef __cplusplus
#define C_API_START extern "C" {
#define C_API_END }
#else
#define C_API_START
#define C_API_END
#endif

#ifdef __cplusplus
#define CPP_API_START
#define CPP_API_END
#else
#define CPP_API_START static void UNIQUE_FUNC(base)(void) {
#define CPP_API_END }
#endif

#define CONCAT(a, b) a##b
#define CONCAT_EXPAND(a, b) CONCAT(a, b)
#define UNIQUE_FUNC(base) CONCAT_EXPAND(base, __COUNTER__)

static inline uint64_t CONCAT44(uint32_t left, uint32_t right) {
    uint64_t result = 0;
    result |= (((uint64_t)left) << 32) & 0xffffffff00000000;
    result |= right;
    return result;
}

static inline uint32_t CARRY4(uint32_t a, uint32_t b) {
    return (a + b) < a;
}