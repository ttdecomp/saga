#pragma once

#include <stddef.h>
#include <stdint.h>

// Define fixed-width types for convenience.
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// This is a bit of a hack. It allows us to represent parameters representing
// sizes in a way that's a bit more future-proof while retaining compatibility
// with mangled symbol names, signedness, and width in the NDK-based build. Why
// not just use `size_t` and `ssize_t`? Because `ssize_t` is `long` and so
// mangled symbol names come out wrong.
typedef size_t usize;
#ifdef HOST_BUILD
#include <stdlib.h>
typedef ssize_t isize;
#else
typedef int32_t isize;
#endif

// Ensure that floating-point types are of the expected width.
#ifdef HOST_BUILD
typedef _Float32 f32;
typedef _Float64 f64;
#else
typedef float f32;
typedef double f64;
#endif

#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

#define ALIGN(value, alignment) (((value) + alignment - 1) & -alignment)

// In the original engine, `variptr_u` is a union of a wide variety of pointer
// types from various parts of the engine. This allows for convenient casting of
// buffer memory to the desired resulting allocated type, but we choose to omit
// it, as it is cumbersome and matching results can be achieved without the full
// union.
typedef union variptr_u {
    void *void_ptr;
    char *char_ptr;
    i16 *short_ptr;
    u8* uchar_ptr;
    usize addr;
} VARIPTR;
