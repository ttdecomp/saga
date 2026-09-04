# Repository scripts

Use Bazel labels for Python tools whenever one is available. Those labels run
with the Python 3.12.12 toolchain pinned in `MODULE.bazel`; direct `python3`
commands use the host interpreter instead.

## Automated path

The configured Git hook has one short call chain:

```text
git commit
  -> .githooks/pre-commit
  -> bazel run //scripts:pre_commit
  -> git diff --cached --check
  -> bazel test //scripts/checks:checks
  -> bazel build --config=target //src:saga_target
  -> bazel run //scripts/checks:check_symbols
  -> bazel run //scripts:generate_bazel_objdiff_report
  -> bazel run //scripts:plot_binary_match_map
```

The build and Pages steps are skipped when the locally supplied
`res/libTTapp.so` is absent. The hook writes `matching.json` and
`doc/pages/index.html`, then stops the commit if either generated file needs
to be staged. `.github/workflows/plot-pages.yaml` only deploys those committed
files; it does not regenerate them.

## Current tools

| Tool | Invocation | Purpose and dependencies |
|---|---|---|
| `generate_bazel_objdiff_report.py` | `bazel run //scripts:generate_bazel_objdiff_report` | Generates the custom whole-binary matching data. Calls Bazel and external `objdiff-cli`; writes `matching.json`. |
| `plot_binary_match_map.py` | `bazel run //scripts:plot_binary_match_map` | Turns `matching.json` into the static Pages application at `doc/pages/index.html`. Standard library only. |
| `objdiff-cli.py` | `bazel run //scripts:objdiff_cli -- SYMBOL` | Primary compact diff for one symbol. Calls external `objdiff-cli` and resolves the target-config library through Bazel. |
| `wasm_server.py` | `bazel run --config=wasm //scripts:wasm_server` | Serves the WASM build with isolation headers, the local OBB endpoint, and an HTTP(S) proxy. Standard library only. |

## Checks and launchers

- `scripts/checks/` contains three source-only Bazel tests, the post-build
  symbol check, and their support data. See
  [`checks/README.md`](checks/README.md).
- `scripts/lib/` contains import-only Python libraries. Its modules are
  `py_library` dependencies and have no `bazel run` target.
- `.githooks/pre-commit` only locates the workspace and enters Bazel;
  the cross-platform `pre_commit.py` Bazel target owns the actual hook sequence.
- `src/host/harness/run_native.sh` backs `//src:run_native`, restores the
  directory where `bazel run` was invoked, and then executes `saga_native`.
- `src/host/harness/make_capture_montage.sh` is a standalone diagnostic helper
  that calls ImageMagick `montage` and writes a capture contact sheet.

## Provenance of the current layout

The report generator, Pages copy/rewrite, Pages workflow, Bazel pre-commit
runner and native run wrapper are new in the current
Bazel/report work. The checks themselves are existing tools moved under
`scripts/checks/`; their logic is retained, with Bazel-run path handling added
where needed. `objdiff-cli.py` and `wasm_server.py` are existing tools extended
for the new Bazel workflow. The remaining tools are older, manual utilities
retained for their narrower use cases.
