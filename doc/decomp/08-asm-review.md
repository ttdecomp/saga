# 08 — Assembly-level review workflow (command-only)

The review loop for a function whose `fuzzy_match_percent` is below 100: find
the symbol → find its TU on both sides → get per-function numbers → classified
mismatch list → full text diff → (source interleave) → map to a cause in 07.

Everything below is **pure command lines** (pipelines, awk/sed/jq one-liners,
plus two inline shell functions defined in §6 that are convenience, not files).
Every command was run against the real build and the sample outputs are verbatim
(Aug 2026, HEAD 755ab13). Verified on three functions, one per -O level:
`_Z19DecodeDeflatedBlockP14DEFLATECONTEXT` (-O3, 41.5%),
`_ZN14NuDynamicLight17computeWarpEffectERNS_9RenderSetE` (-O2, 0.4%, recompiled side is a stub),
`NuMain` (-O0, 0.5%).

Sibling docs: 06 §7 (TU identification), 07 (mismatch *catalog* — this doc is
the *extraction* half; 07 §A is the condensed older version of it), 03 §8/§9
(report/objdiff semantics), 01 §opt (-O levels).

---

## 1. Which layer, when

| layer | command family | answers / when |
|---|---|---|
| classified list | `objdiff-cli diff` + jq | **default**: which *kinds* of differences, at which offsets, with source lines. Good even on 0–5 % (stub) functions — shows the shape of the whole gap |
| full text diff | objdump → normalize → `diff -u` | exact instruction-for-instruction AT&T comparison, register names visible. Use when the function is close (score ≥ ~90 %) or when the classified list is too noisy to eyeball |
| source interleave | `objdump -S` (recompiled side only) | "which source line produced this diff line". Split objects have **no debug info** — the interleave only exists on the right side |

None of these trigger a build. Use all three in that order; the classified list
tells you *where*, the text diff tells you *what*, the interleave tells you *which line*,
07 §B/C tells you *why*.

## 2. Prereqs

```bash
OJ="$(pwd)/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-objdump"
NM="$(pwd)/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-nm"
# canonical NDK bin prefix: ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-
# objdiff-cli 3.6.1 at /home/fabian/.cargo/bin/objdiff-cli (cargo-installed, 03 §9)
# jq (/usr/bin/jq), column, diff --color, less — all standard
"$OJ" --version | head -1     # GNU objdump (GNU Binutils) 2.22.90.20120727
```

## 3. Step-by-step

Set once per session (§6 block 0). Example throughout: SYM=`_Z19DecodeDeflatedBlockP14DEFLATECONTEXT` (-O3, deflate.cpp).

### (a) Symbol in the binary
```bash
$NM res/libTTapp.so | grep -w "$SYM"
# 00315b10 T _Z19DecodeDeflatedBlockP14DEFLATECONTEXT
```
`T/t` = defined; lowercase = local. Absent → library code (gonk `extra_units`/`ignore`), not yours.

### (b) TU on both sides
```bash
$NM -A build/split/*.o 2>/dev/null | grep -w "$SYM"      # original side (2>/dev/null: some split .o have no symtab)
# build/split/deflate.cpp.o:00000588 T _Z19DecodeDeflatedBlockP14DEFLATECONTEXT
# (a symbol referenced from other TUs also prints their `U` undefined lines — the
# defining TU is the `T` line; append `| grep ' T '` to skip the noise)
$NM -A build/CMakeFiles/saga.dir/**/*.o 2>/dev/null | grep -w "$SYM"    # recompiled side (empty AND the split name = unit is source-orphaned)
# build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o:00000590 T ...
```
or look up both paths in `objdiff.json` at once (authoritative pairing; also covers `extra_units`):
```bash
jq -r '.units[] | select(.name == "nu2api/nucore/deflate.cpp" or (.name | test("deflate"))) | [.name, .target_path, .base_path] | @tsv' objdiff.json
# nu2api/nucore/deflate.cpp	build/split/deflate.cpp.o	/home/fabian/git/saga/build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o
```
Unit names in `report.json`/`objdiff.json` are the last 3 path components of the
source path (03 §2): `src/nu2api/nucore/deflate.cpp` → `nu2api/nucore/deflate.cpp`
(`src/` dropped because the path is 4 deep), but `src/legoapi/qrand.cpp` stays
`src/legoapi/qrand.cpp` (3 deep — `src/` is kept). `base_path` always carries the
full recompiled path. If `nm -A` on the recompiled side is empty, the
source file for that unit doesn't exist yet — the whole unit is missing, not just
one function; recompiled side is a stub or absent entirely.

### (c) Per-function numbers (report.json)
```bash
jq -r '.units[] | select(.functions != null) | .functions[]? | select(.fuzzy_match_percent != null and .fuzzy_match_percent < 100) | [.fuzzy_match_percent, .name, .address, .size] | @tsv' report.json | sort -k1 -n | head
# per-TU instead: add --arg u 'deflate' and select(.name | test($u)) on the unit
jq -r --arg u 'deflate' '.units[] | select(.name | test($u)) | .functions[]? | select(.fuzzy_match_percent != null) | [.fuzzy_match_percent, .name, .address, .size] | @tsv' report.json | sort -k1 -n
# 3.3333333	_Z23DecodeUncompressedBlockP14DEFLATECONTEXT	2480	382
# 20.941572	_Z22DecompressHuffmanTreesP14DEFLATECONTEXT	2864	1488
# 41.49671	_Z19DecodeDeflatedBlockP14DEFLATECONTEXT	1416	1059
# 75.251854	_Z14DecodeDeflatedP14DEFLATECONTEXT	4352	426
# 99.55556	_Z19InitHuffmanDefaultsv	1104	312
# 100.0	_Z16BuildHuffmanTreeP10DEFHUFFMANPhi	0	1102
# 100.0	InflateBuffer	4840	95
# 100.0	_Z7InflateP14DEFLATECONTEXTPci	4784	51
```
`.address` is the symbol's offset in the split `.text` (0x588 → 1416) — same
numbering as the objdiff instruction addresses (§4). 100.0 = done, skip. Six units
(e.g. `saga/src/globals.cpp`, `ai/aisys/aisys.cpp`, `nu2api/nu3d/nutexanm.c`,
`nufile/android/NuFileAndroidDeviceAPK.cpp`, `nu2api/nusound/nusound.cpp`,
`src/java/jni_stub.cpp`) have `functions == null` (source-orphaned: nothing paired
into them) — guard with `?`/`? //`. The `ogg_vorbis` extra_units unit is *not* one
of them — it has 36 functions like a normal TU.
Note: committed `report.json`/`report.md` are snapshots and can lag the current
tree (NuMain reads 0.521 both in the committed snapshot and in a live generate at
HEAD) — use `objdiff-cli report generate` for today's truth; numbers here are for
triage only.

### (d) Classified mismatch list (objdiff-cli diff + jq)
One-shot JSON dump to stdout, then straight into jq — never eyeball it raw:
```bash
objdiff-cli diff -1 build/split/deflate.cpp.o -2 build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o -o - > /tmp/xl.json
# whole unit is dumped (0.75 MB here; the nucore.cpp unit ≈ 5.8 MB) — the <symbol> arg is IGNORED (§5)
```
Aligned per-position view, left↔right, with the recompiled side's `.debug_line` line number:
```bash
jq -r --arg sym "$sym" '
  . as $j
  | ($j.left.symbols[]  | select(.name == $sym)) as $L
  | ($j.right.symbols[] | select(.name == $sym)) as $R
  | range(0; ($L.instructions | length)) as $i
  | ($L.instructions[$i]) as $l | ($R.instructions[$i]) as $r
  | select($l.diff_kind != null)
  | [($l.instruction.address // ""), ($l.instruction.formatted // "-"), $l.diff_kind,
     ($r.instruction.line_number // "-"), ($r.instruction.formatted // "-")]
  | @tsv
' /tmp/xl.json | column -t -s $'\t' | less -S
# 1448  mov esi, [ebp+0x8]                DIFF_DELETE        -  -
# 1451  cmp esi, 0xf                      DIFF_REPLACE     167  cmp dword ptr [ebp+0x8], 0xf
# 1454  mov ecx, esi                      DIFF_DELETE        -  -
# 1458  mov eax, [ebp+0xc]                DIFF_ARG_MISMATCH 167  mov esi, [ebp+0xc]
# 1461  mov edx, eax                      DIFF_ARG_MISMATCH 173  mov eax, esi
# ...
```
Per-kind counts for the function (what is the *shape* of the gap):
```bash
jq -r --arg sym "$sym" '. as $j | ($j.left.symbols[] | select(.name == $sym)) as $L | range(0; ($L.instructions | length)) as $i | select(($L.instructions[$i].diff_kind // "") != "") | $L.instructions[$i].diff_kind' /tmp/xl.json | sort | uniq -c | sort -rn
# 106 DIFF_ARG_MISMATCH
#  78 DIFF_DELETE
#  72 DIFF_INSERT
#  36 DIFF_REPLACE
#   2 DIFF_OP_MISMATCH
```
Source-line view — which source lines own mismatches (right side only; from `.debug_line`):
```bash
jq -r --arg sym "$sym" '. as $j | ($j.left.symbols[] | select(.name == $sym)) as $L | ($j.right.symbols[] | select(.name == $sym)) as $R | range(0; ($L.instructions | length)) as $i | select(($L.instructions[$i].diff_kind // "") != "") | ($R.instructions[$i].instruction.line_number // empty)' /tmp/xl.json | sort -n | uniq -c
#      12 8
#      10 9
#      10 10
#       7 11
#      13 143
#      11 144
```

### (e) Full text diff (objdump + normalize + diff)
```bash
OJ=...; cd /home/fabian/git/saga
$OJ -d build/split/deflate.cpp.o > /tmp/orig.asm
$OJ -d build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o > /tmp/recomp.asm
```
(keep `-d`; add `-r` only when you need relocation target names. Never `-S` here —
the split side has no debug info and `-S` injects blank lines that break the
extraction range, §5.) Then use the inline `xasm()` from §6 and diff:

```bash
xasm() { awk "/^[0-9a-f]+ <$1>:/,/^\$/" "$2" | awk 'NF' | tail -n +2 | sed -E 's/^[[:space:]]*[0-9a-f]+:[[:space:]]*//; s/[[:space:]]+/ /g; s/[[:space:]]+$//; s/ [0-9a-f]+ (<[^>]+>)/ \1/g'; }
xasm "$sym" /tmp/orig.asm > /tmp/a.txt
xasm "$sym" /tmp/recomp.asm > /tmp/b.txt
diff --color=always -u /tmp/a.txt /tmp/b.txt | less -R
```
Output (verbatim excerpt — `-` = original, `+` = recompiled; the PIC prologue
`call 58d <…+0x5>` / `call 595 <…+0x5>` normalizes away because the sed rewrites
`<sym+0x5>` relative targets; both sides then agree):
```
 89 f6 mov %esi,%esi
 8d bc 27 00 00 00 00 lea 0x0(%edi,%eiz,1),%edi
-8b 75 08 mov 0x8(%ebp),%esi
-83 fe 0f cmp $0xf,%esi
-89 f1 mov %esi,%ecx
-7e 53 jle <_Z19DecodeDeflatedBlockP14DEFLATECONTEXT+0x7d>
-8b 45 0c mov 0xc(%ebp),%eax
+83 7d 08 0f cmpl $0xf,0x8(%ebp)
+7e 50 jle <_Z19DecodeDeflatedBlockP14DEFLATECONTEXT+0x76>
+8b 75 0c mov 0xc(%ebp),%esi
```
This is where the *instance* of a 07 §B pattern appears: original loads `0x8(%ebp)`
into esi, recompiled compares the memory operand directly (07 cat. 3/9; register
allocation and cmp-vs-load shapes).

### (f) Source-interleaved view (recompiled side only)
```bash
$OJ -S build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o \
  | awk '/^[0-9a-f]+ <'"$sym"'>:/{c=1} c && /^[0-9a-f]+ <[^>]+>:/{if (x++) exit} c' | less
```
(terminate on the *next symbol header* — not the /^$/ blank line — because `-S`
inserts source text containing blank lines, which would end the range early).
Shows, e.g., line 167 `if (ctx->num_bits_available < 0x10)` sitting above the
`cmp dword ptr [ebp+0x8], 0xf` — the same `167` that the classified list printed.
The split side cannot interleave (no DWARF).

### (g) Iterate
- Map the concrete lines to a cause: 07 §C decision tree (cmp-vs-load ⇒ 3/9,
  register-name-only swaps ⇒ 3/14, whole-body delete ⇒ 1 wrong -O or stub),
  then 07 §B for the verified fix.
- Fix, rebuild one TU, re-split only if pairing could have moved (03 §7),
  re-run (c)–(e). `bloat`-style re-check is just re-running (d) on the same two .o.
- Cross-check ambiguous instructions in Ghidra (`+0x10000`, 06 §0) — file address
  1416 (0x588) → Ghidra 0x10588.

## 4. Reading the output

- **left = `-1` = target** = the carved original `build/split/*.o`. **right = `-2` = base** = the recompiled `build/CMakeFiles/saga.dir/**/*.o`. The side on `-1` is what `match_percent` is computed *against* and the instruction alignment is directional: flipped operands, `_Z19DecodeDeflatedBlock…` measures **38.39** instead of **41.50** (verified) — keep split on `-1` always.
- **`match_percent`**: `.left.symbols[] | .match_percent` (and mirrored on right), per symbol, equals the report's `fuzzy_match_percent` for the same build; a 100.0 match means instruction-for-instruction equality.
- **`diff_kind`** (per aligned position, present on both sides): `DIFF_ARG_MISMATCH` same mnemonic/op order, operand values differ (often pure register-allocation drift); `DIFF_OP_MISMATCH` mnemonic differs; `DIFF_REPLACE` neither op nor args match; `DIFF_DELETE`/`DIFF_INSERT` the instruction exists on one side only — the absent side's entry has `"instruction": null` (jq prints empty fields). `null` diff_kind = matched.
- **Positions, not streams**: left/right `instructions[]` are **index-aligned pairs** from objdiff's matcher — `left.instructions[i]` ↔ `right.instructions[i]`, same `diff_kind` on both. Never zip by address.
- **Addresses**: `.instruction.address` is a **decimal string, section offset** in its own object (0x588 → `"1416"`; right side numbers are the recompiled layout — they drift apart as the bodies diverge). Never mix with nm file addresses or Ghidra (`+0x10000`).
- **Syntax**: objdiff's `.formatted` is Intel-ish (`mov ebp, [esp+0x1c]`); objdump is AT&T (`mov 0x1c(%esp),%ebp`). They never agree verbatim — the text diff (e) uses objdump on *both* sides precisely so the syntaxes match.
- **`line_number`**: only on the **right** side (`$R.instructions[i].instruction.line_number`), from `.debug_line`; absent on the split side (no DWARF). It ties the classified list to the `-S` view.
- **Unit dump shape**: `.left/.right` each have `.sections[]` (binary arrays — you don't need them) and `.symbols[]` — *all* symbols of the unit including data arrays (`_ZL14DefaultLengths`, … `[.rodata-0]`) which carry `instructions: null` or 0-length lists. Filter by `.name == "$sym"`.

## 5. Pitfalls

- **The `<symbol>` CLI argument is ignored** — the whole unit is dumped. Every jq view must carry `select(.name == $sym)`. Pass `--arg sym "$sym"`.
- **`-1`/`-2` order matters**: `match_percent` and the alignment are computed against the `-1` side as target — flipping the operands changes the score (DecodeDeflatedBlock: 41.50 vs 38.39) and the `diff_kind` layout. Split object on `-1`, recompiled on `-2`, always.
- **Never mix file offsets between sides** — always re-extract per symbol from each side's own objdump; the recompiled `.text` is a different length. Addresses in the aligned jq view come from each side's own object.
- **`-S` breaks the blank-line awk range** (`/^$/`) because source text is inserted; use the next-header terminator from §3(f).
- **`functions == null` units** (6 of them, §3c): guard `?`-gestures; there are no
  per-function rows to query. (`ogg_vorbis`, the extra_units unit, *does* have functions.)
- **Raw dump is big** (0.75–5.8 MB observed) — pipe into jq immediately; don't cat, don't save then re-open in an editor.
- **`-p <project> -u <unit>` alternative**: `objdiff-cli diff -p . -u nu2api/nucore/deflate.cpp -o -` reads the paths out of `objdiff.json` (no build, no state) and yields identical output — use it when you don't want to echo the two object paths; the explicit `-1/-2` form stays the primary because the unit name in `objdiff.json` is not obviously the TU basename.
- **Committed `report.json` is a snapshot** — stale percentages are normal (NuMain: 0.521 in the committed snapshot, same as live at HEAD). Triage with it, verify with `objdiff-cli diff`.
- objdump column layout varies with binutils version — the sed in `xasm` assumes whitespace-separated `addr: bytes` then mnemonic; if your `diff` output looks unfiltered, check `$OJ --version` (verified with 2.22.90).

## 6. Copy-paste blocks

Block 0 — session vars (edit these two/three, everything else stays):
```bash
cd /home/fabian/git/saga
BJ=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-
OJ=${BJ}objdump; NM=${BJ}nm
SYM=_Z19DecodeDeflatedBlockP14DEFLATECONTEXT        # mangled name from report.json / nm
# TU + unit name (for -p/-u): nu2api/nucore/deflate.cpp — derived in §3(b), not guessed
xasm() { awk "/^[0-9a-f]+ <$1>:/,/^\$/" "$2" | awk 'NF' | tail -n +2 | sed -E 's/^[[:space:]]*[0-9a-f]+:[[:space:]]*//; s/[[:space:]]+/ /g; s/[[:space:]]+$//; s/ [0-9a-f]+ (<[^>]+>)/ \1/g'; }
aligned() { jq -r --arg sym "$1" '. as $j | ($j.left.symbols[] | select(.name == $sym)) as $L | ($j.right.symbols[] | select(.name == $sym)) as $R | range(0; ($L.instructions | length)) as $i | ($L.instructions[$i]) as $l | ($R.instructions[$i]) as $r | select($l.diff_kind != null) | [($l.instruction.address // ""), ($l.instruction.formatted // "-"), $l.diff_kind, ($r.instruction.line_number // "-"), ($r.instruction.formatted // "-")] | @tsv' "$2"; }
```

Block A — the full review of one function (substitute SYM, and the two .o paths you got from (b)):
```bash
$NM res/libTTapp.so | grep -w "$SYM"                        # (a) exists?
$NM -A build/split/*.o 2>/dev/null | grep -w "$SYM"         # (b) TU (split side)
$NM -A build/CMakeFiles/saga.dir/**/*.o 2>/dev/null | grep -w "$SYM"   # (b) TU (recompiled side)
objdiff-cli diff -1 build/split/deflate.cpp.o \
  -2 build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o -o - > /tmp/xl.json   # (d)
aligned "$SYM" /tmp/xl.json | column -t -s $'\t' | less -S   # (d) classified list
$OJ -d build/split/deflate.cpp.o > /tmp/orig.asm             # (e)
$OJ -d build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o > /tmp/recomp.asm
xasm "$SYM" /tmp/orig.asm > /tmp/a.txt && xasm "$SYM" /tmp/recomp.asm > /tmp/b.txt
diff --color=always -u /tmp/a.txt /tmp/b.txt | less -R       # (e) text diff
$OJ -S build/CMakeFiles/saga.dir/src/nu2api/nucore/deflate.cpp.o \
  | awk '/^[0-9a-f]+ <'"$SYM"'>:/{c=1} c && /^[0-9a-f]+ <[^>]+>:/{if (x++) exit} c' | less   # (f)
```

Block B — by project/unit instead of raw paths (same output as block A's diff step):
```bash
objdiff-cli diff -p . -u nu2api/nucore/deflate.cpp -o - > /tmp/xl.json
aligned "$SYM" /tmp/xl.json | column -t -s $'\t' | less -S
```

Block C — triage a file's whole unit in one shot (all non-matching functions, worst first, with per-kind gap shape):
```bash
jq -r --arg u 'deflate' '.units[] | select(.name | test($u)) | .functions[]? | select(.fuzzy_match_percent != null) | [.fuzzy_match_percent, .name, .address, .size] | @tsv' report.json | sort -k1 -n
objdiff-cli diff -p . -u nu2api/nucore/deflate.cpp -o - > /tmp/xl.json
jq -r 'range(0; (.left.symbols | length)) as $i | select((.left.symbols[$i].instructions // []) | length > 0) | .left.symbols[$i].name as $n | .left.symbols[$i].instructions | map(.diff_kind // "MATCH") | group_by(.) | map({k: .[0], n: length}) | [$n, (map([.k, .n] | join("=")) | join(" "))] | @tsv' /tmp/xl.json
```
(last line prints, per symbol, e.g. `_Z19DecodeDeflatedBlockP14DEFLATECONTEXT  DIFF_ARG_MISMATCH=106 DIFF_DELETE=78 DIFF_INSERT=72 DIFF_OP_MISMATCH=2 DIFF_REPLACE=36 MATCH=82` — the gap shape without any per-line reading.)