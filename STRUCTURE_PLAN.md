# _saga_ — Decomp Structure Plan

> Living document for getting **every** function in the target `libTTapp.so` into the
> repo as a compiling stub, organized into a sensible, stable file tree.
> Revise this file as the mapping is refined; treat it as the source of truth for layout.

## 1. Goal

Every non-thunk function symbol in `libTTapp.so` must have a corresponding C/C++
definition in this repo that produces the same (mangled) symbol, even if the body is
still a `(void)param;` stub. The existing file structure is preserved; we only add
files and functions. Real implementation (byte-matching) happens later, file by file,
on top of these stubs.

## 2. Target facts (from Ghidra, `libTTapp.so`)

- Format: ELF32 x86 (`x86:LE:32:default`), gcc, `image_base 0x00010000`.
- Total Ghidra functions: **13,617**; symbol count ~161,921 (kept symbol table).
- Non-thunk, non-`FUN_`, non-init-array target functions: **~11,391**.
- Original translation units (from `_GLOBAL__sub_I_<file>` init stubs): **320**.
- Mangled names retained (`_Z…`), matching tracked by `gonk` + `objdiff` per symbol.
- Optimization is set per TU (file), not per function — track it per source file.

### Current repo state

- `src/` has 178 `.cpp/.c` files, ~30,665 lines.
- Build produces **~825** of the ~11,391 target symbols → **~10,566 still need stubs**.
- 77 of the 320 original TUs already have a same-named repo file; **243 need new files**
  (or an existing combined/renamed file).

## 3. Source-file inventory → module mapping

The 320 TUs are the authoritative skeleton. They split into logical modules below.
Each module is a directory under `src/`. "Match" column: whether a same-named repo
file already exists (M) or must be added (A). Counts are TUs per module.

| Module (repo dir)                                   | TUs | Status |
|-----------------------------------------------------|-----|--------|
| `legoapi/` (game logic)                             | 86  | many M |
| `legoapi/gizmos/` (world gizmos)                    | 42  | many M |
| `nu2api/nu3d/` (+ `android/`)                       | 33  | many M |
| `nu2api/nusound/`                                   | 26  | many M |
| `gamelib/` util / third-party                        | 20  | few M |
| `MechInputTouch/` (mech touch-control system)       | 20  | none M |
| `nu2api/nucore/` (+ `android/`)                     | 18  | many M |
| `nu2api/saga-platform/` (platform entry)            | 17  | few M |
| `gameapi/ai/`                                       | 8   | few M |
| `nu2api/nufile/` (+ `android/`)                     | 7   | many M |
| `gameapi/edtools/`                                  | 5   | few M |
| `nu2api/numath/`                                    | 4   | mostly M |
| `legogame/`                                         | 3   | M |
| `gameframework/`                                    | 3   | M |
| `gameapi/gui/`                                      | 2   | M |
| `nu2api/nuplatform/`                                | 1   | M |
| `nu2api/numusic/`                                   | 1   | M |
| `MISC` (needs per-file triage)                      | 24  | mixed |

### 3.1 Per-module TU lists

**`legoapi/`** (game logic, top level)
`areas, attracto, audio, batarang, bolt, bolts, buffers, camera, carrying,
characters, charconfig, charpivot, charplatforms, charshadows, cheats, chris,
climbing, collection, config, contexts, creature, credits, customise, customiser,
cutscenes, detonator, doors, dropinout, episodeI..VI, episodes, explosions,
fighting, game_deb, game_obj, gameanim, gameantinode, gamehint, gameliball,
gameobjects, gamepads, gcutscn, hub, jumping, levels, lighting, menu, menus,
minikits, missions, move, movies, netplay, objectsall, panel, particles, parts,
playeritems, players, pursuit, pushing, radio, ripples, shadow, shop, simpletons,
snake, socksysall.c, specialmoves, specialsfx, speederchase, streaks, suit,
supportall, surfaces, tagging, text, thermaldetonators, things, traffic, transform,
worldinfo` (all `.cpp` unless noted).

**`legoapi/gizmos/`**
`cable, gizactions, gizbombgen, gizbuildit, gizflow, gizforce, gizgame, gizmessage,
gizminicut, gizmo, gizmoblowups, gizmopickup, gizmopickups, gizobstacle, gizpanel,
gizportal, gizrandom, gizspecial, gizspinner, giztimers, giztorpedo, gizturret,
grabber, grapples, guidelines, hatmachine, ledge, lever, plugs, pulses, rope,
securitydoor, shard, shovesys, signal, techno, teleport, tightrope, torpedo, tubes,
whip, zipup`.

**`nu2api/nu3d/`** (+ `android/`)
`NuRenderDevice, NuRenderDevice_gles2, NuScreen, nu3d_includes, nucamera.c,
nudeferredfilter, nudlist_android.c, nueffecttex_android, nuframebuffer_android,
nugscn_android.c, numainfilter, numotionaccumfilter, numotionfilter, numtl_android,
nuportal_android.c, nupostfilter, nuprim_android.c, nurendercontext,
nurenderthread, nurndr.c, nurndr_android.c, nurndrstat_android.c,
nushadermanager_gen, nushadermanagerios, nushaderobject, nushaderobjectbase,
nushaderprogram_android, nuspeedblurfilter, nutex_android.c, nutex_ios_ex,
nuvertexformat_android, nuvport.c, nuwater`.

**`nu2api/nusound/`**
`nusound, nusound3_include, nusound_android, nusound_buffer, nusound_bus,
nusound_callback, nusound_clock, nusound_decoder, nusound_decoder_ogg,
nusound_effect, nusound_effect_doppler, nusound_effect_fader,
nusound_effect_pitchramp, nusound_handle, nusound_listener, nusound_loader,
nusound_loader_ogg, nusound_loader_wav, nusound_memorymanager, nusound_mixer,
nusound_routing, nusound_sample, nusound_source, nusound_streamer, nusound_voice,
nusound_voice_android`.

**`gamelib/util`** (misc/third-party helpers)
`AndroidOBBUtils, CRC16, Controllers, Ftp, Message, Network, Nos, Refpack, Stats,
TouchHacks, Transporter, Utilities, V2SessionManager, VirtualStackAllocator,
glutils.c, inflate, listman.c, nuwind.c, shaderSemantics, utilities`.

**`MechInputTouch/`** (new module — touch/controller input system)
`MechAutoJumpManager, MechAutofireAddon, MechEdgeStopAddon, MechInputTouch,
MechInputTouchBonusCavalryController, MechInputTouchButton,
MechInputTouchContextTasks, MechInputTouchDeathStarTurretController,
MechInputTouchGestureBasedController, MechInputTouchGestureTracker,
MechInputTouchMainController, MechInputTouchMenuController,
MechInputTouchPodraceController, MechInputTouchSpeederChaseBikeController,
MechInputTouchVirtualConsoleController, MechJumpAutopilotAddon, MechObjectInterface,
MechSystems, MechTouchUI, MechTouchUIElements`.

**`nu2api/nucore/`** (+ `android/`)
`NuInputDevice_android, NuInputManager, NuMemoryManager, NuMemoryPool,
NuThreadManager, NuThread_android, nuapi_android.c, nucore, nucore_includes.c,
numemory, numemory_android, nunew_android, nupad_android.c, nupad_interface,
nustring_c.cpp, nuthread.c, nutime_android.c, nuvideo_android.c`.

**`nu2api/saga-platform/`** (platform entry & android glue)
`NuGLES2Extensions, NuPhoneOSMessage, androidbatman, ios_graphics,
nu2api2013_stubs, nubridge_android.c, nuerror_android.c, nufmv_android, nuhgo_android.c,
nunetemu, nuobject_android.c, nuonline_android, nurain_android.c,
nuscratch_android.c, squish_pch, stubs_android.c, stubvideo.c`.

**`gameapi/ai/`**
`aipathcnxhelper, aisysall, aitrigger, gameai, gameaiscript, legoai, lsw_hub_ai,
starwars_gameai`.

**`nu2api/nufile/`** (+ `android/`)
`NuFileDeviceAndroidAPK, nufile_android, nufilebase, nufiledevice, numc.c, tm_client,
tm_common`.

**`gameapi/edtools/`**
`edbobs, edgizshadow, edlevelall, edsplines, edtoolsall`.

**`nu2api/numath/`**
`nufloat_android.c, numath_includes.c, numaths.c, nutrig_gen`.

**`legogame/`**: `game, main, startup`. **`gameframework/`**: `gamestatus_lsw,
gamestructure, saveload.c`. **`gameapi/gui/`**: `gamemenuall, gamemessages`.
**`nu2api/nuplatform/`**: `NuPlatform`. **`nu2api/numusic/`**: `sfx`.

**`MISC` (triage individually)**
`NuApplicationState, NuDDSFunctions, NuEntryPoint_android, NuVirtualTouchDevice,
ShaderMtlDescFilter, ShaderObjectKey, animation, apiobject.c, bgproc_android, fade,
faders, hint, hits, legoapi_status, legoapi_tag, minicamcut, nudevicespecs,
nuptl_android.c, nuspecial_android.c, pushblocks, rtl.c, screen, screengrab_android,
timing`.

## 4. Function → file clustering strategy

Function names are demangled plain C identifiers with strong, file-specific prefixes
(the binary keeps its symbol table). Cluster by:

1. **TU skeleton** — the 320 `_GLOBAL__sub_I_<file>` names define the file set.
2. **Naming prefix → file** (primary rule; refined iteratively):
   - `Nu*` → `nu2api/*` (`NuVec*`→numath, `NuFile*`→nufile, `NuGScn*`/`NuPrim*`/`NuMtl*`/`NuRndr*`→nu3d, `NuPad*`→nucore, `NuSound*`→nusound).
   - `ed*`/`Ed*` → `gameapi/edtools/*` (edtoolsall, edlevelall, edsplines, edbobs, edgizshadow).
   - `Action_*`/`Condition_*` → `legoapi/gizmos/gizactions.cpp` (level-script actions/conditions).
   - `CS_*`/`CC_*` → `legoapi/gcutscn.cpp` / `cutscenes.cpp` (cutscene-script commands).
   - `Giz*` / `<Gizmo>_AddGizmos` (e.g. `Levers_AddGizmos`, `TightRopes_AddGizmos`, `GizSpinner_*`) → matching `legoapi/gizmos/<giz>.cpp`.
   - `Mech*` → `MechInputTouch/*`.
   - `ov_*` → `gamelib/` ogg-vorbis unit (already an `extra_unit` in gonk.toml).
   - `Java_com_tt_tech_*` → `src/java/*`.
   - `_GLOBAL__sub_I_*` / `__static_initialization_and_destruction_0` → the named TU's own file (as a `_start`/init helper).
   - Default: assign by symbol → xref of a known anchor, or a per-module catch-all `*_stubs.cpp`.
3. **Cross-checks** — strings referenced (`__FILE__` anchors), call-graph proximity to an anchor, and the existing repo splits (e.g. `world.cpp` already split into subsystem homes). Reconcile against the 320-TU list so every file has a real TU backstop.

Precision improves over time; the plan treats assignment as **iterative** — each new file
is created with the TUs it is confident about, and stragglers are moved as call-graph
evidence lands.

## 5. Stub format & generation

Stubs are generated **from the Ghidra function table** (demangled name + args) so
signatures are correct from the start. Per stub:

```cpp
#include "<module>_stubs.h"   // or the specific module header

// <mangled>, <TU source file> — <short purpose> (see <file>:<line>)
void <name>(<params>) {
    (void)<p1>;
    (void)<p2>;
    return <default>;   // 0 / NULL / false as appropriate
}
```

- Preserve `extern "C"` linkage exactly where the symbol is a C symbol (verified by the
  mangled name: `_Z…` ⇒ C++, plain ⇒ `extern "C"`).
- Keep signature types consistent with Ghidra's prototype (cast to canonical types where
  headers exist; `void*`/opaque where types are unknown yet).
- Each `.cpp` carries a per-file banner comment listing its TUs and the per-file `-O`
  flag (see §7).

### Generator

`scripts/tu_split.py` consumes the mangled-symbol inventory (demangled via
`c++filt`) and emits, per module, a `<module>_types.h` scaffolding header plus
per-TU `.cpp`/`_plain.cpp` source files and an explicit `stub_sources.cmake`
manifest that CMake includes. It is idempotent (re-running overwrites its own
prior output in place) and **never overwrites real repo files** (colliding names
get a `_stubs` suffix). ABI-special symbols (`_ZTh`/`_ZTv` thunks, template-typed
methods) are emitted as assembly stubs in `src/stubs/specials.S`.

## 6. Build integration

- `CMakeLists.txt` lists sources explicitly (no glob). Every new file is appended to the
  relevant target's source list.
- Add a source group convention mirroring §3 directories.
- Keep a per-module `*_stubs.cpp` so a module can be switched on without adding dozens of
  files at once during bring-up, then split into per-TU files as each is tackled.

## 7. Optimization level (per file)

The original compiles each TU at one `-O` level. Track it as a per-file attribute in the
file banner and (later) a CMake `set_source_files_properties(… COMPILE_OPTIONS …)` when
a file's code approaches matchability. Stub-only files use the target default `-O2`;
do not chase `-O` until real bodies exist.

## 8. Phased implementation

1. **Phase 0 — Inventory & plumbing (this doc).** Export symbol table, build module→TU
   map, stand up `scripts/gen_stubs.py`, add the module directories.
2. **Phase 1 — Self-contained libs first** (low dependency, high confidence, no game globals):
   `nu2api/numath` → `nu2api/nucore` → `nu2api/nusound` → `gamelib/util`. Establishes the
   stub convention and the CMake loop end-to-end.
3. **Phase 2 — nu3d renderer**: `nu2api/nu3d/*` (incl. `android/`).
4. **Phase 3 — Game logic**: `legoapi/` + `legoapi/gizmos/` (largest module; 128 TUs).
5. **Phase 4 — Systems**: `MechInputTouch/`, `gameapi/*`, `legogame/`, `gameframework/`,
   `nu2api/saga-platform/`, `MISC` triage.
6. **Phase 5 — Verify**: full build + lint; every target symbol produced; `report.md`
   shows 0 missing symbols (still low match %, but 100% symbol coverage).

Order rationale: dependencies flow from low-level libs (math, core, sound, file) up to
game logic, so each stub compiles standalone and matches the real call graph.

## 9. Process

- Keep this document up to date as mapping is refined; each implementation session
  starts by reading it.
- New files are created only with a real TU backstop from §3; no ad-hoc filenames.
- On ambiguity (a symbol matches multiple candidate files), put it in the best-prefix
  file and note it in this doc's "MISC/triage" section; resolve via call-graph/strings.
- Progress is tracked against the ~11,391-symbol target; `report.json` regenerates via
  the existing pre-commit hook.

## 10. Reference data

- Full Ghidra function table: dumped to
  `/home/fabian/.local/share/opencode/tool-output/tool_ff97aebdf0011LQtRy0jDUEU1Q`
  (re-generate with `list_functions_enhanced` on `libTTapp.so`).
- Source-file anchors (`.c`/`.cpp` `__FILE__` strings): see Ghidra
  `batch_string_anchor_report`, paths rooted at `i:/SagaTouch-Android_9176564/`.
- Symbol matching config: `gonk.toml` (ignore list + ogg_vorbis extra unit).

## 11. Status

- **Phase 1 pilot — numath (DONE)**: `src/nu2api/numath/numath_stubs.cpp` added with
  all confirmed-missing numath symbols (51 stubs) grouped by intended TU
  (nuang/nutrig/nuvec/numtx/nuquat/nurand/nuvec4). C++ (_Z) functions use exact
  c++filt signatures; C-linkage functions use best-effort inferred types (provisional,
  to be corrected in Phase 3 matching). Verified: compiles under g++ -std=c++17 -Isrc,
  clang-format clean, clang-tidy clean, and `nm` emits all C + mangled C++ symbols.
  Wired into CMake (added after `nufloat.c`).
- Next: replicate the same stub workflow for each module in the §3 cluster order,
  tracking against the full symbol inventory.

## 12. Symbol provisioning — COMPLETE (Phase 2 done)

All target global text symbols now correspond to a symbol in the build.

- **Mechanism**: `scripts/tu_split.py` reads a target-mangled symbol list and
  emits, per module, a **real header** (`<module>_types.h`) carrying the type
  scaffolding plus **one source file per TU** (`.cpp` for C++, `_plain.cpp` for
  plain-C `extern "C"` symbols) distributed per the section 3 TU lists and the
  section 4 prefix rules. A per-module `*_misc.cpp` catch-all absorbs stragglers
  (assignment is iterative, per section 9). Files whose name already exists as a
  real repo file get a `_stubs` suffix so generated stubs never overwrite real
  code. ABI-special symbols (multiple-inheritance `_ZTh`/`_ZTv` thunks and the
  template-typed NuSound methods) are emitted as `src/stubs/specials.S` assembly
  `ret` stubs. `scripts/tu_split.py` writes `src/stubs/stub_sources.cmake`, an
  explicit source manifest that CMake `include()`s — no fragile glob, and real
  files stay the source of truth.
- **Layout (per module)**: `src/<module>/<module>_types.h` (scaffolding header,
  `#pragma once`, dependency-ordered so nested types resolve), one `.cpp` per
  TU (`nusound_voice_stubs.cpp`, `MechAutoJumpManager.cpp`, …), a per-module
  `*_misc.cpp`, and `*_plain.cpp` for plain-C symbols. This replaces the earlier
  single self-contained blob per module (`*_decl_stubs.cpp`).
- **Verified** (`nm res/libTTapp.so` vs `nm build/saga`, minus `gonk.toml`
  ignore list):
  - target global text symbols: **9,201**
  - provided by real repo code: ~1,378
  - provided by generated per-TU stubs: ~7,704 (plus 45 ABI-special stubs)
  - missing: **0**
- Added 3 compiler-runtime/library symbols (`__udivdi3`, `__umoddi3`,
  `ogg_stream_flush_fill`) to the `gonk.toml` ignore list (provided by libgcc /
  libogg at link, not game logic).
- Android x86 target build: clean (`cmake --build build`), and
  `clang-format --dry-run --Werror --style=file` is clean on every generated
  file. `scripts/tu_split.py` runs clang-format twice per file because
  clang-format 22 is non-idempotent on empty function bodies.
- Note: generated signatures that use `long long`/`short`/`unsigned short`
  trip clang-tidy's `google-runtime-int` (the `lint` target). This is inherent:
  those are the target's actual types, and normalizing them to `i64`/`i16`/`u16`
  would change the mangled symbol name. Do not "fix" these.
- The 36 `_ZThn...` virtual-adjustment thunks are provisioned as stubs; when the
  underlying classes are implemented for real, remove their stubs so the
  compiler can emit the genuine thunks.
- Local/static target functions (`t`, not exported) are matched by objdiff by
  address, not by symbol; they are outside the symbol-correspondence scope.
- Next: Phase 3 — replace stubs with real implementations module-by-module,
  tracking via `report.json` / gonk.
