---
name: "Saga Decompile"
tools: [read, edit, search, execute, todo, "ghidra-mcp/*"]
argument-hint: "Function, symbol, or binary behavior to investigate and implement"
user-invocable: true
---

# File Locations

| What                                 | Where                            |
| ------------------------------------ | -------------------------------- |
| Source                               | `src/`                           |
| Split original objects               | `build/split/<source path>.o`    |
| Our objects                          | `build/CMakeFiles/saga.dir/src/` |
| Ghidra                               | MCP-connected program            |
| CMakeLists.txt                       | `CMakeLists.txt`                 |
| CMake file specifying compiler flags | `src/target.cmake`               |

# Workflow

Read `skills/saga-decomp/SKILL.md` and `doc/decomp/00-index.md` first. Treat
Ghidra output as evidence, not source code to copy literally: recover types,
control flow, and intent, then use the original disassembly and objdiff result
to test the reconstruction.

DO NOT USE GHIDRA FOR TOO LONG AT THE START. YOU HAVE TO START WITH A STUB IMPLEMENTATION THAT BUILDS AND DIFFS. DO NOT START ASSEMBLY ANALYSIS OR GO TOO DEEP INTO GHIDRA ANALYSIS WITHOUT A STUB IMPLEMENTATION THAT BUILDS AND DIFFS.

## Build & Diff

```bash
cmake --build build -j
./gonk/target/release/gonk split
python3 scripts/objdiff-cli.py MANGLED_SYMBOL --no-color
python3 scripts/check_symbols.py --list
```

Repeat the build/split/diff cycle. Split objects preserve the source directory
tree, and objdiff unit names include `.o`; query `objdiff.json` instead of
guessing a path. If a task forbids CMake or repository builds, respect that
boundary and use a direct compiler experiment under `/tmp` when useful.

Once that is done, you can start to change the code to match the original binary and to resemble real C/C++ code.

The following rules should be implemented before you stop:

- **Ghidra.** Resolve types, structs, and names in Ghidra if neccessary. If the decompiler shows raw offsets, `undefined*`, or `FUN_` names, you might want to analyze that later.
- **Original binary is truth.** Verify with `objdump` when the decompiler is ambiguous.
- **Never add or remove symbols** vs the original.
- **Stub uncertain logic** with `UNIMPLEMENTED()`. Do not invent behavior.
- **Match ABI exactly** — signatures, struct layouts, global sizes and alignment.
- **Prefer focused edits** over broad mechanical rewrites. Run only the
  formatting and build checks authorized for the task.
- **C in `.c`, C++ in `.cpp`.** Avoid mixing or many `extern "C"` blocks.
- **Linkage:** prefer existing declarations and preserve the target symbol's
  actual linkage. File-local target symbols may require `static`; do not infer
  linkage from a decompiler spelling.
- **Name symbols exactly as in the original.** Only rename to fix a known mismatch.

Before adding any symbol, search the source tree and identify its owning target
translation unit. Do not create or move a file merely to make a symbol easy to
place: file ownership and optimization mode are part of the matching contract.

# Additional verification before committing

```bash
cmake --build build-host
cmake --build build --target lint
```

Run these only when the task authorizes builds and the shared checkout is safe
to rebuild.

# Forbidden Ghidra Artifacts

Never leave transcribed decompiler output in the code. Recover struct fields, arrays, and idiomatic C++.

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

If you find common patterns, look for existing helpers or macros in the relevant
subsystem and in `src/types.h`, `src/fixed_width.h`, and `src/decomp.h`.

Document any assumptions, guesses or hints.

# ALWAYS FOLLOW THE INSTRUCTIONS!

DO NOT change names from the original binary, do not introduce new symbols. ALL SYMBOLS MUST BE PRESENT IN THE ORIGINAL BINARY UNDER THE EXACT SAME NAMES OTHERWISE MATCHING WILL NOT WORK.

Base comparisons and branches on binary/decompiler evidence. Record uncertainty
instead of inventing behavior.

START WITH A STUB, BUILD, DIFF, THEN IMPROVE. DO THIS AS EARLY AS POSSIBLE. DO NOT START ASSEMBLY ANALYSIS OR GO TOO DEEP INTO GHIDRA ANALYSIS WITHOUT A STUB IMPLEMENTATION THAT BUILDS AND DIFFS.
