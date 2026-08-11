---
name: objdiff-diff
description: 'Build the Saga project and use objdiff-cli to diff a specific function or generate progress reports. Use when: verifying a function matches the original binary after implementation; checking fuzzy match percentage for a symbol; running objdiff diff or report; comparing target vs base object files; tracking decompilation progress across units.'
argument-hint: 'Function symbol or unit to diff'
user-invocable: true
---

# Objdiff Diff

Build the Saga C++ project and use `objdiff-cli` to compare compiled object files against the original binary. Use this skill whenever you need to verify that an implementation matches the original or check overall decompilation progress.

## When to Use

- After implementing a function, to verify it matches the original binary
- Before committing, to check for regressions in fuzzy match percentage
- To generate a project-wide progress report
- To inspect a single function's match quality
- To split object files for deeper component-level analysis

## Build Configurations

The project has two build directories, configured from the same `CMakeLists.txt`:

| Build Dir    | Target           | Compiler        | Use Case                                |
| ------------ | ---------------- | --------------- | --------------------------------------- |
| `build`      | Android x86      | NDK r8e GCC 4.7 | Matching original binary (objdiff base) |
| `build-host` | Host (Linux x86) | System GCC      | Local testing and debugging             |

The `objdiff.json` maps source units to their target (our build) and base (original) object files.

## Procedure

### 1. Build the Project

Build the target (Android NDK) build — this produces the object files that `objdiff` compares against the original:

```bash
cmake --build build
```

For host-only testing (not used for diffing):

```bash
cmake --build build-host
```

### 2. Diff a Specific Function

To compare a single function against the original binary, use `objdiff-cli diff`. You need the **unit name** (as defined in `objdiff.json`) and the **mangled symbol name**:

```bash
objdiff-cli diff -p . -u <unit> <symbol>
```

**Options:**

| Option          | Description                                                     |
| --------------- | --------------------------------------------------------------- |
| `-p, --project` | Project directory (usually `.` from repo root)                  |
| `-u, --unit`    | Unit name matching an entry in `objdiff.json`                   |
| `-o, --output`  | Write output to file (`-` for stdout)                           |
| `--format`      | Output format: `json`, `json-pretty`, `proto` (default: `json`) |

**Example — diffing `NuMain` in `batman.cpp`:**

```bash
objdiff-cli diff -p . -u saga/src/batman.cpp _Z7NuMainv
```

**Example — diffing with pretty-printed JSON output:**

```bash
objdiff-cli diff -p . -u saga/src/globals.cpp _Z15SomeFunctionv --format json-pretty
```

**Finding unit names:** Look in `objdiff.json` under the `units` array — each entry has a `name` field.

**Finding symbol names:** Use `nm` on the target object file or `readelf -s` to list symbols in the compiled output:

```bash
nm build/split/batman.cpp.o | grep -i <function_name>
```

Use `jq` to filter the JSON output for specific symbols. Do not start writing huge python script to parse the output, `jq` is sufficient.

### 3. Generate a Progress Report

For a project-wide overview of match quality across all units, generate a report:

```bash
objdiff-cli report generate -p .
```

This produces `report.json` with per-unit and per-function fuzzy match percentages. Key metrics:

- `fuzzy_match_percent` — overall instruction-level match (100% = identical)
- `matched_code_percent` — percentage of code bytes that match
- `matched_data_percent` — percentage of data bytes that match
- `complete_code_percent` — percentage of functions with 100% match

To check what changed since the last report:

```bash
objdiff-cli report changes -p .
```

### 4. Split Objects for Deeper Inspection

When a function mismatch is puzzling, use `gonk split` to extract individual components from the object file for standalone analysis:

```bash
gonk split
```

This uses the configuration in `gonk.toml` to split object files into `build/split/`. The split objects are what `objdiff` uses as its **target** objects for comparison.

### 5. Interpret Diff Results

A successful diff returns JSON with the function's match data. Key fields:

```json
{
  "fuzzy_match_percent": 95.2,
  "matched_code_percent": 98.5,
  "instructions": { "matched": 120, "total": 126 }
}
```

- **100% fuzzy match** — function is fully matched; no further work needed
- **90–99%** — close match; minor differences in instruction selection or register allocation
- **<90%** — significant mismatch; investigate with Ghidra to understand the original logic
- **0% with matched data** — data matches but code does not; possible ABI issue or wrong compiler flags

If the diff output is empty or errors, verify:
- The unit name exists in `objdiff.json`
- The symbol name is correct (use `nm` to confirm)
- The build completed without errors
- `gonk split` has been run to produce split objects

## Common Workflow

After implementing a function from Ghidra decompilation:

```bash
# 1. Build
cmake --build build 2>&1 | tail -20

# 2. Diff the function just implemented
objdiff-cli diff -p . -u saga/src/<file>.cpp <mangled_symbol> --format json-pretty

# 3. If mismatch, inspect the original more closely with Ghidra, then iterate
# 4. After several functions, check overall progress
objdiff-cli report generate -p .
```

## References

- [objdiff.json](./../../objdiff.json) — unit mappings and build config, generated by `gonk split`, DO NOT edit manually
- [gonk.toml](./../../gonk.toml) — split configuration and ignored symbols
- [report.json](./../../report.json) — latest progress report
