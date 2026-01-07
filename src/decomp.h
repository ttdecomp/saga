#pragma once

#include <stdio.h>
#include <stdlib.h>
#define UNIMPLEMENTED(...)                                                                                             \
    do {                                                                                                               \
        fprintf(stderr, "%s:%d: %s: UNIMPLEMENTED", __FILE__, __LINE__, __func__);                                     \
        fprintf(stderr, ": " __VA_ARGS__ "\n");                                                                        \
        exit(EXIT_FAILURE);                                                                                            \
    } while (0)

#define BUFFER_ALLOC(buffer, T) (T *)buffer_alloc((void **)(buffer), sizeof(T), _Alignof(T))

static inline void *buffer_alloc(void **buffer, size_t size, size_t align) {
    size_t current = (size_t)(*buffer);
    size_t aligned = (current + (align - 1)) & ~(align - 1);
    *buffer = (void *)(aligned + size);
    return (void *)aligned;
}