# Agent/reference documentation

Human setup starts in [`README.md`](../README.md). Human development workflows
are in [`CONTRIBUTING.md`](../CONTRIBUTING.md). The documents below preserve
implementation details and investigation knowledge for agents and maintainers.

| Reference | Use it for |
|---|---|
| [Bazel internals](build-bazel.md) | Toolchains, dependency versions, flags, and platform behavior |
| [Source structure](source-structure.md) | Translation-unit ownership, optimization mapping, and safe file moves |
| [Decompilation knowledge base](decomp/00-index.md) | GCC 4.7 code generation, ABI, matching, diagnostics, and assembly review |
| [Host utilities](host-utilities.md) | Automated native captures and host-boundary rules |
| [Matching report](pages/README.md) | `matching.json` schema, generation, and Pages rendering |
| [Script inventory](../scripts/README.md) | Script call graph and executable versus library boundaries |

Codex-compatible agents can also load
[`skills/saga-decomp/SKILL.md`](../skills/saga-decomp/SKILL.md) for the concise
matching workflow. Repository files, Bazel actions, and generated artifacts are
authoritative when a snapshot in these references becomes stale.
