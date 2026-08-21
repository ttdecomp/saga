![Progress](https://img.shields.io/badge/matching-8.36%25-red)
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

The project uses CMake as its build system. There are two main build targets:

The Android x86 build, which uses the NDK toolchain and targets the original Android platform. This build is used for matching the code, its output can not be run.

A host build, which uses the hosts native gcc and targets i686 Linux. This build is used for testing and development, its output can be run on the host.


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

See the [docs/](docs/) directory.

## References

- The Android app can be run with the Waydroid emulator: https://waydro.id/
- The Android NDK can be found here: https://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2

<!-- matching-table-start -->

## Matching progress

| Directory | Fuzzy % | Funcs % | Data % |
|---|---|---|---|
| `(root)` | 0.6% | 0.0% | 68.3% |
| `MechInputTouch` | 4.8% | 0.0% | 0.0% |
| `editor` | 2.5% | 0.0% | - |
| `gameapi` | 6.5% | 1.4% | 94.8% |
| `gameframework` | 65.4% | 12.5% | 100.0% |
| `gamelib` | 5.5% | 0.9% | 5.0% |
| `java` | 10.5% | 0.0% | 0.0% |
| `legoapi` | 4.9% | 4.1% | 5.3% |
| `legoapi/actions` | 1.7% | 0.5% | - |
| `legoapi/ai` | 2.0% | 0.0% | - |
| `legoapi/audio` | 2.9% | 0.0% | - |
| `legoapi/characters` | 3.4% | 0.6% | 39.5% |
| `legoapi/core` | 9.4% | 4.5% | 2.8% |
| `legoapi/cutscenes` | 2.9% | 0.0% | 100.0% |
| `legoapi/gizmo` | 4.9% | 4.7% | 0.4% |
| `legoapi/gizmos` | 12.2% | 4.2% | 100.0% |
| `legoapi/items` | 2.3% | 0.2% | 4.9% |
| `legoapi/menus` | 2.4% | 0.0% | 0.0% |
| `legoapi/misc` | 2.2% | 0.5% | - |
| `legoapi/props` | 7.3% | 2.4% | 13.0% |
| `legoapi/render` | 2.3% | 1.5% | 0.5% |
| `legoapi/world` | 13.6% | 22.8% | 0.3% |
| `legogame` | 4.2% | 0.0% | 100.0% |
| `nu2api` | 27.1% | 19.4% | 81.5% |

<!-- matching-table-end -->
