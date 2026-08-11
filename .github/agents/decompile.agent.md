---
description: "Use when decompiling, reverse-engineering, or matching functions from the LEGO Star Wars Android x86 .so binary. Ghidra-first workflow: investigate and complete types/structs/control-flow in Ghidra before writing any C++. Implements only verified behavior, then builds and diffs."
name: "Saga Decompile"
tools: [read, edit, search, execute, todo, "ghidra-mcp/*"]
argument-hint: "Function, symbol, or binary behavior to investigate and implement"
user-invocable: true
---

## File Locations

| What                   | Where                            |
| ---------------------- | -------------------------------- |
| Source                 | `src/`                           |
| Original binary        | `res/libTTapp.so`                |
| Split original objects | `build/split/*.o`                |
| Our objects            | `build/CMakeFiles/saga.dir/src/` |
| Ghidra                 | MCP-connected program            |

## Hard Rules

- **Ghidra before C++.** Resolve all types, structs, and names in Ghidra first. If the decompiler shows raw offsets, `undefined*`, or `FUN_` names, you are not done.
- **Original binary is truth.** Verify with `objdump` when the decompiler is ambiguous.
- **Never add or remove symbols** vs the original.
- **Stub uncertain logic** with `UNIMPLEMENTED()`. Do not invent behavior.
- **Match ABI exactly** — signatures, struct layouts, global sizes and alignment.
- **Prefer manual edits** over scripts. Run `clang-format` before committing.
- **C in `.c`, C++ in `.cpp`.** Avoid mixing or many `extern "C"` blocks.
- **Linkage:** prefer header inclusion over `extern`/`static`. Private items must NOT be `static` (the original didn't use it), but also don't declare them in the header.
- **Name symbols exactly as in the original.** Only rename to fix a known mismatch.

---

## Symbol Placement

| Path                     | Domain              | Key Prefixes                                                                   |
| ------------------------ | ------------------- | ------------------------------------------------------------------------------ |
| `src/batman.cpp`         | Entry point         | `NuMain`                                                                       |
| `src/globals.cpp`        | Shared globals      | —                                                                              |
| `src/gameapi/ai/aisys/`  | AI / state machines | `AI`, `AISYS`, `AISCRIPT`                                                      |
| `src/gameapi/edtools/`   | Editor (**stubs**)  | `ed`, `Ed`                                                                     |
| `src/gameapi/gui/`       | GUI / menus         | `api`, `Api`, `Menu`                                                           |
| `src/gameframework/`     | Save/load           | `saveload`, `SaveLoad`                                                         |
| `src/gamelib/`           | Utilities           | `CRC`, `Terrain`, `NuWind`                                                     |
| `src/legoapi/`           | Core game objects   | `Area`, `Char`, `Level`, `Mission`, `World`, `Gizmo`, `Episode`, `Collection`… |
| `src/legoapi/gizmos/`    | Gizmo types         | `GizmoType_*`                                                                  |
| `src/legogame/`          | Game state          | `Game`, `Startup`                                                              |
| `src/java/`              | JNI (**stubs**)     | `Java_`, `JNI_`                                                                |
| `src/nu2api/nu3d/`       | 3D / rendering      | `NuRender`, `NuTex`, `NuShader`, `NuMtl`, `NuGScn`, `NuCamera`…                |
| `src/nu2api/nucore/`     | Core systems        | `NuMemory`, `NuInput`, `NuThread`, `NuString`, `NuTime`…                       |
| `src/nu2api/nufile/`     | File I/O / PAK      | `NuFile`, `NuFPar`, `NuMC`, `NuDatFile`, `TMClient`                            |
| `src/nu2api/numath/`     | Math                | `NuVec`, `NuMtx`, `NuQuat`, `NuTrig`, `NuRand`…                                |
| `src/nu2api/numusic/`    | Music / SFX         | `NuMusic`, `SFX`                                                               |
| `src/nu2api/nuplatform/` | Platform            | `NuPlatform`, `NuDeviceSpecs`                                                  |
| `src/nu2api/nusound/`    | Sound               | `NuSound`                                                                      |

**Rules:** Match symbol prefix → place in that file. Gizmo type → `src/legoapi/gizmos/<name>.cpp`. New subsystem (3+ related symbols with new prefix) → create dir + `.cpp`/`.h` + add to `CMakeLists.txt` and `objdiff.json`. Editor prefix (`ed`/`Ed`) → always stub with `UNIMPLEMENTED()`.

---

## Forbidden Ghidra Artifacts

Never transcribe decompiler output. Recover struct fields, arrays, and idiomatic C++.

| ❌ Forbidden                          | ✅ Replace with                |
| ------------------------------------ | ----------------------------- |
| `*(u32 *)((char *)ptr + 0x8000) = v` | `ptr->field_8000 = v`         |
| `*(u32 *)((int)&s + off)`            | Named field access via struct |
| `CONCAT31 / SUB41 / ZEXT24 / SEXT14` | Never appear in source        |
| `volatile` on ordinary vars          | Remove — decompiler guess     |
| `*(byte *)(buf + 9) = c`             | `buf[9] = c` or struct field  |
| `**(u32 **)(*(u32 *)(p + 4) + 8)`    | `p->manager->config`          |

**Common translations:**

| Ghidra                                    | Likely original           |
| ----------------------------------------- | ------------------------- |
| `*(u32 *)(p + 4) = *(u32 *)(p + 4) + 1`   | `p->count++`              |
| `if (*(int *)(p + 8) == 0)`               | `if (!p->data)`           |
| `*(type **)(p + 12) = *(type **)(o + 16)` | `p->next = o->prev`       |
| `*(u32 *)(this + 4) = param_1`            | `this->field_4 = param_1` |

Do NOT invent `#define OFFSET_8000` or fake enums. If truly unknown, use `ptr->unk_<hex>` with a `// TODO` comment.

---

## Workflow

### 1. Ghidra (most of the work)

Identify the symbol → map callers/callees/globals → apply struct types (`add_struct_field`) → resolve `undefined*` → rename locals/globals → create enums. **Do not proceed until the decompiler shows named fields, not raw offsets.**

### 2. Implement

Read the target file first. Match the original's control flow exactly. When uncertain, `UNIMPLEMENTED()`.

### 3. Build & Diff

```bash
cmake --build build                          # Android NDK (for diffing)
cmake --build build-host                     # Host GCC (compile check only)
objdiff-cli diff -p . -u <unit> <mangled>    # Unit from objdiff.json, symbol from nm
objdiff-cli report generate -p .             # Project-wide progress → report.json
```

- **100%** → done. **>50%** → try minor tweaks. **<50%** → back to Ghidra.
- If diff errors: check unit name in `objdiff.json`, confirm symbol with `nm`, ensure `gonk split` ran.

### 4. Finalize

```bash
clang-format -i <changed files>
cmake --build build-host    # verify host compile
```

---

## Disassembly Reference

NDK tools at: `ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/`

Set `NDK_BIN` for convenience. All tools accept `--demangle`.

| Tool                         | Use                                             |
| ---------------------------- | ----------------------------------------------- |
| `i686-linux-android-nm`      | Symbols: `-n` sort by addr, `--print-size`      |
| `i686-linux-android-objdump` | Disasm: `-d`, data: `-s -j .data`, relocs: `-r` |
| `i686-linux-android-readelf` | Headers: `-S` (sections), `-h` (file)           |

Split object name = source stem: `src/legoapi/mission.cpp` → `build/split/mission.cpp.o`.

```bash
# Find object containing a symbol
for f in build/split/*.o; do
    "$NDK_BIN/i686-linux-android-nm" --demangle "$f" 2>/dev/null | grep -q 'Sym' && echo "$f"
done

# Symbol address & size (T=code, t=local code, D=data, B=BSS)
"$NDK_BIN/i686-linux-android-nm" --demangle -n build/split/<file>.o | grep 'MyFunction'

# Disassemble between addresses
"$NDK_BIN/i686-linux-android-objdump" -d --demangle \
    --start-address=0xS --stop-address=0xE build/split/<file>.o

# Compare with our build
objdump -d --demangle build/CMakeFiles/saga.dir/src/<file>.o | head -200
```

Use `-M intel` for Intel syntax. Relocations appear as `00 00 00 00` — use `-r` to resolve.

---

## Decision Flow

```
Symbol found
  ├─ Raw offsets / undefined* in Ghidra? → Fix types first
  ├─ ed/Ed prefix? → Stub with UNIMPLEMENTED()
  ├─ Uncertain logic? → UNIMPLEMENTED()
  ├─ Raw pointer cast? → Struct field, array[index], or global name
  └─ Build + diff → <90% = back to Ghidra; ≥90% = iterate; 100% = done
```

## Report Template

```
## Symbol: <demangled>
**File:** `src/path/file.cpp`
**Evidence:** <Ghidra summary, objdump checks, xrefs>
**Implementation:** <key decisions>
**Build:** <result>
**Diff:** <fuzzy_match_percent>
**Status:** ✅ / ⚠️ XX% / ❌
```

