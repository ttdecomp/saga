# 03 — Matching and verification

The matching workflow compares the original Android x86 shared object with the
shared object produced by Bazel. There is no generated split-object or report
pipeline in the repository.

## Terminology

- **target build**: the reconstructed Android x86 `libTTapp.so`.
- **original**: the reference binary at `res/libTTapp.so`.
- **base/current**: the newly built `bazel-bin/src/libTTapp.so` in objdiff
  terminology.
- **native build**: the Linux, Windows, or macOS executable used for runtime
  diagnostics. It is not the matching artifact.

## Build and repository checks

```bash
bazel build --config=target //src:saga_target
bazel test //scripts:checks
```

The target build preserves the NDK r8e compiler, API 9 sysroot, source list,
and per-file optimization levels. `bazel/android_per_file_copts.bazelrc` is the
optimization authority; absence of an override means `-O0`.

The repository test suite currently checks:

- source membership and the boundary between engine and native-only code;
- every target optimization override, including missing files and duplicate or
  invalid optimization assignments.

## Compare one symbol

With `objdiff-cli` available on `PATH`:

```bash
python3 scripts/objdiff-cli.py _Z5qrandv
```

The helper runs objdiff directly against the two complete shared objects. Its
left side is the original and its right side is the current build. Read `-` and
`~` lines as behavior present in the original, and `+` and `>` lines as code
emitted by the current build.

For a dependency-free symbol-table or raw-disassembly check, use the NDK tools
described in [08-asm-review.md](08-asm-review.md). `disasm_diff.py` is another
option when the Python `capstone` package is installed.

## Check symbol coverage

When the original binary is present:

```bash
python3 scripts/check_symbols.py
```

This compares defined text symbols in the original and current shared objects,
applies `scripts/symbols_ignore.txt`, and enforces the documented extra-symbol
baseline. It is a diagnostic for symbol coverage; byte matching still requires
instruction comparison.

## Optional external reports

`matching_report.py` and `objdiffdiff.py` remain available for reports generated
outside the build. Bazel does not generate or commit `objdiff.json`,
`report.json`, split objects, or README progress updates.

## Common pitfalls

1. A function can have the right behavior and still compile differently because
   its signature, linkage, source order, or optimization level differs.
2. `int` and `long` are both 32-bit in this ABI but have different C++ mangled
   names. Use the ABI types documented in [04-types-abi.md](04-types-abi.md).
3. Moving a function between files can change its optimization level. Compare
   the source and destination entries in
   `bazel/android_per_file_copts.bazelrc` first.
4. `HOST_BUILD` code is for native/WASM diagnostics and must not leak into the
   matching target.
5. The target is a shared object with no `main`; only native and WASM builds use
   the host harness.

## Authoring loop

1. Confirm the exact original symbol name with `nm`.
2. Locate its source owner with `rg`.
3. Confirm the file's effective target options with `bazel aquery`.
4. Implement the smallest source change that matches the ABI and expected
   code-generation pattern.
5. Rebuild `//src:saga_target`, compare the symbol, and run
   `bazel test //scripts:checks`.
