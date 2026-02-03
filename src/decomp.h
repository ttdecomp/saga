#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nu2api.saga/nucore/common.h"

#ifndef __x86_64__
#ifdef __cplusplus
static_assert(sizeof(void *) == 4, "Unsupported pointer size");
#endif
#endif

// Define undefined* types from ghidra
typedef u8 undefined;
typedef u8 byte;
typedef u8 dwfenc;
typedef u32 dword;
typedef u64 longlong;
typedef u8 uchar;
typedef u64 ulonglong;
typedef u8 undefined1;
typedef u16 undefined2;
typedef u32 undefined3;
typedef u32 undefined4;
typedef u64 undefined5;
typedef u64 undefined6;
typedef u64 undefined7;
typedef u64 undefined8;
typedef u16 ushort;
typedef u16 word;

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

#define BUFFER_ALLOC(buffer, T, align) (T *)buffer_alloc_aligned((buffer), sizeof(T), (align))
#define BUFFER_ALLOC_ARRAY(buffer, count, T, align) (T *)buffer_alloc_aligned((buffer), sizeof(T) * (count), (align))
#define BUFFER_ALLOC_ARRAY_ALIGNED(buffer, count, T) BUFFER_ALLOC_ARRAY((buffer), (count), T, alignof(T))

static inline void *buffer_alloc_aligned(void **buffer, usize size, usize align) {
    usize current = (isize)(*buffer);
    usize aligned = (current + (align - 1)) & ~(align - 1);
    *buffer = (void *)(aligned + size);
    return (void *)aligned;
}
