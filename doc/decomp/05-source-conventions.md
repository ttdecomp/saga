# 05 — Source conventions (how the decompiled source is organized)

> Agent-oriented guide to `src/`: where functions live, how files are named, how stubs are
> written, and the workflow for adding a function. Written Aug 2026 against HEAD `755ab13`.
> The historical `STRUCTURE_PLAN.md` (deleted in `2e9b77c`) is now superseded by the
> restructure in `926cf37` ("Implement and stub the full libTTapp.so symbol surface,
> restructure sources, and normalize on fixed-width types"); its *concrete* TU→file maps
> are OUTDATED — everything below was re-verified against the current tree. The plan's
> general conventions (prefix→module clustering, per-TU files, `*_plain`/`*_misc`/`*_types`
> roles) still hold.

## 1. Module layout

One directory per original engine module. 285 `.cpp`, 11 `.c` files total
(the `.cpp` count includes 4 host-only test files under `src/host-tests/`).

| Directory | Role |
|---|---|
| `src/` root | `decomp.h` (type aliases, UNIMPLEMENTED/LOG, SAGA_NOMATCH), `globals.cpp/h` (shared game globals), `batman.cpp` (Batman-era leftovers) |
| `src/gameapi/` | Newer game API: `ai/aisys/` (AI script system), `edtools/` (in-game editor: `edfile.cpp`, `edlevelall.cpp`, `edsplines.cpp`, `edtoolsall.cpp`, `edstubs.cpp`), `gui/` (`apimenu.cpp`) |
| `src/gameframework/` | `saveload.cpp` — save/load system |
| `src/gamelib/` | Utilities and third-party-ish libs: `NewTerrain.cpp`, `crc/`, `nuwind/`, `util/` (`AndroidOBBUtils.cpp`, `CRC16.cpp`, `Ftp.cpp`, `Network.cpp`, `TouchHacks.cpp`, `Transporter.cpp`, `V2SessionManager.cpp`, `VirtualStackAllocator.cpp` + `gamelib_util_misc.cpp`) |
| `src/legoapi/` | Game logic, top level: `world.cpp`, `area.cpp`, `level.cpp`, `players.cpp`, `cutscene.cpp`, `mission.cpp`, `gcutscn.cpp`, `gizmo_sys.cpp`, `gizmos_gizactions.cpp` (level-script actions/conditions), `gamelib_ogg.cpp` (`ov_*` ogg-vorbis symbols), `legoapi_plain.cpp`, `legoapi_misc.cpp`, `legoapi_types.h` |
| `src/legoapi/gizmos/` | World gizmos, one file per gizmo type: `lever.cpp`, `teleport.cpp`, `tubes.cpp`, `gizbombgen.cpp`, `newblowup.cpp`, … (each with `.h`) |
| `src/legogame/` | Entry point: `game.cpp`, `startup.cpp`, `target_android.cpp` |
| `src/MechInputTouch/` | Mech touch-control system (Android-only input): `MechInputTouch.cpp`, `MechAutoJumpManager.cpp`, `MechSystems.cpp`, … |
| `src/nu2api/nu3d/` | Renderer: `nurndr.cpp`, `nugscn.cpp`, `numtl.cpp`, `NuRenderDevice.cpp`, `nuscreen.cpp`, `nutex.cpp`, `nuportal.cpp`, …; `android/` subdir for GLES2 platform code (`nurndr_android.c`, `nutex_android.cpp`, …); `generic/` for split-out generated data (`nucamera_gen.cpp`) |
| `src/nu2api/nucore/` | Core runtime: `nuapi.c/cpp`, `numem.c`, `nulist.cpp`, `nustring.cpp`, `nupad.cpp`, `nuthread.cpp`, `nu2api_nucore_misc.cpp`, `nu2api_nucore_types.h`; `android/` subdir (`nuapi_android.cpp`, `NuInputDevice_android.cpp`, `nupad_android.cpp`, `nutime_android.cpp`, …) |
| `src/nu2api/nufile/` | File I/O: `nufile.c/cpp`, `nufiledevice.cpp`, `nufilepak.cpp`, `numc.cpp`, `tmclient.cpp`, `nufile_plain.cpp`; `android/` (`nufile_android.cpp`, `NuFileDeviceAndroidAPK.cpp`) |
| `src/nu2api/numath/` | Math: `nuvec.cpp`, `numtx.cpp`, `nuquat.cpp`, `nuplane.cpp`, `nurand.cpp`, `nufloat.c`, `nutrig.c/cpp`, `nutrig_gen.cpp`, `nuvec4.c`, `numaths_plain.cpp` |
| `src/nu2api/numusic/` | `numusic.cpp`, `sfx.cpp` |
| `src/nu2api/nuplatform/` | `nuplatform.cpp`, `nudevicespecs.cpp` |
| `src/nu2api/nusound/` | Sound: `nusound.cpp`, `nusound_buffer.cpp`, `nusound_decoder*.cpp`, `nusound_loader*.cpp`, `nusound_voice*.cpp`, `nusound_system.cpp`, `nu2api_nusound_types.h/cpp`, `nusound_plain.cpp`, `nusound_android.cpp` |
| `src/nu2api/nuandroid/` | Android/iOS platform glue: `ios_graphics.cpp`, `nuphoneos.cpp` |
| `src/java/` | JNI stubs: `jni_stub.cpp`, `java.h`, `jni.h` |
| `src/host-tests/` | Host-only test harness (`main.cpp`, `test_load.hpp`, `test_window.hpp`, `nuios/`) |
| `src/host.cmake` / `src/target.cmake` | Build glue; `target.cmake` sets per-file `-O` (see §8) |

## 2. Function → file assignment heuristics

GENERAL heuristics, treated as evolving — re-verify every assignment against the
TU lookup below. Prefix → module rules:

- `Nu*` → `nu2api/*` (`NuVec*`/`NuMtx*` → `numath`, `NuFile*` → `nufile`, `NuGScn*`/`NuPrim*`/`NuMtl*`/`NuRndr*` → `nu3d`, `NuPad*`/`NuThread*`/`NuMem*` → `nucore`, `NuSound*` → `nusound`).
- `Giz*`/gizmo types (`Lever_AddGizmos`, `GizSpinner_*`) → `legoapi/gizmos/<gizmo>.cpp`.
- `Mech*` → `MechInputTouch/` (one file per class, e.g. `MechInputTouchButton.cpp`).
- `ed*`/`Ed*` → `gameapi/edtools/` (`edstubs.cpp` header comment: "all symbols with the 'ed' prefix belong here").
- `Action_*`/`Condition_*` → `src/legoapi/gizmos_gizactions.cpp` (level-script actions/conditions; e.g. `void Action_Circle(AISYS_s *, …)`).
- `ov_*` → `src/legoapi/gamelib_ogg.cpp` — the ogg-vorbis extra unit in `gonk.toml` `[extra_units] ogg_vorbis` (36 `ov_` references; provided by `libogg` at link, not game code).
- `Java_*` → JNI: the `Java_com_tt_tech_*` natives live in `src/legoapi/legoapi_plain.cpp:742-764`
  (24 `Java_*` symbols: `JNI_OnLoad` + `Java_com_tt_tech_TTActivity_native*` +
  `Java_com_tt_tech_CheckGamepadStatus_nativeSetGamePadConnected`), not in `src/java/`.
  `src/java/jni_stub.cpp` holds plain-named JNI host stubs (`AttachCurrentThread`, `GetEnv`, …)
  with no `Java_` prefix.
- `CS_*`/`CC_*` → cutscene code: globals like `CS_area`, `CS_cutsys` live in `src/legoapi/cutscene.cpp`; script command functions land in `cutscenes.cpp`/`gcutscn.cpp`.
- `_GLOBAL__sub_I_<basename>` → the named TU's own file (init stubs; per `STRUCTURE_PLAN.md` §12 — no concrete current example, treat as heuristic).
- No-prefix C game symbols → per-file by TU (see below); stragglers land in the module's `*_misc.cpp` catch-all.

### Exact TU→file lookup (verified commands)

1. Confirm the symbol exists in the target and get its address:
   ```
   nm res/libTTapp.so | grep " T NuVecAdd"        # -> 00279072 T NuVecAdd
   ```
2. Find which gonk-carved TU object defines it (this is the authoritative TU mapping):
   ```
   nm -A build/split/*.o | grep " T NuVecAdd"     # -> build/split/nuvec.cpp.o:00000108 T NuVecAdd
   ```
   (plain `nm` collapses the per-file headers; `-A` prints the object name.)
3. Map `build/split/<name>.o` → `src/<module>/<name>.cpp`. The split object names mirror
   the source tree (verified: `build/split/ai_sys.cpp.o` defines `AISysLoad`;
   `build/split/legoapi_plain.cpp.o` defines `AISysLoadEx`; `build/split/nuvec.cpp.o`
   defines `NuVecAdd`).
4. If `res/libTTapp.so` lists the symbol but no split object defines it, it's a link-time
   library symbol — check `gonk.toml` `ignore` (e.g. `__udivdi3`, `ogg_stream_flush_fill`).

## 3. File role patterns

- `*_plain.cpp` (9 files) — **plain-C symbols**: the whole file is one `extern "C" { … }`
  block of empty stubs, e.g. `src/legoapi/legoapi_plain.cpp`, `src/nu2api/nu3d/nurndr_plain.cpp`,
  `src/gamelib/util/CRC16_plain.cpp`, `src/gameapi/edtools/edtoolsall_plain.cpp`. Full list:
  `edtoolsall_plain.cpp`, `CRC16_plain.cpp`, `legoapi_plain.cpp`, `nurndr_plain.cpp`,
  `nucore_plain.cpp`, `nufile_plain.cpp`, `numaths_plain.cpp`, `nu2api_nusound_plain.cpp`,
  `nusound_plain.cpp` (each in its module dir).
  C-linkage is dictated by the unmangled symbol (C++ symbols are `_Z…`, C symbols aren't).
- `*_misc.cpp` (4 files) — **catch-all for stragglers**: `src/legoapi/legoapi_misc.cpp`,
  `src/gamelib/util/gamelib_util_misc.cpp`, `src/nu2api/nucore/nu2api_nucore_misc.cpp`,
  `src/nu2api/nusound/nu2api_nusound_misc.cpp`. One stub per line, anonymous params
  (`void NuMemAlloc(i32) {}`). Symbols get promoted to a real file once their TU is known.
- `<module>_types.h` (9 files, plus one `nu2api_nusound_types.cpp`) — **Ghidra-derived type
  scaffolding**: forward declarations + empty placeholder `struct Foo {};` definitions so
  stubs compile with correct-ish signatures, dependency-ordered includes of
  `nu2api/nucore/fixed_width.h`; e.g. `src/nu2api/nusound/nu2api_nusound_types.h`,
  `src/legoapi/legoapi_types.h`. The paired `nu2api_nusound_types.cpp` holds placeholder
  bodies for Ghidra-reconstructed classes whose virtual signatures don't match the
  hand-written headers (`NuSoundLoaderOGG::Close()` etc.).
- `*_android.cpp/h/c/hpp` (27 files — 1 `.c`, 18 `.cpp`, 7 `.h`, 1 `.hpp`) — **Android
  platform glue**, usually in an `android/` subdir: `src/nu2api/nu3d/android/nurndr_android.c`,
  `src/nu2api/nucore/android/nupad_android.cpp`, `src/nu2api/nusound/nusound_android.cpp`,
  `src/legogame/target_android.cpp`. Note there are **two** `nufile_android.cpp` files:
  `src/nu2api/nufile/android/nufile_android.cpp` (real impl + SAGA_NOMATCH funcs) and
  `src/nu2api/nufile/nufile_android.cpp` (4 empty `NuFileAndroidAPK` method stubs).
- `*_gen.cpp` (2 files) — **content split out of a large original TU**: `src/nu2api/numath/nutrig_gen.cpp`
  (trig table init) and `src/nu2api/nu3d/generic/nucamera_gen.cpp` (camera globals/clip matrices,
  split in `19182c6`). No generated-code pipeline — hand-maintained splits.
- `.c` vs `.cpp` (11 `.c` files) — plain-C TUs kept as `.c`: `src/nu2api/nu3d/nudlist.c`,
  `src/nu2api/nu3d/nuspline.c`, `src/nu2api/nu3d/nucamvu0.c`, `src/nu2api/nu3d/nutexanm.c`,
  `src/nu2api/nucore/nuapi.c`, `src/nu2api/nucore/numem.c`, `src/nu2api/nufile/nufile.c`,
  `src/nu2api/numath/nufloat.c`, `src/nu2api/numath/nutrig.c`, `src/nu2api/numath/nuvec4.c`,
  `src/nu2api/nu3d/android/nurndr_android.c`.

## 4. Naming rules

Quoted from `CONTRIBUTING.md` §"Naming Conventions" (verbatim):

- "In order to match the original binary, it's necessary to use original names for many functions and types. Whenever mangled symbols are present, these are to be matched."
- "When function and type names are known from another source (e.g., mangled symbols from another TT Games release), they are preferred."
- "When function names are not known, they should be named in `PascalCase` and follow existing patterns to the extent possible."
- "Struct and class members, function parameters, and local variables are to be given descriptive names in `snake_case`, without regard to original naming. Even when original names are known from debug symbols, these are generally considered to be insufficiently descriptive or too inconsistently named to be appropriate, and there is no need for matching."
- "Enum members are named in `UPPER_SNAKE_CASE`."
- "Macros are named in `UPPER_SNAKE_CASE`."
- "In C-style code: Types are most commonly named in `lowercase` and followed by a tag, `_s` for structs and `_e` for enums, accompanied by a typedef of the same name in `UPPERCASE` without the trailing tag."
- "In C++-style code: Classes and their constituent types are named in `PascalCase`."

Tree examples: C types `struct cheat_s` + `typedef struct cheat_s CHEAT;` (`src/globals.h:13,35`),
`nuvec_s`/`numtx_s` (`src/nu2api/numath/nu2api_numath_types.h`); C++ classes
`NuFileAndroidAPK`, `NuSoundLoaderOGG`, `MechInputTouchButton`; C functions `AISysLoad`,
`edbitsRegisterBaseScene`; globals in both `UPPER_SNAKE` (`PLAYERITEMTYPECOUNT`) and
`PascalCase` (`PlayerItemType`) — the original was inconsistent; don't "normalize"
existing symbols (mangling), only invent names for the not-known cases.

## 5. Type conventions

- Fixed-width typedefs from `src/nu2api/nucore/common.h` (also reachable via `src/decomp.h`):
  `u8 u16 u32 u64 i8 i16 i32 i64`, `usize` (= `size_t`) / `isize` (`ssize_t` on host, `int32_t`
  on target — `ssize_t` is `long` and would mangle wrong), `f32 f64`.
- `src/nu2api/nucore/fixed_width.h` — the same `i/u` typedefs **plus** `abi_long`/`abi_ulong`
  (typedefs over `long`/`unsigned long`): `long` mangles as `_l`, `unsigned long` as `_m` in
  the Itanium ABI, and the reconstructed ABI stubs (ogg/vorbis, legoapi) genuinely used them;
  typedefs keep `google-runtime-int` quiet. On this 32-bit target `abi_long` is
  same-width/same-sign as `i32`.
- Plain `long` is otherwise absent: only 2 non-`fixed_width.h` hits in the whole tree
  (`src/java/jni.h:50` `typedef long long jlong;` and a comment in `common.h:19`). If a
  signature needs a 32-bit `long`, use `abi_long`, not `long`.
- Ghidra aliases in `src/decomp.h:14-30`: `undefined`, `byte`, `dword`, `word`, `uchar`,
  `longlong`, `undefined1..8`, `dwfenc`, `ushort` — use these in placeholder structs.
- **Placeholder struct pattern** (`src/globals.h:13-33`): Ghidra-derived fields keep the
  `fieldN_0x<hexoffset>` names with the widest-safe type, plus a `/* PlaceHolder Structure */`
  plate comment; known fields get real names/types, retyped fields keep their note:
  ```c
  struct cheat_s { /* PlaceHolder Structure */
      char *name;
      undefined field1_0x4;
      undefined field2_0x5;
      ...
      byte enabled; /* Created by retype action */
      ...
      u32 flag;
  };
  typedef struct cheat_s CHEAT;
  ```
- `variptr_u` is deliberately **not** the full engine union — `src/nu2api/nucore/common.h:45-57`
  comment: "the original engine, `variptr_u` is a union of a wide variety of pointer types…
  we choose to omit it, as it is cumbersome and matching results can be achieved without the
  full union." It carries `void*`/`char*`/`i16*`/`u8*`/`u32*`/`usize addr`.

## 6. Stub conventions

Two idioms coexist; pick by file type:

- **Generated stubs** (anonymous params, empty body — params omitted entirely):
  `src/legoapi/gizmos_gizactions.cpp:3` `void Action_Circle(AISYS_s *, AISCRIPTPROCESS_s *, AIPACKET_s *, char **, i32, i32, float) {}`.
  Used throughout `*_misc.cpp` and the `_plain.cpp` files.
- **Hand-written stubs** (named params, explicit `(void)param;`, `return` the right default):
  `src/legoapi/ai_sys.cpp:3-15`:
  ```cpp
  extern "C" void *AISysLoad(void *buf, void *buf_end, i32 size, void *gscn, char *dir, char *name, char *param) {
      (void)buf; ... (void)param;
      return NULL;
  }
  ```
  Also seen: `return {};` for value returns (`src/gameframework/saveload.cpp:111`), `-1`
  for error-returning edtool stubs (`src/gameapi/edtools/edstubs.cpp`).
- `UNIMPLEMENTED("reason");` marks an intentionally-unimplemented body; 561 `UNIMPLEMENTED(`
  uses in `src/**/*.cpp|*.c` (this **already includes** the 6 in `nufile_android.cpp`
  SAGA_NOMATCH funcs; total across all of `src` is 563 call sites — 2 more in
  `src/nu2api/nusound/nusound_system.hpp` — plus the 2 macro definitions in `decomp.h`).
  Compiles to a colored stderr line on
  host; on target it's **empty** (`src/decomp.h:87-96` `#else` branch: `#define UNIMPLEMENTED(...)`).
- `LOG_DEBUG/INFO/WARN/ERR("fmt", …)` — 88 occurrences of `LOG_(DEBUG|INFO|WARN|ERR)(` in
  `src` (8 of which are the macro definitions in `decomp.h:82-85,92-95` → ~80 call sites);
  same compiled-out-on-target
  behavior. Examples: `src/legogame/startup.cpp:27`, `src/gameframework/saveload.cpp:91`.

### SAGA_NOMATCH

`#define SAGA_NOMATCH __attribute__((section(".text.nomatch")))` (`src/decomp.h:32`).
Marks functions that exist in the build for completeness but have **no original
counterpart in `libTTapp.so`** — they land in a separate section so `check_symbols.py`/
objdiff don't treat them as missing/extra symbols. 7 uses total (7 functions):

- `src/nu2api/nuandroid/ios_graphics.cpp:45` `SAGA_NOMATCH char *NuIOS_GetDocumentsPath(void) { return "res/"; }`
  — iOS platform function returning the Android asset path; not in the original.
- `src/nu2api/nufile/android/nufile_android.cpp:40-65` — `NuFileAndroidAPK::OpenFile/CloseFile/SeekFile/ReadFile/GetFilePos/GetFileSize`
  (6): Android-specific APK file APIs that don't exist in the original binary
  (`UNIMPLEMENTED("android specific"); return {};`).

## 7. Globals placement

- **Shared game globals**: `src/globals.cpp` with explicit initializers — `= 0` for
  scalars (`i32 PAL = 0;`), `= {0}` for structs/arrays (`GAMESAVE_s Game = {0};`,
  `char prodcode[16] = {0};`), `= NULL` for pointers (`MusicInfo = NULL`),
  `= "string"` for the few string literals (`char *iconname = "lego.ico";`).
  Declared in `src/globals.h`. Naming is the original's mixed bag: `UPPER_SNAKE`
  (`FRAMETIME`, `GOLDBRICKPOINTS`), `PascalCase` (`PlayerItemType`, `MusicInfo`),
  snake (`prodcode`, `iconname`). The `g_` prefix is used but is the minority
  (`g_music`, `g_BackgroundUsedFogColour`, `g_appWindow`, `g_deviceManufacturer`,
  `g_forceSysMemVbs`, …) — do not rename existing globals, and don't assume any prefix.
- **Module-local globals** live in their module file: `CS_area`, `CS_cutsys` in
  `src/legoapi/cutscene.cpp:25-27`; `NuTrigTable` in `src/nu2api/numath/nutrig_gen.cpp`;
  `global_camera`, `pmtx`, `vmtx`, `clip_planes` in `src/nu2api/nu3d/generic/nucamera_gen.cpp`;
  `saveload_status`, `SAVESLOTS`, `memcard_savedata` in `src/gameframework/saveload.cpp`.

## 8. `__FILENAME__` & logging

- `__FILENAME__` is set **per source file** by `set_filename_macro()` in
  `CMakeLists.txt:433-440`:
  `__FILENAME__="src/<path relative to repo root>"` — note the quirk: the relative path
  already starts with `src/`, so the macro reads `"src/src/<file>"` (confirmed in
  `build/compile_commands.json`: `-D__FILENAME__=\"src/src/batman.cpp\"`).
- It is referenced **only** inside `#ifdef HOST_BUILD` in `src/decomp.h` (lines 39, 81 —
  the UNIMPLEMENTED fprintf and `_saga_log`). On target builds `UNIMPLEMENTED`/`LOG*`
  expand to nothing, so the macro has **zero codegen effect**: 0 of the target `.o` files
  under `build/CMakeFiles/saga.dir` contain the string `"src/src"`. Fixing the prefix
  quirk is cosmetic-only and safe.

## 9. Authoring checklist (adding a function)

1. **Find the symbol** in the target: `nm res/libTTapp.so | grep " T <name>"` (if absent, stop — target code is local/static and tracked by address, or it's a link-lib symbol in `gonk.toml`).
2. **Find the TU**: `nm -A build/split/*.o | grep " T <name>"` → `build/split/<file>.o` → `src/<module>/<file>`.
3. **Pick the file** per §2/§3: real TU file if it exists, else the module `*_misc.cpp` (promote later); plain-C symbol → `*_plain.cpp`; unknown types → the module `*_types.h`.
4. **Write the signature with exact mangling**: demangle with `c++filt` for `_Z…` names; the parameter list (types, order, `const`) determines the mangled name — normalize only via the canonical typedefs (`i32` ↔ `int`, `abi_long` for `long`; see §5, and don't "fix" `long long`/`short` in generated signatures — that would change the mangled symbol).
5. **Body**: real implementation if you have it; otherwise the §6 stub idiom with `(void)` params and correct default return.
6. **Verify the symbol**: rebuild, then `nm build/saga | grep " T <name>"` (or run `scripts/check_symbols.py`, which diffs build vs original).
7. **Check the `-O` level**: `src/target.cmake` sets per-file `COMPILE_OPTIONS` (e.g. `-O3`, `-O2`, `cheat.cpp` uses `-O3;-fPIE`); stub-only files use the target default `-O0` — only chase `-O` when a real body approaches matchability.
8. **Format**: `clang-format` (`--style=file`, `.clang-format` at repo root); lint with `cmake --build build --target lint` (clang-tidy, `--warnings-as-errors="*"`).

## 10. Repo workflow

- **Build**: `cmake -B build . && cmake --build build` (NDK r8e x86 target → `build/saga`); host build via `-DBUILD_FOR_HOST=ON` (pulls SDL3/GLES2 via pkg-config, `src/host.cmake`).
- **Lint**: `cmake --build build --target lint` (clang-tidy on all sources, warnings-as-errors; `-nostdinc++` extra-arg on target).
- **Format**: `clang-format` enforced; commit `d9a3b05` "Apply clang-format across source files" applied it project-wide.
- **Pre-commit hook** (`.githooks/pre-commit`, enable with `git config core.hooksPath .githooks`): builds, runs `gonk split`, generates `report.json` via `objdiff-cli`, re-bumps the README badge (`![Progress](https://img.shields.io/badge/matching-<pct>%25-<color>)`, currently `matching-7.15%25-red`), stages `report.json` + `README.md`, and runs `scripts/check_symbols.py` (fails if any original symbol is missing; enforces an extra-symbol baseline `scripts/symbols_extra_baseline.txt`).
- **Commit style** (from `git log`): lowercase descriptive imperatives, often long:
  `Implement and stub the full libTTapp.so symbol surface, restructure sources, and normalize on fixed-width types`,
  `work on world/area/level stuff (#33)`, `Add NuCameraClipTestExtents, split nucamera_gen`, `Apply clang-format across source files`.
- **Progress tracking**: `report.json` (objdiff fuzzy match %, regenerated by the hook); matching is per-symbol against the gonk-carved `build/split/*.o`.

## Open questions

- `src/gamelib_ogg.cpp` lives in `legoapi/` (not `gamelib/` as the old plan's map suggested) — treat `ov_*` as "ogg extra unit" and keep the current file.
- The `g_` prefix convention is loose; only `globals.cpp` globals are centralized, there is no enforced rule for module-local names.
- `_GLOBAL__sub_I_*` init-stub placement has no current concrete example; the old plan says "the named TU's own file" — treat as heuristic, confirm per case.
