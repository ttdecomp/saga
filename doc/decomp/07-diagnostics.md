# 07 — Mismatch diagnostics

Scope: diagnosing differences between `res/libTTapp.so` and the Bazel target
`bazel-bin/src/libTTapp.so` built with NDK r8e GCC 4.7.

## Investigation loop

```bash
bazel build --config=target //src:saga_target
bazel run //scripts:objdiff_cli -- SYMBOL
bazel test //scripts/checks:checks
```

For the affected source file, also confirm its effective optimization options:

```bash
rg -n 'source/path\.cpp' bazel/android_per_file_copts.bazelrc
bazel aquery --config=target \
  'mnemonic("CppCompile", //src:saga_target)' --include_commandline
```

No optimization-map entry means `-O0`.

## Diagnose by symptom

### Whole function has a different shape

Check the optimization level first. An accidental `-O0`/`-O2`/`-O3` mismatch
changes frame setup, reloads, inlining, branches, and register allocation at
once. Fix the build metadata rather than contorting the C source.

### Branch direction or compare operands differ

Signedness and source ordering both matter. `a > b` and `b < a` can produce
mirrored `cmp`/jump sequences. Confirm the ABI types, then try the source form
that expresses the original operand order.

### `setcc`, branch, or `cmov` differs

Check whether the result type is `bool` or `int`, whether both branches assign a
value, and whether the condition has side effects. Ternaries, early returns,
and assignment-then-return forms are not interchangeable on GCC 4.7.

### Extra loads or missing reloads

Look for cached globals, pointer aliases, and `volatile`. Add `volatile` only
when the original behavior requires an observable reload; it is not a generic
matching hint.

### Float instructions differ

Confirm `float` versus `double`, literal suffixes, intermediate casts, and the
comparison result type. The target normally uses SSE math; x87 often signals a
conversion or ABI boundary.

### Calls appear or disappear

At optimized levels, inspect inlining, static linkage, and constant propagation.
At `-O0`, verify that a helper was not accidentally declared `inline` or moved
to a different source file.

### Tail call versus call-and-return

Small changes to return expressions, cleanup, or local lifetimes can inhibit a
tail call. Compare the final source operation with the original control flow.

### Stack size or member offsets differ

Recheck structure layout, packing, field order, pointer width, enum width, and
`abi_long`/`abi_ulong`. Do not patch offsets manually to compensate for an
incorrect type.

### Global is in `.data` instead of `.bss`

Zero-initialized globals belong in `.bss`; nonzero initializers belong in
`.data`. Match the section and initialization behavior, not only the runtime
value.

### Function or literal addresses drift

Target builds disable function/data sections, so definition order and first use
of string literals can affect layout. Check for inserted helpers, moved
definitions, and reordered literals before changing unrelated code.

### Symbol is missing

Confirm exact mangling, C/C++ linkage, namespace/class ownership, constness,
parameter signedness, and whether the function became local or inline. Use:

```bash
bazel run //scripts/checks:check_symbols -- --list
```

### Extra symbol appears

Determine whether it is intended reconstructed code, a compiler-generated
clone, or an accidental public helper. Prefer local/static linkage for helpers
that are not present in the original symbol surface.

## Verified GCC 4.7 reminders

- `-O0` stack allocation commonly uses `lea`, not `sub`.
- `-fno-exceptions` does not remove static-local guard variables.
- `int` and `long` have equal width but different C++ mangling.
- `__builtin_expect` can change layout and is not a no-op.
- Zero initialization can change both section placement and instruction shape.
- Modern-compiler intuition is not evidence for GCC 4.7 output.

## Minimal experiment

When a source-form question remains ambiguous, create a temporary tiny file
outside the repository, compile it with the same NDK compiler and relevant
optimization level, and inspect it with `i686-linux-android-objdump -dr`. Keep
the experiment narrow enough to answer one code-generation question.

## Completion checklist

1. The target rebuilds successfully.
2. The affected symbol's diff improves or disappears.
3. Its symbol name and linkage remain correct.
4. The source file still has the intended optimization level.
5. `bazel test //scripts/checks:checks` passes.
