#pragma once

#include <stdio.h>
#include <stdlib.h>
#define UNIMPLEMENTED(...)                                                                                             \
    do {                                                                                                               \
        fprintf(stderr, "%s:%d: %s: UNIMPLEMENTED", __FILE__, __LINE__, __func__);                                     \
        fprintf(stderr, ": " __VA_ARGS__ "\n");                                                                        \
        exit(EXIT_FAILURE);                                                                                            \
    } while (0)