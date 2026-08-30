# Source-tree structure

This document explains how the reconstructed source tree relates to the
original Android x86 binary. It is the restructuring reference, but generated
artifacts—not copied counts in prose—are the source of truth.

Snapshot checked on 2026-08-29:

| item | current value |
|---|---:|
| target compile commands | 464 |
| target C translation units | 13 |
| target C++ translation units | 451 |
| objdiff units | 466 (464 TUs, `ogg_vorbis`, `remaining`) |
| optimization levels | 342 default `-O0`, 25 `-O2`, 97 `-O3` |
| special compile mode | one of the 97 `-O3` TUs also uses `-fPIE` |
| original `_GLOBAL__sub_I_` symbols | 325 occurrences, 320 unique basenames |

These counts describe the current generated files and will change as the tree
is reorganized. Recompute them with the recipes below rather than updating
code or CMake merely to make the table agree.

## Ground truth and its limits

The original `res/libTTapp.so` has no DWARF and no `STT_FILE` symbols. Its
compiler-generated `_GLOBAL__sub_I_<basename>` symbols expose 320 unique source
basenames, but only for translation units that required file-scope dynamic
initialization. They are therefore evidence of original filenames and a lower
bound on the original TU count—not a complete TU manifest.

Constant initialization does not create a guard or static-initialization
function. For example, `static int value = 0;` normally occupies `.bss` and
needs no runtime initialization. Do not infer a TU boundary from ordinary
zero-initialized data.

```bash
NDK_BIN=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin
$NDK_BIN/i686-linux-android-nm res/libTTapp.so \
  | rg '_GLOBAL__sub_I_' \
  | sed 's/.*_GLOBAL__sub_I_//' \
  | sort -u
```

Do not reference a contributor's `/tmp` file as a canonical list. If a durable
original-basename inventory becomes necessary, generate and commit it from the
command above.

## Current source hierarchy

The current layout is domain-oriented. Important roots are:

| directory | purpose |
|---|---|
| `src/editor/` | editor-specific implementations |
| `src/gameapi/` | AI, editor tools, and GUI APIs |
| `src/gameframework/` | framework services such as save/load |
| `src/gamelib/` | utilities, CRC, terrain, networking, and wind code |
| `src/legoapi/actions/` | game action implementations |
| `src/legoapi/ai/` | game AI and AI-system glue |
| `src/legoapi/audio/` | game audio and embedded Ogg/Vorbis-facing code |
| `src/legoapi/characters/` | character, player, and creature systems |
| `src/legoapi/core/` | configuration, input, game state, and common systems |
| `src/legoapi/cutscenes/` | cutscene systems |
| `src/legoapi/gizmo/` | gizmo base, object, and generated wrapper TUs |
| `src/legoapi/gizmos/` | concrete world-gizmo implementations |
| `src/legoapi/items/`, `menus/`, `props/`, `render/`, `world/` | corresponding gameplay domains |
| `src/legogame/` | startup and platform entry points |
| `src/MechInputTouch/` | Android touch-control system |
| `src/nu2api/nu3d/` | renderer, with `android/` and `generic/` subtrees |
| `src/nu2api/nucore/` | core runtime and Android platform adapters |
| `src/nu2api/nufile/`, `numath/`, `numusic/`, `nusound/` | file, math, music, and sound subsystems |
| `src/host-utils/` | host-only utilities, replacements, and harness code |

Use `rg --files src` for the live tree. Historical flat paths such as
`src/legoapi/qrand.cpp` or `src/legoapi/world.cpp` are no longer authoritative;
their current equivalents include `src/legoapi/core/input/qrand.cpp` and
`src/legoapi/world/world.cpp`.

## Per-file optimization is part of the ABI contract

Optimization is assigned per source file in `src/target.cmake`. The effective
command is recorded in `build/compile_commands.json`; that JSON is the final
authority for an existing build.

Moving a function between files can change its optimization level and code
generation even when its source is unchanged. Before moving code, compare the
source and destination commands.

```bash
# Count current target commands.
jq 'length' build/compile_commands.json

# Show the effective command for a path or basename.
jq -r --arg needle 'qrand.cpp' \
  '.[] | select(.file | contains($needle)) | .command' \
  build/compile_commands.json

# Recompute the current optimization distribution.
jq -r '.[].command' build/compile_commands.json \
  | awk '/-O3/{o3++; next} /-O2/{o2++; next} {o0++}
         END {print "-O0", o0, "-O2", o2, "-O3", o3}'

# Review declared overrides.
rg -n 'COMPILE_OPTIONS' src/target.cmake
```

Absence of an `-O` flag means `-O0`. `src/legoapi/core/config/cheat.cpp` is
currently the sole `-O3 -fPIE` file.

## Symbol-to-TU lookup

`gonk split` now preserves the source hierarchy below `build/split/`. Do not
use `nm build/split/*.o`: that glob examines only the handful of top-level
objects and misses nested TUs.

For one symbol, the preferred interface is:

```bash
python3 scripts/objdiff-cli.py _Z5qrandv
```

To obtain exact paths without guessing, query `objdiff.json`:

```bash
jq -r --arg path_part 'qrand' '
  .units[]
  | select(.name | contains($path_part))
  | [.name, .target_path, .base_path]
  | @tsv
' objdiff.json
```

If only the symbol name is known, search recursively:

```bash
find build/split -type f -name '*.o' -print0 \
  | xargs -0 ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-nm -A \
  | rg ' T _Z5qrandv$'
```

Current unit names are relative object paths and include the `.o` suffix, for
example `legoapi/core/input/qrand.cpp.o` and
`nu2api/nucore/deflate.cpp.o`. Use the exact `name` from `objdiff.json` with
`objdiff-cli -u`.

## File-role conventions

- `*_plain.cpp` contains collections of plain C-linkage symbols. This is a
  convention, not a guarantee that every body is an empty stub.
- `*_misc.cpp` is a temporary catch-all. Move a symbol to a better owner only
  when the target path and optimization level are understood.
- `*_types.h` contains provisional type scaffolding. Replace placeholders with
  canonical definitions carefully to avoid ODR and layout errors.
- `android/` contains target-specific implementations; `host-utils/` supplies
  selected host replacements.
- A shared basename in different directories is not evidence that the files
  should be merged. Directory identity is preserved in current split units.

## Restructuring checklist

1. Resolve the symbol's current unit through `objdiff.json` or recursive `nm`.
2. Record the current source and destination compile commands, especially
   `-O2`, `-O3`, and `-fPIE`.
3. Preserve exported names, linkage, signatures, and function definition order.
4. Check for same-named local symbols; gonk's original-symbol lookup is
   name-based and duplicate local names remain a known ambiguity.
5. After the code change, use the normal project verification pipeline from
   `doc/decomp/03-matching.md`. A file move can cause unit-level pairing churn,
   so inspect exact-match totals as well as fuzzy percentages.

Do not maintain a hand-written 320-row TU map in this document. The current
source path is already encoded in `objdiff.json`, while the original basename
inventory is mechanically recoverable from the binary. A copied table becomes
misleading as soon as the domain hierarchy changes.
