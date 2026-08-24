# 00 — Decompiling for _saga_: knowledge base index

Agent-oriented knowledge base for the matching-decompilation of **LEGO Star Wars: The Complete Saga (Android x86)** — reimplementing `res/libTTapp.so` in C/C++ such that the recompiled binary byte-matches the original, instruction for instruction, per translation unit.

All claims below were verified by live experiments with the actual toolchain (Aug 2026). Numbers cite HEAD `755ab13`.

## 1. What this is matching

| | |
|---|---|
| Target | `res/libTTapp.so` — ELF32 DYN, i386, unstripped (full `.symtab`), 7,883,289 B |
| Compiler | GCC 4.7 (NDK r8e `x86-4.7`), `i686-linux-android-g++`; linker gold 1.11 (matches original `.comment`/gold note) |
| Optimization | **per-file**, default **-O0**; 25× -O2, 74× -O3, 1× (-O3 -fPIE `cheat.cpp`) among 291 TUs |
| ABI | cdecl-only (i686), PIC on by default, SSE2 math for float **and** double, x87 only for return values and i64↔double conversions |
| STL | **none** (`cxx-stl/system` = bionic minimal runtime; no `std::string`/`vector`) |
| Matching | gonk carves original symbols into `build/split/*.o`; objdiff compares recompiled vs carved **by symbol name** |

## 2. The files

| file | contents |
|---|---|
| [01-toolchain.md](01-toolchain.md) | Exact compiler/flags, per-file -O derivation, libgcc helpers, linking, host vs target build, external libs |
| [02-codegen.md](02-codegen.md) | **GCC 4.7 x86-32 codegen encyclopedia**: every construct → exact assembly per -O level, with verified snippets. Read this before writing any function |
| [03-matching.md](03-matching.md) | gonk/objdiff mechanics (pairing by name), report interpretation, check_symbols rules, CI/pre-commit pipeline, pairing pitfalls |
| [04-types-abi.md](04-types-abi.md) | Mangling table (int=`_i` vs long=`_l` trap), calling conventions, struct layout, vtables, C1/C2/D1/D2 variants, `_ZThn` thunks, static guards |
| [05-source-conventions.md](05-source-conventions.md) | Source layout, function→file heuristics, stub idioms, `_plain`/`_misc`/`_types` patterns, naming rules, authoring checklist |
| [06-target-binary.md](06-target-binary.md) | Target binary reference: segments, symbol census, TU identification, address conventions (nm vs Ghidra +0x10000), Ghidra state |
| [07-diagnostics.md](07-diagnostics.md) | Mismatch investigation loop + catalog of 16 mismatch causes with verified fixes |
| [08-asm-review.md](08-asm-review.md) | Command-only assembly-diff review workflow (symbol→TU→classified list→text diff→source interleave); the extraction half of 07 §A |
| [09-objdiff-cli.md](09-objdiff-cli.md) | **Per-symbol diffing with `scripts/objdiff-cli.py`** — the primary tool for agents to inspect one function's mismatch; vs `objdiff` (GUI/CLI) for manual and whole-repo work |

## 3. Agent routing (read this first)

| You are about to… | Read |
|---|---|
| Write or fix a function body | 02 (codegen), 04 (types), 05 (naming/stubs); check the file's -O level (01 §opt) |
| Investigate a non-matching function | 07 (workflow), 08 (asm-level commands), 03 §8 (get per-function numbers), 06 §7 (find its TU) |
| See exactly where a single function diverges from the original | **09** (`scripts/objdiff-cli.py <symbol>`) — the primary per-symbol diff tool |
| Review a non-matching function's assembly | 08 (full workflow: classified list, text diff, source interleave), 07 §C (causes) |
| Find why a function mismatches at asm level | 08 §3d/e (extract the diff) → 07 §B/C (catalog + decision tree) |
| Add a new source file / rename one | 03 §7 (pairing pitfalls!) + 05 §authoring |
| Change a function signature | 04 (mangling — changes break pairing, never alter provisioned symbols) |
| Understand an address from Ghidra | 06 §address-convention (Ghidra = ELF vaddr + 0x10000) |
| Pick a type for a global | 04 §type-choice recipes; check `.data` vs `.bss` placement (07 cat. 15) |
| Find out which file owns a symbol | 06 §7 (`nm` lookup recipe) |

## 4. Verified core facts (the "surprise list")

Things about this compiler that differ from mainstream GCC expectations — all verified:

- **Doubles use SSE2** (`movsd/mulsd/addsd`), not x87; x87 appears only for the `st(0)` return round-trip and i64↔double conversions (`fildll`/`fisttpll`). `__SSE2_MATH__` is defined by default. (02 §5)
- **No tail-call optimization** under PIC: `return f(x);` compiles to `call`+`ret`. (02 §9, 07 cat. 12)
- **No `cltd`**: signed division preamble is `mov %eax,%edx; sar $0x1f,%edx`. (02 §4)
- **Division-by-constant magic at -O0** as well: `x/7` → `imul $0x92492493` even unoptimized. (02 §4)
- **All struct returns use a hidden sret pointer** (`-fpcc-struct-return`): even 4-byte PODs return via `ret $4`. (04 §3, 02 §9)
- **`a > b` vs `b < a` are NOT byte-identical** (mirrored `cmp`/`jcc`); equality-symmetric rewrites like `if`/`else` vs early-return ARE identical. (07 cat. 3)
- **bool return at -O2/-O3 = bare `setcc` + NOP padding** (no `movzbl`); int return adds `movzbl`. (02 §3, 07 cat. 5)
- **`&&`/`||` on pure conditions at -O2/-O3 → `setcc` + `andl/orl`** with both operands evaluated, zero branches. (02 §3)
- **`__builtin_expect` is layout-affecting**: `expect(c,0)` mirrors the branch / demotes cmov; not a no-op. (07 §D.2)
- **-O0 frames**: `push %ebp` + `mov`/`lea (%esp),%ebp`; stack via `lea -N(%esp),%esp`, never `sub`; `mov` form dominates 7:1 in the real binary. (02 §1)
- **`int x = 0;` lands in `.bss`**, not `.data`. (07 cat. 15)
- **String literal pools follow first-use order** at all -O levels. (07 cat. 10)
- **Constant stores never force reloads**; only `volatile` forces in-loop reloads. (07 cat. 7)
- **Static-local guards** (`_ZGVZ…` + `__cxa_guard_acquire/release`) are emitted even with `-fno-exceptions`. (04 §8)
- 64-bit division pulls libgcc helpers (`__udivdi3`/`__divdi3`/`__umoddi3`/`__moddi3`); multiply/shift/add/sub/float-conversions are inline. The original embeds `__udivdi3`/`__umoddi3` — the gonk ignore list. (01 §libgcc)

## 5. Non-negotiables (violating these breaks matching, not just accuracy)

1. **Mangled names must match exactly** — int vs long, signedness, `long long`, `size_t` vs `ssize_t` all change the mangled symbol (`_i` vs `_l` …). See 04 §1. Use `abi_long`/`abi_ulong` where the original's signature has `long` (04 §2).
2. **Never change a provisioned signature** once a symbol is stubbed — it re-pairs or vanishes. (03 §7)
3. **Pairing is by symbol name, for local symbols too** — renaming/merging/adding files causes pairing churn with zero code change. (03 §2, §7)
4. **`extern "C"` decision must match the symbol**: plain name ⇒ C (`extern "C"`), `_Z…` ⇒ C++. (04 §9)
5. **New exported symbols are capped**: extras beyond baseline 144 fail `check_symbols.py` (exit 2). (03 §5)
6. **No STL**: write `char*`-based code, own containers. (01 §runtime)
7. **Per-file -O level is part of the contract** — never change `target.cmake` for one function; verify what the file is compiled at (01 §opt).

## 6. Workflow in one screen

```bash
# build + match + report (pre-commit does this automatically)
cmake -B build && cmake --build build -j
gonk split                                  # carve original → build/split/*.o (by name)
objdiff-cli report generate -o report.json  # compare recompiled vs split
python3 scripts/objdiffdiff.py report.json report_old.json   # human diff vs last CI report
python3 scripts/check_symbols.py            # symbol-surface gate (exit 1/2)
```

Verification of a single function: `nm res/libTTapp.so | grep <sym>` → find TU via `nm -A build/split/*.o | grep <sym>` → objdump both `.o`s, diff (08 §6 block A; the old condensed loop is 07 §A).

## 7. Key repo paths

| path | role |
|---|---|
| `src/target.cmake` | per-file optimization levels (the -O contract) |
| `build/compile_commands.json` | exact per-file compile commands (source of truth; regenerate via cmake) |
| `gonk.toml` | ignore list (`__udivdi3`, `__umoddi3`, `ogg_stream_flush_fill`) + `extra_units` (ogg_vorbis) |
| `gonk/` | gonk source (split.rs: split/pairing logic) |
| `objdiff.json` | objdiff config: 293 units, scratch = `ndk-r8e-gcc-4.7` |
| `scripts/check_symbols.py`, `scripts/objdiffdiff.py` | gates + report diff (docstrings = semantics) |
| `scripts/symbols_extra_baseline.txt` | extra-symbol baseline (144) |
| `.githooks/pre-commit` | the full pipeline |
| `src/decomp.h` | LOG/UNIMPLEMENTED (host-only), `SAGA_NOMATCH`, undefined/byte/dword |
| `src/common.h`, `src/fixed_width.h` | type system (i32/u32…, `abi_long`, no `variptr_u`) |
| `res/libTTapp.so` | the original target |

## 8. Authoring checklist (per new function)

1. Find the symbol in the binary (`nm res/libTTapp.so`), note exact spelling/mangling.
2. Find its TU: `nm -A build/split/*.o | grep <sym>`; map TU basename → src/ file (06 §7, 05 §heuristics).
3. Check the file's -O level in `src/target.cmake` (absent = -O0).
4. Write the signature so the mangled name matches (04 §1); keep the `extern "C"` correct.
5. Write the body per 02-codegen patterns for that -O level; if incomplete, use the stub idiom (05 §stubs).
6. Rebuild, re-split, check the symbol still pairs and the function's fuzzy score moved (03 §8).
