"""Compare two objdiff report.json files and report regressions users can act on.

Reliability notes from the objdiff source (report.rs / diff/mod.rs):

- ``fuzzy_match_percent`` is always assigned in memory (0.0-100.0). The JSON
  key is present IFF the value is > 0.0 (proto3 "omit default" serialization),
  so "no key" means "scored 0.0", which happens when objdiff fails to pair the
  symbol to an original. Pairing is by symbol name and gonk re-splits by name,
  so adding/renaming/merging a source file can move functions between units and
  change pairing with zero code change. Key presence is therefore NOT a signal.

- ``matched_code`` / ``matched_functions`` count only symbols at exactly 100%
  and are computed in memory over all symbols, making them robust to both
  unit reorganization and tiny-stub pairing flutter. Treat them as the primary
  regression verdict.

- Per-function fuzzy deltas are reliable only for larger functions (small
  stubs < ~100 bytes flap wildly; functions > 2000 bytes are stable to ~0.12%).
  We threshold partial-match detail by size.

- Comparisons join functions by NAME (not unit), so relocation is not mistaken
  for appearing/disappearing; a move that stays matched contributes 0 delta.
"""

import sys
import json
from os.path import dirname

#: A function counts as "fully matched" at or above this fuzzy value (the ~100%
#: line that matched_code / matched_functions use).
FULL_MATCH = 99.999
#: Minimum function size (bytes) for reporting individual partial-fuzzy deltas.
PARTIAL_SIZE_THRESH = 200
#: Maximum rows in any individual list before collating with "+N more".
CAP = 20


def _num(value):
    """Coerce a report numeric field (often a JSON string) to an int."""
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _pct(value):
    """Coerce a percentage field to a float, or None when absent."""
    if value is None:
        return None
    try:
        return float(value)
    except (TypeError, ValueError):
        return None


def _norm_dir(unit):
    """Parent directory of a unit, normalized to a project-relative path."""
    parts = dirname(unit).split("/")
    if parts and parts[0] == "saga":
        parts = parts[1:]
    if parts and parts[0] == "src":
        parts = parts[1:]
    return "/".join(parts) or "(root)"


def index_by_name(report):
    """Map function name -> first record seen, joining across units."""
    idx = {}
    for unit in report.get("units", []):
        uname = unit["name"]
        for fn in unit.get("functions", []):
            rec = {
                "unit": uname,
                "dir": _norm_dir(uname),
                "size": _num(fn.get("size")),
                "fuzzy": _pct(fn.get("fuzzy_match_percent")),
            }
            idx.setdefault(fn["name"], rec)
    return idx


def is_matched(rec):
    return rec is not None and rec["fuzzy"] is not None and rec["fuzzy"] >= FULL_MATCH


def matched_bytes(rec):
    return rec["size"] if is_matched(rec) else 0


def formatter(x) -> str:
    return ("%.3f" % x).rstrip("0").rstrip(".")


def format_change(x, y, percent: bool = False) -> str:
    if x is None and y is None:
        return ""

    unit = r"\\% " if percent else ""

    single = (
        lambda sign, color, s: r"$\color{"
        + color
        + r"}\textbf{"
        + sign
        + formatter(s)
        + unit
        + r"}$"
    )
    double = (
        lambda sign, color, delta, old, new: r"$\color{"
        + color
        + r"}\textbf{"
        + sign
        + formatter(delta)
        + unit
        + r" \("
        + formatter(old)
        + unit
        + r"} \\; \rightarrow \\; \textbf{"
        + formatter(new)
        + unit
        + r"\)}$"
    )

    if x is None:
        return single("+", "green", y)
    elif y is None:
        return single("-", "red", x)
    elif x == y:
        return single("=", "gray", x)
    elif isinstance(x, (int, float)) and isinstance(y, (int, float)):
        d = y - x
        color = "green" if d > 0 else "red" if d < 0 else "gray"
        sign = "+" if d > 0 else ""
        return double(sign, color, d, x, y)


def main():
    if len(sys.argv) < 3:
        print("Usage: objdiffdiff.py <new.json> <old.json>")
        sys.exit(1)

    with open(sys.argv[1]) as f1, open(sys.argv[2]) as f2:
        new = json.load(f1)
        old = json.load(f2)

    old_meas = old.get("measures", {})
    new_meas = new.get("measures", {})
    old_fns = index_by_name(old)
    new_fns = index_by_name(new)

    top_old_mc = _num(old_meas.get("matched_code"))
    top_new_mc = _num(new_meas.get("matched_code"))
    top_old_mf = _num(old_meas.get("matched_functions"))
    top_new_mf = _num(new_meas.get("matched_functions"))

    # Classify every function (joined by name, across units).
    lost_full = []  # was 100%, now not
    gained_full = []  # wasn't 100%, now is
    partial = []  # both paired, size >= threshold, fuzzy changed
    partial_improved = 0
    moved = 0  # present on both sides but in a different unit
    added = 0
    removed = 0

    for name in set(old_fns).union(set(new_fns)):
        o = old_fns.get(name)
        n = new_fns.get(name)

        if o is None:
            added += 1
            continue
        if n is None:
            removed += 1
            continue
        if o["unit"] != n["unit"]:
            moved += 1

        om, nm = is_matched(o), is_matched(n)
        if om and not nm:
            lost_full.append({"name": name, "old": o, "new": n})
        elif nm and not om:
            gained_full.append({"name": name, "old": o, "new": n})

        # Partial-match trend only when both sides are paired (fuzzy key on both
        # sides) and the function is large enough that the value is meaningful.
        if (
            o["fuzzy"] is not None
            and n["fuzzy"] is not None
            and o["size"] >= PARTIAL_SIZE_THRESH
        ):
            if n["fuzzy"] > o["fuzzy"]:
                partial_improved += 1
            elif n["fuzzy"] < o["fuzzy"]:
                impact = o["size"] * (o["fuzzy"] - n["fuzzy"]) / 100.0
                partial.append(
                    {
                        "name": name,
                        "old": o,
                        "new": n,
                        "impact": impact,
                    }
                )

    lost_full.sort(key=lambda r: r["old"]["size"], reverse=True)
    partial.sort(key=lambda r: r["impact"], reverse=True)
    bytes_lost = sum(r["old"]["size"] for r in lost_full)
    bytes_gained = sum(r["new"]["size"] for r in gained_full)
    moved_lost = sum(1 for r in lost_full if r["old"]["unit"] != r["new"]["unit"])

    # Per-directory attribution (move-robust): assign each function's matched
    # byte delta to its NEW directory. A move that stayed matched adds 0.
    dir_delta = {}
    for name in set(old_fns).union(set(new_fns)):
        o = old_fns.get(name)
        n = new_fns.get(name)
        delta = matched_bytes(n) - matched_bytes(o)
        if delta:
            d = (n or o)["dir"]
            dir_delta[d] = dir_delta.get(d, 0) + delta

    # ---- Reporting ---------------------------------------------------------

    print("\n## Headline\n")
    print("| Metric | Change |")
    print("|--------|--------|")
    print(f"| matched_code (bytes) | {format_change(top_old_mc, top_new_mc)} |")
    print(f"| matched_functions | {format_change(top_old_mf, top_new_mf)} |")
    print(
        f"| fuzzy_match_percent | "
        f"{format_change(_pct(old_meas.get('fuzzy_match_percent')), _pct(new_meas.get('fuzzy_match_percent')), percent=True)} |"
    )
    print(
        f"| regressed (lost full match) | **{len(lost_full)}** "
        f"(\u2248 {int(bytes_lost):,} bytes lost) |"
    )
    print(
        f"| improved (gained full match) | **{len(gained_full)}** "
        f"(\u2248 {int(bytes_gained):,} bytes) |"
    )
    print()
    print(
        "\u26a0 The headline verdict is `matched_code`/`matched_functions`. "
    )
    print("Per-function `fuzzy_match_percent` presence is a pairing/serialize "
          "artifact and is not a regression signal.\n")

    print("## Lost full matches (regressions)\n")
    if not lost_full:
        print("None. Every function that previously matched 100% still does.\n")
    else:
        print("| Function | size | fuzzy old → new | old unit → new unit |")
        print("|---|---|---|---|")
        for r in lost_full[:CAP]:
            o, n = r["old"], r["new"]
            print(
                f"| `{r['name']}` | {o['size']} | "
                f"{format_change(o['fuzzy'], n['fuzzy'], percent=True)} | "
                f"`{o['unit']}` → `{n['unit']}` |"
            )
        hidden = len(lost_full) - CAP
        if hidden > 0:
            print(f"\n… and {hidden} more.")
        if moved_lost:
            print(
                f"\n*{moved_lost} of the {len(lost_full)} lost full matches also "
                "changed units; if the source for a moved-but-lost function is "
                "unchanged, this is a gonk/objdiff re-pairing artifact (see "
                "Context), not a real regression.\n"
            )

    print("## Partial-match regressions (size ≥ %d bytes)\n" % PARTIAL_SIZE_THRESH)
    if not partial:
        print(
            "None. No paired function above the size threshold reduced its "
            "fuzzy-match meaningfully.\n"
        )
    else:
        print("| Function | size | fuzzy old → new | est. bytes |")
        print("|---|---|---|---|")
        for r in partial[:CAP]:
            o, n = r["old"], r["new"]
            print(
                f"| `{r['name']}` | {o['size']} | "
                f"{format_change(o['fuzzy'], n['fuzzy'], percent=True)} | "
                f"~{int(r['impact'])} |"
            )
        hidden = len(partial) - CAP
        if hidden > 0:
            print(f"\n… and {hidden} more.\n")

    print("## By directory (matched Δ, attributed to new layout)\n")
    if not dir_delta:
        print("No change in matched bytes.")
    else:
        print("| Directory | matched Δ (bytes) |")
        print("|---|---|")
        for d, delta in sorted(dir_delta.items(), key=lambda kv: -abs(kv[1])):
            print(f"| `{d}` | {format_change(0, delta)} |")
        print()

    print("## Improvements (summary)\n")
    print(
        f"- **{len(gained_full):,}** function(s) gained a full 100% match "
        f"(\u2248 {int(bytes_gained):,} bytes new)."
    )
    print(
        f"- **{partial_improved:,}** function(s) above the size threshold "
        "improved their partial fuzzy-match."
    )

    print("## Context\n")
    print(
        f"- **{moved:,}** function(s) relocated between units (benign label "
        "churn when sources are reorganized; not a regression)."
    )
    print(f"- **{added:,}** function(s) added, **{removed:,}** removed at the "
          "symbol level.")
    # Self-check: per-directory attribution should sum to the top-level delta.
    check = sum(dir_delta.values())
    if check != top_new_mc - top_old_mc:
        print(
            f"\n\u26a0 internal drift: per-directory sum {check} \u2260 top-level "
            f"delta {top_new_mc - top_old_mc} (duplicate names / complete units)."
        )


if __name__ == "__main__":
    main()