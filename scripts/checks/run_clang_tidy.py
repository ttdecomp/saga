#!/usr/bin/env python3
"""Run clang-tidy and create the Bazel output stamp on success."""

from pathlib import Path
import subprocess
import sys


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: run_clang_tidy.py STAMP CLANG_TIDY [ARGS...]", file=sys.stderr)
        return 2

    stamp = Path(sys.argv[1])
    status = subprocess.run(sys.argv[2:], check=False).returncode
    if status == 0:
        stamp.touch()
    return status


if __name__ == "__main__":
    sys.exit(main())
