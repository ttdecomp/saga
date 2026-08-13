#pragma once

// Fixed-width typedefs for the generated *_types.h scaffolding headers.
// Unlike common.h, this header does NOT declare the engine's `variptr_u`
// union, so including it alongside the scaffolding struct definitions does not
// cause a tag-type clash. Duplicate typedefs naming the same underlying type
// are legal in C++11+, so this also coexists with common.h in a TU.
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// ABI-mangling-compatible `long`/`unsigned long` for the 32-bit target.
// `long` mangles as `_l` and `unsigned long` as `_m` in the Itanium C++ ABI,
// and the reconstructed ABI stubs (ogg/vorbis, legoapi) genuinely used these
// in the original binary, so those signatures must keep that mangling to match
// res/libTTapp.so. Kept as typedefs so google-runtime-int stays quiet (the raw
// `long`/`unsigned long` keywords would be flagged). On this 32-bit target
// `long` is 32 bits, so `abi_long` is same-width/same-sign as `i32`.
typedef long abi_long;          // NOLINT
typedef unsigned long abi_ulong; // NOLINT
