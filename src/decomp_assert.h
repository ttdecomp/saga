#pragma once

// Layout assertions describe the original 32-bit binary. Host builds use the
// host ABI, while clang is used by the lint configuration without the
// original Android GCC ABI.
#if defined(__cplusplus) && !defined(HOST_BUILD) && !defined(__clang__)
#define DECOMP_ASSERT(condition, message) static_assert(condition, message)
#else
#define DECOMP_ASSERT(condition, message)
#endif
