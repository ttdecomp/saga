# Bazel implementation reference

> Agent/reference document. Use [`README.md`](../README.md) for a first build
> and [`CONTRIBUTING.md`](../CONTRIBUTING.md) for human development workflows.

## Toolchains

- Bazel 9.2.0 is pinned by `.bazelversion`.
- `rules_python` supplies Python 3.12.12 to all declared Python targets.
- The matching target uses Android NDK r8e GCC 4.7, API 9, and x86.
- Native Linux intentionally uses i686 with ASan and UBSan.
- Native Windows uses the MSYS2 MINGW64 toolchain selected by
  `--config=windows-mingw`.
- WASM uses emsdk 6.0.8 and a pthread-enabled Emscripten toolchain.

The NDK repository can use an existing absolute `SAGA_NDK_R8E` path. Without
it, the repository rule downloads and verifies NDK r8e. Its host tools support
x86-64 Linux, Windows, and macOS; Apple Silicon needs Rosetta for this target.

## Dependencies

The matching target builds Ogg 1.2.1, Vorbis 1.3.2, and Squish 1.10
hermetically. WASM uses Ogg 1.3.6, Vorbis 1.3.7, Squish 1.10, and SDL 3.4.10.
Native builds discover SDL3, Vorbis/Vorbisfile, and EGL/GLES2 through
pkg-config; Windows uses ANGLE. Squish remains hermetic.

## Preserved target behavior

- All variants use C11/C++11 and disable C++ exceptions and RTTI.
- `native` and `wasm` define both `HOST_BUILD` and `ANDROID`.
- The matching target disables function and data sections.
- Per-file `-O1`, `-O2`, `-O3`, and `-fPIE` overrides live in
  `bazel/android_per_file_copts.bazelrc`.
- `src/BUILD.bazel` is the source-membership authority.
- `//src:run_native` restores `BUILD_WORKING_DIRECTORY` before executing the
  native binary.
- Python tools should be run by Bazel label. Direct `python3` bypasses the
  pinned runtime.

Inspect effective target commands with:

```sh
bazel aquery --config=target \
  'mnemonic("CppCompile", deps(//src:saga_target))' --output=commands
```

The supported labels, host prerequisites, output paths, and CI matrix are kept
in [`CONTRIBUTING.md`](../CONTRIBUTING.md) rather than duplicated here.
