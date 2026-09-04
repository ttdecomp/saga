#!/usr/bin/env python3
"""Run repository checks and refresh generated Pages artifacts before commit."""

from __future__ import annotations

import os
from pathlib import Path
import shlex
import subprocess
import sys


def run(command: list[str], root: Path) -> int:
    print("+", shlex.join(command), flush=True)
    return subprocess.run(command, cwd=root, check=False).returncode


def main() -> int:
    workspace = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if not workspace:
        print("error: pre_commit must be run with bazel run", file=sys.stderr)
        return 2
    root = Path(workspace)

    for command in (
        ["git", "diff", "--cached", "--check"],
        ["bazel", "test", "//scripts/checks:checks"],
    ):
        status = run(command, root)
        if status:
            return status

    if not (root / "res/libTTapp.so").is_file():
        print("gh pages generation skipped: res/libTTapp.so is not present")
        return 0

    for command in (
        ["bazel", "build", "--config=target", "//src:saga_target"],
        ["bazel", "run", "//scripts/checks:check_symbols"],
        ["bazel", "run", "//scripts:generate_bazel_objdiff_report"],
        ["bazel", "run", "//scripts:plot_binary_match_map"],
    ):
        status = run(command, root)
        if status:
            return status

    stale = []
    for relative_path in ("matching.json", "doc/pages/index.html"):
        tracked = subprocess.run(
            ["git", "ls-files", "--error-unmatch", "--", relative_path],
            cwd=root,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        ).returncode == 0
        unchanged = subprocess.run(
            ["git", "diff", "--quiet", "--", relative_path],
            cwd=root,
            check=False,
        ).returncode == 0
        if not tracked or not unchanged:
            stale.append(relative_path)

    if stale:
        print("Generated GitHub Pages files changed; stage them and retry:")
        for relative_path in stale:
            print(f"  git add {relative_path}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
