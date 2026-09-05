# Repository scripts

> Agent/reference document. Human commands and workflows are in
> [`CONTRIBUTING.md`](../CONTRIBUTING.md).

Use Bazel labels for Python tools whenever one is available. Those labels run
with the Python 3.12.12 toolchain pinned in `MODULE.bazel`; direct `python3`
commands use the host interpreter instead.

## Automated path

The configured Git hook has one short call chain:

```text
git commit
  -> .githooks/pre-commit
  -> bazel run //scripts:pre_commit
  -> Bazel-provided clang-format -i (all C/C++ files under src/)
  -> git diff --cached --check
  -> bazel test //scripts/checks:checks
  -> bazel build --config=<mode> //src:clang_tidy_<mode>
  -> bazel build --config=target //src:saga_target
  -> bazel run //scripts/checks:check_symbols
  -> bazel run //scripts:generate_bazel_objdiff_report
```

Safe formatter changes are staged automatically; files with pre-existing
unstaged or untracked edits stop the hook for review. The binary-dependent
steps are skipped when the locally supplied `res/libTTapp.so` is absent. The
hook writes and automatically stages `matching.json` and the marked matching
table in `README.md`. Unsupported clang-tidy modes are omitted: macOS has no
native mode, and Apple Silicon has no direct target mode.
`.github/workflows/plot-pages.yaml` builds the release WASM target, runs
`//scripts:plot_binary_match_map` against that committed report, assembles the
browser assets, and deploys the generated landing page and progress explorer as
static files. The page only loads game data after an explicit user action and
waits for a separate Play click before starting. The browser loads remote OBB
URLs directly, so those origins must permit cross-origin requests; there is no
deployed proxy or API.

## Current tools

| Tool | Invocation | Purpose and dependencies |
|---|---|---|
| `generate_bazel_objdiff_report.py` | `bazel run //scripts:generate_bazel_objdiff_report` | Generates the custom whole-binary matching data and README progress table. Calls Bazel and external `objdiff-cli`; writes `matching.json` and the marked section of `README.md`. |
| `generate_objdiff_gui_config.py` | `bazel run //scripts:generate_objdiff_gui_config` | Generates the ignored root `objdiff.json` for the completely optional visual objdiff GUI, with one source/object unit per Bazel compile action. Builds the target and configures GUI rebuilds through Bazel; it is not part of reports, hooks, or CI. |
| `plot_binary_match_map.py` | `bazel run //scripts:plot_binary_match_map` | Fills `plot_binary_match_map.html` with `matching.json` data to create the static play-and-progress Pages application. Standard library only. |
| `objdiff-cli.py` | `bazel run //scripts:objdiff_cli -- SYMBOL` | Primary compact diff for one symbol. Calls external `objdiff-cli` and resolves the target-config library through Bazel. |
| `wasm_server.py` | `bazel run --config=wasm //scripts:wasm_server` | Serves only the generated static landing page, WASM build, and optional web-root OBB with local isolation headers. Standard library only. |

## Checks and launchers

- `scripts/checks/` contains three source-only Bazel tests, the post-build
  symbol check, and their support data. See
  [`checks/README.md`](checks/README.md).
- `scripts/lib/` contains import-only Python libraries. Its modules are
  `py_library` dependencies and have no `bazel run` target. The matching report
  and GUI config generator share their Bazel source/object query here.
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
