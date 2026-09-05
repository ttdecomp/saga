# _saga_

![Progress](https://img.shields.io/badge/matching-15.32%25-red)
[![Bazel build](https://github.com/ttdecomp/saga/actions/workflows/build-bazel.yaml/badge.svg)](https://github.com/ttdecomp/saga/actions/workflows/build-bazel.yaml)
[![Discord](https://img.shields.io/discord/1467775700894224555?color=%235865F2&logo=discord&logoColor=%23FFFFFF)](https://discord.gg/2HJuMtzA7q)

This is a decompilation of _LEGO Star Wars: The Complete Saga_, based on the
Android x86 release. The repository builds three variants:

| Variant  | Purpose                                             |
| -------- | --------------------------------------------------- |
| `target` | Android x86 shared library used for binary matching |
| `native` | Linux or Windows diagnostic executable              |
| `wasm`   | Browser diagnostic build                            |

Game assets and the original binary are not included or required to compile.

## Build 🔨

You need Git and an x86-64 Linux C/C++ toolchain. The Linux `native`
variant uses system libraries: it needs your distribution's
32-bit SDL 3, Vorbis, EGL, GLES, and `pkg-config` development support. Bazel
downloads the toolchains and libraries used by `target` and `wasm`.

Install Bazelisk as `bazel` and it will select the version in `.bazelversion`.
Alternatively, install that Bazel version directly.

Clone and build every variant:

```sh
git clone https://github.com/ttdecomp/saga.git
cd saga

bazel build --config=target //src:saga_target
bazel build --config=native //src:saga_native
bazel build --config=wasm //src:saga_wasm
bazel test //scripts/checks:checks
```

Optimized, sanitizer-free host builds are available as
`--config=native_release` and `--config=wasm_release`; both compile with
`-O2`.

To run the native or browser build after supplying your own game assets, see
[CONTRIBUTING.md](CONTRIBUTING.md):

```sh
bazel run --config=native //src:run_native -- window
bazel run --config=wasm //scripts:wasm_server
```

<!-- matching-table-start -->

## Matching progress 📊

See https://ttdecomp.github.io/saga/

| Directory | Fuzzy % | Funcs % |
|---|---:|---:|
| `(root)` | 61.8% | 0.0% |
| `MechInputTouch` | 5.8% | 2.0% |
| `editor` | 3.1% | 1.7% |
| `gameapi` | 8.3% | 1.5% |
| `gameframework` | 84.0% | 5.9% |
| `gamelib` | 6.7% | 4.3% |
| `java` | 10.5% | 0.0% |
| `legoapi` | 14.5% | 7.1% |
| `legoapi/actions` | 6.3% | 1.4% |
| `legoapi/ai` | 12.8% | 0.0% |
| `legoapi/audio` | 6.6% | 5.0% |
| `legoapi/characters` | 11.2% | 3.9% |
| `legoapi/core` | 24.6% | 6.7% |
| `legoapi/cutscenes` | 11.0% | 2.3% |
| `legoapi/gizmo` | 11.5% | 3.0% |
| `legoapi/gizmos` | 36.4% | 26.8% |
| `legoapi/items` | 8.7% | 4.0% |
| `legoapi/menus` | 12.9% | 6.1% |
| `legoapi/misc` | 8.8% | 3.6% |
| `legoapi/props` | 28.2% | 2.4% |
| `legoapi/render` | 10.7% | 6.2% |
| `legoapi/world` | 23.7% | 6.4% |
| `legogame` | 50.3% | 0.0% |
| `nu2api` | 33.0% | 14.7% |

<!-- matching-table-end -->

## Legal ⚖️

This educational research project contains reconstructed code only. Do not
commit game assets, leaked source code, or other copyrighted game data. The
original game remains the property of its owners.
