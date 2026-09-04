# Documentation

## Source-tree structure (restructuring reference)

[source-structure.md](source-structure.md) explains the original filename
evidence, current source hierarchy, generated TU mapping, and per-file `-O`
contract. Start there before moving any file or function. `src/BUILD.bazel`
and `bazel/android_per_file_copts.bazelrc` are the live source and optimization
authorities.

## Building

[build-bazel.md](build-bazel.md) documents target, native, and WASM builds,
including prerequisites and output locations.

## Decompiling with GCC 4.7 (knowledge base)

The [decomp/](decomp/00-index.md) directory contains the agent-oriented knowledge base for
matching-decompilation: toolchain, codegen, matching workflow, ABI/types, source conventions,
target binary reference, and the mismatch diagnostics playbook.

Start at [decomp/00-index.md](decomp/00-index.md).

## Host utilities

[host-utilities.md](host-utilities.md) documents interactive and unattended
host utilities, including hidden framebuffer capture and dummy audio. These
diagnostics do not replace original-binary evidence.

## Matching report

[pages/README.md](pages/README.md) documents the generated whole-binary
matching data and the static GitHub Pages visualization.

## Codex skill

Codex-compatible agents can load the repository-local `saga-decomp` skill from
`skills/saga-decomp/SKILL.md`. It summarizes the safe discovery and
verification workflow without duplicating the full knowledge base.
