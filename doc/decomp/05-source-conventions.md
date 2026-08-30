# 05 — Source conventions

Guide to placing and authoring reconstructed code in the current `src/` tree.
The tree is actively reorganized, so use live paths and generated unit metadata
instead of a copied basename map.

## 1. Source layout

The major roots are:

| directory | role |
|---|---|
| `src/editor/` | editor implementations |
| `src/gameapi/` | AI, editor-tool, and GUI APIs |
| `src/gameframework/` | framework systems such as save/load |
| `src/gamelib/` | utilities and supporting engine systems |
| `src/legoapi/` | gameplay, grouped into `actions`, `ai`, `audio`, `characters`, `core`, `cutscenes`, `episodes`, `gizmo`, `gizmos`, `items`, `menus`, `misc`, `props`, `render`, and `world` |
| `src/legogame/` | startup and platform entry points |
| `src/MechInputTouch/` | Android touch controls |
| `src/nu2api/` | core engine modules (`nu3d`, `nucore`, `nufile`, `numath`, `numusic`, `nuplatform`, `nusound`, and platform glue) |
| `src/java/` | JNI compatibility code |
| `src/host-utils/` | host-only utilities, harness, and target replacements |

At the 2026-08-29 snapshot, the target build has 464 TUs: 451 C++ and 13 C.
Run `jq 'length' build/compile_commands.json` and inspect `.file` values for the
current set.

The detailed restructuring rules and current commands live in
[`../source-structure.md`](../source-structure.md).

## 2. Resolve ownership; do not guess it

Prefix heuristics are useful for narrowing a search (`NuSound*` generally
belongs under `nu2api/nusound`, `Mech*` under `MechInputTouch`, and so on), but
they are not authoritative. Current gonk output preserves directories, and
same basenames can legitimately exist in different modules.

Preferred one-symbol workflow:

```bash
python3 scripts/objdiff-cli.py SYMBOL
```

To inspect exact unit paths:

```bash
jq -r --arg part 'deflate' '
  .units[]
  | select(.name | contains($part))
  | [.name, .target_path, .base_path]
  | @tsv
' objdiff.json
```

Current unit names include `.o`, for example
`nu2api/nucore/deflate.cpp.o`. Do not use `nm build/split/*.o`; it misses the
nested split objects. If raw recursive lookup is needed:

```bash
find build/split -type f -name '*.o' -print0 \
  | xargs -0 ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-nm -A \
  | rg ' T SYMBOL$'
```

If a target symbol has no claimed TU, it may be in `remaining`, deliberately
ignored, or supplied by an extra/library unit. Absence from a glob is not proof
that it is external library code.

## 3. File-role patterns

- `*_plain.cpp` groups plain C-linkage definitions. Eleven such files currently
  exist; count them with `find src -name '*_plain.cpp'` rather than maintaining
  a list here.
- `*_misc.cpp` is a catch-all for symbols whose better owner is not yet known.
  Treat it as provisional; moving a function still requires matching the
  destination optimization level.
- `*_types.h` contains provisional/Ghidra-derived type scaffolding. Avoid
  duplicating a type already defined canonically elsewhere.
- `android/` contains target platform code. `host-utils/` contains selected
  host replacements and is not part of the target TU set.
- `*_gen.cpp` denotes manually maintained split/generated content; the suffix
  does not imply a regeneration tool exists.

Examples of current paths:

- qrand: `src/legoapi/core/input/qrand.cpp`
- world: `src/legoapi/world/world.cpp`
- players: `src/legoapi/characters/core/players.cpp`
- AI system: `src/legoapi/ai/core/ai_sys.cpp`
- game audio: `src/legoapi/audio/sfx.cpp`
- gizmo actions: `src/legoapi/gizmo/gizmos/gizmos_gizactions.cpp`
- Ogg-facing code: `src/legoapi/audio/gamelib_ogg.cpp`

## 4. Naming and linkage

Follow `CONTRIBUTING.md`:

- Preserve known original function/type names and every provisioned mangled
  symbol.
- Use descriptive `snake_case` for members, parameters, and locals.
- Use `UPPER_SNAKE_CASE` for enum members and macros.
- C-style tags commonly use lowercase names ending in `_s`/`_e`, with an
  uppercase typedef; C++ classes use `PascalCase`.

Linkage follows the target symbol:

- plain symbol name → normally `extern "C"`
- `_Z...` symbol → C++ linkage

Changing `int` to `long`, signedness, const qualification, a namespace, or a
class name can change mangling even when layout is unchanged. See
[`04-types-abi.md`](04-types-abi.md).

## 5. Types and placeholders

- Use `u8/u16/u32/u64`, `i8/i16/i32/i64`, `f32/f64`, `usize`, and `isize` from
  the engine headers.
- Use `abi_long`/`abi_ulong` only when the target mangle requires `long`/`unsigned long`.
- Match placeholder struct offsets and total size exactly. A field name such as
  `field3_0x14` documents uncertainty; it does not excuse a wrong offset.
- Do not leave known data behind `u8` blobs and raw offset casts. Recover named,
  typed fields as evidence accumulates, then convert all touched callers to
  member access. Keep padding only for ranges whose contents are still unknown.
- Express original-binary ABI checks with `DECOMP_ASSERT`, which is disabled
  for host builds. Do not use an original 32-bit byte size or field offset for
  runtime allocation, iteration, or access: use typed fields and `sizeof` so
  the host ABI remains correct when pointers are wider.
- Prefer a forward declaration over a second empty definition. Run
  `scripts/check_duplicate_definitions.py` when changing shared types.

## 6. Stub and diagnostic conventions

Generated placeholder bodies usually omit parameter names and return a neutral
value. Hand-maintained stubs may name parameters, cast them to `(void)`, and use
`UNIMPLEMENTED("reason")`. Match the surrounding file's style.

`UNIMPLEMENTED` and `LOG_*` produce host diagnostics and compile away in target
builds. `__FILENAME__` is currently a correct repository-relative path such as
`src/legoapi/world/world.cpp`; the former `src/src/` prefix bug is fixed.

`SAGA_NOMATCH` places a function in `.text.nomatch`. It does **not** by itself
hide the symbol from `nm` or exempt it from `check_symbols.py`; a global symbol
can still count toward the extra-symbol baseline. Use the attribute only for
its section-placement purpose, not as a symbol-filtering mechanism.

Host-only diagnostic helpers belong under `src/host-utils/`. Keep asset tools
general: `./build-host/saga load list [filter]` lists DAT entries and
`./build-host/saga load extract <dat-path> <output>` extracts any entry. Do not
add sequence- or level-specific extraction code to target translation units.

Platform replacements follow the same boundary: keep the reconstructed target
body in its original translation unit with weak linkage, then provide the strong
host implementation under `src/host-utils/`. This is especially important for
graphics behavior that the API leaves undefined. For example, a fresh render
target's texels need not have the same initial alpha on Android and Mesa; model
the observed target-driver state in the host replacement instead of adding
`HOST_BUILD` branches or changing the decompiled target body.

Prefix host-only internal functions and storage with `host` (for example,
`host_read_frame` or `g_hostTextureHashes`). Strong replacements keep the
original engine or platform name because the linker-facing API requires it.

## 7. Globals and data placement

Shared globals generally live in `src/globals.cpp` with declarations in
`src/globals.h`; subsystem-local globals stay with their subsystem. Preserve
the original section:

- zero/uninitialized objects normally land in `.bss`
- nonzero constant initialization normally lands in `.data`
- vtables and const pointer tables commonly land in `.data.rel.ro`

Writing `= 0` does not create dynamic initialization or a guard. Confirm the
target symbol class with `nm` before changing initialization.

For initialized registries and pointer tables, preserve the complete original order,
terminator, per-entry flags, and section placement. The target is unstripped, so recover the
table bytes and pointed-to symbols directly as described in `06-target-binary.md` instead of
constructing a minimal table for whichever script or host path happens to run today.

## 8. Authoring checklist

1. Find the exact target symbol and demangle it if needed.
2. Resolve its current objdiff unit and target/base paths.
3. Check the effective compile command in `build/compile_commands.json`.
4. Preserve linkage and mangling; use ABI types deliberately.
5. Reconstruct the body using the codegen patterns in `02-codegen.md` and the
   mismatch catalog in `07-diagnostics.md`.
6. For a provisional body, use the local stub convention and correct return type.
7. Use `scripts/objdiff-cli.py SYMBOL` for the focused diff.
8. Run the project's normal format/lint/matching checks before commit.

The pre-commit hook formats and stages source files and regenerates/stages
matching artifacts. Review its staged changes before committing.
