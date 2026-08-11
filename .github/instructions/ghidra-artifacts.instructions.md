---
description: "Use when writing or reviewing decompiled C++ implementations in src/. Forbids Ghidra decompiler artifacts: raw pointer casts with magic offsets, pcode operations, and integer-pointer arithmetic. Requires proper struct field access, array indexing, and type-safe code that matches the original binary intent."
applyTo: "src/**"
---

# No Ghidra Decompiler Artifacts

Never transcribe Ghidra decompiler output directly into C++ source. Ghidra works at the machine level — pointer arithmetic, byte-level offsets, register tracking. The original source used **named struct fields, typed arrays, and idiomatic C++**. Your job is to recover that intent and write code that compiles to identical machine code.

## Forbidden Patterns

These are Ghidra artifacts, not original source. **Never** write them:

### Raw pointer casts with magic offsets

```cpp
// ❌ NEVER: offset that clearly corresponds to a struct field
*(u32 *)((char *)ptr + 0x8000) = value;
*(int *)((byte *)base + 12);
*(float *)((uint *)obj + 7);

// ❌ NEVER: byte-offset into a known array
*(byte *)(buffer + 9) = count;
```

### Integer arithmetic on pointers

```cpp
// ❌ NEVER: casting pointers to integers for offset math
*(u32 *)((int)&structVar + offset);
*(type *)((uint)ptr + field_offset);
```

### Ghidra pcode / internal operations

```cpp
// ❌ NEVER: Ghidra's internal representation
CONCAT31(hi, lo);
SUB41(value, shift);
ZEXT24(x);
SEXT14(y);
```

### Volatile on ordinary variables

```cpp
// ❌ NEVER: volatile is a decompiler guess at memory-mapped I/O
volatile int counter;       // decompiler artifact
*(volatile u32 *)addr;      // decompiler artifact
```

## Correct Approach

### 1. Complete the Ghidra context first

Before writing code, fix types in Ghidra:
- Apply the correct struct type to pointers so field accesses appear named
- Define array bounds so element indexing replaces pointer math
- Resolve global variable types so they aren't `undefined*`
- Create enums for known constants

Only write C++ when the decompiler shows **meaningful names**, not raw offsets.

### 2. Use struct field access

When Ghidra shows `*(u32 *)(ptr + 0x8000)`, that offset belongs to a struct field:

```cpp
// ✅ CORRECT: define the struct and use named field access
struct MyStruct {
    u32 header;
    u32 flags;      // offset 0x4
    // ...
    u32 field_at_0x8000;
};
ptr->field_at_0x8000 = value;
```

If you don't know the field name yet, name it descriptively (e.g., `unk_8000`, `config_flags`, `buffer_count`) — a meaningful placeholder is better than a raw pointer cast.

### 3. Use array indexing

When Ghidra shows `*(byte *)(buffer + 9)`, it's likely an array element:

```cpp
// ✅ CORRECT: typed array access
buffer[9] = count;

// ✅ CORRECT: if buffer is a struct with a fixed-size array field
struct Packet {
    u8 header[9];
    u8 count;       // at offset 9
};
packet->count = value;
```

### 4. Resolve pointer types before dereferencing

When Ghidra shows a chain like `**(u32 **)(*(u32 *)(ptr + 4) + 8)`:

1. Fix the outer struct so `ptr` has a named field at offset 4
2. Fix that field's type so it's a pointer to a known struct
3. Fix the inner struct so offset 8 is a named field
4. The resulting code becomes `ptr->manager->config`

### 5. Recognize standard patterns

Some Ghidra output has obvious idiomatic equivalents:

| Ghidra output                                       | Likely original                                  |
| --------------------------------------------------- | ------------------------------------------------ |
| `*(u32 *)(ptr + 4) = *(u32 *)(ptr + 4) + 1`         | `ptr->count++` or `++ptr->count`                 |
| `if (*(int *)(ptr + 8) == 0)`                       | `if (!ptr->data)` or `if (ptr->data == nullptr)` |
| `*(type **)(ptr + 12) = *(type **)(obj + 16)`       | `ptr->next = obj->prev`                          |
| `*(u32 *)(this + 4) = param_1`                      | `this->field_4 = param_1`                        |
| `if (*(char *)(*(int *)(param_1 + 8) + 4) == '\0')` | `if (param_1->str->buf[0] == '\0')`              |

## Decision Flow

```
Ghidra shows raw pointer cast with offset
  │
  ├─ Offset is a struct field? → Define/get the struct, use ptr->field
  │
  ├─ Offset is an array index? → Use array[index] with correct element type
  │
  ├─ Offset is a known global? → Use the named global variable
  │
  └─ Truly unknown (rare)?     → ptr->unk_<hex> with a comment, then iterate
```

IMPORTANT: Do not invent new constants or enums just to circumvent these rules, like `#define OFFSET_8000 0x8000` or `enum { WIOFF_GIZ_FLOW_PTR = 0x298c, // void*: giz flow pointer }`. The goal is to recover the original intent. If there is truly no information, keep the constants in the code, but document them with a comment and a TODO to resolve later.

## Quality Check

Before considering an implementation done, verify:

1. No `*(type *)(base + offset)` patterns remain in the new code
2. Ghidra shows named struct fields for all pointer accesses, not raw offsets
3. Array accesses use `[]` syntax, not pointer arithmetic
4. No `CONCAT`, `SUB`, `ZEXT`, `SEXT` or other pcode operations exist
5. The code compiles and `objdiff-cli diff` shows the same instruction sequence
