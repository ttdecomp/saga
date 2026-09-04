# Matching report page

The repository-root `matching.json` is generated from one whole-binary
`objdiff-cli` report for
`res/libTTapp.so` and `bazel-bin/src/libTTapp.so`. The generator uses Bazel's
C++ compile actions and the symbol tables in their output objects to assign
original functions to source translation units.

From the repository root:

```sh
bazel build --config=target //src:saga_target
bazel run //scripts:generate_bazel_objdiff_report
bazel run //scripts:plot_binary_match_map
```

Both scripts are Bazel `py_binary` targets, so these commands use the pinned
Python 3.12.12 toolchain from `MODULE.bazel` rather than system site-packages.
The report generator also requires the external `objdiff-cli` executable on
`PATH` and the locally supplied `res/libTTapp.so` reference binary.

The first script writes the repository-root `matching.json`. Its top-level
`units` list contains
the Bazel source path, object path, and assigned functions. Each function has
its original virtual address, `.text` offset, original symbol size, and
whole-binary objdiff match percentage. Duplicate symbols whose unit cannot be
chosen safely are kept in `ambiguous_functions`; symbols absent from every
Bazel object are kept in `unassigned_functions`.

The report also stores the original binary's global, local, and weak text
symbol names in `original_text_symbols`. CI uses that derived symbol surface to
run `check_symbols` without requiring the copyrighted reference binary.

The second script embeds this data in `index.html`. The Pages workflow deploys
this directory without a separate site build.

The pre-commit hook is a small shim for the `//scripts:pre_commit` Bazel
`py_binary`. That target runs the build, symbol check, and both Bazel Python
generators when the ignored original binary is available. If either generated
file changes, it asks for the refreshed files to be staged and the commit
retried. It skips these binary-dependent steps on checkouts that do not have
`res/libTTapp.so`.
