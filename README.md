[![Target build](https://github.com/ttdecomp/saga/actions/workflows/build-target.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-target.yaml)
[![Linux i686 build](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml)
![Progress](https://img.shields.io/badge/matching-3.14%25-red)
[![Discord](https://img.shields.io/discord/1467775700894224555?color=%235865F2&logo=discord&logoColor=%23FFFFFF)](https://discord.gg/2HJuMtzA7q)

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

| Build             | Initial setup                             | Build (and run)                                                               |
| ----------------- | ----------------------------------------- | ----------------------------------------------------------------------------- |
| Android x86 build | `cmake -B build -DBUILD_FOR_HOST=OFF`     | `cmake --build build`                                                         |
| Host build        | `cmake -B build-host -DBUILD_FOR_HOST=ON` | `cmake --build build-host && ./build-host/saga`                               |
| Tests             | `cmake -B build-host -DBUILD_FOR_HOST=ON` | `cmake --build build-host && ctest --output-on-failure --test-dir build-host` |

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
