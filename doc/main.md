# Documentation

## Source-tree structure (restructuring reference)

[source-structure.md](source-structure.md) is the authoritative map of the original binary's
320 TUs → the `src/` files, the per-file `-O` contract, module grouping, and the outstanding
restructure items. Start here before moving any file or function.

## Decompiling with GCC 4.7 (knowledge base)

The [decomp/](decomp/00-index.md) directory contains the agent-oriented knowledge base for
matching-decompilation: toolchain, codegen, matching workflow, ABI/types, source conventions,
target binary reference, and the mismatch diagnostics playbook.

Start at [decomp/00-index.md](decomp/00-index.md).
