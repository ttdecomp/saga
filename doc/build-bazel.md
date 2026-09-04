# Bazel builds

Bazel is SAGA's build system. Version 9.2.0 is pinned in `.bazelversion`, so
Bazelisk is the recommended launcher.

## Build matrix

| Build | Command | Output | Supported build hosts |
|---|---|---|---|
| Matching Android x86 | `bazel build --config=android-x86 //src:saga_android_x86` | `bazel-bin/src/libTTapp.so` | Linux x86-64, Windows x86-64, macOS Intel |
| Native host | `bazel build --config=host //src:saga_host` | `bazel-bin/src/saga_host` (`.exe` on Windows) | Linux, Windows, macOS 15 Intel and Apple Silicon |
| WebAssembly host | `bazel build --config=wasm //src:saga_wasm` | `bazel-bin/src/saga.html`, `.js`, and `.wasm` | Any host supported by emsdk 6.0.8 |

The legacy Android NDK r8e contains x86-64 host tools only. On Apple Silicon,
run the Android build under an Intel/Rosetta environment or use one of the
other listed hosts. This restriction does not apply to the native macOS build.

The Android output is a 32-bit x86 ELF shared object named `libTTapp.so`; it
does not define `main`. Native host and WebAssembly outputs use the host harness
and are executable applications.

## Prerequisites

The Android build fetches NDK r8e automatically. To use an existing checkout,
set `SAGA_NDK_R8E` to its absolute path. Ogg 1.2.1, Vorbis 1.3.2, and Squish
1.10 are fetched and built hermetically for this target.

The native host uses pkg-config for SDL3, Vorbis/Vorbisfile, and EGL/GLES2
(ANGLE on Windows). Windows uses the MSYS2 MINGW64 environment and Bazel's
`mingw-gcc` toolchain. Squish 1.10 is built hermetically. Install 32-bit
variants of the Linux libraries because the Linux host intentionally remains
i686. CI uses an Ubuntu 26.04 container, whose repositories provide SDL3 for
i386.

The WebAssembly build is hermetic after repository download. It uses emsdk
6.0.8, SDL 3.4.10, Ogg 1.3.6, Vorbis 1.3.7, and Squish 1.10.

## Preserved build behavior

- `--config=host` and `--config=wasm` define both `HOST_BUILD` and `ANDROID`.
- All modes use C11/C++11, disable C++ exceptions and RTTI, and retain the
  existing writable-string warning policy.
- The matching target uses NDK r8e GCC 4.7, Android API 9, x86, the system STL
  for SAGA, GNU libstdc++ headers for Squish, and disabled function/data
  sections.
- Linux host keeps `-m32 -march=i686 -msse2`, ASan, and UBSan. The Windows
  host retains exported executable symbols. Windows and Intel macOS keep
  SSE2, while Apple Silicon remains native. Android builds driven from
  Windows retain the former static GCC/libstdc++ runtime flags.
- Android per-file `-O1`, `-O2`, `-O3`, and `-fPIE` assignments live in
  `bazel/android_per_file_copts.bazelrc`.

Run the repository checks after changing source membership or optimization
settings:

```bash
bazel test //scripts:checks
```

## Convenience tasks

```bash
mise run bazel:android
mise run bazel:host
mise run bazel:wasm
mise run host:run -- window
mise run wasm:serve
```

The WebAssembly server exposes `http://127.0.0.1:8000/` with the cross-origin
isolation headers required by pthread-enabled WebAssembly.

## Continuous integration

`.github/workflows/build-bazel.yaml` keeps all build variants in one workflow.
The shared checks gate the platform jobs, while each job has its own cache key.
This also gives the README one workflow-level build-matrix badge.
