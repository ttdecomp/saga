#pragma once

#include <stddef.h>

#include "decomp.h"

#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

// In the original engine, `variptr_u` is a union of a wide variety of pointer
// types from various parts of the engine. This allows for convenient casting of
// buffer memory to the desired resulting allocated type, but we choose to omit
// it, as it is cumbersome and matching results can be achieved without the full
// union.
typedef union variptr_u {
    void *void_ptr;
    char *char_ptr;
    i16 *short_ptr;
    usize addr;
} VARIPTR;

#define ALIGN(value, alignment) (((value) + alignment - 1) & -alignment)
