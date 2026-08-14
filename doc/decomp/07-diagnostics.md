# 07 — Diagnostics: mismatch investigation & fix catalog

Playbook for diagnosing why a recompiled function does not byte-match the original.
Scope: LEGO Star Wars: TCS (Android x86), matched against `res/libTTapp.so` via gonk
(`build/split/*.o`) + objdiff. Compiler: `i686-linux-android-g++` 4.7 (NDK r8e),
per-file `-O0/-O2/-O3` from `src/target.cmake`, PIC default, `-fno-function-sections
-fno-data-sections`. Every claim below was verified by experiment; all experiments
live in `/tmp/opencode/saga-experiments/a7/` (per-file sources `e*.cpp`, outputs
`e*.o`; compile via `source env.sh && build eN.o -O<level> eN.cpp`).
Independently re-verified in `/tmp/opencode/saga-experiments/verify7/` (same
toolchain; sections with re-verified/spot-check results cite `verify7/e*.cpp`).

Sibling docs: 01 (toolchain + canonical compile command), 03 (matching workflow),
05 (source conventions), 06 (target binary + Ghidra address convention).

---

## 0. Pipeline recap (1 min)

| stage | artifact | symbol names |
|---|---|---|
| original binary | `res/libTTapp.so` | debug-stripped (0 DWARF sections), full `.symtab` kept — nm-able (06 §1) |
| carved TUs | `build/split/*.o` | original symbol names, relocations intact, **no debug info** |
| recompiled TUs | `build/CMakeFiles/saga.dir/**/*.o` | same names, **has `.debug_line`** (`objdump -S` works) |
| pairing | `objdiff.json` (`target_path`/`base_path`), `report.json` | per-function `fuzzy_match_percent` |

## A. Investigation loop (exact commands)

> Steps 3–4 below are the condensed pre-08 version; the full command-line review
> (symbol→TU→classified `objdiff-cli diff` list→normalized objdump text diff→`-S`
> interleave) lives in [08-asm-review.md](08-asm-review.md) §3, with copy-paste
> blocks in §6. Use that doc for these steps; everything here still works.

1. **Which TU owns the symbol?**
   ```
   cd /home/fabian/git/saga
   nm build/split/*.o | grep -w _Z5qrandv        # mangled; use -C or the mangled name from report.json
   # build/split/qrand.cpp.o: 00000000 T _Z5qrandv
   ```
   If nothing: symbol is not in this project's TU set (libogg etc. in `gonk.toml` `extra_units`,
   or `ignore` list — those are library code, not your problem).

2. **Optimization level of that TU** — `grep <file> src/target.cmake`:
   ```
   grep qrand src/target.cmake    # no hit → -O0
   grep players src/target.cmake  # set_source_files_properties(... players.cpp PROPERTIES COMPILE_OPTIONS "-O2")
   ```
   Absent = `-O0`. 100 files are listed: 74×`-O3`, 25×`-O2`, 1×`-O3;-fPIE`
   (cheat.cpp), rest `-O0`. For the
   exact per-TU command line (includes `-O`, `-g`, `-std=gnu++11`, `-DANDROID`):
   `python3 -c 'import json;[print(e["command"]) for e in json.load(open("build/compile_commands.json")) if "qrand" in e["file"]]'`
   (291 entries; `target.cmake` is where fixes go, `compile_commands.json` is the truth for what was built).

3. **Per-symbol numbers**: `objdiff-cli -C /home/fabian/git/saga report generate`
   prints the report JSON to stdout (repo's `report.json`/`report.md` are committed
   snapshots — treat as read-only). Per-symbol view = grep the JSON:
   ```
   objdiff-cli -C . report generate | python3 -c \
     'import json,sys; r=json.load(sys.stdin); print([(u["name"],f["name"],f.get("fuzzy_match_percent"),f.get("size")) for u in r["units"] for f in u.get("functions",[]) if "qrand" in f["name"]])'
   # [('src/legoapi/qrand.cpp', '_Z5qrandv', 100.0, '34')]
   ```
    Six source-orphaned units (e.g. `saga/src/globals.cpp`, `nu2api/nu3d/nutexanm.c`)
    have no `functions` key — ignore; the `ogg_vorbis` extra_units unit has 36 and is
    queryable like any other. A `fuzzy_match_percent`
    of 100 means the function *bytes* already match; 0–99 means disassemble.

4. **Extract both function bodies and diff**:
   ```
   OJ=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-objdump
   $OJ -S build/CMakeFiles/saga.dir/src/legoapi/qrand.cpp.o > /tmp/recompiled.asm   # -S interleaves source: .o HAS .debug_line
   $OJ -d build/split/qrand.cpp.o                          > /tmp/original.asm      # -S useless: split .o has NO debug info
   diff /tmp/original.asm /tmp/recompiled.asm
   ```
   Recompiled objects carry `.debug_line` (verified), so `-S` shows your source
   lines; the split objects do not. PIC artifacts to ignore in both: the leading
   `call __x86.get_pc_thunk.*`, `add $0x2,%ebx`, and GOT-relative `mov 0x0(%reg)`.

5. **Cross-check the original in Ghidra** (MCP, project `LegoDecompilation`, program
   `libTTapp.so` — see 06 for project state). Address convention (06, §0): ELF loads
   at 0x0, Ghidra rebased to `0x10000`; **file address → Ghidra address = `+0x10000`**:
   ```
   nm res/libTTapp.so | grep _Z5qrandv        # 00490210 T  (file address)
   # Ghidra: decompile_function("0x4a0210")  (= 0x490210 + 0x10000)
   ```
   `decompile_function(0x4a0210)` gives `qseed = qseed * 0x24cd + 1 & 0xffff;`
   — exactly the recompiled source. Ghidra is the tie-breaker for type questions
   (byte vs int, signed vs unsigned); the split-object asm is the ground truth for
   layout/order questions.

## B. Mismatch catalog

For each: **symptom**, **cause**, **verified snippet** (`/tmp/opencode/saga-experiments/a7/e*.cpp`, at the -O level stated), **fix**.

### 1. Wrong optimization level
- Symptom: shape is completely different — e.g. -O0 emits `push ebp; lea (esp),ebp`, per-use stack reloads, branch + jmp; -O3 emits cmov on both-sides-computed values.
- Cause: TU compiled at the wrong `-O` (default -O0 when `target.cmake` has no entry).
- Verified: `e1.cpp` `int f(a,b){ if(a>b) return a+b; return a-b; }`
  - -O0: `cmp 0xc(%ebp),%eax / jle / mov 0xc(%ebp),%eax / mov 0x8(%ebp),%edx / add / jmp` (branch, 4 reloads)
  - -O3: `sub %edx,%esi / add %edx,%eax / cmp %edx,%ecx / cmovle %esi,%eax` (both sides, 0 branches)
- Fix: add `set_source_files_properties(... COMPILE_OPTIONS "-O3")` (or -O2) in `src/target.cmake`; rebuild; recheck. Re-verified (`verify7/e1b.cpp`): for these value-computing if/else shapes **-O2 and -O3 compile byte-identically** (same cmov; `if(a>b) return 1; return 0;` gives `setg %al` + `movzbl %al,%eax` at both levels) — the real cross-level tell is the -O0 stack frame + per-use reloads. A "setcc vs cmov" diff is not a -O2-vs-O3 signal on this compiler.

### 2. Signedness of char/int
- Symptom: `movsbl` vs `movzbl` on a byte load; `cmpb` on a byte vs `cmpl` on a dword; signed condition `setg/setle/jg/jle` vs unsigned `seta/setbe/ja/jbe` for the same-looking comparison.
- Cause: `char` (signed on this target) vs `unsigned char` / `i8` vs `u8` in the struct or parameter.
- Verified: `e2c.cpp` at -O2, `p[0] < 5`:
  - `char *`: `movzbl (%eax),%edx / movsbl %dl,%ecx / cmp $0x5,%dl / cmovge %ecx,%eax`
  - `unsigned char *`: `movzbl %dl,%ecx / cmp $0x5,%dl / cmovae %ecx,%eax`
  - `e2b.cpp`: `gs > 0` → `setg`; `gu > 0` → `setne` (compiler knows uchar ≥ 0).
- Fix: pick `char` vs `unsigned char` to match the extension/condition. Where Ghidra shows an *undefined* byte, infer from the asm: `movsbl`/`setg`/`jge` ⇒ signed; `movzbl`/`seta`/`jae` ⇒ unsigned; equality (`cmpb; sete`) is ambiguous — look at a second use.

### 3. Comparison structure (branch shape)
- Symptom: same semantics, different branch layout: jcc target, epilogue placement, duplicated vs shared exit blocks, setcc-AND vs branch.
- Cause: `a>b` vs `b<a` mirror; `&&` vs nested `if`; `||` vs separate `if`s; early-return vs if-else.
- Verified:
  - `e3b.cpp` -O0, `h1: if(a>b) return a*2; return b*3;` == `h3: if(a>b){...}else{...}` — **byte-identical**. So early-return ≡ if-else; never chase this.
  - `e3b.cpp`: `a>b` gives `cmp 0xc(%ebp),%eax / jle`; `b<a` gives `cmp 0x8(%ebp),%eax / jge` — same body, mirrored cmp/jcc pair. **Not byte-identical**: to match, flip the source comparison.
  - `e3c.cpp` -O2 with side-effect calls: `&&` (`k1`) = second call skipped via `jg 30` to a shared exit, return-0 block *inside* the middle; nested `if` (`k2`) = `xor %edx,%edx` pre-zero + inner `setg %dl`; `||` (`k3`) = return-1 reached early, `jle c0` continues; separate `if`s (`k4`) = two independent `mov $1 + jg` shapes.
  - `e3.cpp` -O2 side-effect-free: `&&` compiles to `setg/setg/and` (no branch!), `||` to `setg/setg/or`; only the nested/separate forms branch.
- Fix: restructure to reproduce the original branch shape — nested `if` to get a branch; `&&`/`||` to get setcc folds (pure conditions) or short-circuit (impure); swap `>`/`<` to mirror the cmp/jcc pair.

### 4. Ternary vs if-else vs assignment+return
- Symptom: cmov with `cmovle` + reversed operand order vs `cmovg`; `lea (%ecx,%edx,1)` vs `sub`+`add`.
- Verified: `e4.cpp` -O3: `a>b ? a+b : a-b` == `if/else` — **byte-identical** (both `sub %edx,%esi / add %edx,%eax / cmovle %esi,%eax`). But `int r = a-b; if (a>b) r = a+b; return r;` → `sub %edx,%eax / lea (%ecx,%edx,1),%esi / cmovg %esi,%eax` — different computation order and cmov polarity.
- Fix: if the original computes the "default" first, write assignment+if; otherwise ternary or if-else. All three are semantically equal, only the byte shapes differ.

### 5. bool vs int return of a comparison
- Symptom: `setcc %al` alone (bool) vs `setcc %al; movzbl %al,%eax` (int) at -O2/-O3.
- Verified: `e5.cpp` -O2: `bool r1(a,b){return a==b;}` → `sete %al` + NOP padding (no movzbl!); `int r2` and `int r5` (if-else 1/0) → `sete %al / movzbl %al,%eax`. Order is always cmp → setcc → movzbl; the int result and `if(cmp) r=1; else r=0;` are identical.
- Fix: match the declared return type (`bool`/`i8` vs `int`). Note the bool case leaves the top 24 bits of %eax stale — if the original has only `setcc`, the source returns bool; if movzbl follows, it returns int.

### 6. Float/double
- Symptom: `movss/addss/cvtsd2ss` vs `movsd/addsd/cvtss2sd`; `ucomiss` vs `ucomisd`; `flds` vs `fldl`; extra `cvtss2sd` round-trips.
- Verified: `e6.cpp` -O2: `double a+b` → `movsd/addsd` + `fldl` (return in ST(0) — x87!); `float a+b` → `movss/addss` + `flds`; `(float)a+(float)b` ≡ `(float)(a+b)` when args are already float (the cast is a no-op); but `float` mixed through a `double` temp → `cvtss2sd, addsd, cvtsd2ss` (totally different). `a > b`: float → `ucomiss`, double → `ucomisd` (SSE2, no x87 compare).
- Fix: match arithmetic type to the original (read Ghidra or count `movss` vs `movsd`). On 32-bit, return values always end with `flds`/`fldl` regardless — don't mistake that for x87 math.

### 7. Missing volatile / cached loads
- Symptom: a global read once outside a loop (hoisted/cached) vs reloaded on every iteration; `add %eax,%eax` (cached) vs two separate `mov (%edx),%reg` (reloaded).
- Verified: `e7.cpp` -O3: `loop1` reading global `gcount` with no store in the loop → load hoisted, loop collapses to `imul (%edx),%eax`. `loop2` with `s += gcount; gcount = 0;` per iteration → the load is **hoisted above the loop too** (`mov (%edi),%ecx` once before the loop body) and the store is hoisted below it (single `movl $0,(%edi)` after the loop); the only in-loop residue is `xor %ecx,%ecx`. So a *constant* store does NOT produce per-iteration reloads on GCC 4.7. `e7b.cpp`: `volatile int vcount` genuinely forces an in-loop reload (`mov (%edi),%ecx` **inside** the loop) and `vread` loads the global **twice** (`mov (%edx),%ecx; mov (%edx),%eax; add`). Even a store whose value comes from an opaque call (`gcount = opaque(i);`) keeps the load in a register across iterations on this compiler — the loaded value is just the previous iteration's stored register value.
- Fix: a plain in-loop store does **not** force reloads on this compiler (both load and store get hoisted/folded). If the original reloads per iteration, the global must be `volatile` in the original, or the load must be clobbered by something GCC 4.7 cannot keep in a register (a call that may modify it and whose value the store doesn't carry). Reproduce it as `volatile` in the source; if the original caches, keep the source store-free (and non-volatile).

### 8. 64-bit ops
- Symptom: same `a / b` source shape, but the call target differs — `__udivdi3` vs `__divdi3`; `%` → `__umoddi3`. Plus `fildll` (i64→double) vs `fisttpll` (double→i64) on x87.
- Verified: `e8.cpp` -O2 (`objdump -r`): `u64/u64` → `R_386_PLT32 __udivdi3`; `i64/i64` → `__divdi3`; `u64%u64` → `__umoddi3`; `(double)i64` → `fildll`; `(i64)double` → `fisttpll`.
- Fix: match operand signedness in the source. Spot via `nm build/split/*.o | grep di3` to find which TUs do 64-bit division, and the reloc `objdump -r` in both .o files reveals the helper name (split objects retain relocs).

### 9. Struct layout
- Symptom: field offset differs (`movsbl 0x8(%eax)` vs `movsbl 0xc(%eax)`); access width differs (`movl 0x4(%eax)` vs `movb`).
- Verified: `e9.cpp` -O2, `struct S9 { char a; char b; int c; char d; }` (a@0,b@1,c@4,d@8): `p->c + p->d` → `movsbl 0x8(%edx)` + `add 0x4(%edx)`; `p->d > 5` → `cmpb $0x5,0x8(%eax)`.
- Fix: correct the struct in the `*_types.h` header (e.g. `src/gameapi/edtools/gameapi_edtools_types.h`); confirm layout against the Ghidra struct via `get_struct_layout`. Width tells you the field type (dword = int, byte = char/bool).

### 10. String/rodata ordering
- Symptom: `.rodata` (or `.rodata.str1.1` at -O3) literal pool order differs; string offsets in `lea 0xd(%ebx)` vs `lea 0x0(%ebx)`.
- Cause: literal pools are emitted in **first-use order** — the same strings referenced in a different order produce a different pool.
- Verified: `e10a.cpp` uses `"alpha string"` then `"beta string"`; `e10b.cpp` swaps the use order. At -O0 the `.rodata` pool swaps (`alpha,b` vs `beta,a`); at -O3 the strings land in merged `.rodata.str1.1` and **also keep use order** (no sorting at any level).
- Fix: match the *usage* order of the literals in the source. Interleaved uses within one function emit the pool in that function's use order.

### 11. Static-init / global order (brief)
- Symptom: `.init_array`/`.data.rel.ro` section mismatches across the whole unit; `_GLOBAL__sub_I_<tu>` per-TU functions.
- Verified: `e11c.cpp`: a static object with a ctor yields `_GLOBAL__sub_I_e11c.cpp` (named after the TU; the ctor inlined into it). `e11.cpp`: trivial static locals need **no guard variable** (`static int n = 3;` lives directly in `.data`, `static int m = 0;` in `.bss`).
- Fix: this is a unit-level artifact — match the TU contents (static objects with ctors in the same order), not usually per-function. Don't chase guard-variable mismatches for trivial int/pointer statics: there are none.

### 12. Tail call vs call+ret
- Symptom: a trailing `jmp` (tail call) vs `call ...; ret`.
- Verified surprise: `e12.cpp` -O3 with **PIC** (project default): `return work(a);` compiles to `call + ret` — **no tail-call optimization at all** (also at -O2 PIC). Only with `-fno-pic` does GCC 4.7 emit `jmp` (at -O2 and -O3). The real binary is PIC (10k+ `__x86.get_pc_thunk` refs), so the original never tail-calls either.
- Fix: nothing to do — plain `return f(x);` already matches. A trailing `jmp` in the original is a jump table or a goto, not a tail call. Jump tables (spot-check, `verify7/e18*.cpp`): GCC 4.7 emits **no table at -O0** (compare chain) and no table for few cases at -O3 (compare tree); with ≥8 dense cases at -O3 it emits a PIC-relative offset table — `mov 0x0(%ebx,%eax,4),%eax / add %ebx,%eax / jmp *%eax` with `R_386_GOTOFF .rodata` entries (one 4-byte GOT-relative offset per case). A switch of *constant returns* at -O3 compiles instead to a **value table**: `cmp $0x3,%edx / ja default / mov 0x0(%ecx,%edx,4),%eax` — the .rodata holds the return constants, no jump at all.

### 13. Inlining at -O3
- Symptom: helper body duplicated inline vs an outlined symbol + `call`.
- Verified: `e13.cpp` -O3: single-use `static` helper → fully inlined (no symbol in `nm`); `e13b.cpp`: same helper **non-static** → outlined `_Z6helperi` + `call` from the caller. Re-verified (`verify7/e15*.cpp`): at **-O2 and -O3** a called-once static is always inlined; at **-O0 it is NOT** — the helper stays outlined as a local symbol (`t _ZL6helperi`) and the caller emits `call`. The `-O0` case is indistinguishable from a non-static helper except for the symbol binding (`t`/`T`).
- Fix: check `nm res/libTTapp.so | grep <helper>` (or `nm build/split/<tu>.o`): symbol present ⇒ make the helper non-static/extern in your source; absent ⇒ keep it `static` (or inline the body). At -O0 a static helper *will* appear in `nm` as lowercase `t` — don't be fooled into making it extern; check the TU's -O level first (step A.2).

### 14. Non-obvious
- Symptom: argument values stored in swapped slots; extra `mov` reloads of args; `ret $0x4` vs plain `ret`; calls through a hidden pointer arg.
- Verified: `e14.cpp` -O2, `fx((*p)++, (*p)++)`: the **second** expression is evaluated and stored first (`mov %eax,0x4(%esp)` = arg2, then arg1 at 0(%esp)) — right-to-left, constant on this target. `e14b.cpp` -O0: every arg reloaded from `0x8/0xc/0x10(%ebp)` per use, including the trailing `+ a`. `e14.cpp` `P mkp(int a)`: sret pointer is the hidden **first** arg (read from `0x4(%esp)`, real arg at `0x8(%esp)`), caller passes `lea local,%eax` into slot 0, and the callee ends `ret $0x4` (marker!). Re-verified (`verify7/e10*.cpp`): on this toolchain **every** struct return uses the hidden sret arg + `ret $0x4` — even 1-, 4- and 8-byte POD structs (no register returns); a 1-byte `struct {char c;}` still ends `ret $0x4`. `__builtin_expect(a>b,0)` vs `(,1)` (`bexp/bexp2`): GCC 4.7 x86 emits **no branch-hint prefixes** (there is nothing to emit), but the hint is **not a no-op** — see surprise 2 below; in this particular e14 shape the branch was lowered to arithmetic, so the two bodies differ only in register allocation (`ba 05...01 d0` vs `b9 05...01 c8`) and are **not** byte-identical.
- Fix: match the source-level evaluation order (swap the two `(*p)++` args if the slots are mirrored); match struct-return (sret) shape by keeping the same return type — any struct type produces the hidden pointer + `ret $4`; the `ret $N` markers identify sret functions in both objects. Don't rely on `__builtin_expect` being a no-op: it reorders branches (see surprise 2).

### 15. Globals in .data vs .bss
- Symptom: `D` vs `B` in `nm` for the same-named global; section sizes in the unit diff.
- Verified: `e15.cpp` -O2: `int z1 = 0;` → **.bss** (B) — `=0` still goes to .bss; `int z2;` → .bss; `int nz = 1;` → .data (D); `const char *s1 = 0;` → .bss; `static int sz = 0;` → .bss; `static int snz = 42;` → .data.
- Fix: match the *section*, not the written initializer: `=0` (or uninitialized) must stay `=0`-style in the source. Grep the original: `nm build/split/<tu>.o | grep <sym>` shows `B` or `D` — if the original is `B`, any nonzero initializer in your source is wrong.

### 16. Function ordering within a TU
- Symptom: function bodies are correct but `.text` layout differs between the .o files; objdiff reports unit mismatch while individual functions match.
- Cause: with `-fno-function-sections` (set in CMakeLists), `.text` order = definition order.
- Verified: `e16.cpp` -O2: `aaa, bbb(static), ccc, ddd` → `.text` contains `_Z3aaai, _Z3ccci, _Z3dddi` in definition order (inlined `bbb` vanishes, order of the rest preserved).
- Fix: get the original order — `$OJ -d build/split/<tu>.o | grep '^[0-9a-f]* <'` — then reorder definitions in the source to match. Don't rely on the .cpp's old ordering after merging in functions from Ghidra.

## B2. Category × optimization level applicability

| # | Category | -O0 | -O2 | -O3 | Notes |
|---|---|---|---|---|---|
| 1 | wrong opt level | — | — | — | cross-level comparison *is* the symptom |
| 2 | signedness | reloads, `movsbl`/`cmpl` | `setg` vs `seta`, `cmovge` vs `cmovae` | same as -O2 | equality compares: ambiguous alone |
| 3 | comparison structure | branches, jcc targets | setcc-folds for pure conds | same as -O2 | epilogue-sharing varies per level |
| 4 | ternary vs assign | — | — | cmov ordering | verified cmov polarity at -O3 |
| 5 | bool vs int return | `setcc` w/wo `movzbl` | `setcc` w/wo `movzbl` | same as -O2 | bool = bare `setcc` (+nops); no store for scalar args |
| 6 | float/double | same patterns | `movss`/`movsd` + `flds`/`fldl` | same | return always via x87 ST(0) |
| 7 | volatile/cached | N/A (reloads anyway) | hoisting starts | hoists aggressively | constant in-loop stores hoist too — only `volatile` reloads |
| 8 | 64-bit ops | same helpers | same | same | helper chosen by *signedness only* |
| 9 | struct layout | same | same | same | level-independent |
| 10 | rodata order | `.rodata` first-use | same | `.rodata.str1.1` merged, still first-use | no sorting ever |
| 11 | static init | — | — | — | TU-level artifact |
| 12 | tail call | N/A (PIC) | N/A | N/A | never occurs under PIC |
| 13 | inlining | static helper **not** inlined (`t _ZL...` + call) | all single-use static | all single-use static | helper symbol in `nm` = extern (at -O0 even static shows up, lowercase `t`) |
| 14 | eval order / sret | arg reloads everywhere | `ret $N` marker | — | sret pointer = arg slot 0; **all** struct returns use it (even 1-byte) |
| 15 | .data vs .bss | same | same | same | `=0` still `.bss` |
| 16 | TU ordering | same | same | same | `-fno-function-sections` fixed |

## C. Decision tree — symptom → category

| Symptom (diff on the failing function) | Category | First check |
|---|---|---|
| Whole function different shape: stack frame + reloads vs cmov vs setcc | 1 opt level | grep `target.cmake` |
| `movsbl`/`setg`/`jg` vs `movzbl`/`seta`/`jae` on same operand | 2 signedness | Ghidra type / asm extension |
| Semantics equal, branches/jcc targets/epilogue differ | 3 comparison structure | mirror `>`/`<`, nested vs `&&`, early-return |
| `cmovle` + reversed op order vs `cmovg` + `lea` | 4 ternary vs assign+if | write default first |
| `setcc` w/o movzbl vs w/ movzbl | 5 bool vs int return | change return type |
| `movss`/`ucomiss` vs `movsd`/`ucomisd`/extra cvt | 6 float vs double | match arithmetic type |
| Global load hoisted vs reloaded per loop iter | 7 volatile/aliasing | store in loop / volatile |
| Same division, different helper (`udivdi3` vs `divdi3`) | 8 64-bit signedness | `objdump -r` + `nm \| grep di3` |
| `movsbl 0x8(%r)` vs `0xc(%r)`; `movl` vs `movb` on same field | 9 struct layout | fix `*_types.h`, check Ghidra struct |
| String pool order / `lea` offsets differ | 10 rodata order | match literal use order |
| `.init_array`/`.data.rel.ro` differ unit-wide | 11 static init | TU-level; reorder ctors |
| trailing `jmp` vs `call;ret` | 12 tail call | PIC ⇒ never; ignore |
| helper body duplicated vs `call` + symbol in `nm` | 13 inlining | static vs extern helper |
| swapped arg slots, `ret $N`, missing reload | 14 eval order / sret | swap arg order; match return type |
| Global `B` vs `D` in `nm` | 15 .data/.bss | drop non-zero init |
| Bodies match, `.text` order differs | 16 TU ordering | reorder definitions |

## D. Verified surprises (things the catalog disproved)

1. **No tail calls under PIC** — `return f(x);` is `call+ret` even at -O3 (e12); only `-fno-pic` gives `jmp`. Do not restructure code to "create" tail calls.
2. **`__builtin_expect` is NOT a no-op on GCC 4.7 x86** — it emits no hint *prefixes* (there is nothing to emit on x86), but when its value feeds a branch GCC uses it to choose branch layout: `__builtin_expect(c,1)` ≡ plain `if(c)` byte-for-byte, while `__builtin_expect(c,0)` mirrors the branch (e.g. `je`↔`jne`, `jg`↔`jle` swapped) and at -O3 turns a cmov into a branch. Only when GCC lowers the condition to arithmetic (e.g. e14's `bexp`, where the branch became setcc+shl+add) does the hint have nothing to influence — and even then the two functions differed in register allocation (`ba 05/01 d0` vs `b9 05/01 c8`), so they were **not** byte-identical. Treat likely/unlikely as layout-affecting, not as free.
3. **`if(a>b) return X; return Y;` ≡ if-else** byte-for-byte, but `a>b` vs `b<a` is NOT byte-identical (mirrored `cmp`/`jcc` pair, e3b).
4. **Ternary ≡ if-else** at -O3; only assignment-then-override differs (e4).
5. **bool returns skip `movzbl`** at -O2/-O3 — a `setcc` followed by NOP padding means the source returns `bool` (e5).
6. **`int x = 0;` goes to `.bss`** even with the explicit `=0` initializer (e15).
7. **`&&`/`||` on pure conditions compile to `setcc`+`and`/`or`, no branch at all** (e3) — nested ifs are the only way to get a branch shape.
8. **Ghidra is rebased +0x10000** (06, §0): `nm`/`objdump` output is file-address, Ghidra output is rebased — `qrand` 0x490210 ↔ 0x4a0210. Convert before comparing; never paste a file address into Ghidra as-is.
9. **Split `.o` files have no debug info** (`objdump -S` only works on the recompiled side), but they keep relocations — helper names (`__udivdi3`), GOT refs and string refs are visible via `objdump -r`.

## E. Reproducing a category locally

```
cd /tmp/opencode/saga-experiments/a7
source env.sh                      # OJ, NM, GXX, BASE flags, build <out> <level> <src>
build e3c.o -O2 e3c.cpp
$OJ -d e3c.o | sed -n '/<.*>:/,/^$/p'
```
Flags mirrored from the project: `-fno-stack-protector -msse2 -fno-exceptions
-fno-rtti -fno-function-sections -fno-data-sections`, PIC default. `-O0/-O2/-O3`
per experiment. Each experiment is standalone (no linking needed — relocations
print the call targets).

## F. Five-minute drill (checklist)

1. `nm build/split/*.o | grep -w <mangled>` → TU. Not found → gonk `extra_units`/`ignore` (library code).
2. `grep <tu> src/target.cmake` → -O level; absent = -O0. **Fix this first; it re-shapes everything.**
3. Per-symbol % via `objdiff-cli report generate` + grep of the JSON; skip when 100.
4. `$OJ -S` on the recompiled .o (source interleaved) vs `$OJ -d` on the split .o; `diff` the two, mentally discounting the PIC thunk prologue (`call __x86.get_pc_thunk.*`, `add $2,%ebx`, GOT `mov 0x0(%reg)`).
5. For type questions (byte width, signedness, float vs double, struct offset): confirm in Ghidra (`decompile_function` at file address `+0x10000` — see 06, §0).
6. Match against the decision tree, apply the fix, `ninja`/`make` the one TU, repeat from step 4.
7. When the function matches but the unit doesn't: check 10 (rodata), 15 (globals), 16 (TU order), 11 (static init) — the per-function diff can be invisible here.
