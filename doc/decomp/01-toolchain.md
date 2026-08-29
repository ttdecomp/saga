# 01 — Toolchain reference (target: res/libTTapp.so)
Compiler facts below were measured with the checked-in NDK r8e toolchain.
Repository counts and paths were refreshed on 2026-08-29. Treat
`build/compile_commands.json` as authoritative when the tree changes.
Toolchain root: `ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/`.

## Compiler identification
- `i686-linux-android-gcc --version` → `i686-linux-android-gcc (GCC) 4.7` (same banner for `g++`).
  NDK r8e x86-4.7 toolchain. Also present in `bin/`: `gcc-4.7`, `ar`, `as`, `nm`, `objdump`,
  `objcopy`, `readelf`, `strings`, `strip`, `c++filt`, `ld`/`ld.bfd`/`ld.gold`/`ld.mcld`.
- `.comment` markers:
  - `readelf -p .comment res/libTTapp.so` → `GCC: (GNU) 4.7` (single line) — original built
  with GCC 4.7, same major.minor as ours. Recompiled `saga.dir` objects: `GCC: (GNU) 4.7`.
- `readelf -p .comment build/saga` → `GCC: (GNU) 4.6 20120106 (prerelease)` **and**
  `GCC: (GNU) 4.7`. The 4.6 line is **not** game code: it comes from
  `platforms/android-9/arch-x86/usr/lib/crtbegin_dynamic.o` (sysroot crt, GCC 4.6 prerelease).
  No recompiled `saga.dir` object carries a 4.6 comment (spot-checked 40).
- Linker note: `readelf -n` on **both** `res/libTTapp.so` and `build/saga` →
  `.note.gnu.gold-version` = `gold 1.11`. NDK ld: `i686-linux-android-ld --version` →
  `GNU gold (GNU Binutils 2.22.90.20120727) 1.11`. Original was linked with gold 1.11 —
  the NDK r8e default is byte-for-byte the same linker generation.
- gonk-carved originals under `build/split/`: **no `.comment` section** (readelf warning), so
  per-object compiler provenance is not available from them.

## Canonical target compile command

Per-file commands are in `build/compile_commands.json` (currently 464 entries,
one per target TU). The canonical C++ command is below. C files use the C
driver and `gnu11`; inspect the JSON rather than relying on copied totals.

```
i686-linux-android-g++ \
  --sysroot=/home/fabian/git/saga/ndk/android-ndk-r8e/platforms/android-9/arch-x86 \
  -D__FILENAME__="src/<file>" \
  -I/home/fabian/git/saga/src \
  -isystem .../sources/cxx-stl/system/include \
  -isystem .../libs/ogg-vorbis/include \
  -isystem .../libs/squishlib/include \
  -fno-function-sections -fno-data-sections -g \
  -std=gnu++11 -fno-exceptions -fno-rtti -Wno-write-strings -DANDROID \
  [-O2 | -O3 | -O3 -fPIE] \
  -o <obj> -c <src>
```

Thirteen current TUs are C sources: `edui.c`, `legoapi_misc_c.c`,
`nurndr_android.c`, `nucamvu0.c`, `nuspline.c`, `nutexanm.c`, `nuapi.c`,
`numem.c`, `nufile.c`, `nufloat.c`, `nutrig.c`, `nuvec4.c`, and `vumath.c`.
They compile with `i686-linux-android-gcc -std=gnu11`; the other 451 use
`g++ -std=gnu++11`.

Flag-by-flag "why it matters" for matching:

- `--sysroot=platforms/android-9/arch-x86` — headers + `libc.a`, `libstdc++.a`, crt from the
  android-9 x86 sysroot; defines Android API-9 platform behavior (e.g. `__sFILE` layout).
- `-I.../src` — repo source root.
- `-isystem cxx-stl/system/include` — STL headers (see C++ runtime section); `-isystem`
  (not `-I`) suppresses warnings and changes header search-order semantics.
- `-isystem ogg-vorbis/include`, `-isystem squishlib/include` — vendored lib headers.
- `-fno-function-sections -fno-data-sections` — keeps per-function ordering within the TU
  (no `.text.<fn>` splitting, no section-GC reordering). Matches a fixed function layout;
  `build/saga` has no `.text.nomatch`-style section games in the target path.
- `-g` — debug info in the recompiled objects (does not affect codegen; harmless for objdiff).
- `-std=gnu++11` — GNU extensions enabled (C11/C++11 are set globally in `CMakeLists.txt`).
  Applies to the 451 C++ TUs; the 13 `.c` TUs get `-std=gnu11` via the `gcc` driver.
- `-fno-exceptions` — no `__cxa_throw`/landing pads; `try/throw` unusable, matches original.
- `-fno-rtti` — no `typeinfo`/vtable RTTI emission; `dynamic_cast/typeid` unusable.
  (`libstdc++.a` still ships `type_info` ctor/name/eq machinery — pulled in only by
  `__cxa_pure_virtual` chain, not by user code.)
- `-Wno-write-strings` — allows assigning string literals to `char*` (original code is full
  of it; otherwise a wall of warnings).
- `-DANDROID` — API-guard macro consumed all over `src/` (android-specific branches).
- `-D__FILENAME__` — see dedicated section below.
- `-fPIC` is not passed — PIC is on by default in this toolchain
  (see defaults). `-fPIE` appears in exactly 1 file (`cheat.cpp`).
- Base flags come from `CMakeLists.txt`: target-only
  `-fno-function-sections -fno-data-sections`, plus common
  `-fno-exceptions -fno-rtti -Wno-write-strings -DANDROID`.

## Optimization levels

- Default is `-O0` (no `-O` flag at all in the command) — currently 342 of 464 files.
- Per-file overrides live in `src/target.cmake` via
  `set_source_files_properties(<file> PROPERTIES COMPILE_OPTIONS "<opts>")`.
- Derivation method for any file:
  1. `grep -n 'COMPILE_OPTIONS' src/target.cmake` → exact per-file `-O2`/`-O3`/`-O3;-fPIE` list;
  2. cross-check `build/compile_commands.json` (`grep -o -- '-O\w*'`) — both sources agree.
- Counts (compile_commands.json; python parse, one entry per TU):

  | level | count |
  |-------|-------|
  | `-O0` (no flag) | 342 |
  | `-O2` | 25 |
  | `-O3` | 97 (including the `-fPIE` file) |
  | `-O3 -fPIE` | 1 (`src/legoapi/core/config/cheat.cpp`; subset of `-O3`) |
  | total | 464 |

  `src/target.cmake` and the generated JSON agree. No other `-O` values are
  currently present.
- `-O3` representative (10): `gameapi/ai/aisys/aiscript.cpp`, `gameapi/ai/aisys/aistate.cpp`,
  `gameapi/edtools/edfile.cpp`, `gameapi/gui/apimenu.cpp`, `gamelib/crc/crc.cpp`,
  `gamelib/nuwind/nuwind.cpp`, `legoapi/world/area.cpp`,
  `legoapi/core/config/cheat.cpp` (the `-fPIE` one),
  `legoapi/gizmo/base/gizmo.cpp`, `legoapi/world/level.cpp`.
- `-O2` representative (10): `gamelib/NewTerrain.cpp`, `legoapi/characters/core/players.cpp`,
  `nu2api/nu3d/NuRenderDevice.cpp`, `nu2api/nu3d/nuqfnt.cpp`, `nu2api/nu3d/nuscreen.cpp`,
  `nu2api/nu3d/android/numtl_android.cpp`, `nu2api/nu3d/android/nuqfnt_android.cpp`,
  `nu2api/nu3d/generic/nucamera_gen.cpp`, `nu2api/nucore/NuInputDevice.cpp`,
  `nu2api/nucore/NuInputManager.cpp`.
- `-O0` representative (10): `batman.cpp`, `globals.cpp`, `gameapi/ai/aisys/aisys.cpp`,
  `gameapi/edtools/edlevelall.cpp`, `gameapi/edtools/edsplines.cpp`,
  `gameapi/edtools/edstubs.cpp`, `gameapi/edtools/edtoolsall.cpp`,
  `gameapi/edtools/edtoolsall_plain.cpp`, `gameframework/saveload.cpp`, `java/jni_stub.cpp`.
- Special case: `cheat.cpp` is the only `-fPIE` file; treat its codegen as distinct from the
  rest of the TU set.

## Target defaults (verified)

`i686-linux-android-gcc -Q --help=target` and `-dM -E -x c /dev/null`:

| setting | value | how verified |
|---------|-------|--------------|
| `-m32` | enabled (default) | `-Q --help=target` |
| `-march` | `i686` | `-Q --help=target` (empty = default); `__i686__` predefined |
| `-mtune` | `atom` | `-Q --help=target`; `__tune_atom__` predefined |
| SSE | `-msse -msse2 -msse3` enabled | `-Q --help=target`; `__SSE__ __SSE2__ __SSE3__` predefined |
| SSE math | `-mssemath` implied | `__SSE_MATH__ __SSE2_MATH__` predefined |
| PIC | **on by default** (`__PIC__ 2`) | `-dM` shows `__PIC__ 2`; codegen test: `f(){return g;}` with default flags emits `call __x86.get_pc_thunk.cx` + GOT load; with `-fno-pic` emits bare `mov 0x0,%eax` |
| PIE | off by default | only `cheat.cpp` passes `-fPIE` |

`-fPIC` need not (and is not) passed explicitly — the Android driver spec hardcodes PIC
position-independent codegen for shared-toolchain output.

## `__FILENAME__` macro

- Mechanism: `set_filename_macro(f)` in `CMakeLists.txt` computes
  `file(RELATIVE_PATH FILENAME_REL ${CMAKE_CURRENT_SOURCE_DIR} ${FILENAME_ABS})` and sets
  `COMPILE_DEFINITIONS __FILENAME__="${FILENAME_REL}"` per source file.
- Current target and host commands pass the correct repository-relative value,
  for example `-D__FILENAME__="src/batman.cpp"`. The former `src/src/`
  regression has been fixed.
- Consumption: `src/decomp.h` — `UNIMPLEMENTED` (line 39) and `LOG` (line 81) embed
  `__FILENAME__`. Both macros sit under `#ifdef HOST_BUILD` (decomp.h:34), so `__FILENAME__`
  only affects **host** diagnostics, not the target matching binary.

## C++ runtime limitations

`ndk/android-ndk-r8e/sources/cxx-stl/system/include` contains exactly 20 headers:
`cassert cctype cerrno cfloat climits cmath csetjmp csignal cstddef cstdint cstdio cstdlib
cstring ctime cwchar cwctype_is_not_supported new stl_pair.h typeinfo utility`.

- **Absent**: `string`, `vector`, `map`, `algorithm`, `iostream`, `memory`, `limits`,
  `functional`, `iterator` — i.e. **no `std::string`, no `std::vector`**, no iostreams,
  no smart pointers. Matching code must use the game's own `nustring`/`nulist`/`nuptrblock`
  classes (which is what the original did).
- `nm .../platforms/android-9/arch-x86/usr/lib/libstdc++.a | grep ' T '` yields only:
  `operator new/delete` (`_Znwj`, `_ZdlPv`, array + nothrow variants), `__cxa_guard_*`
  (`acquire/release/abort`), `__cxa_pure_virtual`, `type_info` ctor/dtor/name/before/eq/ne,
  `__x86.get_pc_thunk.*` — a trivial "hosted-lite" runtime.
- Quirk: `libstdc++.a` objects are built with **GCC 4.4.3** (`.comment` of
  `new.o`/`pure_virtual.o`/`typeinfo.o`/`one_time_construction.o`), while everything else is
  4.7. The two are ABI-compatible for this symbol set; just know the provenance when
  diffing those routines.
- Link: `-lstdc++` pulls this archive (build.ninja LINK_LIBRARIES, below).

## libgcc helpers

`i686-linux-android-gcc -print-libgcc-file-name` →
`.../lib/gcc/i686-linux-android/4.7/libgcc.a` (compiled with GCC 4.7).

Temporary `helpers.cpp` experiment compiled at `-O0`/`-O2`/`-O3` with the
NDK g++, followed by an undefined-symbol scan and relocation dump:

| source construct | helper at -O0 | helper at -O2/-O3 |
|---|---|---|
| `u64 a / b` (unsigned div) | `__udivdi3` (R_386_PLT32) | `__udivdi3` |
| `u64 a % b` | `__umoddi3` | `__umoddi3` |
| `i64 a / b` (signed div) | `__divdi3` | `__divdi3` |
| `(u64)a * b` (u32×u32→u64) | inline `imul`/`mul` | inline |
| `u64 << u64` (reg count) | inline `shld` + `test $0x20` branch | inline |
| 64-bit compare (`a > b`) | inline `cmp`/`ja`/`jb` | inline |
| `(double)u64` | inline (via `.rodata` consts, `R_386_GOTOFF`) — **no** `__floatundidf` | inline |
| `(u64)double` | inline (`.rodata` consts) — **no** `__fixunsdfdi` | inline |

- Only `__udivdi3`/`__umoddi3`/`__divdi3` are ever emitted; everything else 64-bit is inline.
- Original binary: `nm -S res/libTTapp.so` shows `__udivdi3` (0x106 bytes @ `0x00560420`)
  and `__umoddi3` (0x13b bytes @ `0x00560528`) as **defined** `T` symbols in `.text`
  (section 7) — the original statically linked its own libgcc copies. Its undefined imports
  are only libc/crt: `__cxa_atexit/__cxa_finalize/__cxa_guard_*/__cxa_pure_virtual`,
  `__errno`, `__sF`, `__stack_chk_fail`, `__swbuf`.
- This is why `gonk.toml` ignores `__udivdi3`/`__umoddi3`: the original's embedded libgcc
  bodies can never be reproduced by matching game code; they are linker-provided.
  (`build/saga` currently defines/imports neither — no 64-bit division is compiled in.)
- Caveat: the task brief stated the original "only imports __udivdi3/__umoddi3"; the
  measured truth is that they are **defined** (embedded), not imported. Either way they are
  excluded from matching.

## Linking & build system

- Generator: CMake `Debug` + Ninja (`build/build.ninja`). `CMAKE_BUILD_TYPE` is
  `Debug`; `CMAKE_LINK_DEPENDS_NO_SHARED ON` (the old ld can't take
  `--dependency-file`).
- The generated Ninja link rule links all current target objects plus the
  configured external libraries. Query `build/build.ninja` for the exact rule;
  line numbers and object counts are generated and unstable. Its library set includes:
  `LINK_LIBRARIES = -Wl,-rpath,.../libs/ogg-vorbis/lib:.../libs/squishlib/lib -lGLESv2 -lEGL
  -lvorbisfile -lvorbis -logg -lstdc++ -lsquish`
- `-lGLESv2 -lEGL` come from `src/target.cmake` tail
  (`target_link_libraries(saga PRIVATE GLESv2 EGL)`); the lib archives come from
  `cmake/ogg-vorbis.cmake` + `cmake/squish.cmake` (ExternalProject, see below).
- `build/saga` is linked with gold 1.11 (see notes above) — same as the original.
- Shape mismatch to remember: original is `ET_DYN` (`libTTapp.so`, shared object), recompiled
  `build/saga` is `ET_EXEC` (executable) — matching is per-object in gonk, so link shape
  does not matter, but don't expect `readelf -d`/PLT layout parity.
- Original `readelf -h`: Intel 80386, DYN. No SONAME entry in `-d` dump.

## External libs (excluded from matching)

- **ogg + vorbis** (`cmake/ogg-vorbis.cmake`): ExternalProject builds `libogg-1.2.1` +
  `libvorbis-1.3.2` from source into `libs/ogg-vorbis/`. Configure is cross:
  `CC/CXX/AR/RANLIB` = NDK tools, `CFLAGS="--sysroot=<sysroot> -I<prefix>/include"`,
  `--host=i686-linux-android --enable-shared=no` → static `libogg.a`, `libvorbis.a`,
  `libvorbisfile.a` (`.comment`: GCC 4.7). Recompiled archive not intended for matching —
  instead `gonk.toml [extra_units] ogg_vorbis` maps the original's `ov_*` exports onto these
  (mangled C++ `ov_*` wrappers in the original are listed there too).
- **squish** (`cmake/squish.cmake`): `squish-1.10` tarball, `make` with
  `CXX=${CMAKE_CXX_COMPILER}` (NDK g++) and
  `CPPFLAGS="--sysroot=... -I<sources/cxx-stl/gnu-libstdc++/4.7/include> -I<.../4.7/libs/x86/include> -I<prefix>/include -include climits -fno-rtti -fno-exceptions"`.
  - Anomaly: squish is compiled against **gnu-libstdc++ 4.7 headers** while the game code
    uses `cxx-stl/system` headers — squish objects must never be diffed against the
    original's squish code (it is only a link-time runtime dep here). `.comment` GCC 4.7.
- Both are linked in only to satisfy undefined symbols at link time; gonk ignores their
  bodies (extra_units for ogg; squish via ignore/extra too). `libs/squishlib/lib/libsquish.a`
  members: `alpha clusterfit colourblock colourfit colourset maths rangefit singlecolourfit squish`.

## Host build differences (BUILD_FOR_HOST)

- `build-host/` is configured with the **host compiler, not the NDK one**.
  The current cache uses `/usr/bin/c++` and `/usr/bin/cc`.
- Representative command from `build-host/compile_commands.json` (470 entries):
  ```
  /usr/bin/c++ -DHOST_BUILD -D__FILENAME__="src/<file>" -I.../src
    -fno-stack-protector -msse2 -m32 -march=i686 -fsanitize=address
    -g -std=gnu++11 -fno-exceptions -fno-rtti -Wno-write-strings -DANDROID
  ```
- The current host JSON contains 470 commands (457 C++, 13 C). Versus target:
  **no** `--sysroot`, **no** `-isystem cxx-stl`, **no**
  `-fno-function-sections/-fno-data-sections`; adds `-DHOST_BUILD`,
  `-fno-stack-protector`, `-msse2`, `-m32 -march=i686`, `-fsanitize=address`
  (from `CMakeLists.txt` + `src/host.cmake`); still `-O0` everywhere (0/470 have `-O`) and
  still passes `-DANDROID`.
- deps via pkg-config (`src/host.cmake`): `vorbis vorbisfile sdl3` (+ `glesv2 egl` on Linux,
  `angleproject` on Windows; `libsquish` is **commented out** in the list).
- TU set differs: host adds eight files under `src/host-tests/` and removes the
  two Android texture TUs. The target instead adds those two Android TUs plus
  `src/nu2api/nuandroid/ios_graphics.cpp`. Compare the two generated JSON files
  for the current exact set.
- `HOST_BUILD` gates all of `src/decomp.h` (LOG/UNIMPLEMENTED) — the only user of
  `__FILENAME__`.
