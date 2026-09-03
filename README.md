![Progress](https://img.shields.io/badge/matching-15.23%25-red)
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

# run the game
./build-host/saga window

# enter the Cantina and rotate the camera through 360 degrees in 10 seconds
./build-host/saga window --camera-orbit

# enter the Cantina with a free camera (numpad 8/5/4/6; hold Shift to move)
./build-host/saga window --camera-free
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
| `(root)` | 62.4% | 66.7% | 0.0% |
| `MechInputTouch` | 5.8% | 3.7% | 4.6% |
| `editor` | 3.1% | 1.7% | 100.0% |
| `gameapi` | 8.4% | 4.8% | 61.9% |
| `gameframework` | 76.4% | 23.5% | 100.0% |
| `gamelib` | 7.3% | 5.9% | 12.1% |
| `java` | 10.7% | 0.0% | 0.0% |
| `legoapi` | 14.3% | 16.6% | 50.0% |
| `legoapi/actions` | 6.6% | 4.0% | 99.1% |
| `legoapi/ai` | 13.1% | 5.9% | 88.7% |
| `legoapi/audio` | 6.5% | 9.2% | 96.7% |
| `legoapi/characters` | 11.9% | 8.9% | 36.2% |
| `legoapi/core` | 25.3% | 14.3% | 97.8% |
| `legoapi/cutscenes` | 10.6% | 6.8% | 99.6% |
| `legoapi/gizmo` | 10.8% | 11.1% | 49.4% |
| `legoapi/gizmos` | 36.3% | 34.6% | 98.2% |
| `legoapi/items` | 8.5% | 17.0% | 23.2% |
| `legoapi/menus` | 11.9% | 16.6% | 97.7% |
| `legoapi/misc` | 8.5% | 7.0% | 100.0% |
| `legoapi/props` | 30.1% | 9.9% | 59.8% |
| `legoapi/render` | 10.8% | 13.3% | 95.2% |
| `legoapi/world` | 22.7% | 31.7% | 3.7% |
| `legogame` | 51.6% | 41.2% | 85.4% |
| `nu2api` | 33.9% | 28.0% | 9.4% |

<!-- matching-table-end -->
