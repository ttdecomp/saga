# 03 — Matching & verification

How the project measures how much of the original `res/libTTapp.so` the recompiled
code reproduces, and how agents should (and shouldn't) react to the numbers.

Terminology (objdiff convention, as wired up by gonk):
- **target** = the original binary's code, carved into per-TU objects (`build/split/*.o`).
- **base** = what we compiled (`build/CMakeFiles/saga.dir/**/*.o`, or `build/saga` for extra units).
- A function is **matched** when base ≈ target.

## 1. Pipeline overview

One paragraph: verification is **local-only for symbol completeness, CI-only for
match-progress reporting**. `check_symbols.py` needs `res/libTTapp.so` and only runs
in the pre-commit hook on a developer machine; GitHub CI never builds the game or
runs gonk — it only diffs the committed `report.json` against the previous commit's
and posts the verdict as a PR comment. The pre-commit hook is therefore the gate:
every commit must carry an up-to-date `report.json` and README badge, regenerated in
the hook itself.

`.githooks/pre-commit` (enable with `git config core.hooksPath .githooks`, per
CONTRIBUTING.md:104-106), in order:

```bash
cmake -B build . && cmake --build build            # 1. recompile base objects + build/saga
cargo build --release --manifest-path gonk/Cargo.toml
./gonk/target/release/gonk split                   # 2. carve build/split/, rewrite objdiff.json
objdiff-cli installed on demand if missing         # 3. pre-commit:8-11: cargo install objdiff-cli --git https://github.com/ttdecomp/objdiff
objdiff-cli report generate -o report.json         # 4. measure everything
git add report.json
PROGRESS=$(jq ".measures.fuzzy_match_percent" report.json)
PROGRESS=$(printf "%.2f" "$PROGRESS")              # pre-commit:18-19: 2-decimal rounding for the badge
                                                   # badge color by % (>=90 brightgreen, >=70 green, >=50 yellow, >=30 orange, else red)
sed -i "s|https://img.shields.io/badge/matching-[^)]*|...|g" README.md
git add report.json README.md
if [ -f res/libTTapp.so ]; then python3 scripts/check_symbols.py || exit 1; fi   # 5. symbol surface gate (skipped in CI)
```

CI (`.github/workflows/diff.yaml`, on PR + push to main): fetches `BASE_SHA`
(`pull_request.base.sha`, or `github.event.before` for pushes),
`git show $BASE_SHA:report.json > report_old.json`, runs
`python scripts/objdiffdiff.py report.json report_old.json`, and posts the output as a
PR comment via `mshick/add-pr-comment`. The build workflows
(`.github/workflows/build-linux-*.yaml`, etc.) only build + lint; they never run gonk,
objdiff, or check_symbols.

## 2. gonk split mechanics

`gonk split` (gonk/src/main.rs:23; the only subcommand) reads `gonk.toml`,
`build/compile_commands.json` and `res/libTTapp.so` (gonk/src/split.rs:674-689).

**Per-TU carving** (split.rs:698-755): for each compile command, the recompiled TU
object's symbols are filtered (`symbol_filter`, split.rs:611-633: drop empty names,
`__x86.get_pc_thunk*`, `__discard`, `_GLOBAL_OFFSET_TABLE_`, `_DYNAMIC`, zero-size
symbols, non-Text/Data kinds) and then **paired by name** against the original
library's symbol table: `.filter_map(|sym| original_lib.symbols_by_name.get(...))`
(split.rs:711). The join is **by name only** — there is no address-based pairing
path anywhere in `split()`: `symbols_by_address` (split.rs:657-661) exists solely to
resolve call targets and GOT indirections during reloc recovery (split.rs:307,
528, 564). This includes local `t` symbols: a build TU's local symbol looks up the
original's `symbols_by_name` exactly like a global does, keyed by the build
symbol's name. Two consequences: (a) a build-local name absent from the original
is silently dropped (never carved); (b) the original often holds several
same-named locals (37 copies of `VuVecSet`, 37 of
`_Z41__static_initialization_and_destruction_0ii`), but `symbols_by_name` is a
plain HashMap keeping **one** entry per name (split.rs:650-655), so every TU
defining such a name carves the same single survivor and the unclaimed copies
fall to `remaining`. Text and data symbols are carved out of the original's
sections (symbol address..address+size) into a new ELF object — the "target".
Output goes to
`build/split/<tu-basename>.o` (flat; `command.output.file_name()`, split.rs:725).

**Relocation recovery** (`should_process_relocs=true` for TU units; split.rs:276-468):
the carved bytes are decoded with `iced_x86` and rewritten to compile-time relocations:
- Calls to `__x86.get_pc_thunk.{bx,cx,dx}` (detected by name, split.rs:573-585) set a
  "thunk register"; the following `add <reg>, <got-offset>` is rewritten to an
  `R_386_GOTPC` reloc against `_GLOBAL_OFFSET_TABLE_` (split.rs:354-384).
- Memory operands addressed off the thunk register become `R_386_GOT32` (GOT entry
  pointing at a symbol outside the object, split.rs:406-450) or `R_386_GOTOFF`
  (direct offset to a symbol/section inside the object), via `generate_rewrite`
  (split.rs:522-570).
- Calls to local symbols → `R_386_PC32`; calls to global symbols → `R_386_PLT32`
  (split.rs:314-318).
- **PLT resolution**: calls into the `.plt` range go through
  `create_reloc_for_extern_sym` (split.rs:474-520): decode the `.plt` stub, read the
  `.got.plt` displacement, map `got_plt_address + displacement` through
  `dyn_relocs_by_addr` (the dynamic relocations, split.rs:663) to the dynamic symbol,
  and emit an `R_386_PLT32` reloc to that extern.
- The GOT-displacement bytes are patched **by hand** (`local_rewrites`, split.rs:456-465),
  deliberately NOT with `iced_x86`'s encoder, "as it can create output which differs
  from the input in more ways than we want" (split.rs:456-458).

Data symbols go through `build_data_sections` (split.rs:184-243): `.bss` →
`append_section_bss`, everything else byte-copied.

**extra_units** (split.rs:762-813, gonk.toml): `ogg_vorbis` exists because the original
binary ships the ov_* API of a statically linked libvorbis, and `build/saga` does the
same — but those symbols appear in **no per-TU compile_commands unit**, so without
extra_units they'd fall into `remaining` and be unmatchable forever. The unit is
carved with `should_process_relocs=false` (raw copies) from **both** binaries:
target = `build/split/__extra_ogg_vorbis_target.o` (from libTTapp.so), base =
`build/split/__extra_ogg_vorbis_base.o` (from `build/saga`). The TOML list must name
**both** the plain C exports (`ov_read`, …) and the mangled C++ forms
(`_Z7ov_readP14OggVorbis_FilePciiiiPi`, …) — 36 C + 36 mangled entries today —
because each side carves strictly by exact symbol name and the two binaries
export different sets (verified via nm): the original's symtab contains only the
36 mangled forms (its libvorbis was compiled as C++), while `build/saga` defines
both. The target object therefore holds the 36 mangled symbols and the base
object all 72; a listed name absent from a given binary's symtab is silently
skipped on that side. (The plain C names matter to check_symbols too: its
gonk.toml fold, §5, keeps saga's C exports off the extra-symbol count.)

**remaining** (split.rs:815-839): every original Text/Data symbol not claimed by any
TU unit (`used_symbols`) and not ignored. Target = `build/split/remaining.c.o` (a
1,769,392-byte file, ~1.07 MB of it code), base = `remaining.c.o` at the repo
root — a 693-byte committed stub with no symbols. No `scratch` block → objdiff can never pair anything → no fuzzy keys anywhere
in its report section (verified in report.json: `remaining` has only `total_*`
measures — no fuzzy/match fields — and its section/function records carry no
`fuzzy_match_percent`). It can only shrink (as new TUs claim symbols) and never gains
matched bytes.

**Unit naming** (split.rs:734-743): last **3 path components** of the source path as
recorded in compile_commands.json — `src/gameapi/ai/aisys/aiscript.cpp` becomes
`ai/aisys/aiscript.cpp`; `saga/src/batman.cpp` stays `saga/src/batman.cpp` (the `saga`
CMake target records a 3-component path). Renaming/moving a source file **changes the
unit name** — this is the #1 source of false regressions (see §7).

## 3. objdiff.json

`objdiff.json` (regenerated by `gonk split`) is the objdiff project config, 293 units:
- Top level: `build_base: true`, `build_targets: false`,
  `custom_make: "sh"`, `custom_args: ["-l", "-c", "cmake -B build && cmake --build build -j -- "]`.
  `build_base=true` means objdiff runs the custom make (a login shell running the full
  CMake build) so base objects exist before comparison; `build_targets=false` because
  targets are gonk's static carved artifacts and must never be rebuilt.
- Per unit: `name` (as above), `target_path` (carved original), `base_path`
  (`build/CMakeFiles/saga.dir/**/*.o` for TU units; the carved saga objects for
  extra units; `remaining.c.o` for remaining), and `scratch`:
  `{"platform": "android_x86", "compiler": "ndk-r8e-gcc-4.7", "c_flags": "-fno-exceptions -fno-rtti"}`.
  The scratch block tells objdiff to compile the unit's source itself with the NDK
  r8e GCC 4.7 toolchain instead of trusting a prebuilt base object — the game was built
  with this toolchain, so code must be written to it. `remaining` is the only unit
  without scratch (split.rs:834-839).

## 4. Scoring & reports

`report.json` top-level `measures` (verified in the current file): `fuzzy_match_percent`
(7.152305), `total_code` (4,782,566), `matched_code` (71,971), `matched_code_percent`,
`total_data` / `matched_data` / `matched_data_percent`, `total_functions` (13,732) /
`matched_functions` (814) / `matched_functions_percent`, `total_units` (293). Per unit:
the same measures + `sections[]` (`name` `.text`/`.data`/`.bss`/…, `size`,
`fuzzy_match_percent`, `metadata`) and `functions[]` (`name`, `size`,
`fuzzy_match_percent`, `address`, `metadata.demangled_name`).

Empirically verified from report.json: `matched_code` == sum over units of `matched_code`
== sum of sizes of functions with `fuzzy_match_percent` **exactly 100.0** (71,971 bytes,
814 functions). 291 functions sit in (99, 100) and count as **not** matched. So
100.0 is a hard line, not a "close enough".

`scripts/objdiffdiff.py` compares two reports. Its docstring, verbatim:

> Reliability notes from the objdiff source (report.rs / diff/mod.rs):
>
> - ``fuzzy_match_percent`` is always assigned in memory (0.0-100.0). The JSON
>   key is present IFF the value is > 0.0 (proto3 "omit default" serialization),
>   so "no key" means "scored 0.0", which happens when objdiff fails to pair the
>   symbol to an original. Pairing is by symbol name and gonk re-splits by name,
>   so adding/renaming/merging a source file can move functions between units and
>   change pairing with zero code change. Key presence is therefore NOT a signal.
>
> - ``matched_code`` / ``matched_functions`` count only symbols at exactly 100%
>   and are computed in memory over all symbols, making them robust to both
>   unit reorganization and tiny-stub pairing flutter. Treat them as the primary
>   regression verdict.
>
> - Per-function fuzzy deltas are reliable only for larger functions (small
>   stubs < ~100 bytes flap wildly; functions > 2000 bytes are stable to ~0.12%).
>   We threshold partial-match detail by size.
>
> - Comparisons join functions by NAME (not unit), so relocation is not mistaken
>   for appearing/disappearing; a move that stays matched contributes 0 delta.

Its verdict classes (objdiffdiff.py:157-220): `lost_full` / `gained_full`
(100% ↔ not), `partial` (both paired, size ≥ 200, fuzzy dropped, ranked by est. bytes),
`moved` (same name, different unit — benign), `added`/`removed`, and per-directory
matched-byte attribution. It also emits the tell-tale artifact note when lost matches
moved units:

> *{n} of the {m} lost full matches also changed units; if the source for a
> moved-but-lost function is unchanged, this is a gonk/objdiff re-pairing artifact (see
> Context), not a real regression.

**Headline metrics**: the README badge (pre-commit:36) tracks
`measures.fuzzy_match_percent` (currently 7.15%, red) — the overall fuzzy average.
objdiffdiff.py's "headline verdict" is `matched_code`/`matched_functions`. The repo-root
`report.md` is a **stale artifact** of an earlier markdown-diff format: its numbers
match neither `report.json` (293 units) nor `report_old.json` (180 units), and neither
current `objdiffdiff.py` (prints to stdout) nor `objdiff-cli report changes` (JSON only)
produces it. Don't trust it; regenerate nothing from it.

## 5. check_symbols.py

`python3 scripts/check_symbols.py [BUILD_BIN=build/saga] [ORIG_BIN=res/libTTapp.so]`
(scripts/check_symbols.py, using the NDK's `i686-linux-android-nm`). It enforces the
**symbol surface**: every defined text symbol of the original must exist in the build.

- `T` from the original must be provided; a build-side weak `W` counts (inline/implicit
  special members, functionally identical); local `t` in the original makes a build
  symbol "not extra" (defined_text_symbols, check_symbols.py:43-67).
- `_ZThn*` (Itanium vtable-adjustment thunks) are exempt from the missing check
  (is_thunk, check_symbols.py:70-74).
- Ignore mechanisms (load_ignore, check_symbols.py:76-92): `scripts/symbols_ignore.txt`
  (one mangled name per line) **plus** every first-quoted string of every line of
  `gonk.toml` — i.e. the ignore list **and the whole extra_units list** are folded in.
- Extra-symbol baseline: `scripts/symbols_extra_baseline.txt` (first non-comment token;
  currently `144`). If `len(build_strong - orig_any)` exceeds it:
  exit code 2 with (check_symbols.py:157-163):

  > ERROR: extra symbols (%d) exceed baseline (%d).
  > New extra symbols must be documented; add them to the ignore list
  > (scripts/symbols_ignore.txt / gonk.toml) or bump the baseline
  > (scripts/symbols_extra_baseline.txt) before they can be merged.

- Exit codes: 0 = complete + within baseline; 1 = ≥1 original symbol missing; 2 = no
  missing but extras over baseline (missing always wins; --no-baseline / --baseline-extra
  override). Only the pre-commit hook runs it, and only when `res/libTTapp.so` exists.

## 6. gonk.toml

`ignore` (gonk.toml:1-6) removes symbols from gonk's view of the original entirely
(split.rs:650-655, applied in `Lib::from_elf`): they can't be carved into any TU,
extra, or remaining unit (none defines them), but they can still surface as
**undefined extern reloc targets** in carved objects — e.g. `U __udivdi3` in 8
`build/split/*.o` (and `U __umoddi3` in the two nucore ones): the ignore filter
only removes them from `symbols_by_name`; call-target resolution via
`symbols_by_address` and the `.plt`/dynsym path (split.rs:307, 474-520) are not
filtered. Current entries:
`__udivdi3`, `__umoddi3` (gcc 32-bit libgcc runtime helpers the build links from
libgcc, never from project code) and `ogg_stream_flush_fill` (libogg internal).
`check_symbols.py` inherits the same list via its regex fold (§5).

`[extra_units]` maps unit name → exact symbol list. **To edit**: add/remove symbol
names (both C and mangled C++ forms — a missing name silently drops that symbol, it
just stays in `remaining`), then rerun `gonk split` (regenerates the carved objects +
`objdiff.json`) and `objdiff-cli report generate`. Symbols are filtered **per
binary** — a listed name missing from one side's symtab is just skipped there (the
plain-C `ov_*` names exist only in `build/saga`; the mangled forms only in the
original).

## 7. Agent pitfalls (read before touching code)

1. **Adding/renaming/merging source files moves symbols between units → pairing churn
   with zero code change.** Unit names are the last-3 path components; gonk re-pairs by
   symbol name; objdiffdiff joins by name. A file rename therefore makes every function
   in it report as "lost full match" (pairing by name re-runs) even if the bytes are
   identical. How to tell: (a) the source file itself didn't change (`git diff`), (b)
   objdiffdiff.py prints the moved-unit note (current run: 14 lost, 11 of them also
   changed unit — `nufile/android/nufile_android.cpp` → `nu2api/nufile/nufile_android.cpp`
   with all 11 small functions reported lost at −100%). The verdict fields
   `matched_code`/`matched_functions` are immune (name-level, exact-100 counts).
2. **Duplicate same-name functions across files.** The original's
   `symbols_by_name` is a plain HashMap (split.rs:650-655): one entry per name. Two TUs
   defining the same function carve the *same* original symbol → one function appears
   in two units (verified: 268 function names appear in ≥2 units of report.json, and
   1,005 T/W definitions are duplicated across build TU objects); objdiffdiff's
   `index_by_name` keeps the first record (`setdefault`, objdiffdiff.py:78). Local
   `_ZL*` names legitimately repeat across TUs. Expect
   duplicate-name artifacts in the diff, not in the top-line numbers.
3. **Symbol name changes (mangling) break everything.** The mangled name is the key at
   every stage: gonk carve (by name), objdiff pairing (by name), check_symbols diff (by
   name). Rename a function and its original symbol is no longer claimed by any TU →
   it falls to `remaining` (unmatchable) and check_symbols flags it missing — while the
   new symbol is "extra". Verified empirically: renaming a `_ZLn…` static symbol in a
   base object drops its `match_percent` from 100.0 to unpaired.
4. **Tiny stubs flap.** < ~100-byte functions wobble wildly between reports; only
   ≥ 200-byte functions get partial-delta reporting (PARTIAL_SIZE_THRESH). Don't chase
   small stubs' fuzzy numbers.
5. **Local (static, `t`) symbols pair by name, not address** — verified in the
   code: the TU join is `original_lib.symbols_by_name.get(sym.name())`
   (split.rs:708-718) with no address anywhere; `symbols_by_address` is used only
   for call/GOT reloc recovery. Empirically, renaming a local in the base un-pairs
   it (objdiff-cli reports `match_percent: None`), and duplicated original locals
   collapse to one map entry — e.g. `VuVecSet` and
   `_Z41__static_initialization_and_destruction_0ii` each occur 37× in the
   original's symtab; `VuVecSet` (defined only in the original, not in the build)
   sits defined 37× in `build/split/remaining.c.o`. Pairing/join collisions are
   name-level.
6. **New extra exported symbols trip check_symbols.** Any new global `T`/`W` in
   `build/saga` that isn't in the original at any visibility counts against the baseline
   of 144. Fix: add to `scripts/symbols_ignore.txt` (or gonk.toml) or deliberately bump
   `scripts/symbols_extra_baseline.txt` with the justification in the commit.
7. **Never change a signature's mangled form once provisioned** — it's a permanent
   pairing contract (§3 above); changing it is pitfall #3.
8. **`remaining` can't be fuzzy-matched and only shrinks.** Its base is an empty stub
   with no scratch config, so its 3,748 functions / ~1.07 MB of code (report.json:
   `total_code` 1,067,465) never contribute to any matched/fuzzy metric — they only
   count toward `total_code` / `total_functions` / `total_data`. Code left unclaimed
   by a TU can only be counted by shrinking that total; matched progress requires
   implementing symbols in TU units (or adding extra_units for library-provided
   ones).

## 8. Workflow recipes

```bash
# Build + split + report (mirrors the pre-commit pipeline, minus git steps)
cmake -B build . && cmake --build build -j
cargo build --release --manifest-path gonk/Cargo.toml && ./gonk/target/release/gonk split
objdiff-cli report generate -o report.json          # reads ./objdiff.json; -p <dir> to select project

# Diff one function (one-shot JSON; per-symbol match_percent + per-instruction detail)
objdiff-cli diff -1 build/split/aiscript.cpp.o -2 build/CMakeFiles/saga.dir/src/gameapi/ai/aisys/aiscript.cpp.o \
  _ZL20Condition_AlwaysTrueP7AISYS_sP17AISCRIPTPROCESS_sP10AIPACKET_sPcPv -o -
#   -o <file>  one-shot JSON output (stdout with -o -); without -o it's the interactive TUI
#   the trailing <symbol> arg is IGNORED (whole unit is dumped) — filter with jq (08 §3d/§5)
#   -p <project> -u <unit>  resolve a unit from objdiff.json instead of raw -1/-2
#   (this objdiff-cli has NO "scratch" subcommand: report generate / report changes / diff)

# Compare two reports (the CI verdict, LaTeX-formatted markdown to stdout)
python3 scripts/objdiffdiff.py report.json report_old.json > report.md   # stdout is markdown; the committed report.md is a stale artifact (§4) — don't regenerate it

# CI equivalent (diff.yaml:31-38): BASE_SHA = PR base sha or github.event.before
git show $BASE_SHA:report.json > report_old.json
python scripts/objdiffdiff.py report.json report_old.json    # output posted as PR comment

# Symbol surface gate (local; needs res/libTTapp.so)
python3 scripts/check_symbols.py --list
```

## 9. What "fully matched" means in practice

Verified against report.json and the objdiff-cli output, without reading objdiff's
source (objdiff-cli is installed from `ttdecomp/objdiff` via cargo, pre-commit:10;
CONTRIBUTING.md:53 still cites the older `encounter/objdiff` URL; the source is not
vendored in `~/.cargo`):

- **Report level**: a function is counted in `matched_code`/`matched_functions` iff
  `fuzzy_match_percent == 100.0` **exactly** — sum-of-sizes of those functions equals
  `measures.matched_code` (71,971 bytes / 814 fns) and the 291 functions between 99 and
  100 are excluded. objdiffdiff.py uses its own FULL_MATCH = 99.999 threshold for
  classifying regressions, but the report's own counters are exact-100.
- **Diff level**: `objdiff-cli diff` emits per-symbol `match_percent` (plus
  `target_symbol`, `demangled_name`, `size`) and, even for exact-100.0 symbols,
  per-instruction records (formatted mnemonic + tokenized operand parts, with
  instruction address/size; `_Z16BuildHuffmanTreeP10DEFHUFFMANPhi` at 100.0 emits
  269 matched records, verified). Both sides carry **index-aligned** `instructions`
  arrays (a position tagged `DIFF_DELETE`/`DIFF_INSERT` has a null `instruction`
  on the absent side); the right/base side additionally carries
  `instruction.line_number` from `.debug_line` — the original (left) side has
  none. A 100.0 match
  shows instruction-for-instruction equality; a partial value reflects the share of
  instructions matched. So: fully matched = every instruction in the target function
  compares equal to the base's, per objdiff's matcher; a function at 99.99% is not
  matched for progress purposes. See 08 §3d/§4 for the jq views.

Open questions for future docs: none blocking — see the pitfalls for known-behavior
edges (duplicate names, remaining-unit blindness).
