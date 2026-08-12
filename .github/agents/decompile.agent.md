---
name: "Saga Decompile"
tools: [read, edit, search, execute, todo, "ghidra-mcp/*"]
argument-hint: "Function, symbol, or binary behavior to investigate and implement"
user-invocable: true
---

# File Locations

| What                   | Where                            |
| ---------------------- | -------------------------------- |
| Source                 | `src/`                           |
| Split original objects | `build/split/*.o`                |
| Our objects            | `build/CMakeFiles/saga.dir/src/` |
| Ghidra                 | MCP-connected program            |


# Workflow

First of all, get the decompiled code from ghidra and copy it into the specified function.

DO NOT USE GHIDRA FOR TOO LONG AT THE START. YOU HAVE TO START WITH A STUB IMPLEMENTATION THAT BUILDS AND DIFFS. DO NOT START ASSEMBLY ANALYSIS OR GO TOO DEEP INTO GHIDRA ANALYSIS WITHOUT A STUB IMPLEMENTATION THAT BUILDS AND DIFFS.

## Build & Diff

```bash
+-->
| cmake --build build                             # build all objects
| gonk split                                      # split original objects into build/split/*.o by symbol names in build object files
| diff -u \
|  <(objdump -d --disassemble=MyFunc ./build/split/MyFuncObject.cpp.o) \
|  <(objdump -d --disassemble=MyFunc ./build/saga)
| # compare diff, compare with ghidra decompiler output and ghidra original assembly
+- loop
```

Once that is done, you can start to change the code to match the original binary and to resemble real C/C++ code.

The following rules should be implemented before you stop:

- **Ghidra.** Resolve types, structs, and names in Ghidra if neccessary. If the decompiler shows raw offsets, `undefined*`, or `FUN_` names, you might want to analyze that later.
- **Original binary is truth.** Verify with `objdump` when the decompiler is ambiguous.
- **Never add or remove symbols** vs the original.
- **Stub uncertain logic** with `UNIMPLEMENTED()`. Do not invent behavior.
- **Match ABI exactly** — signatures, struct layouts, global sizes and alignment.
- **Prefer manual edits** over scripts. Run `clang-format` before committing.
- **C in `.c`, C++ in `.cpp`.** Avoid mixing or many `extern "C"` blocks.
- **Linkage:** prefer header inclusion over `extern`/`static`. Private items must NOT be `static` (the original didn't use it), but also don't declare them in the header.
- **Name symbols exactly as in the original.** Only rename to fix a known mismatch.

Before adding any symbols check the source tree for existing files and the most appropriate location. If none exists, create a new file in the appropriate directory. 

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

If you find common patterns (e.g. bump allocator -> `BUFFER_ALLOC_...`), have a look around the codebase for existing helpers or macros, especially in `common.h` for fixed buffer allocator macros.

Document any assumptions, guesses or hints.

# ALWAYS FOLLOW THE INSTRUCTIONS!

DO NOT change names from the original binary, do not introduce new symbols. ALL SYMBOLS MUST BE PRESENT IN THE ORIGINAL BINARY UNDER THE EXACT SAME NAMES OTHERWISE MATCHING WILL NOT WORK.

DO NOT START MAKING UP COMPARISONS YOURSELF, ONLY WRITE CODE AND USE THE PROVIDED TOOLS.

START WITH A STUB, BUILD, DIFF, THEN IMPROVE. DO THIS AS EARLY AS POSSIBLE. DO NOT START ASSEMBLY ANALYSIS OR GO TOO DEEP INTO GHIDRA ANALYSIS WITHOUT A STUB IMPLEMENTATION THAT BUILDS AND DIFFS.
