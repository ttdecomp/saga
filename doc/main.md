# Documentation

## Source-tree structure (restructuring reference)

[source-structure.md](source-structure.md) explains the original filename
evidence, current source hierarchy, generated TU mapping, and per-file `-O`
contract. Start there before moving any file or function. `objdiff.json` and
`build/compile_commands.json` are the live mapping/count authorities.

## Decompiling with GCC 4.7 (knowledge base)

The [decomp/](decomp/00-index.md) directory contains the agent-oriented knowledge base for
matching-decompilation: toolchain, codegen, matching workflow, ABI/types, source conventions,
target binary reference, and the mismatch diagnostics playbook.

Start at [decomp/00-index.md](decomp/00-index.md).

## Host testing

[host-testing.md](host-testing.md) documents interactive and unattended host
render checks, including hidden framebuffer capture and dummy audio. Host tests
are diagnostics only; original-binary evidence remains authoritative.

## Codex skill

Codex-compatible agents can load the repository-local `saga-decomp` skill from
`skills/saga-decomp/SKILL.md`. It summarizes the safe discovery and
verification workflow without duplicating the full knowledge base.
