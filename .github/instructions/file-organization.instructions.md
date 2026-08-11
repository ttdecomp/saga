---
description: "Use when placing a new decompiled function or global variable into the src/ tree. Guides proper file selection, symbol naming conventions, and when to create new source files. Covers the ed- prefix rule for editor stubs."
applyTo: "src/**"
---

# Symbol Placement and File Organization

Every decompiled symbol must live in the source file that matches its domain. Never dump functions into random files or create monolithic catch-alls. The directory tree already reflects the original binary's module boundaries — use it.

## Directory Map

| Path                     | Domain                               | Symbol Prefixes                                                                                                                                                      |
| ------------------------ | ------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `src/batman.cpp`         | Main entry point                     | `NuMain`                                                                                                                                                             |
| `src/globals.cpp`        | Global variables                     | (various, no single prefix)                                                                                                                                          |
| `src/gameapi/ai/aisys/`  | AI script and state machines         | `AI`, `AISYS`, `AISCRIPT`                                                                                                                                            |
| `src/gameapi/edtools/`   | Editor tools (**stubs only**)        | `ed`, `Ed`                                                                                                                                                           |
| `src/gameapi/gui/`       | GUI and API menus                    | `api`, `Api`, `Menu`                                                                                                                                                 |
| `src/gameframework/`     | Save/load, framework layer           | `saveload`, `SaveLoad`                                                                                                                                               |
| `src/gamelib/`           | Game utility library                 | `CRC`, `Terrain`, `NuWind`                                                                                                                                           |
| `src/gamelib/crc/`       | CRC/hash functions                   | `CRC_`                                                                                                                                                               |
| `src/gamelib/nuwind/`    | Wind simulation                      | `NuWind`, `NW`                                                                                                                                                       |
| `src/legoapi/`           | Core LEGO game API objects           | `Area`, `Char`, `Level`, `Mission`, `World`, `Gizmo`, `Episode`, `Collection`, `Cheat`, `Store`, `Timer`, `QRand`                                                    |
| `src/legoapi/gizmos/`    | Individual gizmo types               | `GizmoType_*`, `GizmoTypes_*`                                                                                                                                        |
| `src/legogame/`          | Game state and startup               | `Game`, `Startup`                                                                                                                                                    |
| `src/java/`              | JNI stubs (**stubs only**)           | `Java_`, `JNI_`                                                                                                                                                      |
| `src/nu2api/nu3d/`       | 3D rendering, textures, shaders      | `NuRender`, `NuTex`, `NuShader`, `NuMtl`, `NuGScn`, `NuCamera`, `NuWater`, `NuPrim`, `NuDList`, `NuPortal`, `NuVPort`, `NuQfnt`, `NuRndr`, `NuOcclusion`, `NuScreen` |
| `src/nu2api/nucore/`     | Core systems: memory, input, threads | `NuMemory`, `NuInput`, `NuThread`, `NuString`, `NuTime`, `NuVideo`, `NuPad`, `NuKeyboard`, `NuMouse`, `NuCore`, `NuApi`, `NuList`                                    |
| `src/nu2api/nufile/`     | File I/O, PAK archives               | `NuFile`, `NuFPar`, `NuMC`, `NuDatFile`, `TMClient`                                                                                                                  |
| `src/nu2api/numath/`     | Math: vectors, matrices, trig        | `NuVec`, `NuMtx`, `NuQuat`, `NuTrig`, `NuPlane`, `NuRand`, `NuFloat`                                                                                                 |
| `src/nu2api/numusic/`    | Music and SFX playback               | `NuMusic`, `SFX`                                                                                                                                                     |
| `src/nu2api/nuplatform/` | Platform detection and specs         | `NuPlatform`, `NuDeviceSpecs`                                                                                                                                        |
| `src/nu2api/nusound/`    | Sound system                         | `NuSound`, `NUSOUND`                                                                                                                                                 |

Subdirectories like `android/`, `generic/` contain platform-specific implementations of the parent module's symbols.

## Decision Flow for Placing a Symbol

```
New symbol from Ghidra
  │
  ├─ Name starts with "ed" or "Ed"?
  │    └─ YES → Place in the appropriate edtools/ file, stub with UNIMPLEMENTED()
  │
  ├─ Name matches an existing file's prefix?
  │    └─ YES → Place in that file
  │         Example: "NuMusic_Play" → src/nu2api/numusic/numusic.cpp
  │
  ├─ Name belongs to a gizmo type?
  │    └─ YES → Place in src/legoapi/gizmos/<gizmoname>.cpp
  │         Create the file if it doesn't exist yet
  │         Example: "Door_Open" → src/legoapi/gizmos/door.cpp
  │
  ├─ Name matches a Nu2API subsystem (Nu + domain)?
  │    └─ YES → Place in src/nu2api/<subsystem>/<file>.cpp
  │         If new subsystem, create directory + files + add to CMakeLists.txt
  │
  ├─ Name is a global variable shared across modules?
  │    └─ YES → Place in src/globals.cpp, declare extern in appropriate header
  │
  └─ Truly new domain not fitting above?
       └─ Create a new source file with a descriptive name
            Add to CMakeLists.txt in SAGA_SOURCE_FILES
            Create matching header if other files need the declarations
```

## File Creation Rules

Only create a new source file when the symbol clearly belongs to a **new subsystem** not yet represented in the tree. Signs you need a new file:

1. The symbol introduces a new prefix not matching any existing file (e.g., `NuParticle_*` with no particle file)
2. The symbol is part of a class/struct hierarchy not yet represented
3. A group of related symbols (3+) share a prefix with no existing home

**When creating a new file:**

- Name it after the subsystem, not the individual function
- Use lowercase with underscores or CamelCase matching the existing convention in that directory
- Create a matching `.h` header for public declarations
- Add the `.cpp` to `CMakeLists.txt` in the correct position (alphabetical within its group)
- Add an `objdiff.json` unit entry mapping it to its split object

**When NOT to create a new file:**

- A single function that fits an existing file's domain → add it there
- A small helper only used by one other function → keep it in the same file
- A variant of an existing function → same file

## The `ed` / `Ed` Prefix Rule

Symbols beginning with `ed` or `Ed` belong to the **incomplete level editor**. The original editor code is not being decompiled. **Always stub these:**

```cpp
// ✅ CORRECT: stub an editor symbol
static void edShadow_GetMaxGizmos(void *shadow) {
    UNIMPLEMENTED();
}

static int EdClassInterface_GetNextObject(void *a, void *b) {
    UNIMPLEMENTED();
    return 0;
}
```

```cpp
// ❌ WRONG: implementing editor logic
static void edShadow_GetMaxGizmos(void *shadow) {
    // detailed implementation...  ← DON'T DO THIS
}
```

Place editor stubs in:
- `src/gameapi/edtools/edfile.cpp` — file-related editor tools
- `src/legoapi/gizmos/ed*.cpp` — editor gizmos (edgizshadowmachine, etc.)
- Other files where editor symbols naturally group with non-editor symbols of the same subsystem

## Naming Conventions

You have to name the symbol exactly as it appears in the original binary. Do not invent new names or change existing ones unless you are correcting a known mismatch. 
Naming conventions for general code are in `CONTRIBUTING.md`.

## Verify Before Committing

1. Does the file path make sense to another developer reading the tree?
2. Does the symbol name match the prefix/style of neighboring symbols?
3. If editor-prefixed, is it stubbed with `UNIMPLEMENTED()`?
4. If a new file, is it in `CMakeLists.txt` and `objdiff.json`?
5. Does `nm build/split/<file>.cpp.o` show the symbol in the expected object file?
