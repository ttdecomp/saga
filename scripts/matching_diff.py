#!/usr/bin/env python3
"""Render a Markdown report of function-score changes between matching files."""

from __future__ import annotations

import argparse
from collections import defaultdict
from dataclasses import dataclass, field
import html
import json
import os
from pathlib import Path
import sys

FULL_MATCH = 99.999
SCORE_EPSILON = 0.000001
REPORT_MARKER = "<!-- saga-function-matching-report -->"


@dataclass
class _FunctionBuilder:
    address: int
    size: int = 0
    score: float | None = None
    source: str | None = None
    labels: list[str] = field(default_factory=list)


@dataclass(frozen=True)
class Function:
    address: int
    size: int
    score: float | None
    source: str | None
    label: str


@dataclass(frozen=True)
class Change:
    old: Function
    new: Function

    @property
    def old_value(self) -> float:
        return self.old.score or 0.0

    @property
    def new_value(self) -> float:
        return self.new.score or 0.0

    @property
    def delta(self) -> float:
        return self.new_value - self.old_value

    @property
    def improved(self) -> bool:
        return self.delta > 0

    @property
    def old_exact(self) -> bool:
        return self.old_value >= FULL_MATCH

    @property
    def new_exact(self) -> bool:
        return self.new_value >= FULL_MATCH

    @property
    def source(self) -> str:
        return self.new.source or self.old.source or "(unassigned)"


def _score(value: object) -> float | None:
    if value is None:
        return None
    score = float(value)
    if not 0.0 <= score <= 100.0:
        raise ValueError(f"function match score is outside 0..100: {score}")
    return score


def _add_function(
    functions: dict[int, _FunctionBuilder], record: dict, source: str | None
) -> None:
    try:
        address = int(record["address"])
    except (KeyError, TypeError, ValueError) as error:
        raise ValueError("function record has no valid address") from error

    score = _score(record.get("match_percent"))
    size = int(record.get("size") or 0)
    label = str(record.get("demangled_name") or record.get("name") or hex(address))
    function = functions.setdefault(address, _FunctionBuilder(address=address))

    if function.score is not None and score is not None:
        if abs(function.score - score) > SCORE_EPSILON:
            raise ValueError(
                f"aliases at {address:#x} have different match scores "
                f"({function.score} and {score})"
            )
    elif score is not None:
        function.score = score

    function.size = max(function.size, size)
    if source:
        if function.source and function.source != source:
            raise ValueError(
                f"aliases at {address:#x} belong to different sources "
                f"({function.source!r} and {source!r})"
            )
        function.source = source
    if label not in function.labels:
        function.labels.append(label)


def load_functions(path: Path) -> dict[int, Function]:
    """Load and de-duplicate original functions by their stable ELF address."""
    with path.open(encoding="utf-8") as report_file:
        report = json.load(report_file)

    if not isinstance(report, dict) or not isinstance(report.get("units"), list):
        raise ValueError(f"{path}: not a matching.json report")

    builders: dict[int, _FunctionBuilder] = {}
    for unit in report["units"]:
        source = unit.get("source") or unit.get("name")
        for record in unit.get("functions", []):
            _add_function(builders, record, source)
    for section in ("ambiguous_functions", "unassigned_functions"):
        for record in report.get(section, []):
            _add_function(builders, record, None)

    functions = {}
    for address, builder in builders.items():
        labels = builder.labels[:2]
        label = " / ".join(labels)
        if len(builder.labels) > len(labels):
            label += f" / +{len(builder.labels) - len(labels)} aliases"
        functions[address] = Function(
            address=address,
            size=builder.size,
            score=builder.score,
            source=builder.source,
            label=label,
        )
    return functions


def compare(old: dict[int, Function], new: dict[int, Function]) -> list[Change]:
    """Return score changes for functions present in both reports."""
    shared_addresses = old.keys() & new.keys()
    if not shared_addresses:
        raise ValueError("the reports have no functions in common")
    changes = []
    for address in shared_addresses:
        change = Change(old=old[address], new=new[address])
        if abs(change.delta) > SCORE_EPSILON:
            changes.append(change)
    return sorted(changes, key=_change_sort_key)


def _change_sort_key(change: Change) -> tuple[int, float, str, int]:
    if change.old_exact and not change.new_exact:
        priority = 0
    elif change.new_exact and not change.old_exact:
        priority = 1
    elif not change.improved:
        priority = 2
    else:
        priority = 3
    return (priority, -abs(change.delta), change.source, change.new.address)


def _percent(score: float | None) -> str:
    if score is None:
        return "unmatched"
    if score >= FULL_MATCH:
        return "100%"
    precision = 4 if 0 < score < 0.01 else 2
    return f"{score:.{precision}f}%"


def _delta(delta: float) -> str:
    precision = 4 if abs(delta) < 0.01 else 2
    return f"{delta:+.{precision}f} pp"


def _magnitude(delta: float) -> str:
    precision = 4 if abs(delta) < 0.01 else 2
    return f"{abs(delta):.{precision}f} pp"


def _cell(value: str) -> str:
    return html.escape(value, quote=False).replace("|", "&#124;")


def _function_cell(change: Change) -> str:
    label = change.new.label or change.old.label
    return f"<code>{_cell(label)}</code><br><sub>{change.new.address:#x}</sub>"


def _transition(change: Change) -> str:
    old = _percent(change.old.score)
    new = _percent(change.new.score)
    return f"{old} → {new}<br><sub>{_delta(change.delta)}</sub>"


def _change_table(changes: list[Change]) -> list[str]:
    lines = [
        "| Function | Source | Match change |",
        "|---|---|---:|",
    ]
    for change in changes:
        lines.append(
            f"| {_function_cell(change)} | <code>{_cell(change.source)}</code> | "
            f"{_transition(change)} |"
        )
    return lines


def _largest_label(change: Change) -> str:
    direction = "↑" if change.improved else "↓"
    label = html.escape(change.new.label or change.old.label, quote=True)
    return f'<abbr title="{label}">{direction} {_magnitude(change.delta)}</abbr>'


def _group_table(changes: list[Change], group_limit: int) -> list[str]:
    groups: dict[str, list[Change]] = defaultdict(list)
    for change in changes:
        groups[change.source].append(change)

    ranked = sorted(groups.items(), key=lambda item: (-len(item[1]), item[0]))
    visible = ranked[:group_limit]
    hidden = ranked[group_limit:]
    lines = [
        "### Changes by source file",
        "",
        "| Source | Improved | Regressed | Exact + | Exact − | Largest move |",
        "|---|---:|---:|---:|---:|---:|",
    ]

    def add_row(name: str, grouped: list[Change]) -> None:
        improved = sum(change.improved for change in grouped)
        regressed = len(grouped) - improved
        exact_gained = sum(
            change.new_exact and not change.old_exact for change in grouped
        )
        exact_lost = sum(
            change.old_exact and not change.new_exact for change in grouped
        )
        largest = max(grouped, key=lambda change: abs(change.delta))
        lines.append(
            f"| <code>{_cell(name)}</code> | {improved} | {regressed} | "
            f"{exact_gained} | {exact_lost} | {_largest_label(largest)} |"
        )

    for name, grouped in visible:
        add_row(name, grouped)
    if hidden:
        combined = [change for _, grouped in hidden for change in grouped]
        add_row(f"Other {len(hidden)} source files", combined)
    return lines


def render_report(
    changes: list[Change],
    old_label: str,
    new_label: str,
    detail_limit: int,
    group_limit: int,
) -> str:
    improved = sum(change.improved for change in changes)
    regressed = len(changes) - improved
    exact_gained = sum(change.new_exact and not change.old_exact for change in changes)
    exact_lost = sum(change.old_exact and not change.new_exact for change in changes)

    lines = [
        REPORT_MARKER,
        "## Function matching report",
        "",
        f"Comparing `{_cell(old_label)}` → `{_cell(new_label)}`.",
        "",
    ]
    if not changes:
        lines.extend(
            [
                "✅ **No function matching scores changed.**",
                "",
                "<sub>Functions are compared by original ELF address; aliases at "
                "the same address count once.</sub>",
            ]
        )
        return "\n".join(lines) + "\n"

    if exact_lost:
        verdict = "⚠️"
    elif regressed:
        verdict = "🟡"
    else:
        verdict = "✅"
    lines.extend(
        [
            f"{verdict} **{improved:,} improved** · **{regressed:,} regressed** · "
            f"**{len(changes):,} changed**",
            "",
            "| Exact-match transitions | Score changes |",
            "|---|---|",
            f"| **+{exact_gained:,}** gained · **−{exact_lost:,}** lost | "
            f"**↑ {improved:,}** improved · **↓ {regressed:,}** regressed |",
            "",
        ]
    )

    show_all = detail_limit == 0 or len(changes) <= detail_limit
    if show_all:
        lines.extend(["### Function changes", ""])
        lines.extend(_change_table(changes))
    else:
        lines.extend(_group_table(changes, group_limit))
        lines.extend(
            [
                "",
                f"<details><summary>Exact transitions, regressions, and largest "
                f"improvements "
                f"({detail_limit} of {len(changes)})</summary>",
                "",
            ]
        )
        lines.extend(_change_table(changes[:detail_limit]))
        lines.extend(["", "</details>"])

    lines.extend(
        [
            "",
            "<sub>Only function score changes are shown. Functions are compared "
            "by original ELF address; aliases at the same address count once. "
            "“Unmatched” means no score was available.</sub>",
        ]
    )
    return "\n".join(lines) + "\n"


def _positive_or_zero(value: str) -> int:
    parsed = int(value)
    if parsed < 0:
        raise argparse.ArgumentTypeError("must be zero or greater")
    return parsed


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("old", type=Path, help="base matching.json")
    parser.add_argument("new", type=Path, help="head matching.json")
    parser.add_argument("--old-label", help="label for the base report")
    parser.add_argument("--new-label", help="label for the head report")
    parser.add_argument(
        "--detail-limit",
        type=_positive_or_zero,
        default=40,
        help="individual rows before grouping (0 shows all; default: 40)",
    )
    parser.add_argument(
        "--group-limit",
        type=_positive_or_zero,
        default=25,
        help="source rows in a grouped report (default: 25)",
    )
    return parser.parse_args(argv)


def _workspace_path(path: Path) -> Path:
    """Resolve CLI paths from the invocation directory under ``bazel run``."""
    workspace = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if workspace and not path.is_absolute():
        return Path(workspace) / path
    return path


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    if args.group_limit == 0:
        print(
            "matching_diff.py: --group-limit must be greater than zero", file=sys.stderr
        )
        return 2
    try:
        old = load_functions(_workspace_path(args.old))
        new = load_functions(_workspace_path(args.new))
        changes = compare(old, new)
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"matching_diff.py: {error}", file=sys.stderr)
        return 2

    print(
        render_report(
            changes,
            args.old_label or args.old.name,
            args.new_label or args.new.name,
            args.detail_limit,
            args.group_limit,
        ),
        end="",
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
