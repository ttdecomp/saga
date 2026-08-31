#!/usr/bin/env python3
"""Reject host dependencies and behavioral build forks in shared source."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "src"
HOST_ROOTS = (SRC / "host" / "harness", SRC / "host" / "platform")
HOST_BUILD_ALLOWLIST = {
    SRC / "decomp.h",
    SRC / "decomp_assert.h",
    SRC / "gameframework/saveload.cpp",
    SRC / "nu2api/nucore/common.h",
}
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".h", ".hh", ".hpp"}
HOST_INCLUDE = re.compile(r"^\s*#\s*include\s*[<\"]host/(?:harness|platform)/", re.MULTILINE)


def is_under(path: Path, roots: tuple[Path, ...]) -> bool:
    return any(path.is_relative_to(root) for root in roots)


def main() -> int:
    errors: list[str] = []
    legacy = SRC / "host-utils"
    if legacy.exists() and any(path.is_file() for path in legacy.rglob("*")):
        errors.append("src/host-utils still exists; use src/host/harness or src/host/platform")

    for path in SRC.rglob("*"):
        if not path.is_file() or path.suffix not in SOURCE_SUFFIXES:
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if not is_under(path, HOST_ROOTS) and HOST_INCLUDE.search(text):
            errors.append(f"{path.relative_to(ROOT)} includes host-only code")
        if "HOST_BUILD" in text and path not in HOST_BUILD_ALLOWLIST and path != SRC / "host.cmake":
            errors.append(f"{path.relative_to(ROOT)} contains a behavioral HOST_BUILD fork")

    if errors:
        for error in errors:
            print(f"host-boundary: {error}", file=sys.stderr)
        return 1
    print("host-boundary: shared source is independent of host code")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
