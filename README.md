![Progress](https://img.shields.io/badge/matching-14.51%25-red)
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
| `(root)` | 62.3% | 50.0% | 0.0% |
| `MechInputTouch` | 5.0% | 0.0% | 5.1% |
| `editor` | 2.6% | 0.0% | - |
| `gameapi` | 8.1% | 3.9% | 92.6% |
| `gameframework` | 76.4% | 23.5% | 100.0% |
| `gamelib` | 5.8% | 0.8% | 12.1% |
| `java` | 10.7% | 0.0% | 0.0% |
| `legoapi` | 13.5% | 14.5% | 32.6% |
| `legoapi/actions` | 5.9% | 1.8% | 62.3% |
| `legoapi/ai` | 11.5% | 2.2% | 88.5% |
| `legoapi/audio` | 6.2% | 7.4% | 96.0% |
| `legoapi/characters` | 11.6% | 7.5% | 36.2% |
| `legoapi/core` | 25.1% | 12.8% | 97.8% |
| `legoapi/cutscenes` | 7.3% | 5.2% | 95.7% |
| `legoapi/gizmo` | 10.4% | 10.1% | 33.0% |
| `legoapi/gizmos` | 35.5% | 31.4% | 98.2% |
| `legoapi/items` | 6.8% | 13.9% | 7.9% |
| `legoapi/menus` | 11.1% | 13.9% | 21.4% |
| `legoapi/misc` | 6.6% | 3.0% | 100.0% |
| `legoapi/props` | 30.1% | 9.9% | 59.8% |
| `legoapi/render` | 10.2% | 11.9% | 95.1% |
| `legoapi/world` | 21.9% | 30.1% | 1.3% |
| `legogame` | 48.7% | 43.8% | 85.4% |
| `nu2api` | 33.1% | 25.9% | 9.3% |

<!-- matching-table-end -->
