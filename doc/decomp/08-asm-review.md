# 08 — Assembly review workflow

> Agent/reference document. Human matching workflows are in
> [`CONTRIBUTING.md`](../../CONTRIBUTING.md).

Use the complete original and rebuilt shared objects for review. The repository
does not require generated split objects or a generated objdiff project. The
optional Pages report described in [03-matching.md](03-matching.md) summarizes
the same whole-binary comparison.

## Prerequisites

```bash
bazel build --config=target //src:saga_target
test -f res/libTTapp.so
```

The NDK r8e `i686-linux-android-nm`, `objdump`, `readelf`, and `c++filt` tools
are the reference binutils. Put the matching host's NDK toolchain `bin/`
directory on `PATH`, or use its absolute path.

## Fast path

For one symbol and a compact classified diff:

```bash
bazel run //scripts:objdiff_cli -- _Z5qrandv
```

The helper compares `res/libTTapp.so` with the target-config Bazel output.

## Raw symbol checks

```bash
SYM=_Z5qrandv
i686-linux-android-nm -A --defined-only res/libTTapp.so | rg " $SYM$"
i686-linux-android-nm -A --defined-only bazel-bin/src/libTTapp.so | rg " $SYM$"
```

`T`/`t` means a global/local text definition; `U` means undefined. If the
symbol exists only on one side, fix name, linkage, or source membership before
studying instructions.

To compare the complete symbol surface:

```bash
bazel run //scripts/checks:check_symbols -- --list
```

## Raw disassembly

```bash
SYM=_Z5qrandv
i686-linux-android-objdump -dr \
  --disassemble="$SYM" res/libTTapp.so > /tmp/original.asm
i686-linux-android-objdump -dr \
  --disassemble="$SYM" bazel-bin/src/libTTapp.so > /tmp/current.asm
diff -u /tmp/original.asm /tmp/current.asm
```

Addresses and relocation rendering can differ even when instruction structure
is close. Compare mnemonics, operands, control flow, constants, and relocation
targets before treating address drift as a source mismatch.

For source-interleaved output on the current build, use `objdump -S` if debug
line information is present:

```bash
i686-linux-android-objdump -S \
  --disassemble="$SYM" bazel-bin/src/libTTapp.so
```

## Reading `objdiff-cli.py`

- `-` and `~` describe the original target side.
- `+` and `>` describe the current Bazel build.
- Add behavior represented by missing `-` lines, remove/fix extra `+` lines,
  and make each `>` operand match its paired `~` operand.

## Common traps

- Demangled names are for reading; use the exact mangled symbol for commands.
- A target build must be rebuilt after every source or optimization change.
- Native/WASM binaries are not substitutes for the Android x86 matching target.
- `objdump -d` without `--disassemble` is noisy and makes comparisons harder.
- A missing symbol is usually an ABI/linkage problem, not an instruction diff.
- Local symbols can share names; confirm the source owner before editing.

## Iteration loop

1. Build `//src:saga_target`.
2. Confirm the symbol exists on both sides.
3. Run the compact diff.
4. Apply the mismatch catalog in [07-diagnostics.md](07-diagnostics.md).
5. Rebuild and repeat.
6. Finish with `bazel test //scripts/checks:checks`.
