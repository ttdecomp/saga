# 01 — Toolchain reference

The matching target is built with the Android NDK r8e x86 GCC 4.7 toolchain.
The Bazel entry point is:

```bash
bazel build --config=target //src:saga_target
```

The result is `bazel-bin/src/libTTapp.so`, a 32-bit Android x86 ELF shared
object. `bazel/android_cc_toolchain_config.bzl` and `src/BUILD.bazel` are the
authoritative toolchain and target definitions.

## Compiler and linker

- C: `i686-linux-android-gcc` 4.7 with `-std=gnu11`.
- C++: `i686-linux-android-g++` 4.7 with `-std=gnu++11`.
- Sysroot: Android API 9, x86.
- Linker: NDK r8e gold 1.11.
- C++ headers: the NDK system STL for game sources; Squish uses the NDK GNU
  libstdc++ 4.7 headers required by its original build setup.

The external NDK archive is selected by host OS and checksum-pinned in
`bazel/android_ndk_repository.bzl`. Set `SAGA_NDK_R8E` to an absolute existing
checkout to avoid the download.

## Matching flags

All target C/C++ actions keep these important options:

- `-fno-exceptions`
- `-fno-rtti`
- `-Wno-write-strings`
- `-fno-function-sections`
- `-fno-data-sections`
- `ANDROID`
- `__FILENAME__=__FILE__`

No explicit `-O` means `-O0`. Debug information does not change machine code
and is retained by Bazel's debug compilation mode.

Inspect the exact command line rather than copying a generated path:

```bash
bazel aquery --config=target \
  'mnemonic("CppCompile", //src:saga_target)' --include_commandline
```

## Optimization levels

Per-source overrides are stored in
`bazel/android_per_file_copts.bazelrc`. Each listed source has exactly one
`-O1`, `-O2`, or `-O3`; `src/legoapi/core/config/cheat.cpp` also uses `-fPIE`.
Unlisted sources compile at `-O0`.

```bash
rg -n -- '--per_file_copt' bazel/android_per_file_copts.bazelrc
bazel test //scripts:check_bazel_optimization_map
```

Moving a function between files can therefore change its code generation even
when the source text is unchanged.

## Target defaults

Measured NDK r8e defaults relevant to reverse engineering:

| property | behavior |
|---|---|
| architecture | i686 Android x86 |
| SSE | SSE, SSE2, and SSE3 enabled by the driver |
| floating-point math | SSE math is the normal path; x87 still appears in some conversions |
| PIC | enabled by the Android driver; `__PIC__` is 2 |
| PIE | disabled except for the explicit `cheat.cpp` override |
| stack protector | toolchain default unless a source/compiler rule changes it |

Do not add modern compiler flags because they appear harmless. GCC 4.7 option
defaults and instruction selection are part of the matching contract.

## `__FILENAME__`

`src/BUILD.bazel` defines `__FILENAME__=__FILE__` locally for source targets.
`src/decomp.h` uses it only inside `HOST_BUILD` diagnostics, so it does not
alter target behavior.

## C++ runtime limitations

The NDK system STL has only a small set of headers such as C wrappers, `new`,
`typeinfo`, `utility`, and `stl_pair.h`. It does not provide normal modern STL
containers or iostreams. Matching code must use the game's own string,
container, and pointer types.

The system `libstdc++.a` provides a small runtime surface including allocation,
guard variables, pure-virtual handling, and basic `type_info` operations. Avoid
introducing language features that pull in a different runtime surface.

## libgcc helpers

GCC can emit helpers for 64-bit division and remainder:

- `__udivdi3`
- `__umoddi3`
- `__divdi3`
- `__moddi3`

Most 64-bit multiply, shift, compare, and conversion operations remain inline.
The original binary embeds some unsigned helpers; treat them as compiler/runtime
code rather than hand-authored game functions.

## Linking and dependencies

The target is linked as `libTTapp.so` with GLESv2, EGL, OpenSL ES, and the NDK
system C++ runtime. Bazel checksum-pins and builds these source dependencies:

- Ogg 1.2.1
- Vorbis 1.3.2
- Squish 1.10

The source archives and overlay BUILD files live in `MODULE.bazel` and
`third_party/`. The target remains a shared object and deliberately has no
`main`.

## Native differences

```bash
bazel build --config=native //src:saga_native

# Windows, from an MSYS2 MINGW64 shell
bazel build --config=native --config=windows-mingw //src:saga_native
```

Native builds use the platform compiler and system SDL3, Vorbis, EGL/GLES2 (or
ANGLE), plus the hermetic Squish source archive. They define both `HOST_BUILD`
and `ANDROID` and include `src/host/harness/` and `src/host/platform/`.

- Linux remains i686 with `-m32 -march=i686 -msse2`, ASan, and UBSan.
- Windows uses MINGW64 and exports executable symbols.
- macOS remains native Apple Silicon/ARM64.
- Native builds do not inherit the target's per-file optimization map.

The native executable is a diagnostic environment, not a matching artifact.
