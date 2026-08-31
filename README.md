![Progress](https://img.shields.io/badge/matching-13.12%25-red)
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

The project uses CMake as its build system. There are two build modes:

The Android x86 build uses the NDK r8e toolchain and targets the original
Android platform. It is the matching build; its output is not a host-runnable
game executable.

A host build uses the configured host C/C++ compiler and is used for testing
and development. The default host target builds 32-bit i686 code on non-Windows
systems; a separate host64 target builds native 64-bit code. Both enable
AddressSanitizer by default on non-Windows systems.


### Android x86 build

```bash
# configure
cmake -B build
# build
cmake --build build
```

### Host build

```bash
cmake -B build-host -DBUILD_FOR_HOST=ON
cmake --build build-host

# executable
./build-host/saga
```

### 64-bit host build

```bash
cmake -B build-host64 -DBUILD_FOR_HOST64=ON
cmake --build build-host64

# executable
./build-host64/saga
```

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
| `(root)` | 61.3% | 50.0% | 0.0% |
| `MechInputTouch` | 5.0% | 0.0% | 5.1% |
| `editor` | 2.6% | 0.0% | - |
| `gameapi` | 7.1% | 1.5% | 94.8% |
| `gameframework` | 76.4% | 23.5% | 100.0% |
| `gamelib` | 5.8% | 0.8% | 11.6% |
| `java` | 10.7% | 0.0% | 0.0% |
| `legoapi` | 11.5% | 12.1% | 31.6% |
| `legoapi/actions` | 5.8% | 1.4% | 62.3% |
| `legoapi/ai` | 10.0% | 1.1% | 88.5% |
| `legoapi/audio` | 5.1% | 6.6% | 88.7% |
| `legoapi/characters` | 10.1% | 6.9% | 36.1% |
| `legoapi/core` | 20.8% | 12.4% | 97.8% |
| `legoapi/cutscenes` | 6.1% | 2.3% | 95.5% |
| `legoapi/gizmo` | 9.3% | 9.1% | 32.9% |
| `legoapi/gizmos` | 21.5% | 15.0% | 98.0% |
| `legoapi/items` | 6.5% | 13.9% | 4.0% |
| `legoapi/menus` | 10.5% | 12.6% | 21.1% |
| `legoapi/misc` | 5.0% | 2.0% | 100.0% |
| `legoapi/props` | 22.7% | 9.9% | 58.8% |
| `legoapi/render` | 8.9% | 11.2% | 95.1% |
| `legoapi/world` | 21.7% | 29.8% | 1.3% |
| `legogame` | 48.0% | 43.8% | 85.4% |
| `nu2api` | 32.0% | 25.1% | 9.3% |

<!-- matching-table-end -->
