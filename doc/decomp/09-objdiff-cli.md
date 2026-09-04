# 09 — per-symbol diffing with `scripts/objdiff-cli.py`

The primary tool for drilling into **one symbol's** mismatch is
`scripts/objdiff-cli.py`, not the raw `objdiff` tools. Use it whenever you are
reviewing or fixing a single function against the original. The human-facing
`objdiff` CLI/GUI is for **manual** and **whole-repository** work (generating
`report.json`, scanning for regressions, eyeballing the interactive UI).

> Rule of thumb:
> - **Agents / one symbol at a time → `scripts/objdiff-cli.py`** (dense, already
>   stripped of objdiff's json noise, printed with decompiling hints).
> - **Manual / broad look → `objdiff`** (`objdiff-cli report generate -o
>   report.json` for the aggregate numbers; `objdiff-gui` for interactive
>   browsing). In this project that is the per-directory progress table in
>   `report.json`/`README.md`, plus the `scripts/objdiffdiff.py` regression
>   report.

## What it does

It shells out to `objdiff-cli diff -1 res/libTTapp.so -2
bazel-bin/src/libTTapp.so <symbol> -o -`, keeps **only the instructions where
the recompiled code and the original diverge**, and prints them aligned, with
context, colours, and trailing `#` hints (string / float constants, named
`.LC` rodata refs, resolved jump/call targets such as `-> 4e12c0
<_Z17Cheats_CheckFlagsj>`). Resolved rodata values appear inline at the end of
the referencing line, on changed and context lines alike.

The two sides are objdiff's fixed order (target on the left, base on the
right); the explicit `-1` and `-2` paths fix the direction:

| | side | source | meaning |
|---|---|---|---|
| left  | **target** | `res/libTTapp.so` | the ORIGINAL, our goal |
| right | **base**   | `bazel-bin/src/libTTapp.so` | the CURRENT code |

Marker letters are counterintuitive — they name *which side* a line comes
from, then you act:

- `' '` — unchanged (context; runs longer than `-C` collapse to `.. N identical instructions ..`).
- `'-'` — target-only = in the **ORIGINAL**, missing from your code ⇒ **ADD** it (you are missing this behaviour).
- `'+'` — base-only = in your **current build**, absent from the original ⇒ **REMOVE/fix** it (extra code the original lacks).
- `'~'` — same opcode, operands differ. The `'~'` line is the **ORIGINAL**; the `'>'` line below is your **current** build. Make `'>'` equal `'~'`.
- `'>'` — the base (current) version of the paired `'~'`.
- `'..'` — a collapsed run of N unchanged instructions.

Net: **ADD every `-` you lack, REMOVE every `+`, and make every `>` equal its
`~`.** Don't be misled by the arrow: a `-` means "in the goal, not in your
code", not "delete".

A symbol that is essentially matched (objdiff ≥ 99.999 %) prints a short
"symbol matches at N%" line and no diff.

## Usage

```bash
# prerequisite: build the matching Android x86 shared object
bazel build --config=android-x86 //src:saga_android_x86

python scripts/objdiff-cli.py SYMBOL
```

Arguments (see also `--help`, which carries the full legend):

| flag | effect |
|---|---|
| `SYMBOL` | mangled symbol to diff, e.g. `_Z13ResetPodStuffv` |
| `-b FILE` / `--base-path FILE` | original binary (default `res/libTTapp.so`) |
| `-t FILE` / `--target-path FILE` | current build (default `bazel-bin/src/libTTapp.so`) |
| `-C N` / `--context N` | unchanged context lines shown around each change (default 4) |
| `--full` | print the entire base instruction listing, no `..` collapsing |
| `--no-color` | disable ANSI colours |

Read the `-`/`+`/`~`/`>` glyphs per the legend above, edit `src/`, rebuild,
and re-run until only context (`' '`/`..`) remains.

## Relationship to the other docs

- **`03-matching.md`** covers pairing by name, `report.json` interpretation and
  the whole-repo numbers; this doc is the funnel for the *one function you are
  actually fixing*.
- **`08-asm-review.md`** gives the full command-line review workflow (symbol →
  TU → classified list → text diff → source interleave) and is the extraction
  half of a mismatch investigation. `objdiff-cli.py` automates the "get the
  per-symbol diff" step with decompiling hints baked in.
- **`07-diagnostics.md`** is the mismatch-cause catalog to apply once you can
  see the diff.
