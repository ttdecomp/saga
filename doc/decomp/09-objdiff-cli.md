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

It shells out to `objdiff-cli diff -p . <symbol> -o -`, keeps **only the
instructions where the recompiled code and the original diverge**, and prints
them aligned, with context, colours, and trailing `#` hints (string / float
constants, named `.LC` rodata refs, resolved jump/call targets such as
`-> 4e12c0 <_Z17Cheats_CheckFlagsj>`). A `referenced data (base)` list is
appended when the symbol touches data, so you can see what the current build
points at without decoding addresses by hand.

The two sides are objdiff's fixed order (target on the left, base on the
right), and the direction is **verified from the object files**, not guessed:

| | side | source | meaning |
|---|---|---|---|
| left  | **target** | `build/split` (carved from `res/libTTapp.so`) | the ORIGINAL, our goal |
| right | **base**   | `build/CMakeFiles` (our `src/` compiled)      | the CURRENT code |

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
# prerequisite: build + carve the original
cmake -B build . && cmake --build build
./gonk/target/release/gonk split          # (or the pre-commit hook / CI)

python scripts/objdiff-cli.py SYMBOL
```

Arguments (see also `--help`, which carries the full legend):

| flag | effect |
|---|---|
| `SYMBOL` | mangled symbol to diff, e.g. `_Z13ResetPodStuffv` |
| `-p DIR` / `--project DIR` | project dir passed through to `objdiff-cli` (default `.`) |
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
