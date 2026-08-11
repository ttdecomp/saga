import sys
import json


def try_parse(value):
    if isinstance(value, str):
        try:
            return json.loads(value)
        except json.JSONDecodeError:
            return value
    return value


def diff_dict(a, b):
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
        (unit["name"], section["name"]): {
            "fuzzy_match_percent": section.get("fuzzy_match_percent")
        }
        for unit in a.get("units", [])
        for section in unit.get("sections", [])
    }
    bSections = {
        (unit["name"], section["name"]): {
            "fuzzy_match_percent": section.get("fuzzy_match_percent")
        }
        for unit in b.get("units", [])
        for section in unit.get("sections", [])
    }

    aFunctions = {
        (unit["name"], function["name"]): function["measures"]
        for unit in a.get("units", [])
        for function in unit.get("functions", [])
    }
    bFunctions = {
        (unit["name"], function["name"]): function["measures"]
        for unit in b.get("units", [])
        for function in unit.get("functions", [])
    }

    return (
        {
            k: diff
            for k in set(aUnits.keys()).union(set(bUnits.keys()))
            if (diff := diff_dict(aUnits.get(k, {}), bUnits.get(k, {}))) is not None
        },
        {
            k: diff
            for k in set(aSections.keys()).union(set(bSections.keys()))
            if (diff := diff_dict(aSections.get(k, {}), bSections.get(k, {})))
            is not None
        },
        {
            k: diff
            for k in set(aFunctions.keys()).union(set(bFunctions.keys()))
            if (diff := diff_dict(aFunctions.get(k, {}), bFunctions.get(k, {})))
            is not None
        },
    )


def format_item(x) -> str:
    if isinstance(x, float):
        return f"{x:.2f}"
    elif isinstance(x, int):
        return f"{x}"
    else:
        return str(x)


def format_change(x, y) -> str:
    if x is None and y is None:
        return ""

    if x is None:
        return f"+{format_item(y)}"
    elif y is None:
        return f"-{format_item(x)}"
    elif x == y:
        return f"{format_item(x)}"
    else:
        return f"{format_item(x)} -> {format_item(y)} ({format_item(y - x)})"


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

    print("Differences between the two JSON files:")

    units, sections, functions = diff(old, new)

    # table of units
    for unit, changes in units.items():
        print(f"{unit}:")
        print("  Total:")
        for measure, (old_value, new_value) in changes.items():
            print(f"    {measure:<25} {format_change(old_value, new_value)}")

        for (unit_, section), changes in sections.items():
            if unit_ != unit:
                continue

            print(f"  Section {section}:")
            print("    Total:")
            for measure, (old_value, new_value) in changes.items():
                print(f"      {measure}: {format_change(old_value, new_value)}")

            print(f"    Functions:")
            for (unit_, function), changes in functions.items():
                if unit_ != unit:
                    continue

                print(f"      Function {function}:")
                for measure, (old_value, new_value) in changes.items():
                    print(f"      {measure}: {format_change(old_value, new_value)}")

    print(functions)


if __name__ == "__main__":
    main()
