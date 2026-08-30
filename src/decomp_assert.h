#pragma once

// Layout checks describe the original 32-bit binary. Host builds use the host
// ABI, so pointer-bearing structures legitimately have different sizes and
// offsets there.
#if defined(__cplusplus) && !defined(HOST_BUILD) && !defined(__clang__)
#define DECOMP_ASSERT(condition, message) static_assert(condition, message)
#else
#define DECOMP_ASSERT(condition, message)
#endif
