![Progress](https://img.shields.io/badge/matching-15.03%25-red)
[![Discord](https://img.shields.io/discord/1467775700894224555?color=%235865F2&logo=discord&logoColor=%23FFFFFF)](https://discord.gg/2HJuMtzA7q)

[![Bazel build matrix](https://github.com/ttdecomp/saga/actions/workflows/build-bazel.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-bazel.yaml)

# _saga_

This repository contains decompiled source code for the game _LEGO Star Wars:
The Complete Saga_, based on the Android x86 build of the game.

This build was chosen due to the presence of mangled symbol names, minimal
function inlining, and the ready availability of a matching compiler. Core
systems and game logic are believed to be identical.

## Disclaimer

This project is for educational and research purposes only. The decompiled code
is provided "as is" without any warranties. The original game is the
intellectual property of its respective owners. This repository does not contain
any game assets, media, original source code, or any other copyrighted material.

## Build Instructions

The project uses Bazel for the matching `target`, `native` Linux/Windows
builds, and `wasm`. Bazel 9.2.0 is pinned through
`.bazelversion`; Bazelisk is the recommended launcher.

The `target` build uses the Android x86 NDK r8e toolchain and matches the
original Android platform. Its output is not a host-runnable game executable.

The `native` build uses the configured host C/C++ compiler. Linux builds
32-bit i686 code with AddressSanitizer and UndefinedBehaviorSanitizer; Windows
uses MinGW64.

See [the Bazel build guide](doc/build-bazel.md) for prerequisites, platform
details, dependency versions, and build-mode behavior.

```bash
# matching target (Android x86; NDK r8e is fetched automatically)
bazel build --config=target //src:saga_target

# build and run the native executable from the current directory
bazel run --config=native //src:run_native -- window

# native executable on Windows (from an MSYS2 MINGW64 shell)
bazel run --config=native --config=windows-mingw //src:run_native -- window

# WebAssembly host
bazel build --config=wasm //src:saga_wasm

# build and serve WebAssembly at http://127.0.0.1:8000/
bazel run --config=wasm //scripts:wasm_server
```

The WebAssembly server automatically exposes
`res/main.1060.com.wb.lego.tcs.obb` to the page when it is present. Use
`-- --obb /path/to/file.obb` to serve a different copy; without one, the page
keeps its local-file and remote-URL controls available.

The target produces the shared object `bazel-bin/src/libTTapp.so`; the native
build produces `saga_native` (or `saga_native.exe` on Windows), and the
WebAssembly build produces the `saga.html`, `saga.js`, and `saga.wasm` browser
bundle under `bazel-bin/src/`.

To run the native build:

```bash
# run the game
bazel run --config=native //src:run_native -- window

# enter the Cantina and rotate the camera through 360 degrees in 10 seconds
bazel run --config=native //src:run_native -- window --camera-orbit

# enter the Cantina with a free camera (numpad 8/5/4/6; hold Shift to move)
bazel run --config=native //src:run_native -- window --camera-free
```

`//src:run_native` restores the directory where `bazel run` was invoked before
starting the executable, so relative asset and output paths behave normally.

## Contributing

Contributions are welcome! If you find any issues or have suggestions for
improvements, please open an issue or submit a pull request.

However, please note that any contributions which include original game assets
or source code will be rejected and expunged for the sake of the project.

See [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

### Documentation

Documentation and explanations for various systems and components will be added
over time. Contributions to the documentation are highly appreciated.

See the [documentation index](doc/main.md).

## References

- The Android app can be run with the [Waydroid emulator](https://waydro.id/).
- The matching toolchain is [Android NDK r8e](https://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2).

<!-- matching-table-start -->

## Matching progress

| Directory | Fuzzy % | Funcs % |
|---|---:|---:|
| `(root)` | 61.8% | 0.0% |
| `MechInputTouch` | 5.8% | 2.0% |
| `editor` | 3.1% | 1.7% |
| `gameapi` | 8.3% | 1.5% |
| `gameframework` | 84.0% | 5.9% |
| `gamelib` | 6.7% | 4.3% |
| `java` | 10.5% | 0.0% |
| `legoapi` | 14.0% | 7.0% |
| `legoapi/actions` | 6.3% | 1.4% |
| `legoapi/ai` | 12.8% | 0.0% |
| `legoapi/audio` | 6.6% | 5.0% |
| `legoapi/characters` | 11.0% | 3.9% |
| `legoapi/core` | 24.6% | 6.7% |
| `legoapi/cutscenes` | 11.0% | 2.3% |
| `legoapi/gizmo` | 10.7% | 3.0% |
| `legoapi/gizmos` | 35.6% | 26.6% |
| `legoapi/items` | 8.3% | 4.0% |
| `legoapi/menus` | 11.9% | 5.7% |
| `legoapi/misc` | 8.0% | 3.6% |
| `legoapi/props` | 28.2% | 2.4% |
| `legoapi/render` | 10.2% | 6.2% |
| `legoapi/world` | 22.7% | 6.4% |
| `legogame` | 50.5% | 0.0% |
| `nu2api` | 33.0% | 14.8% |

<!-- matching-table-end -->
