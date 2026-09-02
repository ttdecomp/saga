#!/usr/bin/env python3
"""Print markdown tables of matching progress per source subdirectory.

Aggregates the per-unit measures from an objdiff ``report.json`` and groups
them by the immediate subdirectories of ``src/`` and of ``src/legoapi/``.

Gonk names units after the symbol owner file and does not always carry the
full source path (and the layout is still being reorganized), so each unit is
resolved to a real file under ``src/`` by path when possible and by filename
otherwise (disambiguated with the longest matching path suffix).

Usage:
    matching_report.py [report.json] [--src SRC]
"""

import argparse
import json
import os
import re
import sys
from collections import defaultdict

ROOT_LABEL = "(root)"


def _num(value):
    """Coerce a report numeric field (often a JSON string) to an int."""
    if value is None or value == "":
        return 0
    try:
        return int(value)
    except (TypeError, ValueError):
        try:
            return int(float(value))
        except (TypeError, ValueError):
            return 0


def _suffix_score(rel, cand):
    """Number of trailing path components shared by unit name and candidate."""
    a = rel.split("/")
    b = cand.split("/")
    n = 0
    for x, y in zip(reversed(a), reversed(b)):
        if x != y:
            break
        n += 1
    return n


def find_src_file(name, src):
    """Resolve a unit name to a path relative to ``src`` (or None)."""
    rel = name
    for prefix in ("saga/src/", "saga/", "src/"):
        if rel.startswith(prefix):
            rel = rel[len(prefix):]
            break
    # Unit names carry the object extension (e.g. "Foo.cpp.o"); strip it so we
    # can resolve to the actual source file ("Foo.cpp").
    if rel.endswith(".o"):
        rel = rel[:-2]
    direct = os.path.join(src, rel)
    if os.path.isfile(direct):
        return os.path.relpath(direct, src)

    base = os.path.basename(rel)
    if not base or "." not in base:
        return None
    cands = []
    for root, _dirs, files in os.walk(src):
        if base in files:
            cands.append(os.path.relpath(os.path.join(root, base), src))
    if not cands:
        return None
    return max(cands, key=lambda c: _suffix_score(rel, c))


def _zero():
    return {"units": 0, "code": 0, "fuzzy": 0.0,
            "funcs": 0, "matched_funcs": 0,
            "data": 0, "matched_data": 0}


FIELDS = ("units", "code", "fuzzy", "funcs", "matched_funcs",
          "data", "matched_data")


def _aggregate(units, src):
    """Map directory -> dict of summed measures."""
    agg = defaultdict(_zero)
    for unit in units:
        rel = find_src_file(unit["name"], src)
        if rel is None:
            continue
        m = unit.get("measures", {})
        key = os.path.dirname(rel) or ROOT_LABEL
        code = _num(m.get("total_code"))
        agg[key]["units"] += 1
        agg[key]["code"] += code
        agg[key]["fuzzy"] += _fuzzy(m.get("fuzzy_match_percent")) * code
        agg[key]["funcs"] += _num(m.get("total_functions"))
        agg[key]["matched_funcs"] += _num(m.get("matched_functions"))
        agg[key]["data"] += _num(m.get("total_data"))
        agg[key]["matched_data"] += _num(m.get("matched_data"))
    return agg


def _fuzzy(value):
    """Coerce a fuzzy_match_percent value (already a percentage) to a float."""
    try:
        return float(value or 0.0)
    except (TypeError, ValueError):
        return 0.0


def _pct(matched, total):
    return (100.0 * matched / total) if total else 0.0


def _fmt_pct(matched, total, present):
    if not present:
        return "-"
    return "%.1f%%" % _pct(matched, total)


def _render_table(rows):
    """Render the markdown table (header + rows) as a list of lines."""
    lines = [
        "| Directory | Fuzzy % | Funcs % | Data % |",
        "|---|---|---|---|",
    ]
    for key in rows:
        a = rows[key]
        fuzzy = "-" if a["code"] == 0 else "%.1f%%" % (a["fuzzy"] / a["code"])
        funcs = _fmt_pct(a["matched_funcs"], a["funcs"], a["funcs"] > 0)
        data = _fmt_pct(a["matched_data"], a["data"], a["data"] > 0)
        lines.append(f"| `{key}` | {fuzzy} | {funcs} | {data} |")
    return lines


def _print_table(rows, title):
    print(f"## {title}\n")
    print("\n".join(_render_table(rows)))
    print()


SECTION_START = "<!-- matching-table-start -->"
SECTION_END = "<!-- matching-table-end -->"


def _badge_color(progress):
    if progress >= 90:
        return "brightgreen"
    if progress >= 70:
        return "green"
    if progress >= 50:
        return "yellow"
    if progress >= 30:
        return "orange"
    return "red"


def _update_readme(path, table_lines, progress):
    """Insert the table into README, replacing the marked section if present."""
    section = "\n".join([
        SECTION_START,
        "",
        "## Matching progress",
        "",
        *table_lines,
        "",
        SECTION_END,
    ])
    if not os.path.isfile(path):
        with open(path, "w") as f:
            f.write(section)
        return
    with open(path) as f:
        content = f.read()
    start = content.find(SECTION_START)
    end = content.find(SECTION_END)
    if start != -1 and end != -1 and end > start:
        content = content[:start] + section + content[end + len(SECTION_END):]
    else:
        content = content.rstrip("\n") + "\n\n" + section
    badge = "https://img.shields.io/badge/matching-%.2f%%25-%s" % (
        progress, _badge_color(progress))
    content = re.sub(
        r"https://img\.shields\.io/badge/matching-[^)]*", badge, content)
    with open(path, "w") as f:
        f.write(content)


def _subdir(rel):
    """Immediate subdirectory of ``src`` for an aggregated key (a dirname path)."""
    parts = rel.split(os.sep)
    return parts[0] if parts and parts[0] else ROOT_LABEL


def _dirs_on_disk(src):
    return sorted(d for d in os.listdir(src)
                  if os.path.isdir(os.path.join(src, d)))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("report", nargs="?", default="report.json",
                    help="path to objdiff report.json (default: report.json)")
    ap.add_argument("--src", default="src",
                    help="path to the source tree (default: src)")
    ap.add_argument("--readme", nargs="?", const="README.md", default=None,
                    metavar="PATH",
                    help="write the table into README.md (default: "
                         "print to stdout only; --readme enables it)")
    ap.add_argument("--no-readme", action="store_true",
                    help="do not write to README.md")
    args = ap.parse_args()

    if not os.path.isfile(args.report):
        print(f"error: no such report file: {args.report}", file=sys.stderr)
        sys.exit(1)
    with open(args.report) as f:
        report = json.load(f)

    agg = _aggregate(report.get("units", []), args.src)

    lego_base = os.path.join(args.src, "legoapi")
    if not os.path.isdir(lego_base):
        print(f"error: no directory {lego_base}", file=sys.stderr)
        sys.exit(1)

    # Top-level rows: immediate subdirectories of `src/` (including `legoapi`).
    rows = defaultdict(_zero)
    for rel, a in agg.items():
        key = _subdir(rel)
        for field in FIELDS:
            rows[key][field] += a[field]

    # Add the `legoapi/<subdir>` breakdown rows to the same table.
    for rel, a in agg.items():
        if rel.startswith("legoapi/"):
            key = "legoapi/" + rel[len("legoapi/"):].split("/")[0]
            for field in FIELDS:
                rows[key][field] += a[field]

    keys = set(_dirs_on_disk(args.src)) | {
        "legoapi/" + d for d in _dirs_on_disk(lego_base)
    } | set(rows)
    if ROOT_LABEL in rows:
        keys.add(ROOT_LABEL)
    keys.discard("host")
    table = {k: rows.get(k, _zero()) for k in sorted(keys)}
    _print_table(table,
                 "Matching progress for `src/` subdirectories and `legoapi/*`")

    if not args.no_readme:
        readme = args.readme or os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "..", "README.md")
        progress = _fuzzy(report.get("measures", {}).get("fuzzy_match_percent"))
        _update_readme(readme, _render_table(table), progress)


if __name__ == "__main__":
    main()
