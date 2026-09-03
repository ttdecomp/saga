#!/usr/bin/env python3
"""List matching status for functions selected by name.

An absent per-function fuzzy percentage in objdiff's JSON means 0%, not 100%.
Exact status therefore comes only from an explicit score at or above 99.999.
"""

import argparse
import json
import re


FULL_MATCH = 99.999


def number(value):
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def percentage(value):
    try:
        return float(value or 0.0)
    except (TypeError, ValueError):
        return 0.0


def load_functions(report, pattern):
    functions = {}
    for unit in report.get("units", []):
        for function in unit.get("functions") or []:
            display_name = function.get("metadata", {}).get("demangled_name") or function["name"]
            if pattern.search(display_name) is None:
                continue
            record = {
                "name": function["name"],
                "display_name": display_name,
                "unit": unit["name"],
                "size": number(function.get("size")),
                "fuzzy": percentage(function.get("fuzzy_match_percent")),
            }
            previous = functions.get(record["name"])
            if previous is None or record["fuzzy"] > previous["fuzzy"]:
                functions[record["name"]] = record
    return list(functions.values())


def print_summary(functions):
    exact = sum(function["fuzzy"] >= FULL_MATCH for function in functions)
    small = [function for function in functions if function["size"] <= 128]
    small_exact = sum(function["fuzzy"] >= FULL_MATCH for function in small)
    print(f"exact: {exact}/{len(functions)}")
    print(f"small (<=128 bytes): {small_exact}/{len(small)} exact, {len(small) - small_exact} open")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", nargs="?", default="report.json")
    parser.add_argument("--regex", default="reset", help="function-name regular expression")
    parser.add_argument("--case-sensitive", action="store_true")
    parser.add_argument("--max-size", type=int)
    parser.add_argument("--exact-only", action="store_true")
    parser.add_argument("--open-only", action="store_true")
    parser.add_argument("--limit", type=int, default=0)
    args = parser.parse_args()

    flags = 0 if args.case_sensitive else re.IGNORECASE
    pattern = re.compile(args.regex, flags)
    with open(args.report, encoding="utf-8") as report_file:
        functions = load_functions(json.load(report_file), pattern)

    print_summary(functions)

    if args.max_size is not None:
        functions = [function for function in functions if function["size"] <= args.max_size]
    if args.exact_only:
        functions = [function for function in functions if function["fuzzy"] >= FULL_MATCH]
    if args.open_only:
        functions = [function for function in functions if function["fuzzy"] < FULL_MATCH]

    functions.sort(key=lambda function: (function["fuzzy"] >= FULL_MATCH, function["size"], function["name"]))
    if args.limit > 0:
        functions = functions[: args.limit]

    if functions:
        print("\nscore\tsize\tfunction\tunit")
    for function in functions:
        print(
            f'{function["fuzzy"]:.3f}\t{function["size"]}\t'
            f'{function["display_name"]}\t{function["unit"]}'
        )


if __name__ == "__main__":
    main()
