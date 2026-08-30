![Progress](https://img.shields.io/badge/matching-11.19%25-red)
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

A host build uses the configured host C/C++ compiler. On non-Windows systems it
builds 32-bit i686 code with AddressSanitizer and is used for testing and
development.


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
| `(root)` | 57.3% | 16.7% | 0.0% |
| `MechInputTouch` | 5.0% | 0.0% | 0.9% |
| `editor` | 2.6% | 0.0% | - |
| `gameapi` | 7.1% | 1.5% | 94.8% |
| `gameframework` | 68.5% | 11.8% | 100.0% |
| `gamelib` | 5.6% | 0.9% | 5.0% |
| `java` | 10.7% | 0.0% | 0.0% |
| `legoapi` | 8.3% | 6.8% | 30.9% |
| `legoapi/actions` | 1.7% | 0.5% | 3.2% |
| `legoapi/ai` | 3.9% | 0.0% | 89.2% |
| `legoapi/audio` | 4.9% | 5.8% | 98.1% |
| `legoapi/characters` | 7.2% | 2.7% | 36.1% |
| `legoapi/core` | 16.2% | 5.1% | 97.8% |
| `legoapi/cutscenes` | 4.5% | 0.0% | 93.8% |
| `legoapi/gizmo` | 4.9% | 4.7% | 0.4% |
| `legoapi/gizmos` | 12.4% | 4.9% | 100.0% |
| `legoapi/items` | 3.2% | 0.2% | 6.7% |
| `legoapi/menus` | 9.7% | 10.7% | 18.1% |
| `legoapi/misc` | 4.6% | 1.0% | - |
| `legoapi/props` | 17.8% | 5.5% | 59.7% |
| `legoapi/render` | 5.4% | 4.5% | 95.6% |
| `legoapi/world` | 21.1% | 28.7% | 0.2% |
| `legogame` | 6.1% | 5.3% | 81.2% |
| `nu2api` | 31.4% | 24.0% | 9.3% |

<!-- matching-table-end -->
