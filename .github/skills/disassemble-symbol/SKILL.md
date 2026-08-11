---
name: disassemble-symbol
description: "Disassemble a specific symbol (function or data) from the original Saga binary using NDK objdump, nm, and readelf. Use when: inspecting the original assembly for a function before implementing it; verifying instruction sequences in the original binary; checking symbol addresses, sizes, or relocations; comparing original vs recompiled code at the instruction level; understanding calling conventions or stack layouts from the original."
argument-hint: "Symbol name to disassemble from the original binary"
user-invocable: true
---

# Disassemble Symbol from Original Binary

Use the Android NDK r8e binutils to inspect individual symbols in the original binary's split object files. This is the fastest way to see exactly what instructions the original compiler generated for a function — no Ghidra decompilation, no guessing, just the raw assembly.

## When to Use

- Before implementing a function, to see the exact instruction sequence the original produces
- When objdiff shows a mismatch and you need to compare instructions side-by-side
- To check the original's calling convention, stack frame size, or register allocation
- To find the size, alignment, or initial value of a global variable
- To verify which relocations a function uses (external calls, global references)

## Tools

All tools live in the NDK toolchain at:

```
ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/
```

Define a convenience variable in your shell:

```bash
NDK_BIN=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin
```

| Tool                         | Purpose                                |
| ---------------------------- | -------------------------------------- |
| `i686-linux-android-objdump` | Disassembly, section contents, headers |
| `i686-linux-android-nm`      | Symbol listing, addresses, types       |
| `i686-linux-android-readelf` | ELF headers, sections, relocations     |
| `i686-linux-android-gcc-nm`  | Same as nm (GCC variant)               |

All tools accept `--demangle` / `-C` to decode C++ mangled names.

## Procedure

### 1. Find Which Object Contains Your Symbol

Search all split objects for the symbol name (demangled or mangled):

```bash
# Demangled search (preferred)
for f in build/split/*.o; do
    "$NDK_BIN/i686-linux-android-nm" --demangle "$f" 2>/dev/null | grep -q 'MyFunction' && echo "$f"
done

# Mangled search (if you know the exact mangled name)
for f in build/split/*.o; do
    "$NDK_BIN/i686-linux-android-nm" "$f" 2>/dev/null | grep -q '_Z10MyFunctionv' && echo "$f"
done
```

**Mapping from source to split object:** The split object name matches the source file stem — e.g., `src/legoapi/mission.cpp` → `build/split/mission.cpp.o`. If you know which source file the symbol belongs to, go directly to that object.

### 2. Find the Symbol's Address and Size

```bash
# Show all symbols with addresses and types
"$NDK_BIN/i686-linux-android-nm" --demangle -n build/split/<file>.cpp.o

# Find a specific symbol
"$NDK_BIN/i686-linux-android-nm" --demangle build/split/<file>.cpp.o | grep 'MyFunction'
```

Output format: `000012e8 T MyFunction` — address, type (`T` = text/code, `D` = data, `B` = BSS, `t` = local text), name.

The `-n` flag sorts by address, so adjacent entries show symbol boundaries.

### 3. Disassemble the Symbol

Use `--start-address` and `--stop-address` to isolate a single function:

```bash
"$NDK_BIN/i686-linux-android-objdump" -d --demangle \
    --start-address=0x12e8 --stop-address=0x1400 \
    build/split/<file>.cpp.o
```

**Finding the stop address:** The next symbol in the `nm -n` output marks the end. For the last function in `.text`, check the section end with `readelf -S`.

**One-liner to disassemble a named function:**

```bash
# Extract a function by name using nm + objdump
sym='MyFunction'; file=build/split/mission.cpp.o
start=$("$NDK_BIN/i686-linux-android-nm" --demangle "$file" | awk -v s="$sym" '$3==s{print "0x"$1}')
# Find next symbol's address as stop
"$NDK_BIN/i686-linux-android-objdump" -d --demangle --start-address="$start" "$file" | head -80
```

### 4. Inspect Data Symbols

For global variables, use `-s` (full contents) instead of `-d`:

```bash
# Show .data and .rodata sections
"$NDK_BIN/i686-linux-android-objdump" -s -j .data -j .rodata \
    build/split/<file>.cpp.o
```

For BSS (zero-initialized) globals, check the symbol size:

```bash
"$NDK_BIN/i686-linux-android-nm" --demangle --print-size --size-sort \
    build/split/<file>.cpp.o | grep ' B '
```

### 5. Inspect Relocations

To see which external symbols a function references:

```bash
"$NDK_BIN/i686-linux-android-objdump" -r build/split/<file>.cpp.o
```

This shows every relocation entry (external function calls, global variable references) with offset and symbol name.

## Useful Option Combinations

| Task                                  | Command                                  |
| ------------------------------------- | ---------------------------------------- |
| Full disassembly with raw bytes       | `objdump -d --demangle --show-raw-insn`  |
| Intel syntax (vs default AT&T)        | `objdump -d --demangle -M intel`         |
| Disassemble all sections (incl. data) | `objdump -D --demangle`                  |
| Section headers (find .text bounds)   | `objdump -h` or `readelf -S`             |
| All symbols sorted by address         | `nm --demangle -n`                       |
| Symbols with sizes                    | `nm --demangle --print-size --size-sort` |
| File header (architecture, entry)     | `objdump -f` or `readelf -h`             |

## Common Workflow

```bash
NDK_BIN=ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin

# 1. Find the object
"$NDK_BIN/i686-linux-android-nm" --demangle build/split/mission.cpp.o | grep -i mission

# 2. Get symbol address
"$NDK_BIN/i686-linux-android-nm" --demangle -n build/split/mission.cpp.o | grep -A1 'Missions_Configure'

# 3. Disassemble (address range from step 2: 0x0 to 0x5a0)
"$NDK_BIN/i686-linux-android-objdump" -d --demangle \
    --start-address=0x0 --stop-address=0x5a0 \
    build/split/mission.cpp.o

# 4. Compare with our build's objdump
objdump -d --demangle build/CMakeFiles/saga.dir/src/legoapi/mission.cpp.o | head -200
```

## Interpreting the Output

- **AT&T syntax** (default): `mov 0x10(%esp),%eax` — source first, then destination. Registers prefixed with `%`.
- **Intel syntax** (`-M intel`): `mov eax, [esp+0x10]` — destination first, brackets for memory.
- **Relocations show as `00 00 00 00`** in disassembly — the linker fills these in. Use `-r` to see what symbol goes there.
- **Local (static) functions** have type `t` / `T` (lowercase = local). Global functions are `T`.
- **Thumb mode** — not applicable here (x86 only).

## References

- `build/split/` — original binary split into per-source object files
- `objdiff.json` — unit mappings between source files and split objects
- [objdiff-diff skill](../objdiff-diff/SKILL.md) — for comparing recompiled vs original
