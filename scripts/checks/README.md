# Repository checks

Run the complete fast suite with:

```sh
bazel test //scripts/checks:checks
```

The suite runs the three source-only checks. The symbol check runs after the
target build in both the pre-commit hook and CI; invoke it manually with:

```sh
bazel run //scripts/checks:check_symbols -- --list
```

CI supplies `--original-symbols matching.json`, whose symbol surface
is generated from the local reference ELF alongside the Pages report.

| File | Kind | What it checks |
|---|---|---|
| `check_bazel_optimization_map.py` | automatic `py_test` | Every Android per-file optimization mapping is exact, valid, and points to an existing source. |
| `check_host_boundary.py` | automatic `py_test` | Shared source does not include host-only code or add unapproved `HOST_BUILD` forks. |
| `check_duplicate_definitions.py` | automatic `py_test` | Duplicate C/C++ type definitions, plus optional duplicate object symbols. |
| `check_symbols.py` | post-build `py_binary` | Missing and unexpected text symbols relative to the original symbol surface; calls an NDK-compatible `nm`. |
| `symbols_ignore.txt`, `symbols_extra_baseline.txt` | support data | Documented symbol exceptions and the exact allowed extra-symbol set. |

Shared Python code that is not an executable lives in `scripts/lib/`.
`scripts/lib/ndk_tools.py` locates prefixed, LLVM, or host tool variants for
the duplicate-symbol and symbol-surface checks.
