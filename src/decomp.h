#pragma once

#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SAGA_NOMATCH __attribute__((section(".text.nomatch")))
#define UNIMPLEMENTED(...)                                                                                             \
    ({                                                                                                                 \
        fprintf(stderr, "%s:%d: %s: UNIMPLEMENTED: %s\n", __FILE__, __LINE__, __func__, #__VA_ARGS__);                 \
        exit(EXIT_FAILURE);                                                                                            \
    })

#ifdef HOST_BUILD

enum log_level {
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};

static const char *log_level_names[] = {
    [LOG_LEVEL_WARN] = "WARN",
    [LOG_LEVEL_INFO] = "INFO",
    [LOG_LEVEL_DEBUG] = "DEBUG",
};
static const char *log_level_colors[] = {
    [LOG_LEVEL_WARN] = "\x1b[33m",  // Yellow
    [LOG_LEVEL_INFO] = "\x1b[32m",  // Green
    [LOG_LEVEL_DEBUG] = "\x1b[90m", // Bright Black (Gray)
};
static const char *reset = "\x1b[0m";

#include <stdarg.h>

static const enum log_level LOG_LEVEL = LOG_LEVEL_DEBUG;

static void _saga_log(enum log_level level, const char *file, int line, const char *func, const char *fmt, ...) {
    if (level > LOG_LEVEL) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s[%s] %s:%d: %s: ", log_level_colors[level], log_level_names[level], file, line, func);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "%s\n", reset);
    va_end(args);
}

#define LOG(level, ...) _saga_log(level, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...) LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)

#else

#define LOG(_, ...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_DEBUG(...)

#endif

#define BUFFER_ALLOC(buffer, T) (T *)buffer_alloc_aligned((buffer), sizeof(T), alignof(T))
#define BUFFER_ALLOC_ARRAY(buffer, count, T) (T *)buffer_alloc_aligned((buffer), sizeof(T) * (count), alignof(T))

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
