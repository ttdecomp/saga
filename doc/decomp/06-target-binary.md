# 06 — Original target binary reference

This document records measured facts about `res/libTTapp.so`, the original
Android x86 binary. Re-run the listed tools when exact numbers matter.

## Identification

- ELF class: 32-bit, little-endian.
- Machine: Intel 80386.
- Type: `ET_DYN` shared object.
- Entry point: `0x0`.
- Compiler marker: GCC 4.7.
- Linker note: GNU gold 1.11.

```bash
i686-linux-android-readelf -h -l -S -d res/libTTapp.so
i686-linux-android-readelf -p .comment res/libTTapp.so
i686-linux-android-readelf -n res/libTTapp.so
```

The target is linked at image base zero. RX virtual addresses match file
offsets; the RW load segment is displaced by `0x1000` from its file offset.
The Ghidra import is rebased by `0x10000`, so:

```text
Ghidra address = ELF virtual address + 0x10000
```

Apply the RW segment offset separately when translating a data virtual address
to a raw file offset.

## Important sections

The largest code section is `.text`; most runtime memory is `.bss`. The binary
also has normal Android x86 PIC machinery in `.got`, `.got.plt`, `.plt`, and
relocation sections. Query current addresses and sizes rather than copying them:

```bash
i686-linux-android-readelf -SW res/libTTapp.so
i686-linux-android-readelf -lW res/libTTapp.so
```

The original has hundreds of `.init_array` entries and many
`_GLOBAL__sub_I_*` symbols, which provide useful evidence for original source
basenames and static initialization order.

## Symbol surface

```bash
i686-linux-android-nm -S --defined-only res/libTTapp.so
i686-linux-android-readelf -sW res/libTTapp.so
```

Relevant symbol classes:

| letter | meaning |
|---|---|
| `T` / `t` | global / local text definition |
| `D` / `d` | initialized writable data |
| `B` / `b` | zero-initialized data |
| `W` | weak definition |
| `V` | weak object, often a vtable |
| `U` | undefined import |

Host and NDK binutils can classify a few linker section symbols differently;
use NDK r8e binutils for reproducible comparisons.

## PIC function shape

Android x86 functions commonly materialize a GOT base with
`__x86.get_pc_thunk.*`, then access globals through GOT relocations. This is
normal target code generation, not evidence of an extra source-level call.

Known addresses in the original ELF virtual-address space:

| symbol | address |
|---|---|
| `_Z5qrandv` | `0x490210` |
| `_Z8getqseedv` | `0x490240` |
| `edqrand` | `0x35b0f0` |
| `_Z10ResetSeedsv` | `0x490260` |

```bash
i686-linux-android-objdump -dr \
  --start-address=0x4901f0 --stop-address=0x4902a0 res/libTTapp.so
```

`qrand` is a useful sanity check because the reconstructed implementation is
small and its GCC 4.7 shape is well understood.

## Data recovery

For a data symbol, start with `nm -S`, identify its section with `readelf -sW`,
then translate its virtual address to a file offset using the load segments.
Do not assume virtual addresses and offsets are identical for RW data.

Use this process for initialized registries, tables, and pointer arrays:

1. resolve the symbol and size;
2. identify `.data`, `.rodata`, or `.bss`;
3. map virtual address to file offset;
4. decode values with the target's 32-bit little-endian ABI;
5. follow relocations before treating a word as an absolute pointer.

## Ghidra project

The project program is `Android (x86) libTTapp.so`. Its image base is
`0x10000`. A pristine `.keep` import is useful for distinguishing original
symbols from analysis-time renames. Other platform builds in the project can
provide semantic hints, but their ABI and code generation are not matching
evidence for Android x86.

The known `qrand` Ghidra address is `0x4a0210` after rebasing.

## Source-owner lookup

The build compares complete shared objects. To investigate one symbol:

```bash
bazel build --config=target //src:saga_target
python3 scripts/objdiff-cli.py _Z5qrandv
rg -n 'qrand' src
```

Use `bazel aquery --config=target` when source membership or effective compile
options are uncertain. There is no generated split-object ownership map in the
repository.

## Facts to preserve

- Android API 9 and i686 ABI.
- GCC 4.7 and gold 1.11 behavior.
- PIC/GOT-based global access.
- Exact C/C++ symbol names and linkage.
- 32-bit pointer, `long`, and structure layout.
- `.data` versus `.bss` placement.
- Function, literal, and static-initialization order where it affects bytes.
