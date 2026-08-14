# Source Tree Structure (restructuring reference)

Authoritative cross-reference between the original binary (`res/libTTapp.so`) and the
decompiled source tree under `src/`. This is the canonical document for restructuring
decisions: which source file maps to which original TU, what optimization level each file
is compiled at, and what smells remain. Regenerate the mechanical parts before any large
restructure; the prose rules below hold regardless.

## 1. Ground truth: the original source filenames

The original `.so` has **no DWARF/debug sections** and **no `STT_FILE` symbols**, so the only
reliable per-file evidence embedded in the binary is the GCC static-initializer symbols:

```
_GLOBAL__sub_I_<basename>        # one per original TU that had a static initializer
```

There are **320 such symbols**, giving the set of original source files. The canonical list
is `/tmp/opencode/orig_tus.txt` (reproduced in §4). Key facts:

- **283 `.cpp` + 37 `.c`** original TUs.
- These are compiler-runtime emitted only for TUs that DEFINE a static-local/global that needs
  a guard or runtime init (e.g. `int x = 0;` lands in `.bss` but still triggers a guard for
  order). TUs with no statics do **not** get a `_GLOBAL__sub_I_`, so 320 is a lower bound on
  the true TU count — but it is the only authoritative filename source we have.
- Amplification method (always re-check after a new build):
  ```
  ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-nm \
     res/libTTapp.so | rg '_GLOBAL__sub_I_' | sed 's/.*_GLOBAL__sub_I_//' | sort -u
  ```

## 2. Optimization-level contract (the -O is per file, and matching depends on it)

Optimization is set **per source file**, not per function. GCC 4.7 NDK, default `-O0`; only
files that override via `set_source_files_properties(... COMPILE_OPTIONS "<opts>")` in
`src/target.cmake` are non-`-O0`. This is the single most important constraint for matching
decompilation: **only functions that were compiled at the same -O level in the original may
share a file**, because the reconstructed file is compiled at exactly one level.

Derivation (both sources must agree):
1. `grep -n 'COMPILE_OPTIONS' src/target.cmake`
2. `rg -o '(?<![\w.])-O[0-9]' build/compile_commands.json` (true source of truth)

Current target distribution (compile_commands.json):

| level | count |
|-------|-------|
| `-O0` (no flag) | 322 |
| `-O2` | 25 |
| `-O3` | 78 |
| `-O3 -fPIE` | 1 (`src/legoapi/cheat.cpp`) |

Total 426 `.cpp`/`.c` **build** TUs (the CMake source list; includes scattered `_misc`/
`_plain` aggregates that do not correspond 1:1 to an original TU). Doc `01-toolchain.md` quotes
291 because it predates later file splits; the grep against the live `target.cmake`/JSON is
authoritative, not the prose count.

Rule for restructuring: **when you move a function between files, verify the destination
file's `-O` equals the source file's `-O`.** Moving an `-O3` function into an `-O0` file
(because it shares a subsystem name) silently changes its codegen and breaks matching.

## 3. Module → directory grouping (original-implied layout)

The original source tree groups TUs into modules; the current `src/` mirrors it about 90%.

| Module (dir) | Typical original TU prefixes | Current dir |
|---|---|---|
| nu2api/nu3d | `nu3d`, `nu*.c`/`*.cpp`, `NuRender*`, `nushader*`, `nuvport` | `src/nu2api/nu3d/` (+`android/`, `generic/`) |
| nu2api/nucore | `nuapi`, `nucore`, `nuthread`, `numem`, `nu2api`, `NuInput*`, `NuMem*`, `NuThread*` | `src/nu2api/nucore/` (+`android/`) |
| nu2api/nusound | `nusound_*` | `src/nu2api/nusound/` |
| nu2api/nufile | `nufile*`, `numc`, `tm_*`, `listman` | `src/nu2api/nufile/` (+`android/`) |
| nu2api/numath | `nuvec`, `numtx`, `nuquat`, `nuplane`, `nurand`, `nufloat`, `nutrig`, `numaths`, `numc` | `src/nu2api/numath/` |
| nu2api/nuplatform | `nuplatform`, `nudevicespecs`, `NuPlatform` | `src/nu2api/nuplatform/` |
| nu2api/nuandroid | `ios_graphics`, `nuphoneos`, `NuPhoneOSMessage` | `src/nu2api/nuandroid/` |
| MechInputTouch | `Mech*`, `MechInputTouch*` | `src/MechInputTouch/` |
| gameapi/edtools | `ed*`, `Ed*` | `src/gameapi/edtools/` |
| gameapi/ai(/aisys) | `gameai_*`, `aisys`, `aiscript`, `aistate` | `src/gameapi/ai/` (+`aisys/`) |
| gameapi/gui | `apimenu` | `src/gameapi/gui/` |
| legoapi | game logic (approx 210 files) | `src/legoapi/` |
| legoapi/gizmos | `giz*`, gizmo types | `src/legoapi/gizmos/` (`<gizmo>.cpp` + `.h`) |
| legogame | `game`, `startup`, `target_android` | `src/legogame/` |
| gamelib(/util,/crc,/nuwind) | `AndroidOBBUtils`, `CRC16`, `Ftp`, `Network`, `Transporter`, `nuwind`, `NewTerrain` | `src/gamelib/util`, `src/gamelib/crc`, `src/gamelib/nuwind`, `src/gamelib/NewTerrain.cpp` |
| gameframework | `saveload` | `src/gameframework/` |

Exceptions / merges that BUY the 90% (do not re-split unless byte-matching demands it): the
PCH/"kitchen-sink" TUs (`nu3d_includes.c`, `nucore_includes.c`, `numath_includes.c`,
`squish_pch.cpp`) were split into the module `.cpp` files; several original TUs were merged
into one current file (`tm_client.cpp`+`tm_common.cpp`→`tmclient.cpp`; `MechTouchUI.cpp`→2
Mech files; `edbobs.cpp`→`edtoolsall.cpp`; `gizgame.cpp`→`gizmo_sys.cpp`; `Refpack.cpp`→
`inflate.cpp`; `pursuit.cpp`→`episodes.cpp`; `saveload.c`→2 `saveload.cpp`; `socksysall.c`→2;
`gameaiscript.cpp`→`ai_sys.cpp`). `*.c`→`*.cpp` renames are pervasive and intentional
(the engine source was re-authored as C++).

## 4. Full original-TU list (320) and reconciliation

> Mechanical table regenerated from `/tmp/opencode/orig_tus.txt` vs `find src -name '*.c*'`.

For each original TU the table records: exact current file (`=`), a `.c→.cpp` note, a merge
(`→` one current file), a split (across several), a near-name reparent, or "no owner" (orphan
that must be mapped by symbol).

Module : nu2api/nu3d (render core)
- `nucamera.c` → `nucamera.cpp` (+`generic/nucamera_gen.cpp` split)
- `nurndr.c` → `nurndr.cpp` (+`nurndr_plain.cpp` split)
- `nuvport.c` → `nuvport.cpp`
- `nuwater.cpp` = `nuwater.cpp`
- `nudlist_android.c` → `nudlist.cpp`
- `nugscn_android.c` → `android/nugscn_android.cpp`
- `numtl_android.cpp` = `android/numtl_android.cpp`
- `nurenderthread.cpp` = `android/nurenderthread.cpp`
- `nutex_android.c` → `android/nutex_android.cpp`
- `nuvertexformat_android.cpp` = `android/nuvertexformat_android.cpp`
- `NuRenderDevice.cpp` = `NuRenderDevice.cpp`
- `NuRenderDevice_gles2.cpp` → merged `NuRenderDevice.cpp`
- `nurendercontext.cpp` → `nucore_plain.cpp`/`android/nuapi_android.cpp`
- `NuScreen.cpp` = `nuscreen.cpp` + `NuScreen.cpp`
- `nuportal_android.c` → `nuportal.cpp`
- `nuprim_android.c` → `nuprim.cpp`
- `nuptl_android.c` → `nuprim.cpp`/`nurndr_plain.cpp`
- `nurain_android.c` → `nuwater.cpp`
- `nuspecial_android.c` → `nu2api_nucore_misc.cpp` + `nucore_plain.cpp`
- `nuscratch_android.c` → `nucore_plain.cpp`
- `nushadermanager_gen.cpp` → `nushader.cpp` + `shader.cpp`
- `nushadermanagerios.cpp` → `nushader.cpp` + `nurndr_plain.cpp`
- `nushaderobjectbase.cpp` → `nushader.cpp`
- `nushaderobject.cpp` → `nushader.cpp` + `shader.cpp`
- `nushaderprogram_android.cpp` → `nushader.cpp` + `nuvertexformat_android.cpp`
- `NuDDSFunctions.cpp` → `nu2api_nucore_misc.cpp`
- `nueffecttex_android.cpp` → `nucore.cpp`
- `nuframebuffer_android.cpp` → `nucore.cpp` + `nucore_plain.cpp`
- `NuGLES2Extensions.cpp` → `nu2api_nucore_misc.cpp` + `nutex.cpp` + `nutex_ios_ex.cpp`
- `nufmv_android.cpp` → `nucore_plain.cpp`
- `nu3d_includes.c` → merged into `nushader.cpp`/`nucore.cpp`/`nurndr.cpp`

Module : nu2api/nucore (platform/core)
- `nucore.cpp` = `nucore.cpp`
- `nucore_includes.c` → merged `nucore.cpp` + `nucore_plain.cpp`
- `nuapi_android.c` → `android/nuapi_android.cpp`
- `nubridge_android.c` → `android/nuapi_android.cpp` (verify)
- `NuEntryPoint_android.cpp` → `android/nuapi_android.cpp` (+`target_android.cpp`)
- `NuApplicationState.cpp` → `android/nuapi_android.cpp` (+`target_android.cpp`)
- `nuerror_android.c` → `nu2api_nucore_misc.cpp`
- `nuhgo_android.c` → no owner (→`nu2api_nucore_misc.cpp`)
- `nunetemu.cpp` → `nucore.cpp`
- `nunew_android.cpp` → `nunew.cpp`
- `nuobject_android.c` → `nu2api_nucore_misc.cpp`
- `nuonline_android.cpp` → `nu2api_nucore_misc.cpp` + `nucore_plain.cpp`
- `nupad_android.c` → `android/nupad_android.cpp`
- `NuPhoneOSMessage.cpp` → `nuandroid/nuphoneos.cpp`
- `NuPlatform.cpp` → `nuplatform.cpp`
- `NuThread_android.cpp` = `android/NuThread_android.cpp`
- `NuThreadManager.cpp` = `NuThreadManager.cpp`
- `nuthread.c` → `nuthread.cpp`
- `nutime_android.c` → `android/nutime_android.cpp`
- `nuvideo_android.c` → `android/nuvideo_android.cpp`
- `stubs_android.c` → `nucore_plain.cpp`
- `bgproc_android.cpp` = `android/bgproc_android.cpp`
- `NuInputDevice_android.cpp` = `android/NuInputDevice_android.cpp`
- `NuVirtualTouchDevice.cpp` = `NuVirtualTouchDevice.cpp`
- `numemory.cpp` = `numemory.cpp` ; `numemory_android.cpp` = `android/numemory_android.cpp`
- `NuMemoryManager.cpp` = `NuMemoryManager.cpp` ; `NuMemoryPool.cpp` = `NuMemoryPool.cpp`
- `squish_pch.cpp` → merged `nucore.cpp` + `nutex_ios_ex.cpp`

Module : nu2api/numath
- `numaths.c` → `numaths.cpp` (+`numaths_plain.cpp`)
- `numath_includes.c` → `numaths.cpp`/`numaths_plain.cpp`
- `nufloat_android.c` → `nufloat.c` (verify)
- `numc.c` → `nufile/numc.cpp`
- `nutrig_gen.cpp` = `nutrig_gen.cpp` ; `nuvec.cpp`,`nuplane.cpp`,`nuquat.cpp`,`numtx.cpp`,`nurand.cpp` = exact

Module : nu2api/nufile
- `nufilebase.cpp`,`nufiledevice.cpp` = exact
- `nufile_android.cpp` = `nufile_android.cpp` + `android/nufile_android.cpp`
- `NuFileDeviceAndroidAPK.cpp` = `nufile/` + `android/NuFileAndroidDeviceAPK.cpp`
- `nu2api2013_stubs.cpp` → `nufile_plain.cpp`
- `tm_client.cpp`/`tm_common.cpp` → `tmclient.cpp`
- `listman.c` → no owner (→`numc.cpp`/`nufile_plain.cpp`)

Module : nu2api/nusound
- `nusound.cpp`,`nusound3_include.cpp` = exact
- `nusound_buffer`,`nusound_bus`,`nusound_clock`,`nusound_decoder`,`nusound_decoder_ogg`,`nusound_effect`,`nusound_effect_doppler`,`nusound_effect_fader`,`nusound_effect_pitchramp`,`nusound_handle`,`nusound_listener`,`nusound_loader`,`nusound_loader_ogg`,`nusound_loader_wav`,`nusound_memorymanager`,`nusound_mixer`,`nusound_routing`,`nusound_sample`,`nusound_source`,`nusound_streamer`,`nusound_voice`,`nusound_voice_android`,`nusound_android` = exact
- `nusound_callback.cpp` → `nusound.cpp`/`nusound_system.cpp`

Module : nu2api/nuplatform · nuandroid
- `nudevicespecs.cpp` = exact ; `NuPlatform.cpp` → `nuplatform.cpp` ; `ios_graphics.cpp` = exact ; `NuPhoneOSMessage.cpp` → `nuphoneos.cpp`

Module : MechInputTouch (all exact except merges)
- all 18 `Mech*.cpp` = exact
- `MechTouchUI.cpp` → `MechInputTouch.cpp` + `MechTouchUIElements.cpp`

Module : gameapi/edtools
- `edlevelall.cpp`,`edsplines.cpp`,`edtoolsall.cpp` = exact (also mirrored `legoapi/`)
- `edbobs.cpp` → `edtoolsall.cpp`
- `edgizshadow.cpp` → `legoapi/gizmo.cpp` + `gizmos/edgizshadowmachine.cpp`
- all other `ed*` + callbacks = exact

Module : legoapi / legogame / gameapi
- `game.cpp` → `legogame/game.cpp` + `legoapi/game.cpp`
- `startup.cpp`,`main.cpp` = `legoapi/` exact
- `androidbatman.cpp` = exact
- `apiobject.c` → `apiobject.cpp`
- `areas.cpp` → `area.cpp` + `area_editor.cpp`
- `aitrigger.cpp` = `aitrigger.cpp` (renamed 2026-08-14 from the transposed `aittrigger.cpp`)
- `Controllers.cpp` → `gamepads.cpp`
- `contexts.cpp` → `players.cpp`
- `customiser.cpp` → `customise.cpp`
- `gameaiscript.cpp` → `ai_sys.cpp` + `gameapi/ai/aisys/aiscript.cpp`
- `game_obj.cpp` → `game_object.cpp`
- `gamestructure.cpp` → `legoapi_misc.cpp` / `misc_a_game.cpp`
- `gizgame.cpp` → `gizmo.cpp`/`gizmo_sys.cpp`
- `gizspinner.cpp` → `gizmos/spinner.cpp` + `gizmos_spinner.cpp`
- `gizturret.cpp` → `gizmos/gizturrets.cpp` + `gizmos_gizturrets.cpp`
- `menu.cpp` → `menus.cpp`
- `Message.cpp` → `gizmessage.cpp`/`gizmos/gizaimessage.cpp` (verify)
- `Nos.cpp` → no owner (verify)
- `pursuit.cpp` → `episodes.cpp`
- `Refpack.cpp` → `inflate.cpp`
- `saveload.c` → `gameframework/saveload.cpp` + `legoapi/saveload.cpp`
- `socksysall.c` → `socksysall.cpp` + `socksysall_stubs.cpp`
- `Stats.cpp` → `gamestatus_lsw.cpp`/`charconfig.cpp`
- `worldinfo.cpp` → `world.cpp`
- `legoapi_tag.cpp` → `tagging.cpp`/`legoapi_status.cpp`
- `rtl.c` → `rtl.cpp`
- `gcutscn.cpp`,`gamehint.cpp`,`gamemessages.cpp`,`gamestatus_lsw.cpp`,`gameanim.cpp`,`gameantinode.cpp`,`gameliball.cpp`,`gamemenuall.cpp`,`game_deb.cpp`, etc. = exact

Module : gamelib
- `AndroidOBBUtils.cpp`,`CRC16.cpp`,`Ftp.cpp`,`Network.cpp`,`TouchHacks.cpp`,`Transporter.cpp`,`V2SessionManager.cpp`,`VirtualStackAllocator.cpp` = exact
- `Utilities.cpp` → `gamelib_util_misc.cpp`
- `glutils.c` → no owner (verify)
- `nuwind.c` → `gamelib/nuwind/nuwind.cpp`
- `inflate.cpp`,`zipup.cpp` = `legoapi/` exact

## 5. Current bugs / smells awaiting restructure

1. ~~Transposed letters~~ **DONE**: `src/legoapi/aittrigger.cpp` → `aitrigger.cpp` (pure
   rename, CMakeLists + `git mv`, verified build passes).
2. **Huge `extern "C"` stub blocks inside real `.cpp` files** (smell; extract or move):
   - `src/legoapi/terrain.cpp:25-165` (~27 stubs) and `:166-641` (~158 stubs)
   - `src/legoapi/ai_sys.cpp:95-354` (~86 stubs)
   - `src/legoapi/render.cpp:457-642` (~61 stubs)
   - `src/legoapi/cutscene.cpp:266-391` (~42 stubs)
   These are C-linkage stubs glued onto real C++ files. They belong in a `*_plain.cpp`/ stub
   TU (or the matching `.c` file), not mid-real-file — and only if their `-O` matches.
3. **Mega `_plain.cpp` extern-"C" dumps** (canonical catch-alls; acceptable but audit size):
   `nu2api/nucore/nucore_plain.cpp` (~910 stubs, 1837 lines), `gameapi/edtools/edtoolsall_plain.cpp`
   (~244), `nu2api/nu3d/nurndr_plain.cpp` (~198), `nu2api/nusound/nusound_plain.cpp` (~84),
   `nu2api/nufile/nufile_plain.cpp` (~67), `nu2api/numath/numaths_plain.cpp` (~37).
4. **Duplicated basenames** between `legoapi/` and `legoapi/gizmos/`
   (`gizbombgen.cpp`, `gizforce.cpp`, `gizmopickups.cpp`, `gizpanel.cpp`, `gizportal.cpp`,
   `gizrandom.cpp`, `gizspecial.cpp`, `guidelines.cpp`, `hatmachine.cpp`, `lever.cpp`,
   `plugs.cpp`, `teleport.cpp`, `tubes.cpp`) — the bare `legoapi/giz*.cpp` set is likely
   stale; verify before deleting.
5. **Duplicate `nufile_android.cpp`** in both `nufile/` (impl) and `nufile/android/` (stubs).
6. **`src/globals.h`** carries 123 `undefined fieldN_0x…` placeholder bytes in `CHEAT` etc.
7. **Misplaced catch-alls**: `nu2api_nucore_misc.cpp` dumps symbols from many modules;
   `legoapi_misc.cpp`, `nu2api_nusound_misc.cpp`, `gamelib_util_misc.cpp` hold foreign symbols.

## 6. How to verify any restructure (do not skip)

```bash
cmake --build build -j            # target build must succeed
cmake --build build --target lint # clang-tidy warnings-as-errors + duplicate-def check + check_symbols
cargo build --release --manifest-path gonk/Cargo.toml && ./gonk/target/release/gonk split
objdiff-cli report generate -o report.json
python3 scripts/objdiffdiff.py report.json report_old.json  # progress delta sanity
```
Critical checks after any file move:
- Symbol names unchanged (objdiff pairing is by name; renaming a symbol breaks pairing).
- Destination file `-O` == source `-O` for every moved function (see §2).
- `check_symbols.py` total missing = 0, extra within baseline (144).
- `cmake --build build --target lint` green (duplicate-definition check catches split collisions).