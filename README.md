![Progress](https://img.shields.io/badge/matching-10.28%25-red)
[![Discord](https://img.shields.io/discord/1467775700894224555?color=%235865F2&logo=discord&logoColor=%23FFFFFF)](https://discord.gg/2HJuMtzA7q)

|                 | Target (Android x86)                                                                                                                                                                                  | Host                                                                                                                                                                                  |
| --------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Linux           | [![Linux target](https://github.com/ttdecomp/saga/actions/workflows/build-linux-target.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-linux-target.yaml)                   | [![Linux host](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml)         |
| Windows (MSYS2) | [![Windows target](https://github.com/ttdecomp/saga/actions/workflows/build-windows_msys2-target.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-windows_msys2-target.yaml) | [![Windows host](https://github.com/ttdecomp/saga/actions/workflows/build-windows_msys2.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-windows_msys2.yaml) |
| MacOS | [![macOS Target build](https://github.com/ttdecomp/saga/actions/workflows/build-macos-target.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-macos-target.yaml) | - |

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

The project uses CMake as its build system. Install mise, then bootstrap every
pinned tool and the Git hooks with one command:

```bash
mise run setup
```

There are three build modes:

The Android x86 build uses the NDK r8e toolchain and targets the original
Android platform. It is the matching build; its output is not a host-runnable
game executable.

A host build uses the configured host C/C++ compiler. On non-Windows systems it
builds 32-bit i686 code with AddressSanitizer and is used for testing and
development.


### Android x86 build

```bash
mise run target:build
```

### Host build

```bash
mise run host:build

# build and run
mise run host:run
```

### WebAssembly host build

Build the browser target with the pinned Emscripten SDK:

```bash
mise run wasm:build
```

Serve the result with the headers required by browser pthreads, then open
`http://localhost:8000`:

```bash
mise run wasm:serve
```

Select a locally owned Android OBB or enter a direct OBB URL in the browser. The
file is loaded into browser memory and is never copied into the source tree or
build output.

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

| Directory | Fuzzy % | Funcs % | Data % |
|---|---|---|---|
| `(root)` | 57.0% | 16.7% | 0.0% |
| `MechInputTouch` | 5.0% | 0.0% | 0.9% |
| `editor` | 2.6% | 0.0% | - |
| `gameapi` | 6.9% | 1.4% | 94.8% |
| `gameframework` | 68.5% | 11.8% | 100.0% |
| `gamelib` | 5.6% | 0.9% | 5.0% |
| `java` | 10.7% | 0.0% | 0.0% |
| `legoapi` | 7.0% | 5.1% | 31.1% |
| `legoapi/actions` | 1.7% | 0.5% | - |
| `legoapi/ai` | 2.2% | 0.0% | - |
| `legoapi/audio` | 3.0% | 0.0% | 100.0% |
| `legoapi/characters` | 5.8% | 1.0% | 36.1% |
| `legoapi/core` | 10.1% | 4.5% | 97.8% |
| `legoapi/cutscenes` | 4.5% | 0.0% | 93.8% |
| `legoapi/gizmo` | 4.9% | 4.7% | 0.4% |
| `legoapi/gizmos` | 12.2% | 3.7% | 100.0% |
| `legoapi/items` | 2.7% | 0.2% | 6.8% |
| `legoapi/menus` | 3.5% | 0.4% | 16.5% |
| `legoapi/misc` | 4.5% | 1.0% | - |
| `legoapi/props` | 17.8% | 5.5% | 59.7% |
| `legoapi/render` | 4.3% | 3.0% | 95.6% |
| `legoapi/world` | 20.1% | 28.2% | 0.1% |
| `legogame` | 6.1% | 5.3% | 81.2% |
| `nu2api` | 30.3% | 20.9% | 9.3% |

<!-- matching-table-end -->
