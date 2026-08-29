---
name: saga-decomp
description: Work safely and effectively on saga's Android x86 matching decompilation, including symbol ownership, ABI signatures, GCC 4.7 codegen, objdiff diagnosis, source placement, and documentation maintenance. Use for changes or reviews involving reconstructed game code, matching percentages, gonk, split objects, or decompilation docs; do not use for unrelated generic C++ work.
---

# Saga matching decompilation

Preserve the repository's matching contract while making the smallest useful
change. The checkout is often edited by multiple people or agents at once, so
inspect current state and never overwrite unrelated work.

## Start from live state

1. Run `git status --short` and note existing modifications. Treat them as
   user-owned unless the task explicitly identifies them as yours.
2. Read `doc/decomp/00-index.md`, then only the routed chapters relevant to the
   task. Read `doc/source-structure.md` before moving a function or file.
3. Prefer generated facts over copied prose:
   - effective compiler command: `build/compile_commands.json`
   - target/base unit paths: `objdiff.json`
   - matching snapshot: `report.json`
   - per-file optimization overrides: `src/target.cmake`
4. Check that generated artifacts are newer than the source/configuration they
   describe. If they are stale and regeneration is outside the user's scope,
   say so rather than treating their values as current.

## Resolve one symbol

Use the repository wrapper first:

```bash
python3 scripts/objdiff-cli.py MANGLED_SYMBOL
```

It reports the original on the left and current build on the right. Read `-`
as target behavior missing from the current code, `+` as extra current code,
and `~`/`>` as target/current operand variants.

For exact object paths, query `objdiff.json`. Split objects preserve source
directories and unit names include `.o`; never rely on the non-recursive
`build/split/*.o` glob.

```bash
jq -r --arg part 'deflate' '
  .units[]
  | select(.name | contains($part))
  | [.name, .target_path, .base_path]
  | @tsv
' objdiff.json
```

If raw lookup is necessary, recurse safely:

```bash
find build/split -type f -name '*.o' -print0 \
  | xargs -0 ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-nm -A \
  | rg ' T SYMBOL$'
```

## Non-negotiable matching rules

- Preserve the exact symbol name, linkage, namespace/class, signature, and ABI
  types. `int` and `long` are both 32-bit here but mangle differently.
- Preserve the owning TU's optimization mode. Absence of `-O` means `-O0`;
  moving code between files can silently change codegen.
- Plain names normally require `extern "C"`; `_Z...` names use C++ linkage.
- The target uses GCC 4.7 from NDK r8e, i386 PIC, SSE2 arithmetic, disabled
  exceptions/RTTI, and PCC-style hidden struct-return pointers.
- PIC does not prohibit all tail calls: interposable external calls use the
  tested `call`/`ret` shape, while local or hidden callees can use `jmp`.
- `SAGA_NOMATCH` changes section placement; it does not hide a symbol from
  `nm` or automatically exempt it from the extra-symbol baseline.
- Function definition order, data section, literal first-use order, signedness,
  field offsets, and static initialization can all affect matching.

Read `doc/decomp/02-codegen.md`, `04-types-abi.md`, and
`07-diagnostics.md` before changing source to chase an assembly shape.

## Working boundaries

- Follow the user's requested scope. A documentation audit does not authorize
  code or build-system edits; a diagnosis does not authorize implementing a fix.
- Use `/tmp` for compiler experiments. Compile directly with the documented NDK
  driver when CMake or repository writes are out of scope.
- Do not run CMake, formatters, hooks, or generators when the user has excluded
  them. The pre-commit hook formats/stages source and regenerates/stages reports,
  so never invoke it casually in a shared checkout.
- Use `apply_patch` for edits. Preserve concurrent modifications and stop if the
  requested lines overlap unexplained user changes.

## Verification

Choose checks proportional to the task:

- documentation: local-link scan, stale-path/count scan, command `--help`, and
  focused read-only command tests
- one symbol: `scripts/objdiff-cli.py`, exact unit paths, `nm`, and `objdump -dr`
- symbol surface: `python3 scripts/check_symbols.py --list`
- report regression: `python3 scripts/objdiffdiff.py NEW.json OLD.json`
- full source change: only run the documented build/lint/split/report pipeline
  when authorized and safe in the current checkout

Report what was actually run, what was intentionally skipped, and whether live
generated artifacts may have changed concurrently.
