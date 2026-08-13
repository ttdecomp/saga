import sys
import json
from os.path import dirname


def try_parse(value):
    if isinstance(value, str):
        try:
            return json.loads(value)
        except json.JSONDecodeError:
            return value
    return value


def _group_dir(unit):
    """Parent directory of a unit, normalized to a project-relative path.

    objdiff unit names carry inconsistent path prefixes ("saga/src/...",
    "src/...", or bare "nu2api/..."), so strip a leading "saga/" and "src/"
    component to group related functions under one stable directory.
    """
    parts = dirname(unit).split("/")
    if parts and parts[0] == "saga":
        parts = parts[1:]
    if parts and parts[0] == "src":
        parts = parts[1:]
    return "/".join(parts) or "(root)"


def diff_dict(a, b):
    if isinstance(a, (int, float)) or isinstance(b, (int, float)):
        return (a, b) if a != b else None

    if a == b:
        return None
    elif a is None:
        return {key: (None, try_parse(b.get(key))) for key in b.keys()}
    elif b is None:
        return {key: (try_parse(a.get(key)), None) for key in a.keys()}
    else:
        all_keys = set(a.keys()).union(set(b.keys()))
        return {key: (try_parse(a.get(key)), try_parse(b.get(key))) for key in all_keys}


def diff(a: dict, b: dict):
    aUnits = {unit["name"]: unit["measures"] for unit in a.get("units", [])}
    bUnits = {unit["name"]: unit["measures"] for unit in b.get("units", [])}

    aSections = {
        (unit["name"], section["name"]): section.get("fuzzy_match_percent")
        for unit in a.get("units", [])
        for section in unit.get("sections", [])
    }
    bSections = {
        (unit["name"], section["name"]): section.get("fuzzy_match_percent")
        for unit in b.get("units", [])
        for section in unit.get("sections", [])
    }

    aFunctions = {
        (unit["name"], function["name"]): function.get("fuzzy_match_percent", None)
        for unit in a.get("units", [])
        for function in unit.get("functions", [])
    }
    bFunctions = {
        (unit["name"], function["name"]): function.get("fuzzy_match_percent", None)
        for unit in b.get("units", [])
        for function in unit.get("functions", [])
    }

    return (
        diff_dict(a["measures"], b["measures"]),
        {
            k: diff
            for k in set(aUnits.keys()).union(set(bUnits.keys()))
            if (diff := diff_dict(aUnits.get(k, {}), bUnits.get(k, {}))) is not None
        },
        {
            k: diff
            for k in set(aSections.keys()).union(set(bSections.keys()))
            if (diff := diff_dict(aSections.get(k, None), bSections.get(k, None)))
            is not None
        },
        {
            k: diff
            for k in set(aFunctions.keys()).union(set(bFunctions.keys()))
            if (diff := diff_dict(aFunctions.get(k, None), bFunctions.get(k, None)))
            is not None
        },
    )


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


def print_table(data: list[list], headers: list[str]):
    if not data:
        print("No differences found.")
        return

    # Calculate the maximum width for each column
    col_widths = [len(header) for header in headers]
    for row in data:
        for i, cell in enumerate(row):
            col_widths[i] = max(col_widths[i], len(str(cell)))

    # Print the header
    header_row = " | ".join(
        header.ljust(col_widths[i]) for i, header in enumerate(headers)
    )
    print(header_row)
    print("-" * len(header_row))

    # Print the rows
    for row in data:
        print(" | ".join(str(cell).ljust(col_widths[i]) for i, cell in enumerate(row)))


def main():
    if len(sys.argv) < 3:
        print("Usage: objdiffdiff.py <new.json> <old.json>")
        sys.exit(1)

    new = sys.argv[1]
    old = sys.argv[2]

    with open(new, "r") as f1, open(old, "r") as f2:
        new = json.load(f1)
        old = json.load(f2)

    total, units, sections, functions = diff(old, new)

    # Per-function absolute sizes, keyed by (unit, function). Built before the
    # reporting loops below (which reuse the names `old`/`new` as loop vars).
    sizes = {}
    for report_data in (old, new):
        for unit in report_data.get("units", []):
            for fn in unit.get("functions", []):
                try:
                    size = int(fn.get("size") or 0)
                except (TypeError, ValueError):
                    size = 0
                sizes[(unit["name"], fn["name"])] = size

    # Per-unit matched code bytes, keyed by unit name. Used to weight each
    # directory's average fuzzy_match by how much code is actually matched;
    # units with no matched code get weight 0.
    matched = {}
    for report_data in (old, new):
        for unit in report_data.get("units", []):
            try:
                value = int(unit.get("measures", {}).get("matched_code") or 0)
            except (TypeError, ValueError):
                value = 0
            matched.setdefault(unit["name"], value)

    # Group function-level changes by the parent directory of their unit.
    # Larger commits touch too many functions to list individually, so we
    # average the per-function match percentages and sum the absolute code
    # sizes for each directory.
    dirs = {}
    for (unit, function), (a, b) in functions.items():
        key = _group_dir(unit)
        entry = dirs.setdefault(key, [])
        size = sizes.get((unit, function), 0)
        weight = matched.get(unit, 0) or size
        entry.append((size, weight, a, b))

    print("\n---\n")
    print("\n## Functions by directory\n")
    if not dirs:
        print("No function-level changes.")
    else:
        def weighted_avg(pairs):
            if not pairs:
                return None
            total = sum(weight for weight, _ in pairs)
            if total == 0:
                return None
            return sum(weight * value for weight, value in pairs) / total

        rows = []
        for key in sorted(dirs):
            entry = dirs[key]
            sizes_ = [size for size, _, _, _ in entry]
            old_pairs = [
                (weight, old) for _, weight, old, _ in entry if old is not None
            ]
            new_pairs = [
                (weight, new) for _, weight, _, new in entry if new is not None
            ]
            rows.append(
                [
                    f"`{key}`",
                    str(len(entry)),
                    str(sum(sizes_)),
                    format_change(
                        weighted_avg(old_pairs), weighted_avg(new_pairs), percent=True
                    ),
                ]
            )
        print("| Directory | Functions | Code size (bytes) | Avg fuzzy_match (matched-wtd) |")
        print("|---|---|---|---|")
        for row in rows:
            print("| " + " | ".join(row) + " |")

    print("\n---\n")
    print("\n## Total Changes:\n")

    print("| **Measure** | **Change** |")
    print("|-------------|------------|")
    if total:
        for measure, (old_value, new_value) in sorted(
            total.items(), key=lambda x: x[0]
        ):
            print(
                f"| {measure} | {format_change(old_value, new_value, percent=measure.endswith('percent'))} |"
            )

    print()


if __name__ == "__main__":
    main()
