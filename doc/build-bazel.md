# Bazel builds

Bazel is SAGA's build system. Version 9.2.0 is pinned in `.bazelversion`, so
Bazelisk is the recommended launcher. Python targets run on Python 3.12.12,
which `rules_python` downloads and isolates from system site-packages; they do
not need a virtual environment or separately installed packages.

## Build matrix

| Build | Command | Output | Supported build hosts |
|---|---|---|---|
| Target (Android x86) | `bazel build --config=target //src:saga_target` | `bazel-bin/src/libTTapp.so` | Linux x86-64, Windows x86-64, macOS Intel |
| Native | `bazel build --config=native //src:saga_native` | `bazel-bin/src/saga_native` | Linux |
| Native (Windows) | `bazel build --config=native --config=windows-mingw //src:saga_native` | `bazel-bin/src/saga_native.exe` | Windows MINGW64 |
| WASM | `bazel build --config=wasm //src:saga_wasm` | `bazel-bin/src/saga.html`, `.js`, and `.wasm` | Linux, Windows, macOS |

The legacy Android NDK r8e contains x86-64 host tools only. On Apple Silicon,
run the Android build under an Intel/Rosetta environment or use one of the
other listed hosts.

The target output is a 32-bit Android x86 ELF shared object named
`libTTapp.so`; it does not define `main`. The native output is an executable,
while WASM is a browser bundle. Both use the host harness. Linux and Windows
are the supported and CI-tested native hosts; macOS-specific native rules are
present but are not currently part of the supported build matrix.

## Prerequisites

The Android build fetches NDK r8e automatically. To use an existing checkout,
set `SAGA_NDK_R8E` to its absolute path. Ogg 1.2.1, Vorbis 1.3.2, and Squish
1.10 are fetched and built hermetically for this target.

The native build uses pkg-config for SDL3, Vorbis/Vorbisfile, and EGL/GLES2
(ANGLE on Windows). Windows uses the MSYS2 MINGW64 environment and Bazel's
MinGW toolchain; `--config=windows-mingw` selects it instead of the MSVC
toolchain Bazel registers by default. Squish 1.10 is built hermetically.
Install 32-bit variants of the Linux libraries because the Linux host
intentionally remains i686. CI uses an Ubuntu 26.04 container, whose
repositories provide SDL3 for i386.

The WebAssembly build is hermetic after repository download. It uses emsdk
6.0.8, SDL 3.4.10, Ogg 1.3.6, Vorbis 1.3.7, and Squish 1.10.

## Python environment

`MODULE.bazel` registers Python 3.12.12 with `rules_python`. Bazel downloads
that interpreter for declared `py_binary` and `py_test` targets, and those
targets do not see arbitrary system site-packages. Any third-party Python
dependency must therefore be declared in Bazel; the current targets use only
the standard library.

Run those programs through their Bazel labels, for example
`bazel run //scripts:wasm_server` or `bazel test //scripts/checks:checks`. Bazel does
not create a general-purpose project venv, so invoking
`python3 scripts/tool.py` bypasses the pinned runtime. On POSIX systems, the
generated Bazel launcher also currently needs a `python3` command to bootstrap
before it switches to the downloaded interpreter, but it does not require any
system Python packages.

## Preserved build behavior

- `--config=native` and `--config=wasm` define both `HOST_BUILD` and `ANDROID`.
- All modes use C11/C++11, disable C++ exceptions and RTTI, and retain the
  existing writable-string warning policy.
- The matching target uses NDK r8e GCC 4.7, Android API 9, x86, the system STL
  for SAGA, GNU libstdc++ headers for Squish, and disabled function/data
  sections.
- Linux host keeps `-m32 -march=i686 -msse2`, ASan, and UBSan. The Windows
  host retains exported executable symbols and SSE2. Android builds driven
  from Windows retain the former static GCC/libstdc++ runtime flags.
- Android per-file `-O1`, `-O2`, `-O3`, and `-fPIE` assignments live in
  `bazel/android_per_file_copts.bazelrc`.

Run the repository checks after changing source membership or optimization
settings:

```bash
bazel test //scripts/checks:checks
```

## Running tools

```bash
# build and run the native executable from the current directory
bazel run --config=native //src:run_native -- window

# Windows, from an MSYS2 MINGW64 shell
bazel run --config=native --config=windows-mingw //src:run_native -- window

# build and serve the WASM output with the required isolation headers
bazel run --config=wasm //scripts:wasm_server
```

The `//src:run_native` wrapper changes back to the directory where Bazel was
invoked before starting `saga_native`; arguments after `--` are forwarded
unchanged. The direct Windows output remains `bazel-bin/src/saga_native.exe`.
Python programs exposed as Bazel targets use the pinned Python runtime. Run a
per-symbol comparison with `bazel run //scripts:objdiff_cli -- SYMBOL`; it also
requires the external `objdiff-cli` executable on `PATH`.

The WebAssembly server exposes `http://127.0.0.1:8000/` with the cross-origin
isolation headers required by pthread-enabled WebAssembly. If
`res/main.1060.com.wb.lego.tcs.obb` exists, it is streamed to the page and
loaded automatically. Pass `-- --obb /path/to/file.obb` to override that path;
otherwise the page falls back to its local-file and remote-URL controls.

## Continuous integration

`.github/workflows/build-bazel.yaml` keeps all build variants in one workflow.
The shared checks gate the platform jobs, while each job has its own cache key.
WASM is verified on Linux, Windows, and macOS. This also gives the README one
workflow-level build-matrix badge. The separate `plot-pages.yaml` workflow only
publishes the already generated static files in `doc/pages/` to GitHub Pages.
