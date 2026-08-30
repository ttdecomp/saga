# 06 — Target binary reference (res/libTTapp.so)

> Agent-oriented reference for the original target: what we are matching against, how to
> query it with host/NDK binutils, and what state the Ghidra copy is in. Numbers were
> Binary facts were captured against the checked-in `res/libTTapp.so` and
> rechecked by hash on 2026-08-29. Split/report statistics are more volatile;
> use `objdiff.json` and `report.json` for their current values.
> Toolchain prefix used everywhere below:
> `NDK=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin` (bins:
> `i686-linux-android-{readelf,nm,objdump,c++filt}`; plain host `readelf`/`nm`/`objdump` also
> work — binutils 2.22.90 vs 2.4x output differences are cosmetic).

**Address convention.** The ELF is linked at image base `0x0` (RX `PT_LOAD` vaddr 0 == file
offset 0, verified `readelf -l`). Two offsets must not be confused:

- **RW `PT_LOAD` sits 0x1000 above its file offset**: vaddr 0x611d00, file offset 0x610d00
  (gold laid the RW segment so its first page is page-aligned in memory). This `+0x1000`
  only affects RW sections (`.data`, `.bss`, `.got*`, `.init_array`, ...), never the RX ones.
- **Ghidra rebased the whole image by `+0x10000`** (image base 0x10000, confirmed via MCP
  `get_metadata` + `list_segments`: `.text` 0xe9470, `.bss` 0x67c980). Ghidra address =
  ELF address + `0x10000` everywhere (e.g. `qrand` 0x490210 ↔ 0x4a0210).

`nm`/`objdump`/`readelf -sW` print the ELF **load address (vaddr)**, which equals the file
offset for RX sections but is `file offset + 0x1000` for RW sections. So
`qseed` 0x667e00 sits at file offset 0x666e00, and Ghidra sees it at 0x677e00.
Ghidra output is rebased (+0x10000); convert before comparing.

## 1. Identification

| property | value |
|---|---|
| file | ELF32, little-endian, Intel 80386 (`EM_386`), Type `DYN` (shared object), e_flags 0x0 |
| entry point | 0x0 (no entry; all init via `.init_array` — 327 constructors) |
| file size | 7,883,289 bytes (7.5 MiB) |
| sha256 | `d864055b1db5cc2ee2c16f7968ed68965b69f262ace6b6bfe43558296981c967` |
| md5 | `d6cf4644491d1cf93a98f963e50cfe47` |
| compiler | `.comment` = `GCC: (GNU) 4.7` (single line; same banner as our toolchain) |
| linker | `readelf -n` → `.note.gnu.gold-version` descriptor `gold 1.11` (NDK r8e gold, same gen as ours) |
| build info | stripped of DWARF: **0 debug sections**, but **unstripped**: full `.symtab` (32,819 entries, 525,104 B) + `.strtab` (624,829 B). No `/home/...` strings; 44 embedded source paths `i:/SagaTouch-Android_9176564/...` (Windows NDK `__FILE__`, e.g. `i:/SagaTouch-Android_9176564/saga/androidbatman.cpp`) — the original build tree |
| program headers | 5 total: `PHDR` + 2 `LOAD` + `DYNAMIC` + `GNU_STACK` RW. LOAD 1 (RX): file/vaddr `0x0..0x610cb8`, filesz=memsz 6,360,248 B (6.07 MiB). LOAD 2 (RW): file offset `0x610d00`, vaddr `0x611d00` (= offset+0x1000), filesz `0x5ac68` (371,816 B), memsz `0xd40074` (13,893,748 B = 13.25 MiB) |
| needed (DT_NEEDED) | liblog, libandroid, libOpenSLES, libEGL, libGLESv2, libc, libm, libdl, **libstdc++** (9 libs) |
| dynamic | `SYMBOLIC`, `AUXILIARY [PIC]`; prelinked=false; Ghidra `ELF Original Image Base` 0x0 |

```bash
readelf -h/-l/-d/-S/-p .comment/-n res/libTTapp.so   # all quick; sha256sum + md5sum
```

## 2. Sections (addresses = ELF load vaddr = readelf `sh_addr`; see address convention above)

| section | addr | size | bytes | % of file | perms |
|---|---|---|---|---|---|
| `.dynsym` | 0x000000d4 | 0x03c410 | 246,800 | 3.13% | R |
| `.dynstr` | 0x0003c4e4 | 0x068548 | 427,336 | 5.42% | R |
| `.hash` | 0x000a4a2c | 0x017150 | 94,544 | 1.20% | R (dynstr+hash 521,880 B, 6.62%) |
| `.rel.dyn` | 0x000bbb7c | 0x01c708 | 116,488 | 1.48% | R (**14,561** relocs) |
| `.rel.plt` | 0x000d8284 | 0x0005f0 | 1,520 | 0.02% | R (190 relocs; total 118,008 B, 1.50%) |
| `.plt` | 0x0d8880 | 0x000bf0 | 3,056 | 0.04% | RX |
| `.text` | 0x0d9470 | 0x48918d | 4,755,853 | **60.3%** | RX (74.8% of the RX load's 6,360,248 B) |
| `.rodata` | 0x562600 | 0x02e540 | 189,760 | 2.4% | R |
| `.eh_frame` | 0x590b40 | 0x080178 | 524,664 | 6.66% | R |
| `.dynamic`/`.got`/`.got.plt` | 0x611d00 | 0x004b74 | 20,084 | — | RW (0x118+0x4a58+0x304) |
| `.data` | 0x616b80 | 0x052380 | 336,768 | 4.3% | RW (initialized game globals) |
| `.init_array` | 0x668f00 | 0x00051c | 1,308 | — | RW — **327 entries** (= 325 `_GLOBAL__sub_I_*` + `frame_dummy` + 1 non-function sentinel 0xffffffff) |
| `.data.rel.ro`(+`.local`) | 0x669420 | 0x003538 | 13,624 | — | RW — vtables + const-pointer tables |
| `.fini_array` | 0x66c960 | 0x000008 | 8 | — | RW (2 dtors) |
| `.bss` | 0x66c980 | 0xce53f4 | **13,521,908** (NOBITS) | 171% of file / **97.3% of memsz 13,893,748** | RW — zero-init globals |
| `.symtab`/`.strtab` | — | 0x080330 + 0x0988bd = 0x118bed | 1,149,933 | n/a, not loaded | — (file offsets 0x66be2c / 0x6ec15c) |

Loadable file-backed total 6,732,064 B (85.4% of file). `.text` dominates the file; `.bss`
dominates runtime memory — a huge static-allocation game (see §5).

## 3. Symbol surface (`nm`)

```bash
$NDK/i686-linux-android-nm res/libTTapp.so > /tmp/nm.so   # 32,798 lines (full .symtab)
```

| class | count | notes |
|---|---|---|
| total lines | 32,798 | incl. r/R/A/w leftovers (below); .symtab has 32,819 entries |
| `T` defined global text | **9,201** | **9,201 unique names — zero duplicates** (NDK nm 2.22.90; see toolchain note) |
| `t` local text | **4,559** | static functions, `.isra`/`.constprop` clones |
| `D` / `d` | 1,510 / 307 | global / local initialized data |
| `B` / `b` | 3,474 / 3,151 | global / local bss (6,625 total) |
| `W` weak | 783 | weak **functions**; weak *objects* are class `V` (next row) |
| `V` weak object | **226** | 223 `_ZTV*` vtables (all in `.data.rel.ro`) + 3 non-vtable weak objects (in `.data`) |
| `r` / `R` | 9,357 / 28 | local / global read-only data (`.LC*`, `_ZL*` consts — not "section symbols") |
| `A` | 3 | absolute symbols |
| `w` | 2 | weak undefined (`__register/__deregister_frame_info_bases`) |
| `U` undefined | **197** | 190 with PLT relocs; Ghidra lists 190 imports |

Toolchain note: the census above is with the NDK `i686-linux-android-nm` (binutils 2.22.90).
Host binutils (2.47) classes the two linker section symbols `__INIT_ARRAY__`/`__FINI_ARRAY__`
as `D` instead of `T` (T 9,199 / D 1,512 there) — same symbols, different letter; everything
else is identical.

C++11 generated-name census:

| pattern | count | meaning |
|---|---|---|
| `_GLOBAL__sub_I_*` | **325** | static-init thunks — one per TU ⇒ **TU inventory** (see below) |
| `_ZThn*` | 39 | secondary-vtable adjustment thunks |
| `_ZGVZ*` | 6 | guard variables for local statics |
| `_ZTv*`, `_ZTT*`, `_ZTC*`, `_ZTI*` | 0 | no RTTI (built `-fno-rtti`) |

TU inventory recipe (rename to `src/` file: basename IS the original source filename,
e.g. `camera.cpp` → `src/nu2api/nu3d/...` per doc/05 §2 heuristics; gonk's split pairs
each basename to a `build/split/<basename>.o`):

```bash
$NDK/i686-linux-android-nm res/libTTapp.so | grep '_GLOBAL__sub_I_' | sed 's/.*_GLOBAL__sub_I_//'
# → aipathcnxhelper.cpp, aisysall.cpp, aitrigger.cpp, androidbatman.cpp, AndroidOBBUtils.cpp, ...
# 325 occurrences → 320 unique basenames = 320 TUs (some TUs emit 2 ctors, e.g. camera.cpp;
# `.c` basenames appear too: apiobject.c, ...). This is a basename inventory,
# not a prediction of the current split-object count.
```

## 4. Function archetypes (PIC prologue)

Android x86 PIC prologue — every function materializes its GOT base via a thunk:

```bash
$NDK/i686-linux-android-nm res/libTTapp.so | grep -E '_Z5qrandv|_Z8getqseedv|edqrand|_Z10ResetSeedsv'
# 00490210 T _Z5qrandv    00490240 T _Z8getqseedv   0035b0f0 T edqrand   00490260 T _Z10ResetSeedsv
$NDK/i686-linux-android-objdump -d --start-address=0x4901f0 --stop-address=0x4902a0 res/libTTapp.so
```

**`qrand` 0x490210** (nm/ELF address; Ghidra: **0x4a0210** = +0x10000) — global-access form
(`__x86.get_pc_thunk.cx`, `add %ecx`):
`call __x86.get_pc_thunk.cx; add $0x18665b,%ecx; mov -0x5d4(%ecx),%edx; imul $0x24cd,(%edx),%eax;
add $0x1,%eax; and $0xffff,%eax; mov %eax,(%edx); ret` — 8 insns, 34 bytes (symtab size 0x22; Ghidra body 0x4a0210–0x4a0231).

**`getqseed` 0x490240** — same prologue, one load: `mov -0x5d4(%ecx),%eax; mov (%eax),%eax; ret`
(direct read of global `qseed`).

**`ResetSeeds` 0x490260** — callee-save + thunk.bx + real calls: `push %ebx; call __x86.get_pc_thunk.bx;
add $0x18660a,%ebx; ...; call srand48@plt (0xd93a0); movl $0x3039,(qseed@-0x5d4(%ebx));
movl $0x1f3ad27f,(seed2@-0x3850(%ebx)); ...; call NuRandSeed` — the canonical
"global + call-out" shape; `0x3039` = 12345, qrand's LCG seed.

**`edqrand` 0x35b0f0** — exact same LCG as `qrand` but on `edqseed` (editor variant). The
edu/editor code coexists with game code in the same `.so` (see §8 unclaimed editor classes).

### qrand recompile comparison — instruction-identical

```bash
$NDK/i686-linux-android-g++ --sysroot=ndk/android-ndk-r8e/platforms/android-9/arch-x86 \
  -D__FILENAME__=\"src/legoapi/core/input/qrand.cpp\" -Isrc \
  -isystem ndk/android-ndk-r8e/sources/cxx-stl/system/include \
  -isystem libs/ogg-vorbis/include -isystem libs/squishlib/include \
  -fno-function-sections -fno-data-sections -std=gnu++11 -fno-exceptions -fno-rtti \
  -Wno-write-strings -DANDROID -O3 -c src/legoapi/core/input/qrand.cpp -o /tmp/qrand.o
$NDK/i686-linux-android-objdump -d /tmp/qrand.o
```

Recompiled `_Z5qrandv`: `call __x86.get_pc_thunk.cx; add $0x2,%ecx; mov 0x0(%ecx),%edx;
imul $0x24cd,(%edx),%eax; add $0x1,%eax; and $0xffff,%eax; mov %eax,(%edx); ret`
(reloc slots where the original has real displacements). **All 8 instructions + operand
choices match 1:1** — the canonical `-O3` + GCC 4.7 + PIC-default recipe (doc/01) is
codegen-correct; qrand needs no NOMATCH surgery.

## 5. Data

```bash
$NDK/i686-linux-android-nm res/libTTapp.so | grep ' [DdBb] ' | grep -E 'qseed|hub_from_superstory|FreePlay'
# 00667b90 D hub_from_superstory   00667e00 D qseed   006639b0 D edqseed   012771c0 B FreePlay
```

Notable globals (file addr → Ghidra `+0x10000`): `qseed` 0x667e00 (D, LCG seed, init 0x3039
per ResetSeeds), `edqseed` 0x6639b0 (D), `hub_from_superstory` 0x667b90 (D),
`FreePlay` 0x12771c0 (B). Sizes from `readelf -sW` (ndx 18 = .bss, 15/16 = rel.ro):
biggest objects: `GameMenu` 98,480 B, `g_vertexFormatPool` 81,920, `theNetwork` 69,876,
`ConfigBuffer`/`FS_FileList`/`g_memoryBuffer` 65,536 each, `sfx_name` 51,232,
`part_types` 48,128, `AnimParams` 46,336, `edpp_ptls` 45,056 — all `.bss`.

`.data.rel.ro` (13,176 B) +`.local` (448 B) hold **all 223 `_ZTV*` vtables** (nm class `V`,
e.g. `_ZTV10BaseEditor` 0x66bc40, `_ZTV10CursorTool` 0x66bf00, `_ZTV10EdManScale` 0x66bf90)
plus 18 other const/static objects (static const tables like `credits_music`) = **241 unique
rel.ro symbols** (474 rows in `readelf -sW` because each is listed in both `.dynsym` and
`.symtab`). The 3 non-`_ZTV` `V`-class weak objects (`EdValueControlIf::MouseScale`,
`ShaderManagerTemplate::shininessFactor`, `MechHintUIButton::PulseAgainTime` guard) live in
`.data`, not rel.ro.

### Recover initialized registries from the ELF

Do not recreate a named initialized pointer table from the call sites, or keep only the
entries needed by the current host utility. The unstripped ELF usually preserves the table's
address and size, every pointed-to local function symbol, and its strings. Recover the exact
entry order and flags from the original bytes, then express that complete initializer in the
owning source file. This matters because parsers commonly persist a table index in loaded
data; a shortened registry can parse successfully while dispatching the wrong function.

For example, `readelf -sW res/libTTapp.so` reports `lego_aiactiondefs` as a 2,508-byte
object and `lego_aiconditiondefs` as a 2,136-byte object. Their 12-byte entries can be read
from `.data`; the name and function pointers resolve through `nm -an`, without guessing.
Use ELF virtual addresses for the local file and add `0x10000` only when inspecting the same
objects in Ghidra.

## 6. Ghidra project state (project `LegoDecompilation`, program `Android (x86) libTTapp.so`)

| property | value |
|---|---|
| same file? | **yes** — Executable MD5 `d6cf4644491d1cf93a98f963e50cfe47`, SHA256 `d864055b...` match res/libTTapp.so exactly (the original import path `/home/fabian/Downloads/libTTapp.so` is gone from disk; only the hashes tie the Ghidra copy to the checked-in file) |
| language / compiler | `x86:LE:32:default`, gcc; image base **0x00010000** (rebased from 0x0) |
| created | 2026-01-05, Ghidra 10.1.2, auto-analysis **203.5 s** (`Program Information` → `Analysis Times`, cumulative) |
| function count | **13,617** |
| custom-named functions | **13,044** (96% of 13,617; has_custom_name=true) |
| `FUN_` leftovers | **exactly 1**: `FUN_000e8880` @ 0xe8880 — a function covering the `.plt` stub block (not real code; safe to ignore/delete) |
| account gap | 13,617 − 13,044 − 1 = 572 = **382 thunks** (`.plt` stubs; `isThunk=true` in list_functions_enhanced, 13,427 records total) + **190 EXTERNAL-space imports** (not listed by list_functions at all) — gap explained, see open questions |
| tag `LIKELY_UNUSED` | **200 functions** |
| symbols | 161,921 (vs 161,909 in .keep → 12 names added during RE) |
| qrand | named `qrand` @ **0x4a0210**; decompiles to `qseed = qseed * 0x24cd + 1 & 0xffff;` — matches `src/legoapi/core/input/qrand.cpp` |

Read-only MCP recipes: `get_metadata`, `get_program_options(group="Program Information")`,
`list_segments`, `search_functions_enhanced(has_custom_name=true/false)`,
`search_functions(name_pattern="FUN_")`, `list_function_tags`, `search_functions_by_tag(tag="LIKELY_UNUSED")`,
`decompile_function(address="0x4a0210")` / `"0x490240"` (file addr — convert `+0x10000`).

## 7. Symbol → source-file lookup (matching side)

gonk-carved target objects are produced from this binary and now preserve the
source directory hierarchy. `objdiff.json` is the authoritative target/base
path mapping. The preferred owner query is:

```bash
$NDK/i686-linux-android-nm res/libTTapp.so | grep ' T _Z5qrandv'            # 00490210
python3 scripts/objdiff-cli.py _Z5qrandv
jq -r '.units[] | select(.name | contains("qrand")) | [.name,.target_path,.base_path] | @tsv' objdiff.json
```

For raw symbol lookup, use recursive `find ... -print0 | xargs -0 nm -A` as
shown in `05-source-conventions.md`; `build/split/*.o` sees only top-level
objects. Verify a source edit through objdiff or by diffing the exact paths
from `objdiff.json`.

## 8. The "remaining" concept

Current snapshot: `report.json` assigns 391,472 bytes of code in 1,515
functions and 12,294,420 bytes of data to `remaining`. Its target is
`build/split/remaining.c.o`; its base is the empty root `remaining.c.o`.
Because the unit has no scratch/base symbols, it is unscored and cannot add
matched bytes. These values change whenever symbols become claimed or
unclaimed.

The detailed set census below is a **historical pre-restructure snapshot**. It
documents the method and former categories, but its split-object totals and
unclaimed list are not current. Recompute them before using them for decisions.

gonk assigns every binary symbol to a TU by address range; symbols no TU claims go to the
"remaining" unit, written to `build/split/remaining.c.o` (`gonk/src/split.rs`).

Current state (name-based set math, `T` definitions only):

| set | count |
|---|---|
| `T` in binary (unique names) | 9,201 |
| `T` defined in split archive (unique names) | 9,203 (raw per-file sum 10,488 → **1,285** dup occurrences; 36 of the union are plain-C `ov_*` API symbols from the `__extra_ogg_vorbis_*` units that are **not** in the binary — the binary's 36 `ov_*` are C++-mangled `_Z*` names, claimed by `gamelib_ogg.cpp.o`) |
| `T` in binary **claimed by split TUs** | 9,167 (= 9,201 − 34) |
| `T` defined in `build/split/remaining.c.o` | 143 — ogg/`__emutls_*` support code (`oggpack_*`, `ogg_packet_*`, local `_ov_*` clones; **no `ov_*` API here**); all ⊂ binary T |
| **`T` in binary claimed by nobody** | **34** (below) |

The 34 unclaimed, categorized:
- **3 linker-library symbols, deliberately ignored in `gonk.toml [ignore]`** (provided at
  final link by libgcc/libogg): `__udivdi3`, `__umoddi3`, `ogg_stream_flush_fill`.
- 2 linker-provided section symbols: `__INIT_ARRAY__`, `__FINI_ARRAY__`.
- 15 in-game editor classes (counterpart source: `src/gameapi/edtools/`): `_ZN7EdClass*` ×11
  (`AddType`, `FindObject`, `FindMember`, `FindTypeRef` ×2, `CopyObject`, `Serialise*` ×4,
  `GetStreamClasses` ×1) and `_ZN14EditorSettings*` ×4.
- 4 `_ZN16NuFileAndroidAPK*` methods (APK file device; src `src/nu2api/nufile/android/`).
- 10 world/sfx stragglers — **implemented by hand as empty stubs** in the natural TU:
  `ResetSounds`, `SetLevelSfxBits`, `ResetLevSfx`, `InitSpecialSfx`, `LoadSpecialSfxFile`,
  `StartDoorPositions`, `ActionFromQuiet`, `AmbientFromQuiet`, `Door_FindByName`,
  `Door_FindByIndex` → e.g. `src/legoapi/audio/sfx.cpp` defines `ResetSounds`
  (split TUs still reference it as `U`, so a definition must exist at link time).

Estimate method: binary T unique 9,201 − 34 unclaimed = 9,167 claimed by `build/split/*.o`
(a fresh gonk run would put the 34 into remaining). Note the checked-in root `remaining.c.o`
is a **stale empty artifact** (0 symbols — `nm remaining.c.o` empty) — `build/split/remaining.c.o`
is authoritative. The 15 unclaimed editor functions appear in split TUs **only as `U`
references**; `remaining.c.o` carries only `W`-weak stubs for the 3 `EditorSettings` dtors.

## 9. Other programs in the Ghidra project (read-only)

- **`Android (x86) libTTapp.so.keep`** (`/LEGO Star Wars TCS/`) — pristine backup import of
  the same file (MD5-identical); 13,617 functions, 161,909 symbols = untouched pre-RE baseline.
- **`players.cpp.o`** (`/saga/`) — recompiled split object imported for side-by-side study:
  `build/split/players.cpp.o`, x86:LE:32, base 0x0, 12,260 B, 5 symbols, no functions created.
- Project also holds PC/mac/Wii builds of the same engine (`LEGOStarWarsSaga-PC-GOG.exe`,
  `nu2_lsw`, `x86-32-cpu0x3`, ...) — same game-logic generation, different toolchains;
  useful cross-checks when an Android symbol is ambiguous.

## Key numbers (cheat sheet)

ELF32 DYN EM_386 · entry 0x0 · GCC 4.7 · gold 1.11 · 7,883,289 B · md5 d6cf4644…
.text 4,755,853 B (60.3%) · .bss 13,521,908 B (97.3% of mem) · .init_array 327 entries
.symtab 32,819 syms · nm: T 9,201 (no dups) · t 4,559 · D 1,510 · d 307 · B 3,474 · b 3,151
· W 783 · V 226 (223 _ZTV) · r 9,357 · U 197
_GLOBAL__sub_I_ 325 (320 unique TUs) · _ZThn 39 · vtables 223 · imports 190 (rel.plt) / 197 (U)
qrand 0x490210 (= Ghidra 0x4a0210) · getqseed 0x490240 · edqrand 0x35b0f0 · ResetSeeds 0x490260
qseed 0x667e00 (file off 0x666e00) · hub_from_superstory 0x667b90 · FreePlay 0x12771c0
Ghidra (base 0x10000): 13,617 funcs = 13,044 named + 1 FUN_ (FUN_000e8880, .plt) + 382 thunks + 190 EXTERNAL; LIKELY_UNUSED 200
split/report layout: see current `objdiff.json` (466 units) and `report.json`

## Open questions

1. ~~Ghidra name filters account for only 13,045 of 13,617 functions~~ **RESOLVED** (Aug
   2026): `has_custom_name=false` returns exactly 1 (`FUN_000e8880`); `list_functions_enhanced`
   returns 13,427 records of which 382 are `isThunk=true` (`.plt` stubs); the remaining
   13,617 − 13,427 = 190 are EXTERNAL-space imports that the listing omits.
   Account: 13,044 + 1 + 382 + 190 = 13,617.
2. The 15 unclaimed editor functions (`EdClass` ×11, `EditorSettings` ×4) have no `T`
   definition in any split TU — confirmed: split TUs only *reference* them (`U`), and
   `remaining.c.o` holds `W`-weak stubs for the 3 `EditorSettings` dtors. Still open: are
   they dead editor leftovers (stub/LIKELY_UNUSED candidates) or is a split unit missing?
3. `remaining` is intentionally unscored because the root base object is empty;
   use the generated target and report measures for its current contents.
