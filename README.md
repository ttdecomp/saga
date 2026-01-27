[![Target build](https://github.com/ttdecomp/saga/actions/workflows/build-target.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-target.yaml)
[![Linux i686 build](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-linux-i686.yaml)
![Progress](https://img.shields.io/badge/matching-1.99%25-red)

# Decompilation of "LEGO Star Wars: The Complete Saga" 

This repository contains the decompiled source code of the game "LEGO Star Wars: The Complete Saga" from
the Android x86 build of the game. This build was chosen because it is the only known version of the game
that still contains all symbols, making decompilation feasible. The core game logic should be identical to other
platforms.

## Disclaimer

This project is for educational and research purposes only. The decompiled code is provided "as is" without any warranties.
The original game is the intellectual property of its respective owners. This repository does not contain any
game assets, media, or any other copyrighted material.

## Build Instructions

|                   |                                       |                                                                     |
| ----------------- | ------------------------------------- | ------------------------------------------------------------------- |
| Android x86 build | `cmake -B build -DBUILD_FOR_HOST=OFF` | `cmake --build build`                                               |
| Host build        | `cmake -B build -DBUILD_FOR_HOST=ON`  | `cmake --build build && ./build/saga`                               |
| Tests             | `cmake -B build -DBUILD_FOR_HOST=ON`  | `cmake --build build && ctest --output-on-failure --test-dir build` |


## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please open an issue or submit a pull request.

### Documentation

Documentation / explanations for various systems and components will be added over time. Contributions to the documentation are highly appreciated.

See the [docs/](docs/) directory.

### Naming Conventions

... (to be added)

## References

- the android app can be run with the Waydroid emulator: https://waydro.id/
- the android ndk can be found here: https://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2