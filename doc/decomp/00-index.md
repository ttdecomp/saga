# SAGA matching-decompilation knowledge base

This directory documents the Android x86 binary, its GCC 4.7 ABI and code
generation, and the workflow for comparing reconstructed code with the
original `res/libTTapp.so`.

## Build modes

| mode | purpose | command |
|---|---|---|
| target | matching Android x86 shared object | `bazel build --config=target //src:saga_target` |
| native | Linux/macOS diagnostic executable | `bazel build --config=native //src:saga_native` |
| native | Windows diagnostic executable | `bazel build --config=native --config=windows-mingw //src:saga_native` |
| wasm | browser diagnostic executable | `bazel build --config=wasm //src:saga_wasm` |

The target is `bazel-bin/src/libTTapp.so` and deliberately has no `main`.
Native and WASM builds define `HOST_BUILD` and use the host harness.

## Documentation map

| file | use it for |
|---|---|
| [01-toolchain.md](01-toolchain.md) | NDK r8e compiler, flags, optimization map, and dependencies |
| [02-codegen.md](02-codegen.md) | GCC 4.7 instruction-shape patterns |
| [03-matching.md](03-matching.md) | current build and verification workflow |
| [04-types-abi.md](04-types-abi.md) | i686 ABI, types, mangling, layouts, and thunks |
| [05-source-conventions.md](05-source-conventions.md) | source placement, linkage, stubs, and globals |
| [06-target-binary.md](06-target-binary.md) | measured properties of the original ELF |
| [07-diagnostics.md](07-diagnostics.md) | symptom-to-cause mismatch diagnosis |
| [08-asm-review.md](08-asm-review.md) | raw symbol and disassembly review |
| [09-objdiff-cli.md](09-objdiff-cli.md) | compact per-symbol objdiff helper |

## Non-negotiable matching facts

1. Mangled names must match exactly. `int` and `long` are both 32-bit here but
   have different Itanium ABI encodings.
2. C versus C++ linkage is part of the symbol contract.
3. Source-file optimization is part of the code-generation contract. Read
   `bazel/android_per_file_copts.bazelrc`; no entry means `-O0`.
4. Function and string-literal order can affect emitted bytes because target
   builds disable function/data sections.
5. Matching code uses the limited NDK system C++ runtime, not the modern STL.
6. `HOST_BUILD` behavior is diagnostic only and must not alter the target.
7. The target remains a shared object named `libTTapp.so`; native is the only
   desktop executable.

## Workflow in one screen

```bash
bazel build --config=target //src:saga_target
bazel test //scripts:checks
python3 scripts/objdiff-cli.py _Z5qrandv
python3 scripts/check_symbols.py
```

`objdiff-cli.py` requires `objdiff-cli` on `PATH`. `check_symbols.py` requires
the original binary. The Bazel test suite uses a downloaded, pinned Python
3.12.12 runtime and does not need a venv.

## Key repository paths

| path | role |
|---|---|
| `.bazelrc` | target/native/WASM configuration entry points |
| `src/BUILD.bazel` | source membership, outputs, defines, and platform flags |
| `bazel/android_per_file_copts.bazelrc` | exact target optimization overrides |
| `bazel/android_cc_toolchain_config.bzl` | NDK r8e C++ toolchain definition |
| `scripts/BUILD.bazel` | hermetic Python checks and tools |
| `src/decomp.h` | native-only logging and unfinished-code diagnostics |
| `src/nu2api/nucore/fixed_width.h` | ABI-width types |
| `res/libTTapp.so` | original reference binary |

## Per-function authoring checklist

1. Confirm the exact symbol spelling and linkage in the original binary.
2. Locate the current source owner with `rg`.
3. Confirm the source file's optimization options with `bazel aquery`.
4. Preserve the ABI and use the GCC 4.7 patterns documented here.
5. Rebuild the target and compare the symbol directly.
6. Run `bazel test //scripts:checks` before committing.
