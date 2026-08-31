#!/usr/bin/env python3
"""Reject source patterns that can truncate pointers in native host builds."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".h", ".hh", ".hpp"}

FORBIDDEN = (
    (
        "pointer converted to a fixed-width integer",
        re.compile(
            r"(?:static_cast|reinterpret_cast)\s*<(?:u|i)(?:8|16|32)>\s*\(\s*"
            r"reinterpret_cast\s*<usize>"
        ),
    ),
    (
        "pointer-sized integer narrowed with a C-style cast",
        re.compile(r"\((?:u|i)(?:8|16|32)\)\s*(?:\(\s*usize\s*\)|reinterpret_cast\s*<usize>)"),
    ),
    (
        "non-project pointer integer type used; use usize/isize",
        re.compile(r"\b(?:uintptr_t|intptr_t)\b"),
    ),
)

UNION = re.compile(r"\bunion\s*(?:\w+\s*)?\{(?P<body>[^{}]*)\}", re.DOTALL)
POINTER_MEMBER = re.compile(r"\b(?:[A-Za-z_]\w*\s+)+\*\s*\w+\s*(?:[;=\[])")
NARROW_MEMBER = re.compile(r"\b(?:u|i)(?:8|16|32)\s+\w+\s*(?:[;=\[])")


def line_number(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def main() -> int:
    errors: list[str] = []
    for path in SRC.rglob("*"):
        if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        relative = path.relative_to(ROOT)

        for description, pattern in FORBIDDEN:
            for match in pattern.finditer(text):
                errors.append(f"{relative}:{line_number(text, match.start())}: {description}")

        for match in UNION.finditer(text):
            body = match.group("body")
            if POINTER_MEMBER.search(body) and NARROW_MEMBER.search(body):
                errors.append(
                    f"{relative}:{line_number(text, match.start())}: "
                    "union aliases a pointer with a fixed-width integer"
                )

    if errors:
        for error in errors:
            print(f"host64-safety: {error}", file=sys.stderr)
        return 1

    print("host64-safety: no pointer truncation patterns found")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
